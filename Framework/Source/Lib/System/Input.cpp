#include "Lib/System/Input.h"

#include <winuser.h>

namespace Lib {
namespace System {

Input::Input()
	: mousePoint({ 0,0 })
{
	for (int i = 0; i < 0xFF; i++) {
		KeyStates[i] = KeyState::Free;
	}
}

Input::~Input()
{

}

// ----------------------------------------
// 入力システムの初期化処理
// ----------------------------------------
void Input::Init()
{
	for (int i = 0; i < 0xFF; i++) {
		KeyStates[i] = KeyState::Free;
	}

	for (int i = 0; i < GamePadNum; ++i) {
		mGamepad[i].Init(i);
	}

	mousePoint.x = 0;
	mousePoint.y = 0;
}

// ----------------------------------------
// 入力システムの終了処理
// ----------------------------------------
void Input::Term()
{
}

// ----------------------------------------
// 入力システムの更新処理
// ----------------------------------------
void Input::Update()
{
	_updateKeyboardState();
	_updateMouseState();

	for (int i = 0; i < GamePadNum; ++i) {
		mGamepad[i].Update();
	}
}

// ----------------------------------------
// キーの状態を取得する（キーが押されていないか？）
// ----------------------------------------
bool Input::IsKeyFree(int KeyCode)
{
	if (KeyCode < 0 || KeyCode>255)
		return false;

	return KeyStates[KeyCode] == KeyState::Free;
}

// ----------------------------------------
// キーの状態を取得する（キーが押された瞬間か？）
// ----------------------------------------
bool Input::IsKeyDown(int KeyCode)
{
	if (KeyCode < 0 || KeyCode>255)
		return false;

	return KeyStates[KeyCode] == KeyState::Down;
}

// ----------------------------------------
// キーの状態を取得する（キーが継続して押されているか？）
// ----------------------------------------
bool Input::IsKeyPressed(int KeyCode)
{
	if (KeyCode < 0 || KeyCode>255)
		return false;

	return KeyStates[KeyCode] == KeyState::Press;
}

// ----------------------------------------
// キーの状態を取得する（キーを離した瞬間か？）
// ----------------------------------------
bool Input::IsKeyUp(int KeyCode)
{
	if (KeyCode < 0 || KeyCode>255)
		return false;

	return KeyStates[KeyCode] == KeyState::Up;
}

// ----------------------------------------
//マウスの座標
// ----------------------------------------
const Lib::Math::Vector2f& Input::GetMousePoint()const
{
	return mousePoint;
}

//ゲームパッドのボタンの状態を取得する（ボタンが押されていないか？）
bool Input::IsButtonFree(int padNo, GAMEPAD_BUTTON button) {
	if (padNo < 0 || padNo >= GamePadNum) {
		return false;
	}
	return mGamepad[padNo].IsButtonFree(button);
}

//キーの状態を取得する（キーが押された瞬間か？）
bool Input::IsButtonDown(int padNo, GAMEPAD_BUTTON button) {
	if (padNo < 0 || padNo >= GamePadNum) {
		return false;
	}
	return mGamepad[padNo].IsButtonDown(button);
}

//キーの状態を取得する（キーが継続して押されているか？）
bool Input::IsButtonPressed(int padNo, GAMEPAD_BUTTON button) {
	if (padNo < 0 || padNo >= GamePadNum) {
		return false;
	}
	return mGamepad[padNo].IsButtonPressed(button);
}

//キーの状態を取得する（キーを離した瞬間か？）
bool Input::IsButtonUp(int padNo, GAMEPAD_BUTTON button) {
	if (padNo < 0 || padNo >= GamePadNum) {
		return false;
	}
	return mGamepad[padNo].IsButtonUp(button);
}

//スティックの入力値を取得する
void Input::GetStickInput(int padNo, GAMEPAD_STICK stick, float* pX, float* pY){
	if (padNo < 0 || padNo >= GamePadNum) {
		return ;
	}
	mGamepad[padNo].GetStickInput(stick,pX,pY);
}

Lib::Math::Vector2f Input::GetStickInput(int padNo, GAMEPAD_STICK stick) {
	if (padNo < 0 || padNo >= GamePadNum) {
		return Lib::Math::Vector2f(0.0f,0.0f);
	}
	float x, y;
	mGamepad[padNo].GetStickInput(stick,&x,&y);
	Lib::Math::Vector2f v(x, y);
	return v;
}

void Input::GetStickInput(int padNo, GAMEPAD_STICK stick, Lib::Math::Vector2f* pOut) {
	if (padNo < 0 || padNo >= GamePadNum) {
		return ;
	}
	mGamepad[padNo].GetStickInput(stick, pOut);
}


//仮想ゲームパッド用のキー割り当て
void Input::SetVirtualGamepadDesc(int padNo, const VirtualGamepadDesc& desc) {
	if (padNo < 0 || padNo >= GamePadNum) {
		return ;
	}
	mGamepad[padNo].SetVirtualGamepadDesc(desc);
}

//仮想ゲームパッド用のキー割り当てクリア
void Input::ClearVirtualGamepadDesc(int padNo) {
	if (padNo < 0 || padNo >= GamePadNum) {
		return ;
	}
	mGamepad[padNo].ClearVirtualGamepadDesc();

}


void Input::_updateKeyboardState() {

	char _keyBuffer[0xFF] = { 0 };

	_getHitKeyStateAll(_keyBuffer);

	for (int i = 0; i < 0xFF; i++) {

		if (_keyBuffer[i] == 0)
		{
			if (KeyStates[i] == KeyState::Down || KeyStates[i] == KeyState::Press) {
				KeyStates[i] = KeyState::Up;
			}
			else {
				KeyStates[i] = KeyState::Free;
			}
		}
		else
		{
			if (KeyStates[i] == KeyState::Free || KeyStates[i] == KeyState::Up) {
				KeyStates[i] = KeyState::Down;
			}
			else {
				KeyStates[i] = KeyState::Press;
			}
		}
	}
}

//全キー状態取得
void Input::_getHitKeyStateAll(char* pKeyState) {

	//使う仮想キーコードだけ
	const int keycodeRanges[][2] = {
		 {0x01,0x06} //マウスボタン用
		,{0x08,0x09} //bs,tab
		,{0x0C,0x0D} //Clear,Enter
		,{0x10,0x14} //Shift,Ctrl,ALT,Pause,CAPS LOCK 
		,{0x1B,0x1B} //Esc
		,{0x20,0x2F} //Space～Help
		,{0x30,0x39} //0~9
		,{0x41,0x5A} //A~Z
		,{0x5B,0x5D} //Windows の左キー 右の Windows キー アプリケーション キー
		,{0x5F,0x5F} // コンピューターのスリープ キー
		,{0x60,0x6F} //テンキー
		,{0x70,0x87} //ファンクションキー
		,{0x90,0x91} //NUM LOCK ScrollLock
		,{0xA0,0xA5} //SHIFT ALT
		,{0xBA,0xBF} //そのほか記号
		,{0xC0,0xC0} //そのほか記号
		,{0xDB,0xDF} //そのほか記号
		,{0xE2,0xE2} //そのほか記号
		,{0,0}	//終端
	};

	int idx = 0;

	while (keycodeRanges[idx][0] != 0) {
		for (int i = keycodeRanges[idx][0]; i <= keycodeRanges[idx][1]; ++i)
		{
			int keyCode = i;
			if (GetAsyncKeyState(keyCode) & 0x8001) {
				pKeyState[keyCode] = 1;
			}
		}
		++idx;
	}
}

//マウス状態の更新
void Input::_updateMouseState() {
	POINT p;
	GetCursorPos(&p);
	mousePoint.x = (float)p.x;
	mousePoint.y = (float)p.y;
}

}
}