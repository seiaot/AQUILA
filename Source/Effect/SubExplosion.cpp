#include "SubExplosion.h"

//CollisionManagerを使うのでinclude
#include "Fwk/Framework.h"
//Tilemapを使うのでinclude
#include "GameObjectMng/GameObjectMng.h"

//初期化
void SubExplosion::Init()
{
	//テクスチャの読み込み
	mTexture.Load("Images/GP3/Explosion01.png");

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
		mCollision.SetTag("SubExplosion");
		//コリジョンの形状を指定
		mCollision.SetSphere(0.0f, 0.0f, 0.0f, 40.0f);
		//コールバックイベントの設定
		mCollision.SetCallbackFunction(CreateCollisionEventCallback(SubExplosion::OnHit));
	}
}

//アニメーションの初期化
void SubExplosion::InitAnimation()
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
	Animation SubExplosionAnim;
	CreateAnimationUV(SubExplosionAnim, "SubExplosion", 9, 0.7f, false, uvs);
	mAnimName = "SubExplosion";
	SubExplosionAnim.SetEventCallback(CreateAnimationEventCallback(Effect::OnAnimationEvent));
	mSprite.AddAnimation(SubExplosionAnim);
}

//衝突時処理
void SubExplosion::OnHit(const Collider& pCollisionA, const Collider& pCollisionB)
{

}