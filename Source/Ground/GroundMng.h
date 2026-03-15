#pragma once

#include <string>
#include <vector>		//可変長配列 std::vector を使う
#include <map>
using namespace std;

#include "Lib/Math/Vector.h"
using namespace Lib::Math;

//Groundクラスの宣言をinclude
#include "Ground.h"

//地面タイルプールクラスを使うので#include
#include "GroundPool.h"

//地面タイル管理クラス
class GroundMng
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
	void GeneratePool(int poolSize);

	//地面タイルの生成
	void CreateGround(int loopNum);

	//残りの地面タイルの数
	int GetGroundCount();

private:
	//アクティブな地面タイルオブジェクトのポインタ配列
	vector<Ground*> mActiveGrounds;
	//オブジェクトプール
	GroundPool mGroundPools;

	//地面の色
	Sprite mColorChangeSprite;
};