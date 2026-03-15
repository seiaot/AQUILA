#pragma once

#include <cstdint>
#include <windows.h>

#include "GamePad.h"

#include "Lib/Math/Vector.h"

namespace Lib {
namespace System {

//入力状態取得インターフェース
class IInput
{
public:
	//キーの状態を取得する（キーが押されていないか？）
	virtual bool IsKeyFree(int KeyCode) = 0;
	//キーの状態を取得する（キーが押された瞬間か？）
	virtual bool IsKeyDown(int KeyCode) = 0;
	//キーの状態を取得する（キーが継続して押されているか？）
	virtual bool IsKeyPressed(int KeyCode) = 0;
	//キーの状態を取得する（キーを離した瞬間か？）
	virtual bool IsKeyUp(int KeyCode) = 0;
	//マウスの位置
	virtual const Lib::Math::Vector2f& GetMousePoint()const = 0;

	//ゲームパッド関連
	//パッドの番号は0～3

	//ゲームパッドのボタンの状態を取得する（ボタンが押されていないか？）
	virtual bool IsButtonFree(int padNo, GAMEPAD_BUTTON button) = 0;
	//キーの状態を取得する（キーが押された瞬間か？）
	virtual bool IsButtonDown(int padNo, GAMEPAD_BUTTON button) = 0;

	//キーの状態を取得する（キーが継続して押されているか？）
	virtual bool IsButtonPressed(int padNo, GAMEPAD_BUTTON button)  = 0;

	//キーの状態を取得する（キーを離した瞬間か？）
	virtual bool IsButtonUp(int padNo, GAMEPAD_BUTTON button)  = 0;

	//スティックの入力値を取得する
	virtual void GetStickInput(int padNo, GAMEPAD_STICK stick, float* pX, float* pY)  = 0;
	virtual Lib::Math::Vector2f GetStickInput(int padNo, GAMEPAD_STICK stick)  = 0;
	virtual void GetStickInput(int padNo, GAMEPAD_STICK stick, Lib::Math::Vector2f* pOut)  = 0;

	//仮想ゲームパッド用のキー割り当て
	virtual void SetVirtualGamepadDesc(int padNo, const VirtualGamepadDesc& desc) = 0;
	//仮想ゲームパッド用のキー割り当てクリア
	virtual void ClearVirtualGamepadDesc(int padNo) = 0;

};

class Input : public IInput
{
	Input(const Input&) = delete;
	void operator = (const Input&) = delete;

public:
	
	Input();
	~Input();

	//入力マネージャの初期化処理
	void Init();
	//入力マネージャの終了処理
	void Term();
	//入力マネージャの更新処理
	void Update();

	//キーの状態を取得する（キーが押されていないか？）
	bool IsKeyFree(int KeyCode) override;

	//キーの状態を取得する（キーが押された瞬間か？）
	bool IsKeyDown(int KeyCode) override;

	//キーの状態を取得する（キーが継続して押されているか？）
	bool IsKeyPressed(int KeyCode) override;

	//キーの状態を取得する（キーを離した瞬間か？）
	bool IsKeyUp(int KeyCode) override;

	//マウスの座標
	//取得されるのはスクリーン空間（デスクトップ上のマウスの位置）
	//この戻り値をWindow_I->TransformToClientに渡すとゲーム画面上の座標に変換される
	const Lib::Math::Vector2f& GetMousePoint()const override;

	//ゲームパッドのボタンの状態を取得する（ボタンが押されていないか？）
	bool IsButtonFree(int padNo, GAMEPAD_BUTTON button) override;
	//キーの状態を取得する（キーが押された瞬間か？）
	bool IsButtonDown(int padNo, GAMEPAD_BUTTON button) override;

	//キーの状態を取得する（キーが継続して押されているか？）
	bool IsButtonPressed(int padNo, GAMEPAD_BUTTON button) override;

	//キーの状態を取得する（キーを離した瞬間か？）
	bool IsButtonUp(int padNo, GAMEPAD_BUTTON button) override;;

	//スティックの入力値を取得する
	void GetStickInput(int padNo, GAMEPAD_STICK stick, float* pX, float* pY) override;
	Lib::Math::Vector2f GetStickInput(int padNo, GAMEPAD_STICK stick)  override;
	void GetStickInput(int padNo, GAMEPAD_STICK stick, Lib::Math::Vector2f* pOut)  override;

	//仮想ゲームパッド用のキー割り当て
	void SetVirtualGamepadDesc(int padNo, const VirtualGamepadDesc& desc) override;
	//仮想ゲームパッド用のキー割り当てクリア
	void ClearVirtualGamepadDesc(int padNo)override;

private:

	void _updateKeyboardState();
	void _getHitKeyStateAll(char* pKeyState);

	void _updateMouseState();

	// ----------------------------------------
	// 型定義 
	// ----------------------------------------
	enum class KeyState
	{
		Free,
		Down,
		Press,
		Up,
	};

	enum {
		GamePadNum = 4
	};

	// ----------------------------------------
	// 変数の宣言
	// ----------------------------------------
	KeyState KeyStates[0xFF];

	Lib::Math::Vector2f mousePoint;

	Gamepad mGamepad[GamePadNum];

};

}
}