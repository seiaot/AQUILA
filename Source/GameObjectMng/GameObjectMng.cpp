#include "GameObjectMng.h"

//CSVファイルの読み込み
#include "Fwk/File/CSVFile.h"

//Key-Valueファイルの読み込み
#include "Fwk/File/KeyValueFile.h"

//RenderMangerを使うのでinclude
#include "Fwk/Framework.h"

//画面サイズの定数を使うのでinclude
#include "AppDef.h"

Player mPlayer;
EnemyMng mEnemyMng;
BulletMng mBulletMng;
S_ObjectMng mS_Object;
EffectMng mEffectMng;
GroundMng mGroundMng;
BG mBG;

//全オブジェクト初期化
void GameObjectInit()
{
	//カメラの初期化
	RenderManager_I->GetCamera().SetPosition(0.0f, 0.0f, 0.0f);
	RenderManager_I->GetCamera("default3D").SetPosition(0.0f, 0.0f, 0.0f);
	RenderManager_I->GetCamera("BackGround").SetPosition(0.0f, 0.0f, 0.0f);

	mPlayer.Init();

	//敵マネージャーの初期化
	mEnemyMng.Init();
	//スライムのオブジェクトプールを確保
	mEnemyMng.GeneratePool("Shooter", 200);
	//鬼火のオブジェクトプールを確保
	mEnemyMng.GeneratePool("Onibi", 2);
	//ドラゴンのオブジェクトプールを確保
	mEnemyMng.GeneratePool("Dragon", 1);
	//ドラゴン（体）のオブジェクトプールを確保
	mEnemyMng.GeneratePool("DragonBody", 30);

	//弾マネージャ
	mBulletMng.Init();
	//通常弾のプールを確保
	mBulletMng.GeneratePool("NormalBullet", 100);
	//炎のプールを確保
	mBulletMng.GeneratePool("Fire", 100);

	//周辺物
	mS_Object.Init();
	mS_Object.GeneratePool("Cloud", 100);
	mS_Object.GeneratePool("GameClear", 1);

	//エフェクト
	mEffectMng.Init();
	mEffectMng.GeneratePool("Hit", 30);
	mEffectMng.GeneratePool("Explosion", 30);
	mEffectMng.GeneratePool("SubExplosion", 30);
	mEffectMng.GeneratePool("ExplosionEX", 30);

	mGroundMng.Init();
	//地面のプールを確保
	mGroundMng.GeneratePool(96);
	//地面の生成
	mGroundMng.CreateGround(96);

	CSVFile csvFile;
	csvFile.Load("Data/enemy_arrangement.csv");
	//読み込んだCSVファイル内容をコンソールに出力する（デバッグ用）
	csvFile.PrintCSVData();
	//CSVデータから敵を配置する
	mEnemyMng.CreateEnemies(csvFile.GetCSVData());

	mBG.Init();
}

//全オブジェクト更新
void GameObjectUpdate()
{
	//最初にプレイヤーの更新
	mPlayer.Update();

	//敵マネージャーの更新
	mEnemyMng.Update();

	//弾マネージャーの更新
	mBulletMng.Update();

	//周辺物
	mS_Object.Update();

	//エフェクト
	mEffectMng.Update();

	mBG.Update();
	mGroundMng.Update();
}

//全オブジェクト描画
void GameObjectRender()
{
	mPlayer.Render();
	mEnemyMng.Render();
	mBulletMng.Render();

	//周辺物
	mS_Object.Render();

	//エフェクト
	mEffectMng.Render();

	mBG.Render();
	mGroundMng.Render();
}

//全オブジェクト解放
void GameObjectTerm()
{
	mPlayer.Term();
	mEnemyMng.Term();
	mBulletMng.Term();

	//周辺物
	mS_Object.Term();

	//エフェクト
	mEffectMng.Term();

	mBG.Term();
	mGroundMng.Term();
}

//クラスのポインタを返す
Player* GetPlayer()
{
	return &mPlayer;
}

EnemyMng* GetEnemyMng()
{
	return &mEnemyMng;
}

BulletMng* GetBulletMng()
{
	return &mBulletMng;
}

S_ObjectMng* GetSObjectMng()
{
	return &mS_Object;
}

EffectMng* GetEffectMng()
{
	return &mEffectMng;
}

GroundMng* GetGroundMng()
{
	return &mGroundMng;
}

BG* GetBG()
{
	return &mBG;
}