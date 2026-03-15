#include "ExplosionEX.h"

//CollisionManagerを使うのでinclude
#include "Fwk/Framework.h"
//Tilemapを使うのでinclude
#include "GameObjectMng/GameObjectMng.h"

//初期化
void ExplosionEX::Init()
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
		mCollision.SetTag("ExplosionEX");
		//コリジョンの形状を指定
		mCollision.SetSphere(0.0f, 0.0f, 0.0f, 40.0f);
		//コールバックイベントの設定
		mCollision.SetCallbackFunction(CreateCollisionEventCallback(ExplosionEX::OnHit));
	}
}

//アニメーションの初期化
void ExplosionEX::InitAnimation()
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
	Animation ExplosionEXAnim;
	CreateAnimationUV(ExplosionEXAnim, "ExplosionEX", 9, 1.5f, false, uvs);
	mAnimName = "ExplosionEX";
	ExplosionEXAnim.SetEventCallback(CreateAnimationEventCallback(Effect::OnAnimationEvent));
	mSprite.AddAnimation(ExplosionEXAnim);
}

void ExplosionEX::OnAnimationEvent(const AnimationEvent& animEvent)
{
	switch (animEvent.EventType)
	{

		//アニメーション開始
	case AnimationEventType::Begin:
		break;

		//アニメーション進行
	case AnimationEventType::KeyframeProgressed:
	{
		//一定確率で周辺のどこかに爆発を生成
		if (rand() % 100 < 50.0f)
		{
			GetEffectMng()->CreateEffect("SubExplosion",
				mPos + Vector3f::GetRotatedVector3Axis(Vector3f(1.0f, 0.0f, 0.0f), (float)(rand() % 366), (float)(rand() % 366), (float)(rand() % 366)).GetUnitVector() * (float)((rand() % (201 - 50)) + 50),
				Vector3f(), 0.0f, 0.0f, mSize * 0.8f, (int)CollisionGroup::Effect);
		}
	}
	break;

	//アニメーション完了
	case AnimationEventType::Completed:
	{
		//アニメーションが終わったら非活性
		SetActive(false, false);
	}
	break;

	}
}

//衝突時処理
void ExplosionEX::OnHit(const Collider& pCollisionA, const Collider& pCollisionB)
{

}