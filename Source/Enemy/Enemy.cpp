#include "Enemy.h"

//コリジョングループの列挙型を使うために#include
#include "GameDef.h"
//Timeを使うのでinclude
#include "Fwk/Framework.h"

//初期化
void Enemy::Init()
{
	Object::Init();

	//Waveデータ読み込み
	mSound.Load("Sound/DamageE.wav");
	//音源
	mSoundSource.Init(mSound);

	//コリジョンの設定(共通項目のみ)
	{
		//自分は敵グループのコリジョン
		mCollision.SetGroup((int)CollisionGroup::Enemy);
		//衝突対象を追加
		mCollision.AddHitGroup((int)CollisionGroup::PlayerBullet);
		mCollision.SetOutlineColor(1.0f, 0.0f, 0.0f);
	}

	//ダメージ時に付ける赤色の強さ
	mDamagedColor = 0.0f;

	
}

//後片付け
void Enemy::Term()
{
	Object::Term();

	//サウンドの破棄
	mSound.Unload();
	//音源の解放
	mSoundSource.Term();
}

//ダメージを受ける
void Enemy::OnDamaged(int damage)
{
	//ダメージ分HPを減らす
	mHP -= damage;
	//HPが0以下になったら倒されたので非活性にする
	if (mHP <= 0)
	{
		SetActive(false,false);
		//やられた時の関数を呼び出す
		//派生クラスでOnDefeatedを上書きしていた場合にその関数が呼ばれる
		OnDefeated();
	}
	//ダメージ時に加算する赤色の強さを最大値に（1.0）に設定
	mDamagedColor = 1.0f;
	mSprite.SetAdditionalColor(mDamagedColor, 0.0, 0.0);

	//ダメージ音の再生
	//mSoundSource.Play();
}

//生成された時に呼び出す
//派生クラスで上書きして使う
void Enemy::OnCreated()
{
	//なし
}

//やられたときに呼び出される
//派生クラスで上書きして使う
void Enemy::OnDefeated()
{
	//なし
}

//派生ごとの更新
void Enemy::PersonalUpdate()
{
	//ダメージ時に加算する色（赤）の強さが0でなければ
	if (mDamagedColor > 0.0f)
	{
		//0.25秒くらいで0に戻るように減算
		mDamagedColor -= Time_I->GetDeltaTime() * 4.0f;
		//マイナスの値にならないように調整
		if (mDamagedColor < 0.0f)
		{
			mDamagedColor = 0.0f;
		}
		//加算する赤色の強さを指定
		mSprite.SetAdditionalColor(mDamagedColor, 0.0f, 0.0f);
	}
}

//速度の更新
void Enemy::UpdateVelocity()
{
	float acceleration = mSpeed * Time_I->GetDeltaTime();	//このフレーム速度
	float maxSpeed = mMaxSpeed * Time_I->GetDeltaTime();	//最大速度

	//速度減衰
	mVelocity *= 0.8f;
	//値が0にある程度近ければ0にする
	if (mVelocity.GetLength() <= 0.001f)
	{
		mVelocity *= 0.0f;
	}

	//向きに合わせて加速
	mVelocity += mDirection * acceleration;

	//速度が上限を超えないようにする
	if (fabsf(mVelocity.GetLength()) > maxSpeed)
	{
		mVelocity = mVelocity.GetUnitVector() * maxSpeed;
	}
}