#include "Fwk/Framework.h"
#include "GameObjectMng/GameObjectMng.h"

#include "UI.h"

//‰Šú‰»
void UI::Init()
{
	mSprite.Init();
	mSprite.SetTexture(mTexture);
	mSprite.SetRenderLayer("Scene");
}

//XV
void UI::Update()
{
	mSprite.Update();
}

//•`‰æ
void UI::Render()
{
	mSprite.Draw();
}

//I—¹
void UI::Term()
{
	mSprite.Term();
	mTexture.Unload();
}