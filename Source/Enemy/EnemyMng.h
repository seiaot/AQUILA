#pragma once

#include <string>
#include <vector>		//可変長配列 std::vector を使う
#include <map>
using namespace std;

#include "Lib/Math/Vector.h"
using namespace Lib::Math;

//CSVデータを使うのでCSVFile.hを取り込む
#include "Fwk/File/CSVFile.h"

//Enemyクラスの宣言をinclude
#include "Enemy.h"

//敵プールクラスを使うので#include
#include "EnemyPool.h"

//敵管理クラス
class EnemyMng
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
	void GeneratePool(string enemyName, int poolSize);
	//指定した敵のオブジェクトプールの解放
	void DestroyPool(string enemyName);
	//すべての敵のオブジェクトプールの解放
	void DestroyPoolAll();

	//敵の生成
	void CreateEnemy(string enemyName, Vector3f pos,Enemy** p = nullptr);

	//残りの敵の数
	int GetEnemyCount();

	//CSVデータから敵を生成する
	void CreateEnemies(CSVData* pCsvData);

private:
	//アクティブな敵オブジェクトのポインタ配列
	vector<Enemy*> mActiveEnemies;
	//敵種別ごとのオブジェクトプール
	map<string, EnemyPool> mEnemyPools;
};