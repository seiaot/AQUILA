#include "Fwk/Data/CSVData.h"
#include "Fwk/Data/DataUtil.h"
#include "Lib/Util/Log.h"
#include "Lib/LibDef.h"

#include <ctype.h>
#include <iostream>
#include <conio.h>

#include "Fwk/Framework.h"

using namespace DataUtil;

CSVData::CSVData()
	: m_pData( nullptr )
	, m_pBuffer(nullptr)
	, m_bufferSize(0)
{
	;
}

CSVData::~CSVData()
{
	Unload();
}


const char* FindEndOfString(const char* pText, const char* pEnd) {
	
	int cnt = 0;
	while (pText < pEnd) {

		if (IsMultiByteChar(*pText)) {
			pText += 2;
			continue;
		}
		//ダブルクォーテーションを見つけたら
		if (*pText == '"') {
			//ダブルクォーテーションでなくなるまでループ
			while (pText < pEnd && *pText == '"') {
				++cnt;
				++pText;
			}
			//cntを2で割り切れたらそこが文字列を閉じるダブルクォーテーション
			if (cnt % 2 == 0) {
				return pText-1;
			}
		}
		++pText;
	}

	return nullptr;
}

//テキストデータの先頭アドレスとサイズを指定してデータを構築する
void CSVData::Load(const char* pRawData, int RawDataSize)
{
	Debug::Log("CSVData::Load データの構築を開始。\n");

	if (pRawData == nullptr || RawDataSize < 1 ) {
		Debug::Log("CSVData::Load テキストデータがありません。\n");
		return;
	}

	//BOMチェック(utf8のみ)
	if (RawDataSize > 2)
	{
		unsigned char checkBOM[3];
		memcpy(checkBOM, pRawData, 3);
		if (
			checkBOM[0] == 0xEF &&
			checkBOM[1] == 0xBB &&
			checkBOM[2] == 0xBF
			) {
			//BOMの読み飛ばし
			pRawData += 3;
			RawDataSize -= 3;

			if (RawDataSize < 1) {
				Debug::Log("CSVData::Load テキストデータがありません。\n");
				return;
			}
		}
	}

	Unload();

	m_pData = pRawData;

	//データの読み込み
	{
		const char* p = m_pData;
		const char* pEnd = m_pData + RawDataSize;
		const char findCharas[] = { ',','\n'};

		m_bufferSize = 0;

		while (p < pEnd) {

			//制御コードを含まないように先頭をずらす
			while (p < pEnd && IsControlChar(*p) && *p != '\n') {
				++p;
			}

			const char* pElemHead = p;
			const char* pElemTerm = nullptr;
			//要素の先頭がダブルクォーテーションで始まっていたら、
			//閉じるダブルクォーテーションを探しに行く
			if (*pElemHead == '"') {
				//文字列の終わりを調べる
				pElemTerm = FindEndOfString(pElemHead ,pEnd);
				//文字列が正しく閉じられていない
				if (pElemTerm == nullptr) {
					//とりあえずループ抜ける
					break;
				}
				//閉じるダブルクォーテーションの一つ隣から区切り文字を探しに行く
				p = FindAsciiChar(pElemTerm+1, pEnd, findCharas,2);
				if (p == nullptr) {
					//終端
					p = pEnd;
				}

				//pは区切り文字の隣においておく
				++p;

				//要素として取り出したいのはダブルクォーテーションを除いた部分
				pElemHead = pElemHead + 1;
				pElemTerm = pElemTerm - 1;
			}
			else {

				//ダブルクォーテーションついてない要素の場合

				//区切り文字を見つけに行く
				pElemTerm = FindAsciiChar(pElemHead, pEnd, findCharas,2);

				if (pElemTerm == nullptr) {
					//終端
					if (pElemHead == pEnd) {
						break;
					}
					pElemTerm = pEnd;
				}

				//pは区切り文字の隣においておく
				p = pElemTerm + 1;

				//要素として取り出したいのは区切り文字の一文字前まで
				pElemTerm = pElemTerm - 1;

				//制御コードがあったら取り除いておく
				while (pElemTerm > pElemHead && IsControlChar(*pElemTerm)) {
					--pElemTerm;
				}
			}	
			
			int elemSize = (int)(pElemTerm - pElemHead) + 1;
			Element elem;
			elem.p = (unsigned short)(pElemHead - m_pData);
			elem.size = (unsigned short)elemSize;
			
			m_elements.push_back(elem);

			if ( m_bufferSize < elemSize) {
				 m_bufferSize = elemSize;
			}
		}

		if (m_bufferSize > 0) {
			m_bufferSize += 1;
			m_pBuffer = new	char[m_bufferSize];
		}
	}

	Debug::Logf("CSVData::Load データの構築完了。要素数:%d\n", m_elements.size());
}

//解放
void CSVData::Unload()
{
	m_pData = nullptr;
	
	m_elements.clear();

	m_bufferSize = 0;
	SAFE_DELETE_ARRAY(m_pBuffer);
}

//データの要素数を返す
int CSVData::GetElementCount()const
{
	return (int)m_elements.size();
}

//指定した番号のデータを整数値として返す
int CSVData::GetInt(int index)const
{
	if (index < 0 || index >= (int)m_elements.size()) {
		return 0;
	}

	memcpy_s(m_pBuffer, m_bufferSize, &m_pData[m_elements[index].p], m_elements[index].size);
	m_pBuffer[m_elements[index].size] = 0;

	return atoi(m_pBuffer);
}

//指定した番号のデータを実数値として返す
float CSVData::GetFloat(int index)const
{
	if (index < 0 || index >= (int)m_elements.size()) {
		return 0;
	}

	memcpy_s(m_pBuffer, m_bufferSize, &m_pData[m_elements[index].p], m_elements[index].size);
	m_pBuffer[m_elements[index].size] = 0;

	return (float)atof(m_pBuffer);
}

//指定した番号のデータを文字列として返す(string型に詰めて返すVer)
void CSVData::GetString(int index, std::string* pString)const
{
	if (pString == nullptr) {
		return;
	}

	GetString(index, m_pBuffer, m_bufferSize);
	*pString = m_pBuffer;
}

//指定した番号のデータを文字列として返す(配列に詰めて返すVer）
void CSVData::GetString(int index, char* pBuffer, int bufferSize)const
{
	if (pBuffer == nullptr || bufferSize < 1) {
		return;
	}

	memset(pBuffer, 0, bufferSize);

	const char* p = &(m_pData[m_elements[index].p]);
	const char* pEnd = &(m_pData[m_elements[index].p + m_elements[index].size]);

	int writeSize = 0;

	while (p<pEnd) {
		
		int cpySize = 0;
		if (IsMultiByteChar(*p)) {
			cpySize = 2;
			continue;
		}
		else {
			cpySize = 1;
			if (*p == '"' && *(p + 1) == '"') {
				++p;
			}
		}

		if ((writeSize + cpySize) > (bufferSize - 1)) {
			break;
		}

		memcpy_s(&pBuffer[writeSize], bufferSize-writeSize,p,cpySize);
		writeSize += cpySize;
		p += cpySize;
	}	
}

void CSVData::PrintCSVData()const {

	std::string s;
	for (int i = 0; i < (int)m_elements.size(); ++i) {
		GetString(i, &s);
		std::cout << "\"";
		std::cout << s;
		std::cout << "\",";
	}
	std::cout << "\n";
}

