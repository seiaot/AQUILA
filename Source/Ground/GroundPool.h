#pragma once 

//地面タイルプールクラス

//可変長配列std::vectorを使う
#include <vector>

#include "Ground.h"

class GroundPool
{

public:
	//初期化
	//class Tは、Init関数の呼び出し時に具体的な地面タイルクラスに置き換わる。
	template<class T>
	void Init(int poolSize)
	{
		//プールの全要素分の地面タイルオブジェクトを生成
		for (int i = 0; i < poolSize; ++i)
		{
			//Groundクラスの変数を1つ生成
			Ground* pNewGround = new T();
			//初期化
			pNewGround->Init();
			//最初は非アクティブ
			pNewGround->SetActive(false);
			//プールにアドレスを記録しておく
			mPool.push_back(pNewGround);
		}
	}

	//後片付け
	void Term();

	//オブジェクト１つ活性化して戻す
	Ground* Allic();

protected:
	//Ground*型の可変長配列
	std::vector<Ground*> mPool;
};