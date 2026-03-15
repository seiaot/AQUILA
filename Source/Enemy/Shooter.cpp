#include "Shooter.h"

//CollisionManagerを使うのでinclude
#include "Fwk/Framework.h"
//プレイヤーのアドレスを得るため
#include "GameObjectMng/GameObjectMng.h"

//初期化
void Shooter::Init()
{
	//テクスチャの読み込み
	mTexture.Load("Images/GP3/Shooter.png");

	mSize = { 192.0f,192.0f,50.0f};

	//敵共通の初期化処理
	Enemy::Init();

	//コリジョンの設定
	{
		//コリジョンにタグを設定
		mCollision.SetTag("Shooter");
		//コリジョンの形状を指定（中心位置はmPositionと同じ、サイズはスプライトより少し小さめに）
		mCollision.SetCube(mPos,mSize);
		mCollision.SetCallbackFunction(CreateCollisionEventCallback(Shooter::OnHit));
	}

	//HPを設定
	mHP = 2;
	//ダメージ時に付ける赤色の強さ
	mDamagedColor = 0.0f;
	//加速度を初期化
	mVelocity = { 0.0f,0.0f,0.0f};

	//タイマーは0
	mTimer = 0.0f;
	//射撃していない
	mFired = false;
}

//非アクティブ条件
bool Shooter::Isinactive()
{
	//HPが0になるか描画範囲から出たら
	if (mHP == 0)
	{
		return true;
	}

	return Object::Isinactive();
}

//派生ごとの更新
void Shooter::PersonalUpdate()
{
	Enemy::PersonalUpdate();

	//弾を未発射でプレイヤーに近づいたら
	if (!mFired && GetDistancePlayerZ() <= 600.0f)
	{
		Vector3f vPlayerPos = GetPlayer()->Get3DPosition();	//プレイヤーの位置
		mPos.z = vPlayerPos.z + 600.0f;
		mTimer += Time_I->GetDeltaTime();
		//少し待機
		if (mTimer >= 1.5f)
		{
			//プレイヤーの二秒後の位置を狙う
			vPlayerPos.z += 600.0f;
			float speed = (vPlayerPos - mPos).GetLength() / 2.0f;

			//弾の生成
			GetBulletMng()->CreateBullet("NormalBullet", mPos, vPlayerPos - mPos, speed, speed,Sizef(64.0f,64.0f,64.0f), (int)CollisionGroup::Enemy);

			//発射済みにする
			mFired = true;
		}
	}
}

//やられたときに呼び出される
	//派生クラスで上書きして使う
void Shooter::OnDefeated()
{
	GetEffectMng()->CreateEffect("Explosion", mPos, Vector3f(), 0.0f, 0.0f, mSize * 1.2f, (int)CollisionGroup::Effect);
}

void Shooter::OnHit(const Collider& pCollisionA, const Collider& pCollisionB)
{
	if (pCollisionB.GetGroup() == (int)CollisionGroup::PlayerBullet)
	{
		OnDamaged(1);
	}
}