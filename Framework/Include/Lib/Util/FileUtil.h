#pragma once

#include <string>
#include <Shlwapi.h>

#pragma comment( lib, "shlwapi.lib" )

namespace Lib {
namespace Util {

bool SearchFilePath(const wchar_t* filename, std::wstring& result);

}
}