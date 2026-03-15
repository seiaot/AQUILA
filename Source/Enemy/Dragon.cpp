#include "Dragon.h"

//CollisionManagerを使うのでinclude
#include "Fwk/Framework.h"
//プレイヤーのアドレスを得るため
#include "GameObjectMng/GameObjectMng.h"

//初期化
void Dragon::Init()
{

	//テクスチャの読み込み
	mTexture.Load("Images/GP3/dragon.png");

	mSize = { 350.0f,350.0f,350.0f };

	//敵共通の初期化処理
	Enemy::Init();

	mInverse.Load("Images/GP3/dragonInverse.png");

	//コリジョンの設定
	{
		//コリジョンにタグを設定
		mCollision.SetTag("Dragon");
		//コリジョンの形状を指定
		mCollision.SetSphere(0.0f, 0.0f, 0.0f, 125.0f);
		mCollision.SetCallbackFunction(CreateCollisionEventCallback(Dragon::OnHit));
	}

	//HPを設定
	mHP = 50;
	//ダメージ時に付ける赤色の強さ
	mDamagedColor = 0.0f;

	//タイマーの初期化
	mTimer = 0.0f;
	mSavePosTimer = 0.0f;

	//速度の設定
	mSpeed = 400.0f;
	mMaxSpeed = 0.0f;

	//体用のポインターはnullにしておく
	for (int i = 0; i < 30; ++i)
	{
		pBody[i] = nullptr;
	}

	//初期状態は待機
	mStatus = Status::Idle;

	mMoveX = 500.0f;
	mMoveY = 200.0f;
	mRotX = 0.0f;
	mRotY = 0.0f;
	mAddRotX = 1.0f;
	mAddRotY = 2.0f;
	mMoveWaveX = 0.0f;
	mMoveWaveY = 0.0f;

	mToggleFire = false;
}

//終了
void Dragon::Term()
{
	Enemy::Term();
	mInverse.Unload();
}

//位置の設定
void Dragon::SetPosition(Vector3f vPos)
{
	mPos = vPos;
	mBasePos = vPos;
}

//非アクティブ条件
bool Dragon::Isinactive()
{
	return false;
}

//派生ごとの更新
void Dragon::PersonalUpdate()
{
	//一定間隔で少し前の位置を変更
	mSavePosTimer += Time_I->GetDeltaTime();
	if (mSavePosTimer >= 0.1f)
	{
		mBefPos = mPos;
		for (int i = 0;i < 20;++i)
		{
			pBody[i]->SetBeforPos();
		}
		mSavePosTimer = 0.0f;
	}

	//0～360に収める
	if (mRotX >= 360.0f)
	{
		mRotX -= 360.0f;
	}
	if (mRotY < 0)
	{
		mRotY += 360.0f;
	}
	if (mRotY >= 360.0f)
	{
		mRotY -= 360.0f;
	}
	if (mRotY < 0)
	{
		mRotY += 360.0f;
	}

	//基底更新
	Enemy::PersonalUpdate();

	Vector3f playerPos = GetPlayer()->Get3DPosition();	//プレイヤーの位置はたくさん使う

	switch (mStatus)
	{
		//待機
		case(Status::Idle):
		{
			//起点のxy軸はプレイヤーに追従
			mDirection = (playerPos - mBasePos).GetUnitVector();

			if (GetDistancePlayerZ() <= 650.0f)
			{
				mTimer += Time_I->GetDeltaTime();

				mMaxSpeed = 400.0f;
				//z軸は一定の距離を保つ
				mBasePos.z = playerPos.z + 600.0f;
				if (mTimer >= 6.0f)
				{
					//前回の行動が火じゃなければ火を吐ける
					if (!mToggleFire && rand() % 100 < 60)
					{
						mStatus = Status::Fire;
						mToggleFire = true;
						mAddRotX = 2.0f;
						mAddRotY = 0.0f;
					}
					else
					{
						mStatus = Status::Rushing;
						mToggleFire = false;
						mMaxSpeed = 800.0f;
						mDirection = Vector3f();
						mAddRotX = 0.0f;
						mAddRotY = 0.0f;
					}
					mTimer = 0.0f;
				}
			}
		}
		break;

		//突撃
		case(Status::Rushing):
		{
			//準備ができるまでは近づかない
			if (mMoveWaveY != 90.0f || mMoveX != 120.0f || mMoveY != 120.0f || mRotY != mRotX)
			{
				//波をずらす
				mMoveWaveY += (mMoveWaveY != 90.0f) ? 1.0f : 0.0f;
				//波の大きさ
				mMoveX -= (mMoveX != 120.0f) ? 10.0f : 0.0f;
				mMoveY -= (mMoveY != 120.0f) ? 5.0f : 0.0f;
				//角度
				if (fabsf(mRotX - mRotY < 1.0f))
				{
					mRotX = mRotY;
				}
				mAddRotX = (mRotX != mRotY) ? 1.0f : 0.0f;
				mAddRotY = (mRotY != mRotX) ? -1.0f : 0.0f;

				//一定距離を保つ
				mBasePos.z = playerPos.z + 600.0f;
			}
			//近づき始める
			else
			{
				mTimer += Time_I->GetDeltaTime();

				mAddRotX = 5.0f;
				mAddRotY = 5.0f;

				mDirection = (GetPlayer()->Get2DPosition() - Vector2f(mBasePos)).GetNormalized();
				mBasePos.z = playerPos.z + 600.0f - mTimer * 150.0f;
			}

			//カメラ外にいった位置を記録
			if (!mTargetPos.GetLength() && GetDistanceCameraZ() < 0.0f)
			{
				mTargetPos = mBasePos;
			}

			//プレイヤーよりある程度後ろに行ったら戻る
			if (GetDistancePlayerZ() < -650.0f)
			{
				mMoveWaveY = 0.0f;
				mAddRotX = 0.0f;
				mAddRotY = 5.0f;
				mStatus = Status::Return;
				mBasePos.x = mTargetPos.x;
				mBasePos.y = mTargetPos.y;
				mTargetPos.z = playerPos.z + 100000.0f;
				mTimer = 0.0f;
				mMaxSpeed = 600.0f;
				mSprite.SetTexture(mInverse);
			}
		}
		break;

		//火を放つ
		case(Status::Fire):
		{
			mTimer += Time_I->GetDeltaTime();

			mDirection = playerPos - mBasePos;

			//位置保存タイマーを流用
			if (!mSavePosTimer)
			{
				Vector3f vPos = { mPos.x,mPos.y - 50.0f,mBasePos.z };	//頭の位置
				Vector3f axisX = { 0.0f, 1.0f, 0.0f };	//回転X軸
				Vector3f axisY = { 1.0f, 0.0f, 0.0f };	//回転Y軸
				for (int i = 0;i < 3;++i)
				{
					GetBulletMng()->CreateBullet("Fire", vPos, Vector3f::GetRotatedVector((playerPos - vPos + Vector3f(0.0f, 0.0f, 500.0f).GetUnitVector()), 30.0f * (float)i, axisX), 1.0f, 150.0f, Sizef(100.0f, 100.0f, 50.0f), (int)CollisionGroup::Enemy);
					GetBulletMng()->CreateBullet("Fire", vPos, Vector3f::GetRotatedVector((playerPos - vPos + Vector3f(0.0f, 0.0f, 500.0f).GetUnitVector()), 320.0f + 40.0f * (float)i, axisY), 1.0f, 50.0f, Sizef(100.0f, 100.0f, 50.0f), (int)CollisionGroup::Enemy);
				}
			}


			mBasePos.z = playerPos.z + 600.0f;

			//数秒たったら戻る
			if (mTimer >= 5.0f)
			{
				mAddRotX = 0.0f;
				mAddRotY = 5.0f;
				mStatus = Status::Return;
				mTargetPos = mPos;
				mTargetPos.z = playerPos.z + 5000.0f;
				mTimer = 0.0f;
				mMaxSpeed = 400.0f;
				mSprite.SetTexture(mInverse);
			}
		}
		break;

		//戻る
		case(Status::Return):
		{
			//決めた位置戻る
			mDirection = mTargetPos - mPos;
			//奥方向の画面外に行ったら待機
			if (GetDistancePlayerZ() >= 1250.0f)
			{
				mStatus = Status::Idle;
				mSpeed = 400.0f;
				mMaxSpeed = 0.0f;
				mTargetPos = Vector3f();
				mAddRotX = 1.0f;
				mAddRotY = 2.0f;
				mMoveX = 500.0f;
				mMoveY = 200.0f;
				mSprite.SetTexture(mTexture);
			}
		}
		break;
	}

	//Velocityでは起点を動かす
	UpdateVelocity();
	mBasePos += mVelocity;

	//mPos.z = GetPlayer()->Get3DPosition().z + 300.0f;

	//回転角度
	mRotX += mAddRotX;
	mRotY += mAddRotY;

	mPos.x = mBasePos.x +  mMoveX * sinf(DegToRad(mRotX + mMoveWaveX));
	mPos.y = mBasePos.y +  mMoveY * sinf(DegToRad(mRotY + mMoveWaveY));
	mPos.z = mBasePos.z;

	//体の位置を頭に追従
	for (int i = 0;i < 30;++i)
	{
		pBody[i]->SetPosition(
			//Vector3f(pBody[i - 1]->GetBeforePos().x, pBody[i - 1]->GetBeforePos().y, mBasePos.z + ((mDirection.z > 0.0f) ? -10.0f : 10.0f) * (float)i)
			Vector3f(mBasePos.x + mMoveX * sinf(DegToRad(mRotX - mAddRotX * (float)i * 3.0f + mMoveWaveX)), mBasePos.y + mMoveY * sinf(DegToRad(mRotY - mAddRotY * (float)i * 3.0f + mMoveWaveY)), mBasePos.z + ((mDirection.z > 0.0f) ? -7.0f : 7.0f) * ((float)i + 1.0f))
		);
	};
}

//やられたとき
void Dragon::OnDefeated()
{
	//やられたらプレイヤーをクリア状態にする
	if (mHP <= 0)
	{
		GetPlayer()->SetClear(true);
	}

	GetEffectMng()->CreateEffect("ExplosionEX",mPos,Vector3f(), 0.0f, 0.0f, mSize * 1.2f, (int)CollisionGroup::Effect);

	for (int i = 0;i < 30;++i)
	{
		GetEffectMng()->CreateEffect("ExplosionEX", pBody[i]->Get3dPosition(), Vector3f(), 0.0f, 0.0f, mSize * 1.2f, (int)CollisionGroup::Effect);
	}

	for (int i = 0;i < 30;++i)
	{
		pBody[i]->SetActive(false, false);
	}
}

void Dragon::OnHit(const Collider& pCollisionA, const Collider& pCollisionB)
{
	if (pCollisionB.GetGroup() == (int)CollisionGroup::PlayerBullet)
	{
		OnDamaged(1);
	}
}

//生成された時に呼び出す
void Dragon::OnCreated()
{
	//体の位置
	for (int i = 0;i < 30;++i)
	{
		GetEnemyMng()->CreateEnemy("DragonBody", Vector3f(0.0f, 0.0f, mPos.z + 5.0f * (float)i),  (Enemy**)&(pBody[i]));
	}
}