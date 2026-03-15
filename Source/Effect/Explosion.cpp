#include "Explosion.h"

//CollisionManagerを使うのでinclude
#include "Fwk/Framework.h"
//Tilemapを使うのでinclude
#include "GameObjectMng/GameObjectMng.h"

//初期化
void Explosion::Init()
{
	//テクスチャの読み込み
	mTexture.Load("Images/GP3/Explosion02.png");

	mSize = { 100.0f,100.0f,100.0f};

	//弾共通の初期化処理
	Effect::Init();

	//テクスチャの描画範囲を指定
	mSprite.SetTexCoord(0.0f, 0.0f, 0.0f, 0.0f);

	//Waveデータ読み込み
	mSound.Load("Sound/DamageC.wav");

	//コリジョンの設定
	{
		//コリジョンにタグを設定
		mCollision.SetTag("Explosion");
		//コリジョンの形状を指定
		mCollision.SetSphere(0.0f, 0.0f, 0.0f, 40.0f);
		//コールバックイベントの設定
		mCollision.SetCallbackFunction(CreateCollisionEventCallback(Explosion::OnHit));
	}
}

//アニメーションの初期化
void Explosion::InitAnimation()
{
	//パターン1つ分のUV座標上の幅
	float uvW = 1.0f / 8.0f;

	//8パターン分のUV座標
	UVRect uvs[9] = {};
	for (int i = 0;i < 8;++i)
	{
		uvs[i] = { uvW * (float)i,0.0f,uvW,1.0f };
	}

	//アニメーションの作成
	Animation ExplosionAnim;
	CreateAnimationUV(ExplosionAnim, "Explosion", 9, 0.8f, false, uvs);
	mAnimName = "Explosion";
	ExplosionAnim.SetEventCallback(CreateAnimationEventCallback(Effect::OnAnimationEvent));
	mSprite.AddAnimation(ExplosionAnim);
}

//衝突時処理
void Explosion::OnHit(const Collider& pCollisionA, const Collider& pCollisionB)
{

}