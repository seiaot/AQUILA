#include "Effect.h"

//コリジョングループの列挙型を使うために#include
#include "GameDef.h"
//CollisionManagerを使うのでinclude
#include "Fwk/Framework.h"

//初期化
void Effect::Init()
{
	Object::Init();

	//コリジョンの設定(共通項目のみ)
	{
		mCollision.SetOutlineColor(1.0f, 1.0f, 0.0f);
	}

	InitAnimation();

}

//後片付け
void Effect::Term()
{
	Object::Term();

	//サウンドの破棄
	mSound.Unload();
	//音源の解放
	mSoundSource.Term();
}

//更新
void Effect::Update()
{
	if (!mSprite.IsAnimationPlaying())
	{
		mSprite.PlayAnimation(mAnimName);
	}

	Object::Update();
}

//アニメーションの初期化
void Effect::InitAnimation()
{
	
}

//固有の更新
void Effect::PersonalUpdate()
{
	//移動量の更新
	UpdateVelocity();
	//位置に移動速度を足す
	mPos += mVelocity;
}

//アニメーションのコールバック
void Effect::OnAnimationEvent(const AnimationEvent& animEvent)
{
	switch (animEvent.EventType)
	{

		//アニメーション開始
	case AnimationEventType::Begin:
		break;

		//アニメーション進行
	case AnimationEventType::KeyframeProgressed:
		break;

	//アニメーション完了
	case AnimationEventType::Completed:
	{
		//アニメーションが終わったら非活性
		SetActive(false,false);
	}
	break;

	}
}

//速度の更新
void Effect::UpdateVelocity()
{
	float acceleration = mSpeed * Time_I->GetDeltaTime();	//このフレーム速度
	float maxSpeed = mMaxSpeed * Time_I->GetDeltaTime();	//最大速度

	//向きに合わせて加速
	mVelocity += mDirection * acceleration;

	//速度が上限を超えないようにする
	if (mVelocity.GetLength() > maxSpeed)
	{
		mVelocity = mVelocity.GetUnitVector() * maxSpeed;
	}
}

//向きの設定
void Effect::SetDirection(const Vector3f vDirection)
{
	mDirection = vDirection;
}

//速度の設定
void Effect::SetSpeed(const float speed,const float maxSpeed)
{
	mSpeed = speed;
	mMaxSpeed = maxSpeed;
}

//大きさの設定
void Effect::SetSize(Sizef size)
{
	mSize = size;
	mCollision.SetCubeSize(size);
}

//衝突グループの設定
void Effect::SetCollisionGroup(int collisionGroup)
{
	mCollision.ClearGroup();
	mCollision.ClearHitGroup();

	mCollision.SetGroup((int)CollisionGroup::Effect);

	//生成元を記録
	mCreator = collisionGroup;
}

//生成された時に呼び出す
//派生クラスで上書きして使う
void Effect::OnCreated()
{
	//なし
}