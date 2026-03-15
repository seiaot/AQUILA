#pragma once 

//弾プールクラス

//可変長配列std::vectorを使う
#include <vector>

#include "S_Object.h"

class S_ObjectPool
{

public:
	//初期化
	//class Tは、Init関数の呼び出し時に具体的な弾クラスに置き換わる。 
	template <class T>
	void Init(int poolSize)
	{
		//プールの全要素分の弾オブジェクトを生成
		for (int i = 0; i < poolSize; ++i)
		{
			//指定された弾クラスの変数を1つ生成
			//S_Object*にキャスト出来るクラスが指定されるのが前提
			S_Object* pNewS_Object = new T();
			//初期化
			pNewS_Object->Init();
			//最初は非アクティブ
			pNewS_Object->SetActive(false,false);
			//プールにアドレスを記録しておく
			mPool.push_back(pNewS_Object);
		}
	}

	//後片付け
	void Term();

	//オブジェクト１つ活性化して戻す
	S_Object* Allic();

protected:
	//S_Object*型の可変長配列
	std::vector<S_Object*> mPool;
};