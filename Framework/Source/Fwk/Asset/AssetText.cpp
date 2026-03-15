// ----------------------------------------------------
// テキストアセットクラス
// 
// 
// 
// ----------------------------------------------------
#include "Fwk/Asset/AssetText.h"

#include <fstream>
#include <iostream>
#include <string>

#include "Lib/Util/Log.h"
#include "Fwk/Framework.h"
#include "Lib/Util/FileUtil.h"

namespace Fwk
{

AssetText::AssetText()	
	: m_fileSize(0)
	, m_pBuffer(nullptr)
{
	;
}

AssetText::~AssetText()
{
	Term();
}

bool AssetText::Init(const wchar_t* pFilePath) {

	Debug::Logf("テキストの読み込み開始:%ls\n",pFilePath);

	if (!PathFileExistsW(pFilePath)) {
		Debug::Logf("指定のテキストファイルが見つかりません。:%ls\n",pFilePath);
		return false;
	}

	std::ifstream ifs(pFilePath);
		
	if (ifs.fail()) {
		Debug::Logf("指定のテキストファイルのオープンに失敗しました。:%ls\n",pFilePath);
		return false;
	}

    // １文字ずつ最後まで読み込み
	
	//ファイルの全長を調べる
	
   // 一番最後までseek
    ifs.seekg( 0, std::ios_base::end );
 
    // 現在のポインタ位置取得
    m_fileSize = static_cast<size_t>(ifs.tellg());
 
    // 一番先頭までseek
    ifs.seekg( 0, std::ios_base::beg );

	//読み込み用のバッファの確保
	m_pBuffer = new char[m_fileSize + 1];
	memset(m_pBuffer, 0, m_fileSize + 1);

	//ファイルの読み込み
	for (int i = 0; i < m_fileSize; ++i) {
		ifs.get(m_pBuffer[i]);
	}

	//ファイルを閉じる
	ifs.close();

	return true;
}
void AssetText::OnTerm() {
	SAFE_DELETE_ARRAY(m_pBuffer);
	m_fileSize = 0;
}

size_t AssetText::GetSize()const {
	return m_fileSize;
}

const char* AssetText::GetBuffer()const {
	return m_pBuffer;
}


}