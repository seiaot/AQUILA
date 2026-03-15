#pragma once

namespace Debug
{
void Logf(const char* format, ...);
void Log(const char* pText);
}

#if defined(DEBUG) || defined(_DEBUG)
#define DEBUG_LOGF( x, ... ) Debug::Logf( x "\n", ##__VA_ARGS__ );
#define DEBUG_LOG( x) Debug::Log( x "\n");
#else
#define DEBUG_LOGF( x, ... ) 
#define DEBUG_LOG( x) 
#endif
