#pragma once

#include <windows.h>

#pragma comment (lib, "xinput.lib")
#include <xinput.h>
#include <DirectXMath.h>

#include "Lib/Math/Vector.h"

using namespace DirectX;

enum class GAMEPAD_BUTTON{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	START,
	BACK,
	LEFT_STICK,
	RIGHT_STICK,
	LEFT_SHOULDER,
	RIGHT_SHOULDER,
	A,
	B,
	X,
	Y,

	Count,
};

enum class GAMEPAD_STICK{
	LEFT,
	RIGHT,

	Count,
};

namespace Lib {
namespace System {

//ゲームパッドが未接続時にキーボード入力で代替する為の設定

//ボタンに対応するキー割り当て
struct AssignKeys{
	int asUP;
	int asDOWN;
	int asLEFT;
	int asRIGHT;
	int asSTART;
	int asBACK;
	int asLEFT_STICK;
	int asRIGHT_STICK;
	int asLEFT_SHOULDER;
	int asRIGHT_SHOULDER;
	int asA;
	int asB;
	int asX;
	int asY;

	int asLeftStickUP;
	int asLeftStickDOWN;
	int asLeftStickLEFT;
	int asLeftStickRIGHT;

	int asRightStickUP;
	int asRightStickDOWN;
	int asRightStickLEFT;
	int asRightStickRIGHT;
};

struct VirtualGamepadDesc {
	AssignKeys assignKeys;
};

//ゲームパッド
class Gamepad 
{

	enum class KeyState
	{
		Free,
		Down,
		Press,
		Up,
	};

public:
	
	Gamepad();
	~Gamepad();

	void Init(int id);
	void Update();

	//ボタンの状態を取得する（ボタンが押されていないか？）
	bool IsButtonFree(GAMEPAD_BUTTON button);

	//ボタンの状態を取得する（ボタンが押された瞬間か？）
	bool IsButtonDown(GAMEPAD_BUTTON button);

	//ボタンの状態を取得する（ボタンが継続して押されているか？）
	bool IsButtonPressed(GAMEPAD_BUTTON button);

	//ボタンの状態を取得する（ボタンを離した瞬間か？）
	bool IsButtonUp(GAMEPAD_BUTTON button);

	//スティックの入力値を取得する
	void GetStickInput(GAMEPAD_STICK stick, float* pOutX, float* pOutY);
	Lib::Math::Vector2f GetStickInput(GAMEPAD_STICK stick);
	void GetStickInput(GAMEPAD_STICK stick, Lib::Math::Vector2f* pOut);

	//仮想ゲームパッド用のキー割り当て
	void SetVirtualGamepadDesc(const VirtualGamepadDesc& desc);
	//仮想ゲームパッド用のキー割り当てクリア
	void ClearVirtualGamepadDesc();

private:

	void _clearInputState();
	void _updateInputState(const XINPUT_STATE& state);
	void _updateInputStateVirtual();

	int mId;
	bool mIsConnected;

	KeyState mKeyStates[(int)GAMEPAD_BUTTON::Count];
	XMFLOAT2 mStick[(int)GAMEPAD_STICK::Count];

	VirtualGamepadDesc  mVirtualPadDesc;
};

}
}