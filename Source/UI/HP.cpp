#include "Fwk/Framework.h"
#include "GameObjectMng/GameObjectMng.h"

#include "HP.h"

//‰Šú‰»
void HP::Init()
{
	mTexture.Load("Images/GP3/Icon.png");
	UI::Init();
	//¶‰º‚É‡‚í‚¹‚é
	mSprite.SetPivot(Pivot::BottomLeft);
	mSprite.SetSize(320.0f, 64.0f);
	mSprite.SetPosition(WINDOW_WIDTH / -2.0f, WINDOW_HEIGHT / -2.0f);
}

//XV
void HP::Update()
{
	mSprite.SetTexCoord(0.0f, 0.0f, (float)GetPlayer()->GetHP() * 1.0f, 1.0f);
	mSprite.SetSize(64.0f * (float)GetPlayer()->GetHP(), 64.0f);
	mSprite.Update();
}