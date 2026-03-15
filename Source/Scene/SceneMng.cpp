#include "SceneMng.h"

#include "Fwk/Framework.h"

//各シーンクラスのヘッダ
#include "SceneTitle.h"
#include "SceneInGame.h"
#include "SceneGameOver.h"
#include "SceneGameClear.h"
#include "SceneStageSelect.h"

//初期化
void SceneMng::Init(SceneType startScene)
{
	//現在のシーンは”なし”
	mCurrentScene = SceneType::None;
	//次に動かすシーンを記録しておく
	mNextScene = startScene;

	//現在のシーンクラスの変数ポインタはnullを設定しておく
	mpScene = nullptr;
}

//終了
void SceneMng::Term()
{
	//現在のシーンを終了する
	_endCurrentScene();
	//次のシーンは”なし”としておく
	mNextScene = SceneType::None;
}

//更新
void SceneMng::Update()
{
	//次のシーンが設定されており、
	//現在のシーンがそのシーンと異なっていたらシーンを切り替える
	if (mNextScene != SceneType::None && mCurrentScene != mNextScene)
	{
		//次のシーンを開始する
		_beginScene(mNextScene);
	}

	//現在のシーンを更新
	if (mpScene != nullptr)
	{
		mpScene->Update();
		mNextScene = mpScene->GetNextScene();
	}
}

//描画
void SceneMng::Render()
{
	//現在のシーンを描画
	if (mpScene != nullptr)
	{
		mpScene->Render();
	}
}

//新しいシーンを開始する
void SceneMng::_beginScene(SceneType newScene)
{
	//現在動いているシーンを止める
	_endCurrentScene();

	//新しいシーンを開始する
	switch (newScene)
	{
	case SceneType::Title:
		//タイトルシーンの生成
		mpScene = new SceneTitle();
		break;

	case SceneType::InGame:
		//インゲームシーンの生成
		mpScene = new SceneInGame();
		break;

	case SceneType::GameOver:
		//ゲームオーバーシーンの生成
		mpScene = new SceneGameOver();
		break;

	case SceneType::GameClear:
		//ゲームクリアシーンの生成
		mpScene = new SceneGameClear();
		break;
	}

	//生成したシーンの初期化
	if (mpScene != nullptr)
	{
		mpScene->Init();
	}

	//現在のシーンを設定
	mCurrentScene = newScene;
}

//現在のシーンを停止する
void SceneMng::_endCurrentScene()
{
	if (mpScene != nullptr)
	{
		mpScene->Term();
		SAFE_DELETE(mpScene);
	}

	//シーンを止めたので現在シーンも”なし”にしておく
	mCurrentScene = SceneType::None;
}