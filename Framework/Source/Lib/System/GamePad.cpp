//ゲームパッド

#include "Lib/System/Gamepad.h"
#include "Lib/Util/Log.h"

#include <WinError.h>
#include <algorithm>
#include <winuser.h>

namespace Lib {
namespace System {

Gamepad::Gamepad()
	: mId(-1)
	, mIsConnected( false )
	, mKeyStates()
	, mStick()
	, mVirtualPadDesc()
{
	_clearInputState();
}

Gamepad::~Gamepad() {

}

void Gamepad::Init(int id) {
	mId = id;
	mIsConnected = false;
	_clearInputState();
}

void Gamepad::Update() {
	
	//ゲームパッドの状態を取得
    XINPUT_STATE state;

	bool isConnected = false;

	//ゲームパッドは4本まで.
	if (mId >= 0 && mId < 4) {
		isConnected = (XInputGetState(mId, &state) == ERROR_SUCCESS);
	}

	//接続状態が変わっていたら
	if (isConnected != mIsConnected) {
		
		//グを出す
		if (isConnected) {
			Debug::Logf("ゲームパッド[%d]が接続されました。\n", mId);
		}
		else {
			Debug::Logf("ゲームパッド[%d]が未接続になりました。\n", mId);
		}

		//入力状態をリセットする

		//接続状態を更新する
		mIsConnected = isConnected;
	}


	//ゲームパッドの接続状態に合わせて更新処理を行う
	if (mIsConnected) {
		//接続状態での更新
		_updateInputState(state);
	}
	else {
		//未接続の状態ではキーボードで代替させる
		_updateInputStateVirtual();
	}
}

//ボタンの状態を取得する（ボタンが押されていないか？）
bool Gamepad::IsButtonFree(GAMEPAD_BUTTON button) {
	if ((int)button < 0 || (int)button >= (int)GAMEPAD_BUTTON::Count) {
		return false;
	}
	return (mKeyStates[(int)button] == KeyState::Free);
}

//ボタンの状態を取得する（ボタンが押された瞬間か？）
bool Gamepad::IsButtonDown(GAMEPAD_BUTTON button) {
	if ((int)button < 0 || (int)button >= (int)GAMEPAD_BUTTON::Count) {
		return false;
	}
	return (mKeyStates[(int)button] == KeyState::Down);
}

//ボタンの状態を取得する（ボタンが継続して押されているか？）
bool Gamepad::IsButtonPressed(GAMEPAD_BUTTON button) {
	if ((int)button < 0 || (int)button >= (int)GAMEPAD_BUTTON::Count) {
		return false;
	}
	return (mKeyStates[(int)button] == KeyState::Press);
}

//ボタンの状態を取得する（ボタンを離した瞬間か？）
bool Gamepad::IsButtonUp(GAMEPAD_BUTTON button) {
	if ((int)button < 0 || (int)button >= (int)GAMEPAD_BUTTON::Count) {
		return false;
	}
	return (mKeyStates[(int)button] == KeyState::Up);
}

//スティックの入力値を取得する
void Gamepad::GetStickInput(GAMEPAD_STICK stick, float* pOutX, float* pOutY) {
	if (pOutX == nullptr || pOutY == nullptr) {
		return;
	}
	if ((int)stick < 0 || (int)stick >= (int)GAMEPAD_STICK::Count) {
		return ;
	}
	*pOutX = mStick[(int)stick].x;
	*pOutY = mStick[(int)stick].y;
}

Lib::Math::Vector2f Gamepad::GetStickInput(GAMEPAD_STICK stick) {
	float x;
	float y;
	GetStickInput(stick, &x, &y);
	Lib::Math::Vector2f v(x, y);
	return v;
}
void Gamepad::GetStickInput(GAMEPAD_STICK stick, Lib::Math::Vector2f* pOut) {
	if (pOut == nullptr) {
		return;
	}
	GetStickInput(stick, &pOut->x, &pOut->y);
}

//仮想ゲームパッド用のキー割り当て
void Gamepad::SetVirtualGamepadDesc(const VirtualGamepadDesc& desc) {
	mVirtualPadDesc = desc;
}

//仮想ゲームパッド用のキー割り当てクリア
void Gamepad::ClearVirtualGamepadDesc() {
	memset(&mVirtualPadDesc, 0, sizeof(mVirtualPadDesc));
}

void Gamepad::_clearInputState() {
	for (int i = 0; i < (int)GAMEPAD_BUTTON::Count; ++i) {
		mKeyStates[i] = KeyState::Free;
	}
	for (int i = 0; i < (int)GAMEPAD_STICK::Count; ++i) {
		mStick[i].x = 0.0f;
		mStick[i].y = 0.0f;
	}
}

void Gamepad::_updateInputState(const XINPUT_STATE& state) {

	const WORD keyCodes[] = {
		XINPUT_GAMEPAD_DPAD_UP
		,XINPUT_GAMEPAD_DPAD_DOWN
		,XINPUT_GAMEPAD_DPAD_LEFT
		,XINPUT_GAMEPAD_DPAD_RIGHT
		,XINPUT_GAMEPAD_START
		,XINPUT_GAMEPAD_BACK
		,XINPUT_GAMEPAD_LEFT_THUMB
		,XINPUT_GAMEPAD_RIGHT_THUMB
		,XINPUT_GAMEPAD_LEFT_SHOULDER
		,XINPUT_GAMEPAD_RIGHT_SHOULDER
		,XINPUT_GAMEPAD_A
		,XINPUT_GAMEPAD_B
		,XINPUT_GAMEPAD_X
		,XINPUT_GAMEPAD_Y
	};

	//キーの入力状態を更新
	for (int i = 0; i < sizeof(keyCodes) / sizeof(keyCodes[0]); ++i)
	{
		if ((state.Gamepad.wButtons & keyCodes[i] )== 0)
		{
			if (mKeyStates[i] == KeyState::Down || mKeyStates[i] == KeyState::Press) {
				mKeyStates[i] = KeyState::Up;
			}
			else {
				mKeyStates[i] = KeyState::Free;
			}
		}
		else
		{
			if (mKeyStates[i] == KeyState::Free || mKeyStates[i] == KeyState::Up) {
				mKeyStates[i] = KeyState::Down;

				Debug::Logf("ゲームパッドが押された id=%d\n", i);
			}
			else {
				mKeyStates[i] = KeyState::Press;
			}
		}
	}

	short stickXY[(int)GAMEPAD_STICK::Count][2] = {
		{ state.Gamepad.sThumbLX,state.Gamepad.sThumbLY },
		{ state.Gamepad.sThumbRX,state.Gamepad.sThumbRY },
	};
	

	const short DeadZone[] =
	{
		XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE,
		XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE
	};

	//スティック入力の処理
	for (int i = 0; i < (int)GAMEPAD_STICK::Count; ++i) {

		//X,Yの為のループ
		for (int n = 0; n < 2; ++n) {

		    //ゲームパッドアナログスティックのデッドゾーン処理
			if (stickXY[i][n] < DeadZone[i] && stickXY[i][n] > -DeadZone[i]) {
				stickXY[i][n] = 0;
			}
			else {
				stickXY[i][n] += (stickXY[i][n] < 0) ? DeadZone[i] : -DeadZone[i];
			}
		}

		//正規化
		//スティックの値範囲は -32768 ～ 32767 
		mStick[i].x = (float)(stickXY[i][0]) / (((stickXY[i][0] < 0) ? 32768 : 32767) - DeadZone[i]);
		mStick[i].y = (float)(stickXY[i][1]) / (((stickXY[i][1] < 0) ? 32768 : 32767) - DeadZone[i]);

		float len = sqrtf((mStick[i].x * mStick[i].x) + (mStick[i].y * mStick[i].y));
		if (len > 1.0f) {
			mStick[i].x /= len;
			mStick[i].y /= len;
		}

		if (mStick[i].x != 0.0f || mStick[i].y != 0.0f) {
			//Debug::Logf("スティックの値 id=%d x=%f y=%f\n", i,mStick[i].x,mStick[i].y);
		}
	}
}

void Gamepad::_updateInputStateVirtual() {

	XINPUT_STATE state = {};

	const int assignKeys[] = {
		mVirtualPadDesc.assignKeys.asUP,
		mVirtualPadDesc.assignKeys.asDOWN,
		mVirtualPadDesc.assignKeys.asLEFT,
		mVirtualPadDesc.assignKeys.asRIGHT,
		mVirtualPadDesc.assignKeys.asSTART,
		mVirtualPadDesc.assignKeys.asBACK,
		mVirtualPadDesc.assignKeys.asLEFT_STICK,
		mVirtualPadDesc.assignKeys.asRIGHT_STICK,
		mVirtualPadDesc.assignKeys.asLEFT_SHOULDER,
		mVirtualPadDesc.assignKeys.asRIGHT_SHOULDER,
		mVirtualPadDesc.assignKeys.asA,
		mVirtualPadDesc.assignKeys.asB,
		mVirtualPadDesc.assignKeys.asX,
		mVirtualPadDesc.assignKeys.asY,
	};

	const WORD keyCodes[] = {
		XINPUT_GAMEPAD_DPAD_UP
		,XINPUT_GAMEPAD_DPAD_DOWN
		,XINPUT_GAMEPAD_DPAD_LEFT
		,XINPUT_GAMEPAD_DPAD_RIGHT
		,XINPUT_GAMEPAD_START
		,XINPUT_GAMEPAD_BACK
		,XINPUT_GAMEPAD_LEFT_THUMB
		,XINPUT_GAMEPAD_RIGHT_THUMB
		,XINPUT_GAMEPAD_LEFT_SHOULDER
		,XINPUT_GAMEPAD_RIGHT_SHOULDER
		,XINPUT_GAMEPAD_A
		,XINPUT_GAMEPAD_B
		,XINPUT_GAMEPAD_X
		,XINPUT_GAMEPAD_Y
	};

	//ボタンの状態
	for (int i = 0; i < sizeof(keyCodes) / sizeof(keyCodes[0]); ++i){
		if (assignKeys[i] == 0)
			continue;

		if (GetAsyncKeyState(assignKeys[i]) & 0x8001) {
			state.Gamepad.wButtons |= keyCodes[i];
		}
	}

	{
		const short vInputMax = 32767;
		const short vInputMin = -32768;
		float vx = 0.0f,  vy = 0.0f;
		//左スティック入力
		if (mVirtualPadDesc.assignKeys.asLeftStickRIGHT != 0 && GetAsyncKeyState(mVirtualPadDesc.assignKeys.asLeftStickRIGHT )& 0x8001) {
			vx += 1.0f;
		}
		if (mVirtualPadDesc.assignKeys.asLeftStickLEFT != 0 && GetAsyncKeyState(mVirtualPadDesc.assignKeys.asLeftStickLEFT) & 0x8001) {
			vx += -1.0f;
		}
		if (mVirtualPadDesc.assignKeys.asLeftStickUP != 0 && GetAsyncKeyState(mVirtualPadDesc.assignKeys.asLeftStickUP) & 0x8001) {
			vy += 1.0f;
		}
		if (mVirtualPadDesc.assignKeys.asLeftStickDOWN != 0 && GetAsyncKeyState(mVirtualPadDesc.assignKeys.asLeftStickDOWN) & 0x8001) {
			vy += -1.0f;
		}

		state.Gamepad.sThumbLX = (short)(vx * ((vx  < 0 ) ? vInputMin : vInputMax));
		state.Gamepad.sThumbLY = (short)(vy * ((vy  < 0 ) ? vInputMin : vInputMax));
	}

	{
		const short vInputMax = 32767;
		const short vInputMin = -32768;
		float vx = 0.0f,  vy = 0.0f;
		//右スティック入力
		if (mVirtualPadDesc.assignKeys.asRightStickRIGHT != 0 && GetAsyncKeyState(mVirtualPadDesc.assignKeys.asRightStickRIGHT)& 0x8001) {
			vx += 1.0f;
		}
		if (mVirtualPadDesc.assignKeys.asRightStickLEFT != 0 && GetAsyncKeyState(mVirtualPadDesc.assignKeys.asRightStickLEFT) & 0x8001) {
			vx -= -1.0f;
		}
		if (mVirtualPadDesc.assignKeys.asRightStickUP != 0 && GetAsyncKeyState(mVirtualPadDesc.assignKeys.asRightStickUP) & 0x8001) {
			vy += 1.0f;
		}
		if (mVirtualPadDesc.assignKeys.asRightStickDOWN != 0 && GetAsyncKeyState(mVirtualPadDesc.assignKeys.asRightStickDOWN) & 0x8001) {
			vy -= -1.0f;
		}

		state.Gamepad.sThumbRX = (short)(vx * ((vx  < 0 ) ? vInputMin : vInputMax));
		state.Gamepad.sThumbRY = (short)(vy * ((vy  < 0 ) ? vInputMin : vInputMax));
	}

	_updateInputState(state);
}


}
}