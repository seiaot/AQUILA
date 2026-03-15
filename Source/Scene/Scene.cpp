#include "Scene.h"

#include "Fwk/Framework.h"

//初期化
void Scene::Init()
{
	//”次のシーン”の初期値を”なし”にしておく
	mNextScene = SceneType::None;

	mSprite.Init();
	mSprite.SetTexture(mTexture);
	mSprite.SetRenderLayer("Scene");
	mSprite.SetPriority(1);

	mSprite.SetSize(1920.0f, 1080.0f);
	mSprite.SetPosition(0.0f, 0.0f);
}

//終了
void Scene::Term()
{
	mSprite.Term();
	mTexture.Unload();
}

//更新
void Scene::Update()
{
	mSprite.Update();
}

//描画
void Scene::Render()
{
	mSprite.Draw();
}

//次のシーンを取得する
SceneType Scene::GetNextScene()
{
	return mNextScene;
}