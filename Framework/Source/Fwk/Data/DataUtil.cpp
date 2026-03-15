#include "Fwk/Data/DataUtil.h"

namespace DataUtil
{

	bool IsControlChar(char c) {
		//0x00～0x1f、0x7f は制御コード
		return (c >= 0x00 && c <= 0x1f) || (c == 0x7f);
	}

	bool IsMultiByteChar(char c) {
		//SJISの上位バイトであればtrueを返す
		//0x00～0x1f、0x7f は制御コード
		return (c >= 0x81 && c <= 0x9f) || (c >= 0xe0 && c <= 0xef);
	}
	const char* FindAsciiChar(const char* pText, const char* pEnd, const char* charList, int size) {

		if (pText == nullptr || pEnd == nullptr || charList == nullptr || size < 1) {
			return nullptr;
		}

		while (pText < pEnd) {

			if (IsMultiByteChar(*pText)) {
				pText += 2;
				continue;
			}
			for (int i = 0; i < size; i++) {
				if (*pText == charList[i]) {
					return pText;
				}
			}
			++pText;
		}

		return nullptr;
	}

	const char* FindAsciiChar(const char* pText, const char* pEnd, char c) {
		return FindAsciiChar(pText, pEnd, &c, 1);
	}

	const char* FindAsciiCharInLine(const char* pText, const char* pEnd, char c) {
		char chars[] = { c,'\n' };
		const char* p = FindAsciiChar(pText, pEnd, chars, 2);
		return (*p == '\n') ? nullptr : p;
	}
}