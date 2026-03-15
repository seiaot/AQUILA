#pragma once 

//弾プールクラス

//可変長配列std::vectorを使う
#include <vector>

#include "Effect.h"

class EffectPool
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
			//Effect*にキャスト出来るクラスが指定されるのが前提
			Effect* pNewEffect = new T();
			//初期化
			pNewEffect->Init();
			//最初は非アクティブ
			pNewEffect->SetActive(false,false);
			//プールにアドレスを記録しておく
			mPool.push_back(pNewEffect);
		}
	}

	//後片付け
	void Term();

	//オブジェクト１つ活性化して戻す
	Effect* Allic();

protected:
	//Effect*型の可変長配列
	std::vector<Effect*> mPool;
};