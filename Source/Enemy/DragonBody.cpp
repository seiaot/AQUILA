#include "DragonBody.h"

//CollisionManagerを使うのでinclude
#include "Fwk/Framework.h"
//プレイヤーのアドレスを得るため
#include "GameObjectMng/GameObjectMng.h"

#include "Dragon.h"

//初期化
void DragonBody::Init()
{
	//テクスチャの読み込み
	mTexture.Load("Images/GP3/dragonbody.png");

	mSize = {280.0f,280.0f,250.0f };

	//敵共通の初期化処理
	Enemy::Init();

	//コリジョンの設定
	{
		//コリジョンにタグを設定
		mCollision.SetTag("DragonBody");
		//コリジョンの形状を指定
		mCollision.SetSphere(0.0f, 0.0f, 0.0f,60.0f);
		mCollision.SetCallbackFunction(CreateCollisionEventCallback(DragonBody::OnHit));
	}

	//ダメージ時に付ける赤色の強さ
	mDamagedColor = 0.0f;
	//加速度を初期化
	mVelocity = { 0.0f,0.0f,0.0f};

	//速度の設定
	mSpeed = 10000.0f;
	mMaxSpeed = 300.0f;

	//タイマーは0
	mTimer = 0.0f;
}

//非アクティブ条件
bool DragonBody::Isinactive()
{
	return !mIsActive;
}

//派生ごとの更新
void DragonBody::PersonalUpdate()
{
	////一定間隔で少し前の位置を変更
	//mTimer += Time_I->GetDeltaTime();
	//if (mTimer >= 0.08f)
	//{
	//	mBefPos = mPos;
	//	mTimer = 0.0f;
	//}

}

void DragonBody::SetBeforPos()
{
	mBefPos = mPos;
}

Vector3f DragonBody::GetBeforePos() const
{
	return mBefPos;
}

void DragonBody::OnHit(const Collider& pCollisionA, const Collider& pCollisionB)
{

}