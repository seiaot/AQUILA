#pragma once

#include <string>
#include <vector>		//可変長配列 std::vector を使う
#include <map>
using namespace std;

#include "Lib/Math/Vector.h"
using namespace Lib::Math;

//CSVデータを使うのでCSVFile.hを取り込む
#include "Fwk/File/CSVFile.h"

//Bulletクラスの宣言をinclude
#include "Bullet.h"

//弾プールクラスを使うので#include
#include "BulletPool.h"

//弾管理クラス
class BulletMng
{
public:
	//初期化
	void Init();
	//後片付け
	void Term();
	//更新
	void Update();
	//描画
	void Render();

	//オブジェクトプールの生成
	void GeneratePool(string bulletName, int poolSize);
	//指定した弾のオブジェクトプールの解放
	void DestroyPool(string bulletName);
	//すべての弾のオブジェクトプールの解放
	void DestroyPoolAll();

	//弾の生成
	void CreateBullet(string bulletName, Vector3f pos,Vector3f vDirection,float speed,float maxSpeed,Sizef size,int collisionGroup);

	//残りの弾の数
	int GetBulletCount();

private:
	//アクティブな弾オブジェクトのポインタ配列
	vector<Bullet*> mActiveBullets;
	//弾種別ごとのオブジェクトプール
	map<string, BulletPool> mBulletPools;
};