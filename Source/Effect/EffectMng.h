#pragma once

#include <string>
#include <vector>		//可変長配列 std::vector を使う
#include <map>
using namespace std;

#include "Lib/Math/Vector.h"
using namespace Lib::Math;

//CSVデータを使うのでCSVFile.hを取り込む
#include "Fwk/File/CSVFile.h"

//Effectクラスの宣言をinclude
#include "Effect.h"

//弾プールクラスを使うので#include
#include "EffectPool.h"

//弾管理クラス
class EffectMng
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
	void GeneratePool(string effectName, int poolSize);
	//指定した弾のオブジェクトプールの解放
	void DestroyPool(string effectName);
	//すべての弾のオブジェクトプールの解放
	void DestroyPoolAll();

	//弾の生成
	void CreateEffect(string effectName, Vector3f pos,Vector3f vDirection,float speed,float maxSpeed,Sizef size,int collisionGroup);

	//残りの弾の数
	int GetEffectCount();

private:
	//アクティブな弾オブジェクトのポインタ配列
	vector<Effect*> mActiveEffects;
	//弾種別ごとのオブジェクトプール
	map<string, EffectPool> mEffectPools;
};