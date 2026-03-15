#include "obje.h"

#include "GameObjectMng/GameObjectMng.h"

void obje::Init()
{
	mPosition = { 64.0f,64.0f };
	mZ = 100.0f;

	mW = 0.0f;
	mH = 0.0f;

	mTexture.Load("Images/2dAction/slime.png");

	mSprite.Init();
	mSprite.SetTexture(mTexture);
	mSprite.SetTexCoord(0.0f, 0.0f, 0.5f, 0.5f);
	mSprite.SetPosition(mPosition);
	mSprite.SetSize(mW,mH);
}

void obje::Update()
{
	//mPosition += {1.0f, 0.75f};

	float PlayerZ = GetPlayer()->GetZ();
	float disZ = mZ - PlayerZ;
	if (disZ >= 0.0f)
	{
		disZ += 0.1f;
	}
	else
	{
		disZ = (abs)(disZ) / 10.0f + 1.0f;
	}

	mW = (abs)(1000.0f / disZ);
	mH = (abs)(1000.0f / disZ);

	mSprite.SetPosition(mPosition);
	mSprite.SetSize(mW, mH);
	mSprite.Update();
}

void obje::Render()
{
	mSprite.Draw();
}

void obje::Term()
{
	mSprite.Term();
	mTexture.Unload();
}