#include "Fwk/Framework.h"

#include <stdlib.h>
#include <time.h>
#include <stdarg.h> 

#include "Lib/Util/Log.h"

#include "Fwk/Version.h"

namespace Fwk {

Framework* Framework::g_Instance = nullptr;

void Framework::GenerateInstance() {
	if (g_Instance != nullptr)
		return;

	g_Instance = new Framework();
}

Framework* Framework::GetInstance() {
	return g_Instance;
}

void Framework::ReleaseInstance() {
	if (g_Instance == nullptr)
		return;

	delete g_Instance;
	g_Instance = nullptr;
}


Framework::Framework()
	:
#ifdef _DEBUG
	bDisplayCollision(false)
#endif
{
    ;
}

Framework::~Framework()
{
    Term();
}

bool Framework::Init(const FrameworkDesc& desc) {

	Debug::Logf("実習用フレームワーク version:%d\n", FWK_VERSION);

	//乱数初期化
	srand((unsigned int)time(NULL));

	if (!m_Window.Init(
		desc.width,
		desc.height,
		desc.pClassName,
		desc.pWndTitle,
		desc.WndProc
	)) {
		return false;
	}

	//COMの初期化
	HRESULT result;
	result = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	m_Input.Init();
	m_Time.Init(desc.DeltaTimeMsMax);

	m_AssetManager.Init(
		desc.AssetRootDir,
		desc.AssetBufferSize
	);

	//フォントファイルのパスを作る
	wchar_t fontfilepath[256] = {};
	wcscpy_s(fontfilepath, 256, m_AssetManager.GetAssetsRoot().c_str());
	wcscat_s(fontfilepath, 256, desc.SpriteFontFile);

	//シェーダーのパスを作る
	std::map<string, wstring> ShadersPath;
	ShadersPath["VS_DEFUALT"] = m_AssetManager.GetAssetsRoot() + desc.ShaderFiles.at("VS_DEFUALT");
	ShadersPath["PS_NO_TEX_NO_LIT"] = m_AssetManager.GetAssetsRoot() + desc.ShaderFiles.at("PS_NO_TEX_NO_LIT");
	ShadersPath["PS_TEX_NO_LIT"] = m_AssetManager.GetAssetsRoot() + desc.ShaderFiles.at("PS_TEX_NO_LIT");

	m_RenderManager.Init(
		m_Window.GetHWND(),
		desc.width,
		desc.height,
		desc.RenderRequestBufferSize,
		desc.DescriptorSize_Resouce,
		desc.DescriptorSize_Sampler,
		fontfilepath,
		ShadersPath
	);

	m_FPS.Init(desc.TargetFPS, m_Time.GetTime(),&(m_RenderManager.m_SpriteFont));

	m_Audio.Init();

	//コリジョンマネージャの初期化
	m_CollisionManager.Init(desc.CollisionBufferSize);

	//タイマーの精度を上げる
	timeBeginPeriod(1);

	return true;
}

void Framework::Term()
{
	//タイマーの制度を元に戻す
	timeEndPeriod(1);

	m_CollisionManager.Term();

	m_Time.Term();
	m_Input.Term();

	m_Audio.Term();
	m_RenderManager.Term();
	m_AssetManager.Term();
	m_FPS.Term();

	CoUninitialize();

	m_Window.Term();
}

void Framework::Update()
{
	m_RenderManager.Update(m_Time.GetDeltaTime());
	m_Time.Update();
	m_FPS.Update(m_Time.GetTime());
	m_Input.Update();

#ifdef _DEBUG
	//コリジョン表示の切り替え
	if (m_Input.IsKeyDown(VK_F2)) {
		bDisplayCollision = !bDisplayCollision;
	}
#endif
}

void Framework::LateUpdate() {

	//更新処理が終わった後にコリジョンを処理する
	m_CollisionManager.ProcCollisions();

}

void Framework::Render()
{
	m_FPS.Draw();
#ifdef _DEBUG
	if (bDisplayCollision) {
		m_CollisionManager.DrawDebug();
	}
#endif
	m_RenderManager.Render();
}

void Framework::UpdatePostRender() {

	//ウィンドウの切り替え
	if (m_Input.IsKeyDown(VK_RETURN)
		&& (m_Input.IsKeyPressed(VK_MENU) || m_Input.IsKeyPressed(VK_RMENU) || m_Input.IsKeyPressed(VK_LMENU))
		){
		m_Window.ToggleFullscreen();
	}
}
void Framework::FrameSync()
{
	int waitTime =m_FPS.CalcWaitTime();
	//指定FPSになるよう待つ
	if (waitTime > 0) {
		Sleep(waitTime);
	}
}

void Framework::ShowFPS() {
	m_FPS.SetVisible(true);
}

void Framework::HideFPS() {
	m_FPS.SetVisible(false);
}

void Framework::SetFPSPosition(float x, float y) {
	m_FPS.SetDrawPosition(x, y);
}

//ウィンドウの中心点を原点とした時のカーソル位置を戻す
//ゲーム内座標ではなくウィンドウの内の位置なので注意
Vector2f Framework::GetMousePointInClient()const {
	return m_Window.TransformToClient(m_Input.GetMousePoint());
}

RenderManager* Framework::GetRenderManager() {
	return &m_RenderManager;
}

Fwk::Collision::CollisionManager* Framework::GetCollisionManager() {
	return &m_CollisionManager;
}

IInput* Framework::GetInput(){
	return &m_Input;
}

Time* Framework::GetTime() {
	return &m_Time;
}

Window* Framework::GetWindow() {
	return &m_Window;
}
Lib::Audio::Audio* Framework::GetAudio() {
	return &m_Audio;
}

AssetManager* Framework::GetAssetManager() {
	return &m_AssetManager;
}

}//namespace Fwk