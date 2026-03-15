#include "Fwk/Graphics/RenderManager.h"

#include <algorithm>

#include "Lib/Graphics/Graphics.h"
#include "Lib/LibDef.h"
#include "Lib/Util/Log.h"


#include "Fwk/Graphics/RenderObject.h"
#include "Fwk/Graphics/IDrawable.h"
#include "Fwk/Graphics/Camera.h"

using namespace DirectX;

#define PrintSuccess Debug::Log("　...成功\n");
#define PrintFailed Debug::Log("　...失敗\n");

static Vector3f vZero;

namespace Fwk {

bool RenderManager::cmpareDrawReq(RenderObject* p0, RenderObject* p1) {

	//順番が正しいならtrue、そうでなければfalseを返せ
	
	//DrawPass→Priorityの順にソート
	//3Dの描画物についてはZ値やマテリアルのソートも入れるべきだがそれはもう後々で。

	int pass0 = static_cast<int>(p0->GetRenderPass());
	int pass1 = static_cast<int>(p1->GetRenderPass());
	if (pass0  != pass1)
		return pass0 < pass1;

	return p0->GetPriority() < p1->GetPriority();
}

bool RenderManager::cmpareCamera(Camera* p0, Camera* p1) {
	return (p0)->GetOrder() < (p1)->GetOrder();
}

bool RenderManager::cmpareLayer(Layer* p0, Layer* p1) {
	return p0->order < p1->order;
}

RenderManager::RenderManager()
	: m_IsInitilized(false)
	, m_Layers()
	, m_RequestBufferSize(0)
	, m_Cameras()
	, m_UseTextNum(0)
	, m_TextColor()
	, m_TextScale(1.0f)
{
	m_TextColor[0] = 1.0f;
	m_TextColor[1] = 1.0f;
	m_TextColor[2] = 1.0f;
}
RenderManager::~RenderManager()
{
	;
}

void RenderManager::Init(
	HWND hwnd,
	uint32_t width, uint32_t height,
	uint32_t bufferSize,
	uint32_t DescriptorSize_Resource,
	uint32_t DescriptorSize_Sampler,
	const wchar_t* pFontPath,
	const std::map<string,wstring>& ShadersPath
){
	Term();

	if (bufferSize == 0)
		return;

	//初期化成功フラグ
	bool bSucceed = true;

	Debug::Logf(">>>>>>>> RenderManagerの初期化開始\n");

	Debug::Log("グラフィクスシステムの初期化");

	Graphics::InitializeParameters initParams = {};
	{
		initParams.hwnd = hwnd;
		initParams.NumDescriptors[HEAP_TYPE_RES] = DescriptorSize_Resource;
		initParams.NumDescriptors[HEAP_TYPE_SMP] = DescriptorSize_Sampler;
		//基本的にスワップチェインで作るバッファの数だけあればいい
		initParams.NumDescriptors[HEAP_TYPE_RTV] = Graphics::FrameCount;
		//1つしかつかわないので
		initParams.NumDescriptors[HEAP_TYPE_DSV] = 1;
	}

	if( m_Graphics.Init(initParams) ){
		PrintSuccess;
	}else {
		PrintFailed;
		bSucceed = false;
	}

	//カメラ
	{
		Camera::SetDefaultOrtho((float)width, (float)height, 0.0f, 100.0f);

		Camera* pMainCam = new Camera();
		pMainCam->SetPosition(0.0f, 0.0f);
		pMainCam->SetOrtho((float)width, (float)height, 0.0f, 100.0f);
		m_Cameras.push_back(pMainCam);
	}

	//描画レイヤー
	{
		m_RequestBufferSize = bufferSize;
		Layer* pLayer = new Layer;
		pLayer->requestBuffer.reserve(m_RequestBufferSize);
		pLayer->isActive = true;
		pLayer->order = 0;
		pLayer->name = "default";
		m_Layers.push_back(pLayer);
	}

	//シェーダの読み込み
	Debug::Logf("シェーダファイルの読み込み:%ls", ShadersPath.at("VS_DEFUALT").c_str());
	if (m_VtxShader[(int)VtxShader::Default].Init(ShadersPath.at("VS_DEFUALT").c_str())) {
		PrintSuccess;
	}else {
		PrintFailed;
		bSucceed = false;
	}

	Debug::Logf("シェーダファイルの読み込み:%ls", ShadersPath.at("PS_NO_TEX_NO_LIT").c_str());
	if (m_PixShader[(int)FragShader::NoTexNoLit].Init(ShadersPath.at("PS_NO_TEX_NO_LIT").c_str())) {
		PrintSuccess;
	}
	else {
		PrintFailed;
		bSucceed = false;
	}
	Debug::Logf("シェーダファイルの読み込み:%ls", ShadersPath.at("PS_TEX_NO_LIT").c_str());
	if (m_PixShader[(int)FragShader::TexNoLit].Init(ShadersPath.at("PS_TEX_NO_LIT").c_str())) {
		PrintSuccess;
	}
	else {
		PrintFailed;
		bSucceed = false;
	}

	//ルートシグネチャの作成（これはシェーダと関わっているので
	//シェーダの内容変わったら調整する必要がある）
	for (int i = 0; i < (int)PIPELINE::COUNT; ++i) {
		m_RootSignature[i].Init(
			m_Graphics.GetDevice(), D3D12_FILTER_MIN_MAG_MIP_POINT
		);
	}

	//パイプラインステートの作成

	//3D
	{
		//ブレンドステートの設定
		D3D12_BLEND_DESC descBS = {};
		{
			//レンダーターゲットのブレンド設定
			D3D12_RENDER_TARGET_BLEND_DESC descRTBS = {
				FALSE,FALSE,
				D3D12_BLEND_ONE,D3D12_BLEND_ZERO,D3D12_BLEND_OP_ADD,
				D3D12_BLEND_ONE,D3D12_BLEND_ZERO,D3D12_BLEND_OP_ADD,
				D3D12_LOGIC_OP_NOOP,
				D3D12_COLOR_WRITE_ENABLE_ALL
			};

			descBS.AlphaToCoverageEnable = FALSE;
			descBS.IndependentBlendEnable = FALSE;
			for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
				descBS.RenderTarget[i] = descRTBS;
			}
		}

		//深度ステンシルステートの設定
		D3D12_DEPTH_STENCIL_DESC descDSS = {};
		{
			descDSS.DepthEnable = true;
			descDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
			descDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
			descDSS.StencilEnable = false;
		}

		m_PipelineState[(int)PIPELINE::DEFAULT].Init(
			m_Graphics.GetDevice(),
			m_RootSignature[(int)PIPELINE::DEFAULT].Get(),
			&m_VtxShader[(int)VtxShader::Default],
			&m_PixShader[(int)FragShader::TexNoLit],
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			descBS,
			descDSS
		);
	}

	//2D
	
	//深度ステンシルステートの設定
	{
		D3D12_DEPTH_STENCIL_DESC descDSS_2D = {};
		{
			descDSS_2D.DepthEnable = false;
			descDSS_2D.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
			descDSS_2D.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;
			descDSS_2D.StencilEnable = false;
		};

		//--------------------------------------------------------------
		//2Dデフォルトのレンダーターゲットのブレンド設定
		//--------------------------------------------------------------
		D3D12_RENDER_TARGET_BLEND_DESC descRTBS_Default = {
			TRUE,
			FALSE,

			D3D12_BLEND_SRC_ALPHA,
			D3D12_BLEND_INV_SRC_ALPHA,
			D3D12_BLEND_OP_ADD,

			D3D12_BLEND_ONE,
			D3D12_BLEND_ZERO,
			D3D12_BLEND_OP_ADD,

			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL
		};

		//2Dデフォルトのブレンドステート
		D3D12_BLEND_DESC descBS_Default = {};
		{
			descBS_Default.AlphaToCoverageEnable = FALSE;
			descBS_Default.IndependentBlendEnable = FALSE;
			for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
				descBS_Default.RenderTarget[i] = descRTBS_Default;
			}
		}

		//--------------------------------------------------------------
		//2D_BLEND_ADD
		//--------------------------------------------------------------
		D3D12_RENDER_TARGET_BLEND_DESC descRTBS_Add = descRTBS_Default;
		descRTBS_Add.SrcBlend  = D3D12_BLEND_SRC_ALPHA;
		descRTBS_Add.DestBlend = D3D12_BLEND_ONE;

		//--------------------------------------------------------------
		//2D_BLEND_SUB
		//--------------------------------------------------------------
		D3D12_RENDER_TARGET_BLEND_DESC descRTBS_Sub = descRTBS_Default;
		descRTBS_Sub.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		descRTBS_Sub.DestBlend = D3D12_BLEND_ONE;
		descRTBS_Sub.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;

		//--------------------------------------------------------------
		//2D_BLEND_OPAQUE
		//--------------------------------------------------------------
		D3D12_RENDER_TARGET_BLEND_DESC descRTBS_Opaque = descRTBS_Default;
		descRTBS_Opaque.BlendEnable = FALSE;
		descRTBS_Opaque.SrcBlend = D3D12_BLEND_ONE;
		descRTBS_Opaque.DestBlend = D3D12_BLEND_ZERO;

		//--------------------------------------------------------------
		//2D_BLEND_INVERSE
		//--------------------------------------------------------------
		D3D12_RENDER_TARGET_BLEND_DESC descRTBS_Inverse = descRTBS_Default;
		descRTBS_Inverse.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		descRTBS_Inverse.DestBlend = D3D12_BLEND_ZERO;
		descRTBS_Inverse.BlendOp = D3D12_BLEND_OP_ADD;

		//--------------------------------------------------------------
		D3D12_BLEND_DESC descBS_Add = descBS_Default;
		D3D12_BLEND_DESC descBS_Sub = descBS_Default;
		D3D12_BLEND_DESC descBS_Opaque = descBS_Default;
		D3D12_BLEND_DESC descBS_Inverse = descBS_Default;

		for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
			descBS_Add.RenderTarget[i] = descRTBS_Add;
			descBS_Sub.RenderTarget[i] = descRTBS_Sub;
			descBS_Opaque.RenderTarget[i] = descRTBS_Opaque;
			descBS_Inverse.RenderTarget[i] = descRTBS_Inverse;
		}
		//--------------------------------------------------------------
		//TexNoLit パイプライン
		//--------------------------------------------------------------

		//デフォルト2D描画
		m_PipelineState[(int)PIPELINE::TRANSPARENT_2D].Init(
			m_Graphics.GetDevice(),
			m_RootSignature[(int)PIPELINE::TRANSPARENT_2D].Get(),
			&m_VtxShader[(int)VtxShader::Default],
			&m_PixShader[(int)FragShader::TexNoLit],
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			descBS_Default,
			descDSS_2D
		);

		//加算ブレンド2D
		m_PipelineState[(int)PIPELINE::ADD_2D].Init(
			m_Graphics.GetDevice(),
			m_RootSignature[(int)PIPELINE::ADD_2D].Get(),
			&m_VtxShader[(int)VtxShader::Default],
			&m_PixShader[(int)FragShader::TexNoLit],
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			descBS_Add,
			descDSS_2D
		);

		//減算ブレンド2D
		m_PipelineState[(int)PIPELINE::SUB_2D].Init(
			m_Graphics.GetDevice(),
			m_RootSignature[(int)PIPELINE::SUB_2D].Get(),
			&m_VtxShader[(int)VtxShader::Default],
			&m_PixShader[(int)FragShader::TexNoLit],
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			descBS_Sub,
			descDSS_2D
		);

		//不透明
		m_PipelineState[(int)PIPELINE::OPAQUE_2D].Init(
			m_Graphics.GetDevice(),
			m_RootSignature[(int)PIPELINE::OPAQUE_2D].Get(),
			&m_VtxShader[(int)VtxShader::Default],
			&m_PixShader[(int)FragShader::TexNoLit],
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			descBS_Opaque,
			descDSS_2D
		);

		//反転
		m_PipelineState[(int)PIPELINE::INVERSE_2D].Init(
			m_Graphics.GetDevice(),
			m_RootSignature[(int)PIPELINE::INVERSE_2D].Get(),
			&m_VtxShader[(int)VtxShader::Default],
			&m_PixShader[(int)FragShader::TexNoLit],
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			descBS_Inverse,
			descDSS_2D
		);

		//--------------------------------------------------------------
		//NoTexNoLit パイプライン
		//--------------------------------------------------------------

		//デフォルト2D描画
		m_PipelineState[(int)PIPELINE::TRANSPARENT_2D_NoTex].Init(
			m_Graphics.GetDevice(),
			m_RootSignature[(int)PIPELINE::TRANSPARENT_2D_NoTex].Get(),
			&m_VtxShader[(int)VtxShader::Default],
			&m_PixShader[(int)FragShader::NoTexNoLit],
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			descBS_Default,
			descDSS_2D
		);

		//加算ブレンド2D
		m_PipelineState[(int)PIPELINE::ADD_2D_NoTex].Init(
			m_Graphics.GetDevice(),
			m_RootSignature[(int)PIPELINE::ADD_2D_NoTex].Get(),
			&m_VtxShader[(int)VtxShader::Default],
			&m_PixShader[(int)FragShader::NoTexNoLit],
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			descBS_Add,
			descDSS_2D
		);

		//減算ブレンド2D
		m_PipelineState[(int)PIPELINE::SUB_2D_NoTex].Init(
			m_Graphics.GetDevice(),
			m_RootSignature[(int)PIPELINE::SUB_2D_NoTex].Get(),
			&m_VtxShader[(int)VtxShader::Default],
			&m_PixShader[(int)FragShader::NoTexNoLit],
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			descBS_Sub,
			descDSS_2D
		);

		//不透明
		m_PipelineState[(int)PIPELINE::OPAQUE_2D_NoTex].Init(
			m_Graphics.GetDevice(),
			m_RootSignature[(int)PIPELINE::OPAQUE_2D_NoTex].Get(),
			&m_VtxShader[(int)VtxShader::Default],
			&m_PixShader[(int)FragShader::NoTexNoLit],
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			descBS_Opaque,
			descDSS_2D
		);

		//反転
		m_PipelineState[(int)PIPELINE::INVERSE_2D_NoTex].Init(
			m_Graphics.GetDevice(),
			m_RootSignature[(int)PIPELINE::INVERSE_2D_NoTex].Get(),
			&m_VtxShader[(int)VtxShader::Default],
			&m_PixShader[(int)FragShader::NoTexNoLit],
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			descBS_Inverse,
			descDSS_2D
		);
		//--------------------------------------------------------------
		//LineSegments パイプライン
		//--------------------------------------------------------------

		m_PipelineState[(int)PIPELINE::LINE_SEGMENTS].Init(
			m_Graphics.GetDevice(),
			m_RootSignature[(int)PIPELINE::TRANSPARENT_2D_NoTex].Get(),
			&m_VtxShader[(int)VtxShader::Default],
			&m_PixShader[(int)FragShader::NoTexNoLit],
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
			descBS_Default,
			descDSS_2D
		);
	}

	Debug::Logf("フォントファイルの読み込み:%ls",pFontPath);
	if(m_SpriteFont.Init(&m_Graphics, pFontPath)){
		PrintSuccess;
	}else {
		PrintFailed;
		bSucceed = false;
	}

	//フォント
	{
		for (int i = 0; i < TEXT_BUFFER_MAX; ++i) {
			m_Text[i].Init(&m_SpriteFont);
		}
	}

#if defined(DEBUG) || defined(_DEBUG)
	mDebugLog.Init(&m_SpriteFont);
#endif

	Debug::Logf("<<<<<<<< RenderManagerの初期化終了");
	if(bSucceed){
			PrintSuccess;
	}else {
		PrintFailed;
		bSucceed = false;
	}
	Debug::Logf("\n");

	//初期化済フラグ設定
	m_IsInitilized = bSucceed;
}

void RenderManager::Update(float deltaTime) {
	m_UseTextNum = 0;

#if defined(DEBUG) || defined(_DEBUG)
	mDebugLog.Update(deltaTime);
#endif
}

void RenderManager::Term()
{
	for (int i = 0; i < (int)PIPELINE::COUNT; ++i) {
		m_RootSignature[i].Term();
		m_PipelineState[i].Term();
	}

	for (int i = 0; i < (int)VtxShader::COUNT; ++i) {
		m_VtxShader[i].Term();
	}
	for (int i = 0; i < (int)FragShader::COUNT; ++i) {
		m_PixShader[i].Term();
	}

	m_SpriteFont.Term();

	for (int i = 0; i < m_Layers.size(); ++i) {
		SAFE_DELETE(m_Layers[i]);
	}
	m_Layers.clear();

	for (int i = 0; i < m_Cameras.size(); ++i) {
		SAFE_DELETE(m_Cameras[i]);
	}
	m_Cameras.clear();
	
#if defined(DEBUG) || defined(_DEBUG)
	mDebugLog.Term();
#endif

	for (int i = 0; i < TEXT_BUFFER_MAX; ++i) {
		m_Text[i].Term();
	}
	
	m_Graphics.Term();

	m_IsInitilized = false;
}

void RenderManager::RequestRender(RenderObject* pDrawable)
{
	if (pDrawable == nullptr)
		return;

	Layer* pLayer = _findLayer(pDrawable->GetRenderLayer());

	if (pLayer==nullptr) {
		return;
	}

	if (pLayer->requestBuffer.capacity() <= pLayer->requestBuffer.size())
		return;

	pLayer->requestBuffer.push_back(pDrawable);
}

void RenderManager::Render(){

	if (!m_IsInitilized) {
		Debug::Log("エラー：未初期化のRenderManagerに対して描画命令を要求しました。\n");
		return;
	}

	//テキストフォント
	for (int i = 0; i < m_UseTextNum; ++i) {
		RequestRender(&m_Text[i]);
	}

	//デバッグログ
#if defined(DEBUG) || defined(_DEBUG)
	mDebugLog.Draw();
#endif

	m_Graphics.BeginRender();

	//各レイヤーの描画リクエスト並べ替え
	for (int i = 0; i < m_Layers.size(); ++i) 
	{
		std::vector<RenderObject*>* reqBuffer = &(m_Layers[i]->requestBuffer);
		std::sort(
			reqBuffer->begin(),
			reqBuffer->end(),
			cmpareDrawReq
		);
	}

	//カメラの順番整列
	std::sort(
		m_Cameras.begin(),
		m_Cameras.end(),
		cmpareCamera
	);

	const uint32_t frameIndex = m_Graphics.GetFrameIndex();
	ID3D12GraphicsCommandList* pCommandList = m_Graphics.GetCommandList()->Get();

	//描画処理
	ID3D12DescriptorHeap* const pHeaps[] = {
		m_Graphics.GetDescriptorHeap(Lib::Graphics::HEAP_TYPE_RES)->Get()
	};
	pCommandList->SetDescriptorHeaps(1, pHeaps);

	pCommandList->RSSetViewports(1, &m_Graphics.GetViewPort());
	pCommandList->RSSetScissorRects(1, &m_Graphics.GetScissorRect());

	RENDER_PASS currentPass = RENDER_PASS::None;
	RENDER_MODE currentMode = RENDER_MODE::None;

	PIPELINE currentPipeline = PIPELINE::NONE;
	PIPELINE currentSignature = PIPELINE::NONE;

	for (auto cam = m_Cameras.begin(); cam != m_Cameras.end(); ++cam) {
	
		if (!(*cam)->IsActive()) {
			continue;
		}

		bool isMainCam = (*cam)->GetName()._Equal("main");

		for (auto layer = m_Layers.begin(); layer != m_Layers.end(); ++layer)
		{
			if (!(*layer)->isActive) {
				continue;
			}

			//メインカメラはデフォルトレイヤーを無条件で描画対象とする
			if (!(isMainCam && (*layer)->name._Equal("default")))
			{
				//それ以外はカメラの描画対象となっているか調べる
				if (!(*cam)->ContainsRenderLayer((*layer)->name)) {
					continue;
				}
			}

			std::vector<RenderObject*>& RequestBuffer = (*layer)->requestBuffer;

			for (auto renderObj = RequestBuffer.begin(); renderObj != RequestBuffer.end(); ++renderObj) {

				RenderObject* pRenderObject = *renderObj;

				if (!pRenderObject->IsVisible())
					continue;

				RENDER_PASS pass = pRenderObject->GetRenderPass();
				RENDER_MODE mode = pRenderObject->GetRenderMode();

				if (pass != currentPass) {
					_onRenderPassChanged(pass, currentPass, &m_Graphics);
				}

				if (pass != currentPass || mode != currentMode) 
				{
					PIPELINE pipeline = _getPipelineState(pass, mode);
					PIPELINE signature = pipeline;

					if (pipeline != currentPipeline)
					{
						if (pipeline != PIPELINE::NONE) {
							pCommandList->SetPipelineState(m_PipelineState[(int)pipeline].Get());
						}
						currentPipeline = pipeline;
					}

					if (signature != currentSignature)
					{
						if (signature != PIPELINE::NONE) {
							pCommandList->SetGraphicsRootSignature(m_RootSignature[(int)signature].Get());
						}
						currentSignature = signature;
					}

					currentPass = pass;
					currentMode = mode;
				}

				pRenderObject->OnDraw(&m_Graphics, *cam);
			}
		}
	}

	_onRenderPassChanged(RENDER_PASS::None, currentPass, &m_Graphics);

	m_Graphics.EndRender();

	_postPresent();	
}

void RenderManager::CreateConstantBuffer(Lib::Graphics::ConstantBuffer& ConstBufferRef) {

	ConstBufferRef.Init(
		m_Graphics.GetDevice(),
		m_Graphics.GetDescriptorHeap(Lib::Graphics::HEAP_TYPE_RES),
		sizeof(CB_Transform)
	);

	//変換行列の設定
	CB_Transform* p = (CB_Transform*)(ConstBufferRef.GetPtr());
	p->World = XMMatrixIdentity();

	//3D用
	/* {
		auto eyePos = XMVectorSet(0.0f, 0.0f, -3.0f, 0.0f);
		auto targetPos = XMVectorZero();
		auto upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		auto fovY = XMConvertToRadians(37.5f);
		auto aspect = static_cast<float>(m_Width) / static_cast<float>(m_Height);

		p->View = XMMatrixLookAtLH(eyePos, targetPos, upward);
		p->Proj = XMMatrixPerspectiveFovLH(fovY, aspect, 1.0f, 1000.0f);
	}*/

	//2D用
	{
		Camera* pCam = _findCamera("main");
		p->View = pCam->GetViewMatrix();
		p->Proj = pCam->GetProjectionMatrix();
	}
}

IGraphics* RenderManager::GetGraphics() {
	return &m_Graphics;
}

void RenderManager::SetClearColor(float r, float g, float b){
	m_Graphics.SetClearColor(r, g, b);
}

void RenderManager::SetClearColor(const float rgb[]) {
	m_Graphics.SetClearColor(rgb[0], rgb[1], rgb[2]);
}

void RenderManager::SetClearColor(const DirectX::XMVECTORF32& _color) {
	m_Graphics.SetClearColor(_color.f[0], _color.f[1], _color.f[2]);
}

void RenderManager::AddLayer(const std::string& name) {

	if (_findLayer(name) != nullptr) {
		return;
	}
	
	Layer* pLayer = new Layer();
	pLayer->requestBuffer.reserve(m_RequestBufferSize);
	pLayer->isActive = true;
	pLayer->name = name;

	int maxOrder = m_Layers[0]->order;
	for (int i = 0; i < m_Layers.size(); ++i) {
		if (maxOrder < m_Layers[i]->order) {
			maxOrder = m_Layers[i]->order;
		}
	}

	pLayer->order = maxOrder + 1;

	m_Layers.push_back(pLayer);
}

void RenderManager::RemoveLayer(const std::string& name){

	//デフォルトレイヤーは消せない
	if (name._Equal("default")) {
		return;
	}

	int index = _getLayerIndex(name);

	if (index < 0) {
		return;
	}

	SAFE_DELETE(m_Layers[index]);
	m_Layers.erase(m_Layers.begin() +index);
}

void RenderManager::SetLayerOrder(const std::string& name, int order) {
	
	Layer* pLayer = _findLayer(name);
	
	if (pLayer == nullptr) {
		return;
	}

	pLayer->order = order;

	//レイヤーの順番整列
	std::sort(
		m_Layers.begin(),
		m_Layers.end(),
		cmpareLayer
	);
}

void RenderManager::SetLayerActive(const std::string& name, bool isActive) {
	
	Layer* pLayer = _findLayer(name);
	
	if (pLayer == nullptr) {
		return;
	}

	pLayer->isActive = isActive;
}

void RenderManager::RemoveCamera(const std::string& name) {

	//メインカメラは消せない
	if (name._Equal("main")) {
		return;
	}

	int index = _getCameraIndex(name);

	if (index < 0) {
		return;
	}

	SAFE_DELETE(m_Cameras[index]);
	m_Cameras.erase(m_Cameras.begin() + index);
}

void RenderManager::SetCamera(const Camera& camera) {

	int index = _getCameraIndex(camera.GetName());

	if (index < 0) {
		Camera* pCam = new Camera();
		m_Cameras.push_back(pCam);
		index = (int)m_Cameras.size() - 1;
	}

	*m_Cameras[index] = camera;
}

Camera& RenderManager::GetCamera() {
	return GetCamera("main");
}

Camera& RenderManager::GetCamera(const std::string& name) {
	Camera* pCam = _findCamera(name);
	if (pCam == nullptr) {
		//見つからなかったらとりあえず先頭のカメラ(main);
		return *m_Cameras[0];
	}
	return *pCam;
}

//-----------------------------------------
//デフォルトカメラの操作
//-----------------------------------------

//位置
void RenderManager::SetCameraPosition(const Vector2f& position) {
	SetCameraPosition("main", position);
}
void RenderManager::SetCameraPosition(const Vector3f& position) {
	SetCameraPosition("main", position);
}
Vector3f RenderManager::GetCameraPosition()const {
	return GetCameraPosition("main");
}

//オーダー
void RenderManager::SetCameraOrder(int order) {
	SetCameraOrder("main", order);
}
int RenderManager::GetCameraOrder()const {
	return 	GetCameraOrder("main");
}

//アクティブ
void RenderManager::SetCameraActive(bool isActive) {
	SetCameraActive("main", isActive);
}
bool RenderManager::IsCameraActive()const {
	return IsCameraActive("main");
}

//注視点
void RenderManager::SetCameraFocus(const Vector3f& focus, const FocusMode focusMode) {
	SetCameraFocus("main", focus,focusMode);
}
FocusMode RenderManager::GetCameraFocusMode()const {
	return GetCameraFocusMode("main");
}
const Vector3f& RenderManager::GetCameraFocus()const {
	return GetCameraFocus("main");
}

//ビュー行列
DirectX::XMMATRIX RenderManager::GetCameraViewMatrix()const {
	return GetCameraViewMatrix("main");
}
//プロジェクション行列
void RenderManager::SetCameraPerspective(float viewWidth, float viewHeight, float nearZ, float farZ, float fov) {
	SetCameraPerspective("main",viewWidth, viewHeight, nearZ, farZ, fov);
}
void RenderManager::SetCameraOrtho(float viewWidth, float viewHeight, float nearZ, float farZ) {
	SetCameraOrtho("main", viewWidth, viewHeight, nearZ, farZ);
}
DirectX::XMMATRIX RenderManager::GetCameraProjectionMatrix()const {
	return GetCameraProjectionMatrix("main");
}

//カメラの描画対象レイヤーの操作
void RenderManager::AddCameraRenderLayer(const std::string& targetLayerName) {
	AddCameraRenderLayer("main", targetLayerName);
}
void RenderManager::RemoveCameraRenderLayer(const std::string& targetLayerName) {
	RemoveCameraRenderLayer("main", targetLayerName);
}

//-----------------------------------------
//カメラ名を指定して操作
//-----------------------------------------

//位置
void RenderManager::SetCameraPosition(const std::string& name, const Vector2f& position) {
	Camera* pCam = _findCamera(name);
	if (pCam == nullptr) {
		return;
	}
	pCam->SetPosition(position);
}
void RenderManager::SetCameraPosition(const std::string& name, const Vector3f& position) {
	Camera* pCam = _findCamera(name);
	if (pCam == nullptr) {
		return;
	}
	pCam->SetPosition(position);
}
Vector3f RenderManager::GetCameraPosition(const std::string& name)const {
	const Camera* pCam = _findCameraConst(name);
	if (pCam == nullptr) {
		return vZero;
	}
	return pCam->GetPosition();
}

//オーダー
void RenderManager::SetCameraOrder(const std::string& name, int order) {
	Camera* pCam = _findCamera(name);
	if (pCam == nullptr) {
		return ;
	}
	pCam->SetOrder(order);
}
int RenderManager::GetCameraOrder(const std::string& name)const
{
	const Camera* pCam = _findCameraConst(name);
	if (pCam == nullptr) {
		return 0;
	}
	return pCam->GetOrder();
}

//アクティブ
void RenderManager::SetCameraActive(const std::string& name, bool isActive) {
	Camera* pCam = _findCamera(name);
	if (pCam == nullptr) {
		return ;
	}
	pCam->SetActive(isActive);
}
bool RenderManager::IsCameraActive(const std::string& name)const {
	const Camera* pCam = _findCameraConst(name);
	if (pCam == nullptr) {
		return false;
	}
	return pCam->IsActive();
}

//注視点
void RenderManager::SetCameraFocus(const std::string& name, const Vector3f& focus, const FocusMode focusMode) {
	Camera* pCam = _findCamera(name);
	if (pCam == nullptr) {
		return;
	}
	pCam->SetFocus(focus,focusMode);
}
FocusMode RenderManager::GetCameraFocusMode(const std::string& name)const
{
	const Camera* pCam = _findCameraConst(name);
	if (pCam == nullptr) {
		return FocusMode::None;
	}
	return pCam->GetFocusMode();
}
const Vector3f& RenderManager::GetCameraFocus(const std::string& name)const
{
	const Camera* pCam = _findCameraConst(name);
	if (pCam == nullptr) {
		return vZero;
	}
	return pCam->GetFocus();
}

//ビュー行列
DirectX::XMMATRIX RenderManager::GetCameraViewMatrix(const std::string& name)const {
	const Camera* pCam = _findCameraConst(name);
	if (pCam == nullptr) {
		return DirectX::XMMatrixIdentity();
	}
	return pCam->GetViewMatrix();
}

//プロジェクション行列
void RenderManager::SetCameraPerspective(const std::string& name, float viewWidth, float viewHeight, float nearZ, float farZ, float fov)
{
	Camera* pCam = _findCamera(name);
	if (pCam == nullptr) {
		return;
	}
	pCam->SetPerspective(viewWidth, viewHeight,nearZ,farZ,fov);
}
void RenderManager::SetCameraOrtho(const std::string& name, float viewWidth, float viewHeight, float nearZ, float farZ)
{
	Camera* pCam = _findCamera(name);
	if (pCam == nullptr) {
		return;
	}
	pCam->SetOrtho(viewWidth, viewHeight, nearZ, farZ);
}

DirectX::XMMATRIX RenderManager::GetCameraProjectionMatrix(const std::string& name)const
{
	const Camera* pCam = _findCameraConst(name);
	if (pCam == nullptr) {
		return DirectX::XMMatrixIdentity();;
	}
	return pCam->GetProjectionMatrix();
}


//カメラの描画対象レイヤーの操作
void RenderManager::AddCameraRenderLayer(const std::string& cameraName, const std::string& targetLayerName) {
	Camera* pCam = _findCamera(cameraName);
	if (pCam == nullptr) {
		return ;
	}
	pCam->AddRenderLayer(targetLayerName);
}

void RenderManager::RemoveCameraRenderLayer(const std::string& cameraName, const std::string& targetLayerName) {
	Camera* pCam = _findCamera(cameraName);
	if (pCam == nullptr) {
		return ;
	}
	pCam->RemoveRenderLayer(targetLayerName);
}

void RenderManager::_postPresent() {
	m_SpriteFont.PostPresent(&m_Graphics);
	_clearRequest();
}

void RenderManager::_clearRequest() 
{
	for (int i = 0; i < m_Layers.size(); ++i) 
	{
		m_Layers[i]->requestBuffer.clear();
	}
}

void RenderManager::SyncGpuProc() {
	m_Graphics.SyncGpuProc();
}
/*
RenderManager::ROOT_SIGNATURE RenderManager::_getRootSignature(RENDER_PASS pass, RENDER_MODE mode)
{
	const std::map< RENDER_MODE, ROOT_SIGNATURE> m = {
		 { RENDER_MODE::Texture_Opaque,ROOT_SIGNATURE::SPRITE_2D}
		,{ RENDER_MODE::Texture_Transparent,ROOT_SIGNATURE::SPRITE_2D }
		,{ RENDER_MODE::Texture_Blend_Add,ROOT_SIGNATURE::SPRITE_2D}
		,{ RENDER_MODE::Texture_Blend_Sub,ROOT_SIGNATURE::SPRITE_2D }
		,{ RENDER_MODE::Texture_Blend_Inverse,ROOT_SIGNATURE::SPRITE_2D }
		,{ RENDER_MODE::Color_Opaque,ROOT_SIGNATURE::SPRITE_2D_NoTex }
		,{ RENDER_MODE::Color_Transparent,ROOT_SIGNATURE::SPRITE_2D_NoTex }
		,{ RENDER_MODE::Color_Blend_Add,ROOT_SIGNATURE::SPRITE_2D_NoTex}
		,{ RENDER_MODE::Color_Blend_Sub,ROOT_SIGNATURE::SPRITE_2D_NoTex }
		,{ RENDER_MODE::Color_Blend_Inverse,ROOT_SIGNATURE::SPRITE_2D_NoTex }
	};

	switch (pass)
	{
	case RENDER_PASS::Mesh_Opaque:
		return ROOT_SIGNATURE::DEFAULT;
	case RENDER_PASS::Mesh_Transparent:
		return ROOT_SIGNATURE::DEFAULT;
	case RENDER_PASS::Sprite:
	{
		if (m.count(mode) != 0) {
			return m.find(mode)->second;
		}
		return ROOT_SIGNATURE::SPRITE_2D_NoTex;
	}
	break;
	case RENDER_PASS::SpriteFont:
		return ROOT_SIGNATURE::NONE;
	}

	return ROOT_SIGNATURE::NONE;
}
*/
RenderManager::PIPELINE RenderManager::_getPipelineState(RENDER_PASS pass, RENDER_MODE mode)
{
	const std::map< RENDER_MODE, PIPELINE> m = {
		 { RENDER_MODE::Texture_Opaque,PIPELINE::OPAQUE_2D }
		,{ RENDER_MODE::Texture_Transparent,PIPELINE::TRANSPARENT_2D }
		,{ RENDER_MODE::Texture_Blend_Add,PIPELINE::ADD_2D }
		,{ RENDER_MODE::Texture_Blend_Sub,PIPELINE::SUB_2D }
		,{ RENDER_MODE::Texture_Blend_Inverse,PIPELINE::INVERSE_2D }
		,{ RENDER_MODE::Color_Opaque,PIPELINE::OPAQUE_2D_NoTex }
		,{ RENDER_MODE::Color_Transparent,PIPELINE::TRANSPARENT_2D_NoTex }
		,{ RENDER_MODE::Color_Blend_Add,PIPELINE::ADD_2D_NoTex }
		,{ RENDER_MODE::Color_Blend_Sub,PIPELINE::SUB_2D_NoTex }
		,{ RENDER_MODE::Color_Blend_Inverse,PIPELINE::INVERSE_2D_NoTex }
		,{ RENDER_MODE::LineSegments,PIPELINE::LINE_SEGMENTS }
	}; 

	switch (pass)
	{
	case RENDER_PASS::Mesh_Opaque:
		return PIPELINE::DEFAULT;
	case RENDER_PASS::Mesh_Transparent:
		return PIPELINE::DEFAULT;
	case RENDER_PASS::Sprite:
	{
		if (m.count(mode) != 0) {
			return m.find(mode)->second;
		}
		return PIPELINE::OPAQUE_2D_NoTex;
	}
	case RENDER_PASS::SpriteFont:
		return PIPELINE::NONE;
	}

	return PIPELINE::NONE;
}

void RenderManager::_onRenderPassChanged(RENDER_PASS pass, RENDER_PASS prevPass, Lib::Graphics::IGraphics* g) {

	if (pass == RENDER_PASS::SpriteFont) {
		m_SpriteFont.BeginRender(g);
	}
	if (prevPass == RENDER_PASS::SpriteFont) {
		m_SpriteFont.EndRender();

		ID3D12GraphicsCommandList* pCommandList = m_Graphics.GetCommandList()->Get();

		ID3D12DescriptorHeap* const pHeaps[] = {
			m_Graphics.GetDescriptorHeap(Lib::Graphics::HEAP_TYPE_RES)->Get()
		};
		pCommandList->SetDescriptorHeaps(1, pHeaps);
	}
}

Camera* RenderManager::_findCamera(const string& name) {
	for (int i = 0; i < m_Cameras.size(); ++i) {
		if (name._Equal(m_Cameras[i]->GetName())) {
			return m_Cameras[i];
		}
	}
	return nullptr;
}

const Camera* RenderManager::_findCameraConst(const string& name)const {
	for (int i = 0; i < m_Cameras.size(); ++i) {
		if (name._Equal(m_Cameras[i]->GetName())) {
			return m_Cameras[i];
		}
	}
	return nullptr;
}

int RenderManager::_getCameraIndex(const string& name) {
	for (int i = 0; i < m_Cameras.size(); ++i) {
		if (name._Equal(m_Cameras[i]->GetName())) {
			return i;
		}
	}
	return -1;
}

RenderManager::Layer* RenderManager::_findLayer(const string& name) {
	for (int i = 0; i < m_Layers.size(); ++i) {
		if (name._Equal(m_Layers[i]->name)) {
			return m_Layers[i];
		}
	}
	return nullptr;
}

int RenderManager::_getLayerIndex(const string& name) {
	for (int i = 0; i < m_Layers.size(); ++i) {
		if (name._Equal(m_Layers[i]->name)) {
			return i;
		}
	}
	return -1;
}

void RenderManager::PrintText(const wchar_t* pText)
{
	_PrintTextImpl(pText, 0.0f, 0.0f, m_TextColor, m_TextScale);
}
void RenderManager::PrintText(const wchar_t* pText, float pos_x, float pos_y)
{
	_PrintTextImpl(pText, pos_x, pos_y, m_TextColor, m_TextScale);
}
void RenderManager::_PrintTextImpl(const wchar_t* pText, float pos_x, float pos_y, const float rgb[], float scale)
{
	if (m_UseTextNum >= TEXT_BUFFER_MAX)
		return;

	m_Text[m_UseTextNum].SetText(pText);
	m_Text[m_UseTextNum].SetPosition(pos_x, pos_y);
	m_Text[m_UseTextNum].SetColor(rgb[0], rgb[1], rgb[2]);
	m_Text[m_UseTextNum].SetScale(scale, scale);

	++m_UseTextNum;
}

void RenderManager::PrintText(const char* pText)
{
	_PrintTextImpl(pText, 0.0f, 0.0f, m_TextColor, m_TextScale);
}

void RenderManager::PrintText(const char* pText, float pos_x, float pos_y)
{
	_PrintTextImpl(pText, pos_x, pos_y, m_TextColor, m_TextScale);
}

void RenderManager::_PrintTextImpl(const char* pText, float pos_x, float pos_y, const float rgb[], float scale)
{
	if (m_UseTextNum >= TEXT_BUFFER_MAX)
		return;

	m_Text[m_UseTextNum].SetText(pText);
	m_Text[m_UseTextNum].SetPosition(pos_x, pos_y);
	m_Text[m_UseTextNum].SetColor(rgb[0], rgb[1], rgb[2]);
	m_Text[m_UseTextNum].SetScale(scale, scale);

	++m_UseTextNum;
}

void RenderManager::PrintText(const string& pText) {
	PrintText(pText.c_str(), 0.0f, 0.0f);
}
void RenderManager::PrintText(const string& pText, float pos_x, float pos_y) {
	PrintText(pText.c_str(), pos_x, pos_y);
}

void RenderManager::SetTextColor(float r, float g, float b) {
	m_TextColor[0] = r;
	m_TextColor[1] = g;
	m_TextColor[2] = b;
}

void RenderManager::SetTextColor(const float rgb[]) {
	m_TextColor[0] = rgb[0];
	m_TextColor[1] = rgb[1];
	m_TextColor[2] = rgb[2];
}

void RenderManager::SetTextColor(const DirectX::XMVECTORF32& _color) {
	m_TextColor[0] = _color.f[0];
	m_TextColor[1] = _color.f[1];
	m_TextColor[2] = _color.f[2];
}

void RenderManager::SetTextSize(float scale) {
	//オリジナルのサイズが24.0なので
	m_TextScale = scale / 24.0f;
}

void RenderManager::DebugLogImpl(const char* format, ...)
{
#if defined(DEBUG) || defined(_DEBUG)
	va_list args;
	va_start(args, format);
	mDebugLog.Print(format, args);
	va_end(args);   // (9)
#endif
}

void RenderManager::DebugLogImpl(const string& format, ...) {
#if defined(DEBUG) || defined(_DEBUG)
	mDebugLog.Print(format.c_str());
#endif
}

}