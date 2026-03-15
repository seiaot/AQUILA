#if defined(DEBUG) || defined(_DEBUG)

#include "Fwk/Debug/DebugLog.h"
#include "Fwk/Graphics/SpriteFont.h"

#include "Lib/Util/Log.h"

namespace Fwk
{
//一個のログの表示秒数
const float DebugLog::s_DisplayTimeSec = 2.0f;

DebugLog::DebugLog() 
	: mHead(0)
	, mTail(0)
{

}

DebugLog::~DebugLog() {

}

void DebugLog::Init(Fwk::SpriteFont* pSpriteFont) {

	for (int i = 0; i < DEBUG_LOG_MAX; ++i) {
		mDebugLog[i].font.Init(pSpriteFont);
		mDebugLog[i].font.SetScale(0.5f, 0.5f);
		mDebugLog[i].timeLeft = 0.0f;
		mDebugLog[i].font.SetColor(0.0f,1.0f,1.0f);
	}

	mHead = 0;
	mTail = 0;
}

void DebugLog::Term() {

	for (int i = 0; i < DEBUG_LOG_MAX; ++i) {
		mDebugLog[i].font.Term();
		mDebugLog[i].timeLeft = 0.0f;
	}
	
	mHead = 0;
	mTail = 0;
}

void DebugLog::Update(float deltaTime) {

	int i = mHead;;

	while (i != mTail) {
			
		mDebugLog[i].timeLeft -= deltaTime;

		if (mDebugLog[i].timeLeft <= 0.0f) {
			mDebugLog[i].font.Clear();
			mDebugLog[i].timeLeft = 0.0f;
		}
			
		i = (++i)%DEBUG_LOG_MAX ;
	}

	while (mDebugLog[mHead].timeLeft == 0.0f && mHead != mTail) {
		mHead = (++mHead) % DEBUG_LOG_MAX;
	}
}

void DebugLog::Draw() {

	int i = mHead;
	
	while (i != mTail) {
		mDebugLog[i].font.Draw();
		i = (i+1) % DEBUG_LOG_MAX;
	}
}

void DebugLog::Print(const char* format, ...) 
{
	char msg[1024] = { 0 };

	//可変長引数
	int size = 0;
	{
		va_list arg;
		//可変長引数をまとめて、msgにprintf
		va_start(arg, format);
		size = sprintf_s(msg, 1023, format, arg);
		va_end(arg);
	}

	//テキストを設定.
	mDebugLog[mTail].font.SetText(msg);
	//表示秒数を設定
	mDebugLog[mTail].timeLeft = s_DisplayTimeSec;

	//インクリメント前の最後尾index
	int n = mTail;

	//最後尾のINDEXをインクリメント
	mTail = (++mTail)%DEBUG_LOG_MAX;
	if(mTail == mHead){
		mHead = (++mHead)%DEBUG_LOG_MAX;
	}

	//座標の調整
	{
		float y = 0.0f;
		while (mDebugLog[n].timeLeft > 0.0f && n != mTail)
		{
			if (n >= 0 && n < DEBUG_LOG_MAX) {
				mDebugLog[n].font.SetPosition(0.0f, y);
			}
			y += 16.0f;
			--n;
			if (n < 0) {
				n = DEBUG_LOG_MAX-1;
			}
		}

	}

	//コンソールにも出力
	{
		msg[size] = '\n';
		msg[size+1] = 0;
		//sprintf_s(tmp, 2048, "%s\n", msg);
		Debug::Log(msg);
	}
}

}

#endif
