#include "Lib/Util/Log.h"

#include <cstdio>
#include <cstdarg>
#include <Windows.h>

namespace Debug
{

void Logf(const char* format, ...)
{
#if defined(DEBUG) || defined(_DEBUG)
    char msg[2048];
    memset(msg, 0, sizeof(msg));

    //可変長引数
    va_list arg;

    //可変長引数をまとめて、msgにprintf
    va_start(arg, format);
    vsprintf_s(msg, 2048, format, arg);
    va_end(arg);

    Log(msg);

#endif
}

void Log(const char* pText)
{
#if defined(DEBUG) || defined(_DEBUG)
    // コンソールに出力.
    printf_s("%s", pText);
    // VSの"出力"にも.
    OutputDebugStringA(pText);
#endif
}

}