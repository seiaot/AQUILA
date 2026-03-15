#include "Fwk/Framework.h"
#include "GameObjectMng/GameObjectMng.h"

#include "Ground.h"

//初期化
void Ground::Init()
{
	/*mTexture.Load("Images/GP3/ground.png");

	mSprite[0].Init();
	mSprite[0].SetTexture(mTexture);
	mSprite[0].SetPosition(0.0f, 0.0f);
	mSprite[0].SetSize(2250.0f,1.0f);
	mSprite[0].SetPriority(-1000000);

	for (int i = 1; i <96; ++i)
	{
		mSprite[i].Init();
		mSprite[i].SetTexture(mTexture);
		mSprite[i].SetPosition(mSprite[i - 1].GetPositionX(), mSprite[i - 1].GetPositionY() - (mSprite[i - 1].GetHeight()+ mSprite[i - 1].GetHeight() * 1.048f) / 2.0f);
		mSprite[i].SetSize(mSprite[i - 1].GetWidth() * 1.048f, mSprite[i - 1].GetHeight() * 1.048f);
		mSprite[i].SetPriority(-1000000 +i);
	}*/

	mTexture.Load("Images/GP3/ground01.png");
	mSize.w = WINDOW_WIDTH * 3.0f;
	mSize.h = 5.0f;

	mSprite.Init();
	mSprite.SetTexture(mTexture);
	mSprite.SetPivot(Pivot::Top);
}

//更新
void Ground::Update()
{
	if (!mIsActive)
	{
		return;
	}

	if (GetDistanceCameraZ() < 0.0f)
	{
		mPos.z += 1200.0f;
		mSprite.SetPriority(mSprite.GetPriority() - 96);
	}
	
	/*mTimer += Time_I->GetDeltaTime();

	float f = 0.2f;

	if (mTimer >= f * 1.0f && mTimer < f * 2.0f)
	{
		mSprite.SetTexture(mTexture2);
	}
	else if (mTimer >= f * 2.0f)
	{
		mSprite.SetTexture(mTexture1);
		mTimer = 0.0f;
	}*/


	/*for (int i = 0; i < 96; ++i)
	{
		mSprite[i].Update();
	}*/

	SetSpritePosition();
	SetSpriteSize();

	mSprite.Update();
}

//描画
void Ground::Render()
{
	/*for (int i = 0; i < 96; ++i)
	{
		mSprite[i].Draw();
	}*/


	if (GetDistanceCameraZ() <= 1200.0f)
	{
		mSprite.Draw();
	}
}

//終了
void Ground::Term()
{

	/*mTexture.Unload();
	for (int i = 0; i < 96; ++i)
	{
		mSprite[i].Term();
	}*/
	mSprite.Term();
	mTexture.Unload();
}

//アクティブ状態の設定
void Ground::SetActive(bool bActive)
{
	mIsActive = bActive;
}

//アクティブ状態を返す
bool Ground::IsActive()
{
	return mIsActive;
}

//位置を設定
void Ground::SetPosition(Vector3f pos)
{
	mPos = pos;
}

//視点との距離を取得
float Ground::GetDistanceCameraZ() const
{
	float reDis = 0.0f;	//戻り値用

	//自分の位置からプレイヤーの視点の位置を引く
	reDis = mPos.z - RenderManager_I->GetCameraPosition().z;

	return reDis;
}

//描画順を設定
void Ground::SetPriority(int iPriority)
{
	mSprite.SetPriority(iPriority);
}

//テクスチャの設定
void Ground::ChangeTexture(string texturePas)
{
	mTexture.Load(texturePas);
}

//プレイヤーとの距離に基づく倍率を取得
float Ground::GetRate()
{
	float reRate = 0.0f;	//戻り値用

	//Z軸がプレイヤーの視点と同じとき1倍(1.0f)にする
	//プレイヤーより前なら1.0f未満,後ろなら1.0fより大きくなる
	reRate = (1200.0f - GetDistanceCameraZ()) / 1200.0f;

	return reRate;
}

//表示用の位置を設定
void Ground::SetSpritePosition()
{
	Vector2f vVanishingPoint = GetBG()->GetVanishingPoint();	//画面上の中心の座標
	Vector2f pos2d = { mPos.x,mPos.y };	//現在位置のxとyを取り出す

	Vector2f vMove = pos2d - vVanishingPoint;	//中心から現在位置に向かうベクトル

	//プレイヤーとの距離で位置を変化
	mSprite.SetPosition(vVanishingPoint + (vMove * GetRate()));
}

//表示サイズを変更
void Ground::SetSpriteSize()
{
	//画面に一番近いときこの値がそのまま大きさにかけられる
	float fRate = 90.0912f * GetRate();	//倍率の取得

	//プレイヤーとの距離で大きさを変化
	mSprite.SetSize(mSize.w * fRate * 0.5f, mSize.h * fRate);
}
