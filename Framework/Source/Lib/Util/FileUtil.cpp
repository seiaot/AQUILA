#include "Lib/Util/FileUtil.h"

using namespace std;

namespace Lib {
namespace Util {

bool SearchFilePath(const wchar_t* filename, wstring& result)
{
	if (filename == nullptr)
		return false;

	if (wcscmp(filename, L" ") == 0 || wcscmp(filename, L"") == 0)
		return false;

	wchar_t exePath[520] = {};
	GetModuleFileName(nullptr, exePath, 520);
	exePath[519] = L'\0';
	PathRemoveFileSpecW(exePath);

	wchar_t dstPath[520] = {};

	//--------------------------------------
	wcscpy_s(dstPath, filename);
	if (PathFileExistsW(dstPath) == TRUE) {
		result = dstPath;
		return true;
	}
	swprintf_s(dstPath, L"\\Asset\\%s", filename);
	if (PathFileExistsW(dstPath) == TRUE) {
		result = dstPath;
		return true;
	}
	swprintf_s(dstPath, L"\\Assets\\%s", filename);
	if (PathFileExistsW(dstPath) == TRUE) {
		result = dstPath;
		return true;
	}
	//--------------------------------------
	swprintf_s(dstPath, L"..\\%s", filename);
	if (PathFileExistsW(dstPath) == TRUE) {
		result = dstPath;
		return true;
	}
	swprintf_s(dstPath, L"..\\Asset\\%s", filename);
	if (PathFileExistsW(dstPath) == TRUE) {
		result = dstPath;
		return true;
	}
	swprintf_s(dstPath, L"..\\Assets\\%s", filename);
	if (PathFileExistsW(dstPath) == TRUE) {
		result = dstPath;
		return true;
	}
	//--------------------------------------
	swprintf_s(dstPath, L"..\\..\\%s", filename);
	if (PathFileExistsW(dstPath) == TRUE) {
		result = dstPath;
		return true;
	}
	swprintf_s(dstPath, L"..\\..\\Asset\\%s", filename);
	if (PathFileExistsW(dstPath) == TRUE) {
		result = dstPath;
		return true;
	}
	swprintf_s(dstPath, L"..\\..\\Assets\\%s", filename);
	if (PathFileExistsW(dstPath) == TRUE) {
		result = dstPath;
		return true;
	}
	//--------------------------------------
	swprintf_s(dstPath, L"%s\\%s", exePath, filename);
	if (PathFileExistsW(dstPath) == TRUE) {
		result = dstPath;
		return true;
	}
	swprintf_s(dstPath, L"%s\\Asset\\%s", exePath, filename);
	if (PathFileExistsW(dstPath) == TRUE) {
		result = dstPath;
		return true;
	}
	swprintf_s(dstPath, L"%s\\Assets\\%s", exePath, filename);
	if (PathFileExistsW(dstPath) == TRUE) {
		result = dstPath;
		return true;
	}
	//--------------------------------------
	swprintf_s(dstPath, L"%s\\..\\%s", exePath, filename);
	if (PathFileExistsW(dstPath) == TRUE) {
		result = dstPath;
		return true;
	}
	swprintf_s(dstPath, L"%s\\..\\Asset\\%s", exePath, filename);
	if (PathFileExistsW(dstPath) == TRUE) {
		result = dstPath;
		return true;
	}
	swprintf_s(dstPath, L"%s\\..\\Assets\\%s", exePath, filename);
	if (PathFileExistsW(dstPath) == TRUE) {
		result = dstPath;
		return true;
	}
	//--------------------------------------
	swprintf_s(dstPath, L"%s\\..\\..\\%s", exePath, filename);
	if (PathFileExistsW(dstPath) == TRUE) {
		result = dstPath;
		return true;
	}
	swprintf_s(dstPath, L"%s\\..\\..\\Asset\\%s", exePath, filename);
	if (PathFileExistsW(dstPath) == TRUE) {
		result = dstPath;
		return true;
	}
	swprintf_s(dstPath, L"%s\\..\\..\\Assets\\%s", exePath, filename);
	if (PathFileExistsW(dstPath) == TRUE) {
		result = dstPath;
		return true;
	}

	return false;
}

}
}