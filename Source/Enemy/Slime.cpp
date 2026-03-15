#include "Slime.h"

//CollisionManagerを使うのでinclude
#include "Fwk/Framework.h"
//プレイヤーのアドレスを得るため
#include "GameObjectMng/GameObjectMng.h"

//初期化
void Slime::Init()
{
	//テクスチャの読み込み
	mTexture.Load("Images/2dAction/slime.png");

	mSize = { 192.0f,192.0f,50.0f};

	//敵共通の初期化処理
	Enemy::Init();

	//テクスチャの描画範囲を指定
	mSprite.SetTexCoord(0.0f, 0.0f, 0.5f, 0.5f);

	//Waveデータ読み込み
	mSound.Load("Sound/DamageC.wav");
	//音源
	mSoundSource.Init(mSound);

	//コリジョンの設定
	{
		//コリジョンにタグを設定
		mCollision.SetTag("Slime");
		//コリジョンの形状を指定（中心位置はmPositionと同じ、サイズはスプライトより少し小さめに）
		mCollision.SetCube(mPos,mSize);
		mCollision.SetCallbackFunction(CreateCollisionEventCallback(Slime::OnHit));
	}

	//HPを設定
	mHP = 3;
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
bool Slime::Isinactive()
{
	//HPが0になるか描画範囲から出たら
	if (mHP == 0)
	{
		return true;
	}

	return Object::Isinactive();
}

//派生ごとの更新
void Slime::PersonalUpdate()
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
			GetBulletMng()->CreateBullet("NormalBullet", mPos, vPlayerPos - mPos, speed, speed,Sizef(50.0f,50.0f,50.0f), (int)CollisionGroup::Enemy);

			//発射済みにする
			mFired = true;
		}
	}
}

void Slime::OnHit(const Collider& pCollisionA, const Collider& pCollisionB)
{
	if (pCollisionB.GetGroup() == (int)CollisionGroup::PlayerBullet)
	{
		OnDamaged(1);
	}
}