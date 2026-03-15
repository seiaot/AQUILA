#include "Player.h"

//Input_Iを使う為のヘッダファイルをinclude
#include "Fwk/Framework.h"
//GetMapを使う為GameObjectMng.hをinclude
#include "GameObjectMng/GameObjectMng.h"
//画面サイズの定数を使うのでinclude
#include "AppDef.h"
//Objectクラスを使用
#include "Object/Object.h"

//プレイヤーの初期化
void Player::Init()
{
	//位置の初期化
	mPos = { 0.0f,0.0f,0.0f };
	//大きさの初期化
	mSize = { 256.0f,128.0f,192.0f };
	//速度の初期化
	mVelocity = { 0.0f,0.0f,0.0f };

	//テクスチャの読み込み
	mTexture.Load("Images/GP3/Player.png");

	//スプライトの初期化
	mSprite.Init();
	mSprite.SetTexture(mTexture);
	mSprite.SetSize(mSize.w , mSize.h);
	mSprite.SetPosition(mPos.x,mPos.y);

	//mSprite.SetTexCoord(0.5f, 0.0f, 0.125f, 1.0f);

	/*mSprite.SetVertexPosition(0, -1.0, 0.5, 0.0);
	mSprite.SetVertexPosition(1, 1.0, 0.5, 0.0);*/

	//コリジョンの設定
	{
		//自分はプレイヤーグループのコリジョン
		mCollision.SetGroup((int)CollisionGroup::Player);
		//衝突対象を追加
		mCollision.AddHitGroup((int)CollisionGroup::Enemy);
		mCollision.AddHitGroup((int)CollisionGroup::EnemyBullet);
		//コリジョンにタグを設定
		mCollision.SetTag("Player");
		//コリジョンのオーナーに自分を設定
		mCollision.SetOwner(this);
		//コリジョンを活性にしておく
		mCollision.SetActive(true);
		//コリジョンの形状を指定
		mCollision.SetCube(Vector3f(0.0f, 0.0f, 0.0f), Sizef(mSize.w, mSize.h * 0.5f, mSize.d * 0.8f));
		//コリジョン衝突時のコールバック関数を設定
		mCollision.SetCallbackFunction(CreateCollisionEventCallback(Player::OnHit));
		//コリジョンマネージャーにコリジョンを登録
		CollisionManager_I->Register(&mCollision);
	}

	//角度
	mAngle = { 0.0f,0.0f,0.0f };
	//タイマー
	mTimer = 0.0f;
	mInvincibleTimer = 0.0f;
	//HP
	mHPUI.Init();
	mHP = 4;

	//クリア
	mClear = false;
}

//プレイヤーの更新
void Player::Update()
{
	//移動
	if (mHP > 0)
	{
		UpdateMoving();
	}

	//カメラ(2D)のz軸をプレイヤーに追従
	RenderManager_I->SetCameraPosition(Vector3f(0.0f, 0.0f, mPos.z - 200.0f));

	//無敵時間の減少
	if (mInvincibleTimer > 0.0f)
	{
		mInvincibleTimer -= Time_I->GetDeltaTime();
		mSprite.SetVisible(!mSprite.IsVisible());
	}
	else if (!mSprite.IsVisible())
	{
		mSprite.SetVisible(true);
	}

	mSprite.SetPosition(mPos.x,mPos.y);
	mCollision.SetPosition(mPos.x, mPos.y,mPos.z);
	//描画順の設定
	mSprite.SetPriority(int(1200000.0f - (mPos.z - RenderManager_I->GetCameraPosition().z) * 1000.0f));

	mSprite.Update();

	//HPがゼロだと爆発
	if (mHP <= 0)
	{
		GetEffectMng()->CreateEffect("ExplosionEX", mPos, Vector3f(), 0.0f, 0.0f, mSize, (int)CollisionGroup::Effect);
		mSprite.SetVisible(false);
	}

	//HP
	mHPUI.Update();
}

//描画
void Player::Render()
{
	mSprite.Draw();
	//HP
	mHPUI.Render();
}

//プレイヤーの後片付け
void Player::Term()
{
	//コリジョンマネージャーから登録解除
	CollisionManager_I->Unregister(&mCollision);

	mTexture.Unload();
	mSprite.Term();
	//HP
	mHPUI.Term();

}

//現在位置を取得する(三次元)
Vector3f Player::Get3DPosition() const
{
	return mPos;
}

//現在位置を取得する(二次元)
Vector2f Player::Get2DPosition() const
{
	Vector2f v2Pos = { mPos.x,mPos.y };
	return v2Pos;
}

//移動量の取得
Vector3f Player::GetVelocity() const
{
	return mVelocity;
}

//ステージクリア状態の設定・取得
void Player::SetClear(bool b)
{
	mClear = b;
}

bool Player::IsClear() const
{
	return mClear;
}

//やられているか否か
bool Player::IsDead() const
{
	if (mHP <= 0)
	{
		return true;
	}

	return false;
}

//HPの取得
int Player::GetHP() const
{
	return mHP;
}

//移動時処理
void Player::UpdateMoving()
{
	Vector3f vInput = {};	//入力用

	Vector2f vPadInput = {};	//ゲームパッド入力用
	//ゲームパッドの入力を取得
	Input_I->GetStickInput(0, GAMEPAD_STICK::LEFT, &vPadInput);

	//ゲームパットの2軸の入力を3軸に入れる
	vInput.x = vPadInput.x;
	vInput.y = vPadInput.y;

	//キーボードでの移動速度
	if (Input_I->IsKeyPressed('W'))
	{
		vInput.y += 1.0f;
	}
	else if (Input_I->IsKeyPressed('A'))
	{
		vInput.x -= 1.0f;
	}
	else if (Input_I->IsKeyPressed('S'))
	{
		vInput.y -= 1.0f;
	}
	else if (Input_I->IsKeyPressed('D'))
	{
		vInput.x += 1.0f;
	}
	
	//角度の更新
	UpdateAngle(vInput);

	//弾の発射
	if (Input_I->IsButtonDown(0, GAMEPAD_BUTTON::X) || Input_I->IsKeyPressed('Z'))
	{
		GetBulletMng()->CreateBullet("NormalBullet", mPos, Vector3f(0.0f,0.0f,1.0f), 1000.0f,1000.0f,Sizef(64.0f,64.0f,64.0f),(int)CollisionGroup::Player);
	}

	mTimer += Time_I->GetDeltaTime();

	/*if (mTimer >= 0.1f)
	{
		GetBulletMng()->CreateBullet("NormalBullet", mPos, mCollision.GetShapeCube().vDirection[2], 1000.0f, 1000.0f, Sizef(100.0f, 100.0f, 100.0f), (int)CollisionGroup::Player);

		mTimer = 0.0f;

	}*/

	//速度の更新
	UpdateVelocity(vInput);
	//現在の位置に速度をたす
	mPos += mVelocity;

	//画面外に行かないようにする
	if (fabsf(mPos.x) >= WINDOW_WIDTH / 2.0f - mSize.w / 2.0f )
	{
		mPos.x = mPos.x / fabsf(mPos.x) * (WINDOW_WIDTH / 2.0f - mSize.w / 2.0f);
	}
	if (fabsf(mPos.y) >= WINDOW_HEIGHT / 2.0f - mSize.h / 2.0f)
	{
		mPos.y = mPos.y / fabsf(mPos.y) * (WINDOW_HEIGHT / 2.0f - mSize.h / 2.0f);
	}
}

//角度の更新
void Player::UpdateAngle(Vector3f vInput)
{
	//各軸傾きが徐々に戻る
	for (int i = 0;i < 3;++i)
	{
		if (*(&mAngle.x + i))
		{
			*(&mAngle.x + i) += (*(&mAngle.x + i) > 0) ? -0.8f : 0.8f;
		}
	}

	//右回転
	if (Input_I->IsButtonPressed(0, GAMEPAD_BUTTON::RIGHT_SHOULDER))
	{
		mAngle.z -= 5.0f;
	}
	//左回転
	if (Input_I->IsButtonPressed(0, GAMEPAD_BUTTON::LEFT_SHOULDER))
	{
		mAngle.z += 5.0f;
	}

	//移動時少し傾く
	if (vInput.x)
	{
		mAngle.z += (vInput.x > 0) ? -1.5f : 1.5f;
	}
	if (vInput.y)
	{
		mAngle.x += (vInput.y > 0) ? 1.5f : -1.5f;
	}

	//Z軸の角度は90が限界
	if (fabs(mAngle.z) >= 90.0f)
	{
		mAngle.z = (mAngle.z >= 0) ? 90.0f : -90.0f;
	}
	//X軸の角度は40が限界
	if (fabs(mAngle.x) >= 40.0f)
	{
		mAngle.x = (mAngle.x >= 0) ? 40.0f : -40.0f;
	}

	//コリジョンとスプライトに反映
	mCollision.SetCubeRotation(mAngle);
	mSprite.SetRotation(mAngle.x, mAngle.y, mAngle.z);
}

//速度の更新
void Player::UpdateVelocity(const Vector3f vInput)
{
	float acceleration = 400.0f * Time_I->GetDeltaTime();	//このフレームでのxyの加速量

	float forwardSpeedScale = 1.0f;							//前進速度の倍率
	float forwardSpeed = 300.0f * Time_I->GetDeltaTime() * forwardSpeedScale;	//このフレームでの前進速度(z軸)

	float maxSpeed[3] =			//このフレームでの各軸の最大速度
	{
		2000.0f * Time_I->GetDeltaTime(),
		2000.0f * Time_I->GetDeltaTime(),
		300.0f * Time_I->GetDeltaTime()
	};

	float* pVelocity = &mVelocity.x;	//mVelocity.xのアドレスyzと連なる

	//速度減衰
	{
		mVelocity *= 0.8f;
		//各軸の値が0にある程度近ければ0にする
		for (int i = 0;i < 3;++i)
		{
			if (fabsf(pVelocity[i]) <= 0.001f)
			{
				pVelocity[i] = 0.0f;
			}
		}
	}

	//常時前進
	mVelocity.z += forwardSpeed;

	//速度に、加速分を追加
	mVelocity += vInput * acceleration;

	//入力がなく画面の中心以外にいるなら中心に向かって移動する(Aボタンが押されているときは無効)
	if (!vInput.x && !vInput.y && (mPos.x || mPos.y) && !Input_I->IsButtonPressed(0, GAMEPAD_BUTTON::A))
	{
		Vector2f vD = (Vector2f(0.0f, 0.0f) - Get2DPosition());		//現在地から中心に向かうベクトル

		//中心に戻る動きの方が移動より遅い
		mVelocity.x += (vD.GetNormalized() * acceleration * 0.5f).x;
		mVelocity.y += (vD.GetNormalized() * acceleration * 0.5f).y;

		//中心への距離が移動量以下ならば次の処理で中心と一致する移動量にする
		if (vD.GetLength() <= Vector2f(mVelocity).GetLength())
		{
			//1倍だと中心にと一致するまでしばらくかかる
			mVelocity.x = vD.x * 0.9f;
			mVelocity.y = vD.y * 0.9f;
		}
	}

	//速度が上限を超えないようにする
	for (int i = 0;i < 3;++i)
	{
		if (fabsf(pVelocity[i]) > maxSpeed[i])
		{
			pVelocity[i] = pVelocity[i] / fabsf(pVelocity[i]) * maxSpeed[i];
		}
	}
}

//衝突イベントのコールバック関数
void Player::OnHit(const Collider& pCollisionA, const Collider& pCollisionB)
{
	//無敵時間じゃないか
	if (mInvincibleTimer <= 0.0f && (pCollisionB.GetGroup() == (int)CollisionGroup::EnemyBullet || pCollisionB.GetGroup() == (int)CollisionGroup::Enemy))
	{
		DebugLog("Hit");
		mInvincibleTimer = 1.0f;
		mHP -= 1;
	}
}