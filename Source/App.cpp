#include "App.h"

#include "GameObjectMng/GameObjectMng.h"

#include "Scene/SceneMng.h"

SceneMng sceneMng;

//---------------------------------------------------------
//ゲームの初期化処理を行う関数
//---------------------------------------------------------
void App::Init() 
{
	//3D用カメラ
	{
		RenderManager_I->AddLayer("default3D");

		Camera cam3d;
		cam3d.SetPerspective(WINDOW_WIDTH, WINDOW_HEIGHT, 0.01f, 1000.0f, 120.0f);
		cam3d.SetName("default3D");
		cam3d.AddRenderLayer("default3D");
		cam3d.SetPosition(0.0f, 0.0f, 0.0f);
		RenderManager_I->SetCamera(cam3d);
		RenderManager_I->SetCameraOrder("default3D", 1);
	}

	//シーンカメラ
	{
		RenderManager_I->AddLayer("Scene");

		Camera camScene;
		camScene.SetName("Scene");
		camScene.AddRenderLayer("Scene");
		camScene.SetPosition(0.0f, 0.0f, 0.0f);
		RenderManager_I->SetCamera(camScene);
		RenderManager_I->SetCameraOrder("Scene", 2);
	}

	//背景カメラ
	{
		RenderManager_I->AddLayer("BackGround");

		Camera bg;
		bg.SetName("BackGround");
		bg.AddRenderLayer("BackGround");
		bg.SetPosition(0.0f, 0.0f, 0.0f);
		RenderManager_I->SetCamera(bg);
		RenderManager_I->SetCameraOrder("BackGround", -2);
	}

	//シーン管理クラスを初期化。最初のシーンはタイトルシーン
	sceneMng.Init(SceneType::Title);
}

//---------------------------------------------------------
//ゲームの更新処理を行う関数
//---------------------------------------------------------
void App::Update() 
{
	//左クリックされたら
	if (Input_I->IsKeyDown(VK_LBUTTON))
	{
		//クリックした位置を取得
		Vector2f vMousePos = Framework_I->GetMousePointInClient();

	}

	//シーン更新
	sceneMng.Update();
}

//---------------------------------------------------------
//ゲームの描画処理を行う関数
//---------------------------------------------------------
void App::Render() 
{
	//シーン描画
	sceneMng.Render();
}
//---------------------------------------------------------
// ゲームの後片付け処理を行う関数
//---------------------------------------------------------
void App::Term() 
{
	//シーン解放
	sceneMng.Term();
}
