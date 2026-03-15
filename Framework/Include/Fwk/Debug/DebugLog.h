#pragma once

#if defined(DEBUG) || defined(_DEBUG)

#include "Fwk/Graphics/Font.h"

namespace Fwk
{
class SpriteFont;

class DebugLog
{
	static const float s_DisplayTimeSec;
public:

	DebugLog();
	~DebugLog();

	void Init(Fwk::SpriteFont* pSpriteFont);
	void Term();
	void Update(float deltaTime);
	void Draw();

	void Print(const char* format, ...);

private:


	enum {
		DEBUG_LOG_MAX = 44
	};

	struct Log{
		Font font;
		float timeLeft;
	};

	Log mDebugLog[DEBUG_LOG_MAX];
	
	int mHead;
	int mTail;
};

}

#endif

