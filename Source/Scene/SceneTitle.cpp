#include "SceneTitle.h"

#include "Fwk/Framework.h"

//‰Šú‰»
void SceneTitle::Init()
{
	//hŽŸ‚ÌƒV[ƒ“h‚Ì‰Šú’l‚ðh‚È‚µh‚É‚µ‚Ä‚¨‚­
	mNextScene = SceneType::None;

	mTexture.Load("Images/GP3/GameTitle.png");

	Scene::Init();
}

//XV
void SceneTitle::Update()
{
	//Aƒ{ƒ^ƒ“‚ð‰Ÿ‚µ‚½‚çhŽŸ‚ÌƒV[ƒ“h‚ÉInGame‚ðÝ’è‚·‚é
	if (Input_I->IsKeyDown('X') || Input_I->IsButtonDown(0, GAMEPAD_BUTTON::B))
	{
		mNextScene = SceneType::InGame;
	}

	Scene::Update();
}