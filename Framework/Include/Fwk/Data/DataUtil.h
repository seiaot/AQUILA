#pragma once

namespace DataUtil
{
	bool IsControlChar(char c);
	bool IsMultiByteChar(char c);
	const char* FindAsciiChar(const char* pText, const char* pEnd, const char* charList, int size);
	const char* FindAsciiChar(const char* pText, const char* pEnd, char c);
	const char* FindAsciiCharInLine(const char* pText, const char* pEnd, char c);
}

