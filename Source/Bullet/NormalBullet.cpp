#include "NormalBullet.h"

//CollisionManagerを使うのでinclude
#include "Fwk/Framework.h"
//Tilemapを使うのでinclude
#include "GameObjectMng/GameObjectMng.h"

//初期化
void NormalBullet::Init()
{
	//テクスチャの読み込み
	mTexture.Load("Images/GP3/NormalBullet.png");

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
		mCollision.SetTag("NormalBullet");
		//コリジョンの形状を指定
		mCollision.SetSphere(0.0f, 0.0f, 0.0f, 32.0f);
		//コールバックイベントの設定
		mCollision.SetCallbackFunction(CreateCollisionEventCallback(NormalBullet::OnHit));
	}
}

//衝突時処理
void NormalBullet::OnHit(const Collider& pCollisionA, const Collider& pCollisionB)
{
	GetEffectMng()->CreateEffect("Hit", mPos, Vector3f(), 0.0f, 0.0f, mSize * 0.8f, (int)CollisionGroup::Effect);
	SetActive(false, false);
}