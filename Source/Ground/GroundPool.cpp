#include "GroundPool.h"

//CollisionManagerを使うのでinclude
#include "Fwk/Framework.h"

//後片付け
void GroundPool::Term()
{
	//Ground*配列の各要素にnewされた地面タイルのオブジェクトのアドレスが
	//記録されているので、１つ１つDELETEしていく
	for (int i = 0; i < mPool.size(); ++i)
	{
		SAFE_DELETE(mPool[i]);
	}
	//プールを破棄
	mPool.clear();
}

//地面タイルを１つ活性化して戻す
Ground* GroundPool::Allic()
{
	//戻り値を記録する変数
	Ground* pResult = nullptr;

	for (int i = 0; i < mPool.size(); ++i)
	{
		//非アクティブの地面タイルを見つけたら活性化して戻り値にする
		if (mPool[i]->IsActive() == false)
		{
			//戻り値にする
			pResult = mPool[i];
			//活性化しておく
			pResult->SetActive(true);
			//ループは抜けてよい
			break;
		}
	}
	return pResult;
}