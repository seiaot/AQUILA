#include "Fire.h"

//CollisionManagerを使うのでinclude
#include "Fwk/Framework.h"
//Tilemapを使うのでinclude
#include "GameObjectMng/GameObjectMng.h"

//初期化
void Fire::Init()
{
	//テクスチャの読み込み
	mTexture.Load("Images/GP3/Fire.png");

	mSize = { 100.0f,100.0f,100.0f};

	//弾共通の初期化処理
	Bullet::Init();

	//テクスチャの描画範囲を指定
	mSprite.SetTexCoord(0.0f, 0.0f, 1.0f, 1.0f);

	//Waveデータ読み込み
	mSound.Load("Sound/DamageC.wav");

	//コリジョンの設定
	{
		//コリジョンにタグを設定
		mCollision.SetTag("Fire");
		//コリジョンの形状を指定
		mCollision.SetSphere(0.0f, 0.0f, 0.0f, 40.0f);
		//コールバックイベントの設定
		mCollision.SetCallbackFunction(CreateCollisionEventCallback(Fire::OnHit));
	}

	//初期化
	mTimer = 0.0f;
}

//固有の更新
void Fire::PersonalUpdate()
{
	Vector3f axisX = { 1.0f, 0.0f, 0.0 };	//回転X軸

	//少しづつ上に動く
	mDirection = Vector3f::GetRotatedVector(mDirection,359.0f, axisX);

	mTimer += Time_I->GetDeltaTime();

	//一定時間で分裂
	if (mTimer >= 1.0f)
	{
		Vector3f axisY = { 0.0f, 1.0f, 0.0f };	//回転Y軸
		for (int i = 0;i < 3;++i)
		{
			//下方向に3つ
			GetBulletMng()->CreateBullet("Fire", mPos, Vector3f::GetRotatedVector(mDirection, 350.0f + 20.0f * (float)i, axisX), mSpeed, mMaxSpeed * 1.2f, mSize * 0.999f, mCreator);
			//前に3つ
			GetBulletMng()->CreateBullet("Fire", mPos, Vector3f::GetRotatedVector(mDirection, 330.0f + 30.0f * (float)i, axisY), mSpeed, mMaxSpeed * 1.2f, mSize * 0.999f, mCreator);
		}

		//自分は非アクティブ
		SetActive(false, false);
	}
	mCollision.SetCubeSize(mSize);

	Bullet::PersonalUpdate();
}

//生成時処理
void Fire::OnCreated()
{
	mTimer = 0.0f;
}

//非アクティブにする(派生毎)
bool Fire::Isinactive()
{
	//大きさが一定以下になったら
	if (mSize.w + mSize.h <= 1.0f)
	{
		return true;
	}

	Object::Isinactive();
}

//衝突時処理
void Fire::OnHit(const Collider& pCollisionA, const Collider& pCollisionB)
{
	SetActive(false, false);
}