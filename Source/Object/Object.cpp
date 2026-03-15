#include "Object.h"

//プレイヤーのアドレスを得るため
#include "GameObjectMng/GameObjectMng.h"
//コリジョングループの列挙型を使うために#include
#include "GameDef.h"
//CollisionManagerを使うのでinclude
#include "Fwk/Framework.h"
//画面サイズの定数を使うのでinclude
#include "AppDef.h"

#include "Ground/Ground.h"

//初期化
void Object::Init()
{
	//状態をアクティブに
	mIsActive = true;

	//スプライト関連
	mSprite.Init();
	mSprite.SetTexture(mTexture);
	mSprite.SetPosition(mPos.x,mPos.y);
	mSprite.SetSize(mSize.w,mSize.h);

	//コリジョンの設定(共通項目のみ)
	{
		//コリジョンのオーナーに自分を設定
		mCollision.SetOwner(this);
		//コリジョンを非活性状態にしておく
		mCollision.SetActive(false);
		//コリジョンマネージャーにコリジョンを登録
		CollisionManager_I->Register(&mCollision);
	}
}

//終了
void Object::Term()
{
	//コリジョンマネージャから登録解除
	CollisionManager_I->Unregister(&mCollision);

	//スプライトの破棄
	mSprite.Term();
	//テクスチャの破棄
	mTexture.Unload();
}

//更新
void Object::Update()
{
	//非アクティブにする
	if (Isinactive())
	{
		SetActive(false, false);
	}

	//非アクティブなら更新しない
	if (!mIsActive)
	{
		return;
	}

	//描画範囲内に初めて入ったらコリジョンを有効に
	if (!(mCollision.IsActive()) && IsInDrawingRange())
	{
		mCollision.SetActive(true);
	}

	//コリジョンの状態で更新を判定(描画範囲内に入ったら固有・共通の動きを開始する)
	if (!mCollision.IsActive())
	{
		return;
	}

	//派生先固有の更新
	PersonalUpdate();

	//表示位置の変更
	SetSpritePosition();
	//表示サイズの変更
	SetSpriteSize();

	//透明度を設定
	//mSprite.SetAlpha(GetRate());


	//コリジョンの位置更新
	mCollision.SetPosition(mPos);
	//描画順の設定
	mSprite.SetPriority(int(1200000.0f - GetDistanceCameraZ() * 1000.0f));

	//スプライトのアップデート
	mSprite.Update();
}

//描画
void Object::Render()
{
	//描画範囲内の時のみ表示
	if (IsInDrawingRange())
	{
		mSprite.Draw();
	}
}

//アクティブか？
bool Object::IsActive() const
{
	return mIsActive;
}

//アクティブ状態の設定
void Object::SetActive(const bool IsActive,const bool CollisionActive)
{
	mIsActive = IsActive;
	//コリジョンも
	mCollision.SetActive(CollisionActive);
}

//位置の設定
void Object::SetPosition(Vector3f vPos)
{
	mPos = vPos;
	//コリジョンの位置も
	mCollision.SetPosition(vPos);
}

//位置の取得
Vector2f Object::Get2dPosition() const
{
	return Vector2f(mPos.x, mPos.y);
}

Vector3f Object::Get3dPosition() const
{
	return mPos;
}

//プレイヤーとのZ軸の距離を取得
float Object::GetDistancePlayerZ() const
{
	float reDis = 0.0f;	//戻り値用

	//自分の位置からプレイヤーの位置を引く
	reDis = mPos.z - GetPlayer()->Get3DPosition().z;
	
	return reDis;
}

//視点からの距離を取得
float Object::GetDistanceCameraZ() const
{
	float reDis = 0.0f; //戻り値用

	//自分の位置から視点の位置を引く
	reDis = mPos.z - RenderManager_I->GetCameraPosition().z;

	return reDis;
}

//非アクティブにする(派生毎)
bool Object::Isinactive()
{
	//コリジョンが有効かつ描画範囲内から出たら
	if (mCollision.IsActive() && !IsInDrawingRange())
	{
		return true;
	}

	return false;
}

//派生ごとの更新
void Object::PersonalUpdate()
{

}

//描画範囲内かどうか
bool Object::IsInDrawingRange()
{
	bool reB = false;	//戻り値

	//カメラとの距距離
	float disZ = GetDistanceCameraZ();

	//カメラとの差がカメラの映す範囲内ならtrue
	if (disZ >= CAMERA_NEAR && disZ <= CAMERA_FAR )
	{
		reB = true;
	}

	return reB;
}

//プレイヤーとの距離に基づく倍率を取得
float Object::GetRate()
{
	float reRate = 0.0f;	//戻り値用

	float playerZ = GetPlayer()->Get3DPosition().z;	//プレイヤーのZ座標

	//Z軸がプレイヤーと同じとき1倍(1.0f)にする
	//プレイヤーより前なら1.0f未満,後ろなら1.0fより大きくなる
	reRate = (1000.0f - GetDistancePlayerZ()) / 1000.0f;

	return reRate;
}

//表示用の位置を設定
void Object::SetSpritePosition()
{
	Vector2f origin = GetBG()->GetVanishingPoint();	//画面上の中心の座標
	Vector2f pos2d = { mPos.x,mPos.y };	//現在位置のxとyを取り出す

	Vector2f vMove = pos2d - origin;	//中心から現在位置に向かうベクトル
	Vector2f NormalizedMove = vMove.GetNormalized();	//正規化

	//プレイヤーとの距離で位置を変化
	mSprite.SetPosition(origin + (vMove * GetRate()));
}

//表示サイズを変更
void Object::SetSpriteSize()
{
	float fRate = GetRate();	//倍率の取得

	//プレイヤーとの距離で大きさを変化
	mSprite.SetSize(mSize.w * fRate, mSize.h * fRate);

}