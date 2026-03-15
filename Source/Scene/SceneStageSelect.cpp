#include "SceneStageSelect.h"

#include "Fwk/Framework.h"

//ステージ情報マスタを使う
#include "Mst/StageDataMst.h"
//ゲームプレイデータを使う
#include "GamePlayData/GamePlayData.h"

//初期化
void SceneStageSelect::Init()
{
	Scene::Init();
}

//終了
void SceneStageSelect::Term()
{
	//なし
}

//更新
void SceneStageSelect::Update()
{
	//ステージマスタからステージ情報数を取得
	int stageNum = GetStageDataMst().Size();

	//下ボタンを押したら　Sキーも
	if (Input_I->IsKeyDown(VK_DOWN) || Input_I->IsKeyDown('S') || Input_I->IsButtonDown(0, GAMEPAD_BUTTON::DOWN))
	{
		//次のステージを選択する
		mSelectedIndex += 1;
		//インデックスがステージ数以上になった場合は0に戻す
		if (mSelectedIndex >= stageNum)
		{
			mSelectedIndex = 0;
		}
	}

	//上ボタンを押したら　Wキーも
	if (Input_I->IsKeyDown(VK_UP) || Input_I->IsKeyDown('W') || Input_I->IsButtonDown(0, GAMEPAD_BUTTON::UP))
	{
		//前のステージを選択する
		mSelectedIndex -= 1;
		//インデックスが0未満になった場合は一番下のステージを選択するようにする
		if (mSelectedIndex < 0)
		{
			mSelectedIndex = stageNum - 1;
		}
	}

	//Aボタンを押したら
	if (Input_I->IsKeyDown('Z') || Input_I->IsButtonDown(0, GAMEPAD_BUTTON::A))
	{
		//ゲームプレイデータに、選択したステージの番号を記録しておく
		GetGamePlayData().SetStageNo(mSelectedIndex);
		//"次のシーン"にInGameを設定する
		mNextScene = SceneType::InGame;
	}
}

//描画
void SceneStageSelect::Render()
{
	PrintText("[ステージ選択]", 400.0f, 50.0f);

	//ステージマスタからステージ情報数を取得
	int stageNum = GetStageDataMst().Size();

	//選択中の文字色
	float color_selected[3] = { 1.0f,1.0f,0.0f };
	//通常の文字色
	float color_norma[3] = { 0.85f,0.0f,0.85f };

	//文字列を描画する
	for (int i = 0; i < stageNum; ++i)
	{
		//ステージ情報マスタからステージ情報を取得
		StageData* pData = GetStageDataMst().Get(i);
		//文字色選択
		float* pColor = (mSelectedIndex == i) ? color_selected : color_norma;

		//クリアしているステージであれば[済]表示
		if (GetGamePlayData().IsStageCleared(i))
		{
			PrintText("[済]", 300.0f, 120.0f + i * 40.0f, pColor);
		}

		//ステージ名の表示
		PrintText(pData->Name, 400.0f, 120.0f + i * 40.0f, pColor);
	}
}