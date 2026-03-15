#include "GroundMng.h"

#include "GameObjectMng/GameObjectMng.h"

//初期化
void GroundMng::Init()
{
	//アクティブな地面タイルのポインタの配列を空にしておく
	mActiveGrounds.clear();

	mColorChangeSprite.Init();
	mColorChangeSprite.SetColor(175.0f / 255.0f, 255.0f / 255.0f, 125.0f / 255.0f);
	mColorChangeSprite.SetSize(WINDOW_WIDTH * 2.0f, WINDOW_HEIGHT * 2.0f);
	mColorChangeSprite.SetPivot(0.5f, 1.0f);
	mColorChangeSprite.SetPriority(-10000);
	mColorChangeSprite.SetAlpha(0.9f);
}

//後片付け
void GroundMng::Term()
{
	//アクティブな地面タイルのポインタの配列を空にしておく
	mActiveGrounds.clear();

	mColorChangeSprite.Term();
}

//更新
void GroundMng::Update()
{
	//アクティブな地面タイルを全て更新する
	for (int i = 0; i < mActiveGrounds.size(); ++i)
	{
		//更新
		mActiveGrounds[i]->Update();
	}

	//更新の結果、非アクティブとなった要素を取り除く
	for (int i = 0; i < mActiveGrounds.size(); /*ここは書かない*/)
	{
		if (mActiveGrounds[i]->IsActive())
		{
			//mActiveEnemies[i]がアクティブのままであれば
			//次の要素を処理するのでiをインクリメント
			++i;
		}
		else
		{
			//mActiveEnemies[i]が非アクティブとなっていたらその要素を取り除く
			mActiveGrounds.erase(mActiveGrounds.begin() + i);

			//※ここでは i をインクリメントしない
		}
	}

	mColorChangeSprite.SetPosition(GetBG()->GetVanishingPoint());
	mColorChangeSprite.Update();
}

//描画
void GroundMng::Render()
{
	//アクティブな地面タイルを全て描画する
	for (int i = 0; i < mActiveGrounds.size(); ++i)
	{
		//描画
		mActiveGrounds[i]->Render();
	}

	mColorChangeSprite.Draw();
}

//オブジェクトプールの生成
void GroundMng::GeneratePool(int poolSize)
{
	//生成するクラスにGroundを指定
	mGroundPools.Init<Ground>(poolSize);
}

//地面タイルの生成
void GroundMng::CreateGround(int loopNum)
{
	Vector3f vPos = {};

	for (int i = 0;i < loopNum;++i)
	{
		vPos = { 0.0f, -WINDOW_HEIGHT / 2.0f, 1200.0f - 1200.0f / (float)loopNum * i};

		//指定の地面タイルプールから非アクティブのオブジェクトを取得する
		Ground* pGround = nullptr;

		//地面タイルプールから空きオブジェクトを取得
		pGround = mGroundPools.Allic();

		//非アクティブの地面タイルオブジェクトがなければ生成失敗
		if (pGround == nullptr)
		{
			return;
		}

		pGround->SetPosition(vPos);
		pGround->SetPriority(-100000 + i);

		if (i % 2 == 0)
		{
			pGround->ChangeTexture("Images/GP3/ground02.png");
		}

		//アクティブな配列に追加
		mActiveGrounds.push_back(pGround);
	}
}

//残りの地面タイルの数
int GroundMng::GetGroundCount()
{
	return (int)mActiveGrounds.size();
}