#include "Fwk/Graphics/Sprite.h"

#include <DirectXMath.h>

#include "Lib/Graphics/TextureSRV.h"
#include "Lib/Util/Log.h"

#include "Fwk/Graphics/Texture.h"
#include "Fwk/Asset/Asset.h"
#include "Fwk/Asset/AssetTexture.h"
#include "Fwk/Framework.h"
#include "Fwk/Debug/DebugLog.h"

using namespace Lib::Graphics;

Sprite::Sprite()
	: m_pTexture(nullptr)
	, m_RotDeg()
	, m_RotMtx(DirectX::XMMatrixIdentity())
	, m_TransMtx(DirectX::XMMatrixIdentity())
	, m_SizeMtx(DirectX::XMMatrixIdentity())
	, m_ScaleMtx(DirectX::XMMatrixIdentity())
	, m_TransOffsetMtx(DirectX::XMMatrixIdentity())
	, m_PivotMtx(DirectX::XMMatrixIdentity())
	, m_pPlayingAnimation(nullptr)
	, m_PlayingAnimationId(0)
	, m_BlendMode(BLEND_MODE::Transparent)
	, m_isFlipX(false)
	, m_isFlipY(false)
	, m_IsInitialized(false)
{
	;
}

void Sprite::Init()
{
	//既に初期化済みであれば警告を出したうえで再初期化
	if (m_IsInitialized) {
		Debug::Logf("初期化済みのSpriteを再初期化しました。\n");
		Term();
	}

	//定数バッファの生成
	for (auto i = 0u; i < Graphics::FrameCount; ++i) {
		RenderManager_I->CreateConstantBuffer(m_ConstantBuffer[i]);
	}

	m_pPlayingAnimation = nullptr;
	m_PlayingAnimationId = 0;
	m_Animations.clear();
	m_isFlipX = false;
	m_isFlipY = false;

	{
		const XMFLOAT3 Position[4] = {
			{-0.5f, 0.5f, 0.0f},
			{ 0.5f, 0.5f, 0.0f},
			{ 0.5f,-0.5f, 0.0f},
			{-0.5f,-0.5f, 0.0f},
		};
		const XMFLOAT3 Normal = { 0.0f,1.0f,0.0f };
		const XMFLOAT4 Color = { 1.0f,1.0f,1.0f,1.0f };
		const XMFLOAT2 TexCoord[4] = {
			{0.0f,0.0f},
			{1.0f,0.0f},
			{1.0f,1.0f},
			{0.0f,1.0f},
		};
		const XMFLOAT3 Tangent = { 1.0f,0.0f,0.0f };
		const XMFLOAT3 AddColor = { 0.0f,0.0f,0.0f };

		const MeshVertex vtx[4] = {
			{Position[0],Normal,Color,TexCoord[0],Tangent,AddColor},
			{Position[1],Normal,Color,TexCoord[1],Tangent,AddColor},
			{Position[2],Normal,Color,TexCoord[2],Tangent,AddColor},
			{Position[3],Normal,Color,TexCoord[3],Tangent,AddColor},
		};

		ResMesh resMesh;

		resMesh.Vertices.push_back(vtx[0]);
		resMesh.Vertices.push_back(vtx[1]);
		resMesh.Vertices.push_back(vtx[2]);
		resMesh.Vertices.push_back(vtx[3]);

		resMesh.Indices.push_back(0);
		resMesh.Indices.push_back(1);
		resMesh.Indices.push_back(2);

		resMesh.Indices.push_back(0);
		resMesh.Indices.push_back(2);
		resMesh.Indices.push_back(3);

		resMesh.MaterialId = 0;

		m_plane.Init(RenderManager_I->GetGraphics()->GetDevice(), resMesh,D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	SetPivot(Pivot::Center);

	m_IsInitialized = true;
}

Sprite::~Sprite()
{
	Term();

}
void Sprite::SetTexture(Texture& pTexture)
{
	m_pTexture = &pTexture;
}

void Sprite::ClearTexture()
{
	m_pTexture = nullptr;
}

void Sprite::Term()
{
	if (!m_IsInitialized) {
		return;
	}

	//Frameworkが破棄されていたら何もしない
	auto fwk = Framework_I;
	if (fwk == nullptr)
		return;

	for (auto i = 0u; i < Graphics::FrameCount; ++i) {
		m_ConstantBuffer[i].Term();
	}
	m_plane.Term();
	m_pPlayingAnimation = nullptr;
	m_PlayingAnimationId = 0;
	m_Animations.clear();

	m_IsInitialized = false;
}

void Sprite::Update()
{
	if (m_pPlayingAnimation) {
		float deltaTime = Time_I->GetDeltaTime();
		m_pPlayingAnimation->Update(deltaTime);
		_applyAnimation();
	}
}

void Sprite::Draw()
{
	RenderManager_I->RequestRender(this);
}

void Sprite::SetRotation(float rotDeg) {
	m_RotDeg[2] = rotDeg;
	m_RotMtx = XMMatrixRotationZ(DirectX::XMConvertToRadians(rotDeg));
}

//回転（全軸対象）　引数はDegree 0.0f～360.0fで一周
void Sprite::SetRotation(float rotDegX, float rotDegY, float rotDegZ)
{
	m_RotDeg[0] = rotDegX;
	m_RotDeg[1] = rotDegY;
	m_RotDeg[2] = rotDegZ;
	// Rotates about y-axis (Yaw), then x-axis (Pitch), then z-axis (Roll)
	m_RotMtx = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(rotDegX)
		, XMConvertToRadians(rotDegY)
		, XMConvertToRadians(rotDegZ)
	);
}

//ベクトルの向きに回転する
void Sprite::SetRotation(const Vector2f& vDirection) {

	const Vector2f right(1.0f, 0.0f);

	Vector2f vDir = vDirection.GetNormalized();

	float cos = Vector2f::Dot(right, vDir);
	float sin = Vector2f::Cross(right, vDir);
	float rad = acosf(cos);

	if (sin < 0.0f) {
		rad = XM_2PI - rad;
	}
	SetRotation(XMConvertToDegrees(rad));
}

void Sprite::SetPosition(float x, float y) {
	m_TransMtx = XMMatrixTranslation(x, y, 0.0f);
}
void Sprite::SetPosition(const Lib::Math::Vector2f& vPos) {
	m_TransMtx = XMMatrixTranslation(vPos.x, vPos.y, 0.0f);
}
void Sprite::SetPosition(float x, float y, float z) {
	m_TransMtx = XMMatrixTranslation(x, y, z);
}
void Sprite::SetPosition(const Lib::Math::Vector3f& vPos) {
	m_TransMtx = XMMatrixTranslation(vPos.x, vPos.y, vPos.z);
}
void Sprite::SetSize(float width, float height){
	m_SizeMtx = XMMatrixScaling(width, height, 1.0f);
}

void Sprite::SetScale(float sx, float sy) {
	m_ScaleMtx = XMMatrixScaling(sx, sy, 1.0f);
}
void Sprite::SetScale(const Vector2f& vScale) {
	SetScale(vScale.x, vScale.y);
}

void Sprite::SetTexCoord(float left, float top, float width, float height)
{
	XMFLOAT2 coords[] = {
		{ left ,top}
		,{ left+width,top }
		,{ left + width,top + height }
		,{ left,top + height }
	};
	m_plane.SetTexCoord(0,coords, 4);
}

//頂点別にUV座標を設定する場合(vtxIndexは頂点番号 0:左上 1:右上 2:右下 3:左下)
void Sprite::SetTexCoord(int vtxIndex, float u, float v) {
	XMFLOAT2 coords = { u,v };
	m_plane.SetTexCoord(vtxIndex, coords);
}

void Sprite::SetPivot(Pivot pivot) {
	switch (pivot) {
	case Pivot::Center:SetPivot(0.5f, 0.5f); break;
    case Pivot::TopLeft:SetPivot(0.0f, 1.0f);break;
    case Pivot::Top:SetPivot(0.5f, 1.0f);break;
    case Pivot::TopRight:SetPivot(1.0f, 1.0f);break;
    case Pivot::Left:SetPivot(0.0f, 0.5f);break;
    case Pivot::Right:SetPivot(1.0f, 0.5f);break;
    case Pivot::BottomLeft:SetPivot(0.0f, 0.0f);break;
    case Pivot::Bottom:SetPivot(0.5f, 0.0f);break;
    case Pivot::BottomRight:SetPivot(1.0f, 0.0f);break;
	}
}

//中心軸の設定
void Sprite::SetPivot(float x, float y)
{
	SetPivot(x, y,0.5f);
}

void Sprite::SetPivot(const Lib::Math::Vector2f& pivot)
{
	SetPivot(pivot.x, pivot.y,0.5f);
}

void Sprite::SetPivot(float x, float y, float z) {
	//0.5,0.5が中心位置となるように調整する
	m_PivotMtx = XMMatrixTranslation(-(x - 0.5f), -(y - 0.5f), -(z - 0.5f));
}

void Sprite::SetPivot(const Vector3f& pivot){
	SetPivot(pivot.x, pivot.y, pivot.z);
}

void Sprite::SetSpriteCell(const SpriteCell& cell) {
	SetTexCoord(cell.texcoord[0], cell.texcoord[1], cell.texcoord[2], cell.texcoord[3]);
	SetPivot(cell.pivot[0],cell.pivot[1]);
}

//パラメータ取得系
float Sprite::GetRotationX()const {
	return m_RotDeg[0];
}
float Sprite::GetRotationY()const {
	return m_RotDeg[1];
}
float Sprite::GetRotationZ()const {
	return m_RotDeg[2];
}
float Sprite::GetPositionX()const {
	return m_TransMtx.r[3].m128_f32[0];
}
float Sprite::GetPositionY()const{
	return m_TransMtx.r[3].m128_f32[1];
}
Lib::Math::Vector3f Sprite::GetPosition()const{
	Lib::Math::Vector3f v(
		m_TransMtx.r[3].m128_f32[0],
		m_TransMtx.r[3].m128_f32[1],
		m_TransMtx.r[3].m128_f32[2]
	);
	return v;
}

float Sprite::GetWidth()const {
	return m_SizeMtx.r[0].m128_f32[0];
}
float Sprite::GetHeight()const {
	return m_SizeMtx.r[1].m128_f32[1];
}
float Sprite::GetScaleX()const {
	return m_ScaleMtx.r[0].m128_f32[0];
}
float Sprite::GetScaleY()const {
	return m_ScaleMtx.r[1].m128_f32[1];
}
float Sprite::GetScaledWidth()const {
	return GetWidth() * GetScaleX();
}
float Sprite::GetScaledHeight()const {
	return GetHeight() * GetScaleY();
}

void Sprite::OnDraw(IGraphics* g,Camera* pCamera)
{
	//既に初期化済みであれば警告を出したうえで再初期化
	if (!m_IsInitialized) {
		Debug::Logf("未初期化のSpriteの描画を試みました。\n");
		return;
	}
	//if (m_pTexture == nullptr) {
	//	Debug::Logf("テクスチャの設定されていないSpriteの描画を試みました。\n");
	//	return;
	//}

	auto pCommandList = g->GetCommandList();
	auto frameIndex = g->GetFrameIndex();

	//この第一引数のINDEXはルートシグネチャのパラメータと対応している必要があるよ
	pCommandList->SetGraphicsRootConstantBufferView(0, m_ConstantBuffer[frameIndex].GetAddress());

	//ワールド行列の設定
	Fwk::CB_Transform* pTrans = (Fwk::CB_Transform*)m_ConstantBuffer[frameIndex].GetPtr();

	DirectX::XMMATRIX mtxFlip = DirectX::XMMatrixIdentity();
	if (m_isFlipX) {
		mtxFlip.r[0].m128_f32[0] = -1.0f;
	}
	if (m_isFlipY) {
		mtxFlip.r[1].m128_f32[1] = -1.0f;
	}

	pTrans->World = m_PivotMtx * (m_ScaleMtx * m_SizeMtx) * m_RotMtx * mtxFlip * (m_TransMtx*m_TransOffsetMtx);
	pTrans->View = pCamera->GetViewMatrix();
	pTrans->Proj = pCamera->GetProjectionMatrix();

	//テクスチャを貼る
	if (m_pTexture != nullptr) {
		TextureSRV* pSRV = m_pTexture->GetSRV();
		if (pSRV != nullptr) {
			pCommandList->SetGraphicsRootDescriptorTable(1, pSRV->GetHandleGPU());
		}
	}

	m_plane.Draw(pCommandList->Get());
}

RENDER_PASS Sprite::GetRenderPass()const {
	return RENDER_PASS::Sprite;
}

RENDER_MODE Sprite::GetRenderMode()const
{
	const RENDER_MODE texRenderMode[] = {
		 RENDER_MODE::Texture_Opaque
		,RENDER_MODE::Texture_Transparent
		,RENDER_MODE::Texture_Blend_Add
		,RENDER_MODE::Texture_Blend_Sub
		,RENDER_MODE::Texture_Blend_Inverse
	};
	const RENDER_MODE notexRenderMode[] = {
		 RENDER_MODE::Color_Opaque
		,RENDER_MODE::Color_Transparent
		,RENDER_MODE::Color_Blend_Add
		,RENDER_MODE::Color_Blend_Sub
		,RENDER_MODE::Color_Blend_Inverse
	};

	if (m_pTexture != nullptr) {
		return texRenderMode[(int)m_BlendMode];
	}

	return notexRenderMode[(int)m_BlendMode];
}

void Sprite::SetBlend(BLEND_MODE blendMode) {
	m_BlendMode = blendMode;
}

void Sprite::SetColor(float r, float g, float b, float a) {

	const XMFLOAT4 colors[] =
	{
		{ r,g,b,a }
		,{ r,g,b,a }
		,{ r,g,b,a }
		,{ r,g,b,a }
	};

	m_plane.SetColor(0,colors,4);
}

void Sprite::SetColor(float r, float g, float b) {

	const XMFLOAT3 colors[] =
	{
		{ r,g,b }
		,{ r,g,b }
		,{ r,g,b }
		,{ r,g,b }
	};

	m_plane.SetColor(0,colors,4);
}

void Sprite::SetAlpha(float a) {

	const float alpha[]{
		a,a,a,a
	};
	m_plane.SetAlpha(0,alpha,4);
}

void Sprite::SetAdditionalColor(float r, float g, float b) {
	const XMFLOAT3 colors[] =
	{
		{ r,g,b }
		,{ r,g,b }
		,{ r,g,b }
		,{ r,g,b }
	};
	m_plane.SetAdditionalColor(0,colors,4);
}

//頂点カラー設定(rgba) 頂点番号指定
void Sprite::SetColor(int vtxIndex, float r, float g, float b, float a)
{
	const XMFLOAT4 color = { r,g,b,a };
	m_plane.SetColor(vtxIndex, color);
}
//頂点カラー設定(rgb) 頂点番号指定
void Sprite::SetColor(int vtxIndex, float r, float g, float b)
{
	const XMFLOAT3 color = { r,g,b };
	m_plane.SetColor(vtxIndex, color);
}
//アルファ値設定 頂点番号指定
void Sprite::SetAlpha(int vtxIndex, float a) 
{
	m_plane.SetAlpha(vtxIndex, a);
}
//追加カラー設定
void Sprite::SetAdditionalColor(int vtxIndex, float r, float g, float b)
{
	const XMFLOAT3 color = { r,g,b };
	m_plane.SetAdditionalColor(vtxIndex, color);
}

//アニメーションを追加する(ID自動採番）
int Sprite::AddAnimation(const Animation& animation)
{
	int id = _findFreeId();
	m_Animations[id] = animation;
	return id;
}

//アニメーションを追加する(ID指定）
int Sprite::AddAnimation(int id,const Animation& animation)
{
	m_Animations[id] = animation;
	return id;
}

//アニメーションを削除する(ID指定）
void Sprite::RemoveAnimation(int id) {

	if (m_Animations.count(id) == 0)
		return;

	if (m_pPlayingAnimation && m_PlayingAnimationId == id) {
		StopAnimation();
	}

	m_Animations.erase(id);
}

//アニメーションを削除する(アニメーション名指定）
void Sprite::RemoveAnimation(const char* name) {

	int id=0;
	if (!_findIdByName(name, &id))
		return;

	RemoveAnimation(id);
}

void Sprite::RemoveAnimation(const string& name) {
	RemoveAnimation(name.c_str());
}

//アニメーションを再生する(ID指定）
void Sprite::PlayAnimation(int id, AnimationPlayStyle playStyle) {

	if (m_Animations.count(id) == 0) {
		return;
	}

	//既に同じアニメーションを再生していて、
	//再生スタイルが「継続」であった場合は何もしないで抜ける
	if(IsAnimationPlaying() 
	&& GetAnimationId() == id 
	&& playStyle == AnimationPlayStyle::Continue ) {
		return;
	}

	StopAnimation();

	m_PlayingAnimationId = id;
	m_pPlayingAnimation = &m_Animations[id];
	m_pPlayingAnimation->Start();

	_applyAnimation();
}

//アニメーションを再生する(アニメーション名指定）
void Sprite::PlayAnimation(const char* name, AnimationPlayStyle playStyle) {

	int id = 0;
	if (!_findIdByName(name, &id)) {

		Debug::Logf("登録されていないアニメーション名です。 %s\n",name);
		return;
	}

	PlayAnimation(id, playStyle);
}
void Sprite::PlayAnimation(const string& name, AnimationPlayStyle playStyle) {
	PlayAnimation(name.c_str(), playStyle);
}

//アニメーションを一時停止する
void Sprite::PauseAnimation(bool bPause) {
	if (m_pPlayingAnimation == nullptr)
		return;
	m_pPlayingAnimation->Pause(bPause);
}

//アニメーションを停止する
void Sprite::StopAnimation() {
	if (m_pPlayingAnimation == nullptr)
		return;
	
	m_pPlayingAnimation->Stop();
	//m_pPlayingAnimation をnullにする前にこの処理を入れておく
	_applyAnimation();

	m_pPlayingAnimation = nullptr;
	m_PlayingAnimationId = 0;
}

//アニメーションのループ設定(現在再生中のアニメ）
void Sprite::SetLoopAnimation(bool bLoop) {
	if (m_pPlayingAnimation == nullptr)
		return;
	m_pPlayingAnimation->SetLoop(bLoop);
}

//アニメーションのループ設定(ID指定）
void Sprite::SetLoopAnimation(int id, bool bLoop) {

	if (m_Animations.count(id) == 0)
		return;

	m_Animations[id].SetLoop(bLoop);
}

//アニメーションのループ設定(アニメーション名指定）
void Sprite::SetLoopAnimation(const char* name, bool bLoop) {

	int id = 0;
	if (!_findIdByName(name, &id))
		return;

	m_Animations[id].SetLoop(bLoop);
}
void Sprite::SetLoopAnimation(const string& name, bool bLoop) {
	SetLoopAnimation(name.c_str());
}

//すべてのアニメーションのループ設定
void Sprite::SetLoopAnimationAll(bool bLoop) {
	for(auto& p: m_Animations){
		p.second.SetLoop(bLoop);
	}
}

//アニメーションの再生速度設定(現在再生中のアニメ）
void Sprite::SetAnimationSpeed(float AnimSpeed) {
	if (m_pPlayingAnimation == nullptr)
		return;
	m_pPlayingAnimation->SetAnimSpeed(AnimSpeed);
}

//アニメーションの再生速度設定(ID指定）
void Sprite::SetAnimationSpeed(int id, float AnimSpeed) {
	if (m_Animations.count(id) == 0)
		return;

	m_Animations[id].SetAnimSpeed(AnimSpeed);
}

//アニメーションの再生速度設定(アニメーション名指定）
void Sprite::SetAnimationSpeed(const char* name, float AnimSpeed) {

	int id = 0;
	if (!_findIdByName(name, &id))
		return;

	m_Animations[id].SetAnimSpeed(AnimSpeed);
}

void Sprite::SetAnimationSpeed(const string& name, float AnimSpeed) {
	SetAnimationSpeed(name.c_str(), AnimSpeed);
}

//すべてのアニメーションの再生速度設定
void Sprite::SetAnimationSpeedAll(float AnimSpeed) {
	for (auto& p : m_Animations) {
		p.second.SetAnimSpeed(AnimSpeed);
	}
}

//現在再生中のアニメの再生スピード取得
float Sprite::GetAnimationSpeed()const {
	if (m_pPlayingAnimation == nullptr)
		return 0.0f;
	return m_pPlayingAnimation->GetAnimSpeed();
}

//現在再生中のアニメのIDを取得(未再生だった場合0が戻る)
const int Sprite::GetAnimationId()const {
	if (m_pPlayingAnimation == nullptr)
		return 0;

	return m_PlayingAnimationId;
}

//現在再生中のアニメのアニメ名取得
const char* Sprite::GetAnimationName()const {
	if (m_pPlayingAnimation == nullptr)
		return nullptr;
	return m_pPlayingAnimation->GetName().c_str();
}

//アニメーションを再生しているか
bool Sprite::IsAnimationPlaying()const {
	if (m_pPlayingAnimation == nullptr)
		return false;
	return m_pPlayingAnimation->IsPlaying();
}

//アニメーションが一時停止しているか
bool Sprite::IsAnimationPausing()const {
	if (m_pPlayingAnimation == nullptr)
		return false;
	return m_pPlayingAnimation->IsPausing();
}

int Sprite::_findFreeId() const {

	int id = 0;
	int size = (int)m_Animations.size();

	for (int i = size; i > 0; --i) {
		if (m_Animations.count(i) > 0) {
			continue;
		}
		id = i;
		break;
	}

	return id;
}

//X軸フリップ
void Sprite::SetFlipX(bool isFlip) {
	m_isFlipX = isFlip;
}
bool Sprite::IsFlipX()const {
	return m_isFlipX;
}

//Y軸フリップ
void Sprite::SetFlipY(bool isFlip) {
	m_isFlipY = isFlip;
}
bool Sprite::IsFlipY()const {
	return m_isFlipY;
}

//頂点位置設定
void Sprite::SetVertexPosition(int index, float x, float y, float z) {
	XMFLOAT3 v(x, y, z);
	m_plane.SetVertexPosotion(index, v);
}

void Sprite::SetVertexPosition(int index, const Vector3f& pos) {
	XMFLOAT3 v(pos.x, pos.y, pos.z);
	m_plane.SetVertexPosotion(index, v);
}


bool Sprite::_findIdByName(const char* name, int* retVal)const {

	if (name == nullptr)
		return false;

	auto it = m_Animations.begin();

	while (it != m_Animations.end()) {

		const auto& keyVal = (*it);
		int id = keyVal.first;
		const auto& anim = keyVal.second;

		if (anim.GetName() == name) {
			*retVal = id;
			return true;
		}

		it++;
	}

	return false;
}

void Sprite::_applyAnimation()
{
	const AnimationParam& param = m_pPlayingAnimation->GetAnimParam();
	if (m_pPlayingAnimation->HasProperty(AnimationPropertyType::Rotation)) {
		SetRotation(param.rotDeg);
	}
	if (m_pPlayingAnimation->HasProperty(AnimationPropertyType::Scale)) {
		SetScale(param.scale[0], param.scale[1]);
	}
	if (m_pPlayingAnimation->HasProperty(AnimationPropertyType::UV)) {
		SetTexCoord(param.uv[0], param.uv[1], param.uv[2], param.uv[3]);
	}
	if (m_pPlayingAnimation->HasProperty(AnimationPropertyType::Alpha)) {
		SetAlpha(param.alpha);
	}
	if (m_pPlayingAnimation->HasProperty(AnimationPropertyType::Color)) {
		SetColor(param.color[0], param.color[1], param.color[2]);
	}
	if (m_pPlayingAnimation->HasProperty(AnimationPropertyType::Translate)) {
		m_TransOffsetMtx = XMMatrixTranslation(param.translate[0], param.translate[1], 0.0f);
	}
	if (m_pPlayingAnimation->HasProperty(AnimationPropertyType::Pivot)) {
		SetPivot(param.pivot[0], param.pivot[1]);
	}
	if (m_pPlayingAnimation->HasProperty(AnimationPropertyType::AddColor)) {
		SetAdditionalColor(param.addColor[0], param.addColor[1], param.addColor[2]);
	}
}

