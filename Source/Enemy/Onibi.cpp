#include "Onibi.h"

//CollisionManagerを使うのでinclude
#include "Fwk/Framework.h"
//Tilemapを使うのでinclude
//プレイヤーの位置を参照するので
#include "GameObjectMng/GameObjectMng.h"

//初期化
void Onibi::Init()
{
	//テクスチャの読み込み
	mTexture.Load("Images/2dAction/onibi.png");

	mSize = { 120.0f,168.0f,50.0f };

	//敵共通の初期化処理
	Enemy::Init();

	//テクスチャの描画範囲を指定
	mSprite.SetTexCoord(0.0f, 0.0f, 0.5f, 0.5f);
	//Pivotを調整。真ん中やや下へ
	mSprite.SetPivot(0.5f, 0.25f);

	//スプライトにアニメーションを追加
	_initAnimation();


	//コリジョンの設定
	{
		//コリジョンにタグを設定
		mCollision.SetTag("Onibi");
		//コリジョンの形状を指定（中心位置はmPositionと同じ、サイズはスプライトより少し小さめに）
		mCollision.SetSphere(0.0f, 0.0f,0.0f, 20.0f);
	}

	//HPを設定
	mHP = 3;
}

//アニメーションの初期化
void Onibi::_initAnimation()
{
	//UVの幅と高さ
	float uvW = 20.0f / 128.0f;
	float uvH = 28.0f / 64.0f;

	//スプライトセル構造体リスト(12パターン分)
	UVRect uvs[] =
	{
		{uvW * 0.0f,uvH * 0.0f,uvW,uvH},
		{uvW * 1.0f,uvH * 0.0f,uvW,uvH},
		{uvW * 2.0f,uvH * 0.0f,uvW,uvH},
		{uvW * 3.0f,uvH * 0.0f,uvW,uvH},
		{uvW * 4.0f,uvH * 0.0f,uvW,uvH},
		{uvW * 5.0f,uvH * 0.0f,uvW,uvH},

		{uvW * 0.0f,uvH * 1.0f,uvW,uvH},
		{uvW * 1.0f,uvH * 1.0f,uvW,uvH},
		{uvW * 2.0f,uvH * 1.0f,uvW,uvH},
		{uvW * 3.0f,uvH * 1.0f,uvW,uvH},
		{uvW * 4.0f,uvH * 1.0f,uvW,uvH},
		{uvW * 5.0f,uvH * 1.0f,uvW,uvH},
	};

	//アニメ
	UVRect idle[] =
	{
		uvs[0],uvs[1],uvs[2],uvs[3],uvs[4],uvs[5],
		uvs[6],uvs[7],uvs[8],uvs[9],uvs[10],uvs[11],
	};

	//アニメーションデータ
	Animation anim;

	//アニメーションデータ作成
	CreateAnimationUV(anim, "idle", 12, 1.0f, true, idle);

	//アニメーションをスプライトに登録
	mSprite.AddAnimation(anim);

	//初期のアニメ設定
	mSprite.PlayAnimation("idle");
}

//生成された時に呼び出される
void Onibi::OnCreated()
{
	//体力を初期化
	mHP = 3;
}

//鬼火特有の動き
void Onibi::PersonalUpdate()
{
	//二次元時
	////プレイヤーの位置
	//Vector2f player2DPos = GetPlayer()->Get2DPosition();
	////現在位置からプレイヤー位置までの向きベクトル
	//Vector2f m2DPos = { mPos.x,mPos.y };
	//Vector2f vDirection = (player2DPos - m2DPos).GetNormalized();
	////このフレームでの移動量
	//float speed = 60.0f * Time_I->GetDeltaTime();
	////向きベクトルを移動量でスカラー倍して現在位置に足しこむ
	//mPos.x += (vDirection * speed).x;
	//mPos.y += (vDirection * speed).y;

	//三次元時
	//プレイヤーの位置
	Vector3f playerPos = GetPlayer()->Get3DPosition();
	//現在位置からプレイヤー位置までの向きベクトル
	Vector3f vDirection = (playerPos - mPos).GetUnitVector();
	//このフレームでの移動量
	float speed = 120.0f * Time_I->GetDeltaTime();
	//向きベクトルを移動量でスカラー倍して現在位置に足しこむ
	mPos += (vDirection * speed);
}