#include "GameClear.h"

//CollisionManagerを使うのでinclude
#include "Fwk/Framework.h"
//Tilemapを使うのでinclude
#include "GameObjectMng/GameObjectMng.h"

//初期化
void GameClear::Init()
{
	//テクスチャの読み込み
	mTexture.Load("Images/GP3/GameClearOB.png");

	mSize = { 100.0f,100.0f,100.0f};

	//周辺物共通の初期化処理
	S_Object::Init();

	//テクスチャの描画範囲を指定
	mSprite.SetTexCoord(0.0f, 0.0f, 1.0f, 1.0f);

	//Waveデータ読み込み
	mSound.Load("Sound/DamageC.wav");

	//コリジョンの設定
	{
		//コリジョンにタグを設定
		mCollision.SetTag("GameClear");
		//コリジョンの形状を指定
		mCollision.SetSphere(0.0f, 0.0f, 0.0f, 40.0f);
		//コールバックイベントの設定
		mCollision.SetCallbackFunction(CreateCollisionEventCallback(GameClear::OnHit));
	}
}

//派生の更新
void GameClear::PersonalUpdate()
{
	if (GetDistancePlayerZ() <= 1.0f)
	{
		mPos.z = GetPlayer()->Get3DPosition().z + 1.0f;
	}
}

//衝突時処理
void GameClear::OnHit(const Collider& pCollisionA, const Collider& pCollisionB)
{

}