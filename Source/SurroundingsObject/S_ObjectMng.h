#pragma once

#include <string>
#include <vector>		//可変長配列 std::vector を使う
#include <map>
using namespace std;

#include "Lib/Math/Vector.h"
using namespace Lib::Math;

//CSVデータを使うのでCSVFile.hを取り込む
#include "Fwk/File/CSVFile.h"

//S_Objectクラスの宣言をinclude
#include "S_Object.h"

//弾プールクラスを使うので#include
#include "S_ObjectPool.h"

//弾管理クラス
class S_ObjectMng
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
	void GeneratePool(string sObjectName, int poolSize);
	//指定した弾のオブジェクトプールの解放
	void DestroyPool(string sObjectName);
	//すべての弾のオブジェクトプールの解放
	void DestroyPoolAll();

	//周辺物の生成
	void CreateS_Object(string sObjectName, Vector3f pos,Vector3f vDirection,float speed,float maxSpeed,Sizef size,int collisionGroup);

	//残りの弾の数
	int GetS_ObjectCount();

private:
	//アクティブな弾オブジェクトのポインタ配列
	vector<S_Object*> mActiveS_Objects;
	//弾種別ごとのオブジェクトプール
	map<string, S_ObjectPool> mS_ObjectPools;
};