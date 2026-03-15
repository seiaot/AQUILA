#include "Fwk/Framework.h"
#include "GameObjectMng/GameObjectMng.h"

#include "BG.h"

//初期化
void BG::Init()
{
	mSkyTexture.Load("Images/GP3/Sky2.png");

	mSkySprite.Init();
	mSkySprite.SetTexture(mSkyTexture);
	mSkySprite.SetPivot(Pivot::Bottom);
	mSkySprite.SetSize(WINDOW_WIDTH * 2.0f,WINDOW_HEIGHT);
	mSkySprite.SetPosition(0.0f,0.0f,0.0f);
	mSkySprite.SetRenderLayer("BackGround");
	mSkySprite.SetPriority(INT_MIN);

	for (int i = 0;i < 3;++i)
	{
		mDistantTexture->Load("Images/GP3/Distant0.png");
		mDistantSprite[i].Init();
		mDistantSprite[i].SetTexture(mDistantTexture[i]);
		mDistantSprite[i].SetPivot(Pivot::Bottom);
		mDistantSprite[i].SetSize(WINDOW_WIDTH, 125.0f - 20.0f * (float)i);
		mDistantSprite[i].SetRenderLayer("BackGround");
		mDistantSprite[i].SetPriority(-10 +  i);
		mDistantSprite[i].SetAdditionalColor(-0.08f * (float)i, -0.05f * (float)i, -0.1f * (float)i);
		mDistantUVLeft[i] = 0.0f;
	}

	mTimer = 0.0f;
}

//更新
void BG::Update()
{
	//原点の更新
	UpdateVanishingPoint();

	mTimer += Time_I->GetDeltaTime();

	//秒毎に
	if (mTimer >= 0.1f)
	{
		//層になった雲を生成
		CreateLayeredCloud();
		mTimer = 0.0f;
	}

	mSkySprite.SetPosition(mVanishingPoint);
	mSkySprite.Update();

	Vector3f vPlayerMove = GetPlayer()->GetVelocity();
	mDistantUVLeft[0] += vPlayerMove.x * 0.00001f;
	mDistantUVLeft[1] += vPlayerMove.x * 0.00005f;
	mDistantUVLeft[2] += vPlayerMove.x * 0.0001f;

	for (int i = 0;i < 3;++i)
	{
		mDistantSprite[i].SetTexCoord(mDistantUVLeft[i], 0.0f, 1.0f, 1.0f);
		mDistantSprite[i].SetPosition(0.0f,mVanishingPoint.y);
		mDistantSprite[i].Update();
	}
}

//描画
void BG::Render()
{
	mSkySprite.Draw();

	for (int i = 0;i < 3;++i)
	{
		mDistantSprite[i].Draw();
	}
}

//終了
void BG::Term()
{
	mSkySprite.Term();

	for (int i = 0;i < 3;++i)
	{
		mDistantSprite[i].Term();
	}
}

//原点を返す
Vector2f BG::GetVanishingPoint() const
{
	return mVanishingPoint;
}

//原点を設定
void BG::UpdateVanishingPoint()
{
	Vector2f vPlayerPos = GetPlayer()->Get2DPosition();//プレイヤーの位置

	Vector2f vVP = vPlayerPos - Vector2f();	//プレイヤーから原点へのベクトル

	vVP.x *= -0.5;

	if (vPlayerPos.y <= 0)
	{
		vVP.y *= 0.5;
	}
	else
	{
		vVP.y *= 0.1f;
	}

	mVanishingPoint = vVP;
}

//重なった雲の生成
void BG::CreateLayeredCloud()
{
	float createZ = GetPlayer()->Get3DPosition().z + 1400.0f;
	Vector3f direction = { 0.0f,0.0f,1.0f };
	for (int i = 0;i < 3;++i)
	{
		if (rand() % 100 < 10)
		{
			Sizef size = { (float)(1000 + ((rand() % 300) + 1)),(float)(600 + ((rand() % 300) + 1)) ,(float)(900 + ((rand() % 100) + 1)) };
			GetSObjectMng()-> CreateS_Object("Cloud",
				Vector3f((float)((rand() % 3501) * ((rand() % 2) ? 1.0f : -1.0f))
					, 1000.0f + 1000.0f * (float)i
					, createZ), direction,
				25.0f, 100.0f * (float)i, size, (int)CollisionGroup::S_Object);
		}
	}
}