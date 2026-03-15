#include "Fwk/Data/KeyValueData.h"
#include "Fwk/Data/CSVData.h"
#include "Fwk/Data/DataUtil.h"
#include "Lib/Util/Log.h"
#include "Lib/LibDef.h"

#include <ctype.h>
#include <iostream>
#include <conio.h>

#include "Fwk/Framework.h"


bool IsValidCharAsKeyTop(char c) {
	if (c >= 0x41 && c <= 0x5a) {
		return true;
	}
	if (c >= 0x61 && c <= 0x7a) {
		return true;
	}
	if (c == '_') {
		return true;
	}
	return false;
}

bool IsValidCharAsKey(char c) {

	if (IsValidCharAsKeyTop(c)) {
		return true;
	}
	if (c >= 0x30 && c <= 0x39) {
		return true;
	}
	return false;
}

const char* SkipToEndLine(const char* p,const char* pEOF) {
	p = DataUtil::FindAsciiChar(p, pEOF, '\n');
	return (p == nullptr) ? pEOF : p;
}

//pは行頭をイメージ
//行末までにKeyがあるかを判断する
//キーが見つからない行であればKeySizeは-1で戻り値は改行コードの後ろ
//キーが見つかったのであれば＝の後ろのアドレス
const char* _readKey(const char* p,const char* pEOF, int* pKeySize)
{
	*pKeySize = 0;

	const char* pTmp = p;

	//この行で何か文字を見つけるまでpTmpを先に進める
	while (pTmp < pEOF) {
		//改行コードを見つけたら抜ける
		if (*pTmp == '\n') {
			break;
		}
		//制御コードと空白をスキップする
		if (*pTmp == ' ' || DataUtil::IsControlChar(*pTmp)) {
			++pTmp;
			continue;
		}
		break;
	}

	//先頭がアルファベットもしくは_でなければキーとしては扱わない
	if (!IsValidCharAsKeyTop(*pTmp)) {
		return nullptr;
	}

	//=が出てくるまでその行を検索
	const char* pTerm = DataUtil::FindAsciiCharInLine(pTmp,pEOF,'=');
	//=が見当たらない
	if (pTerm == nullptr) {
		return nullptr;
	}

	//=の一つ前の文字に戻す
	--pTerm;

	//後ろの空白を削る
	while (pTerm > pTmp) {
		if (*pTerm == ' ' || DataUtil::IsControlChar(*pTerm)) {
			--pTerm;
			continue;
		}
		break;
	}

	//キー文字列の長さ.
	int length = (int)(pTerm - pTmp) + 1;

	//キー文字列中に使えない文字を見つけたらこの行は無視
	for (int i = 0; i < length; ++i ){
		//無効文字を見つけた
		if (!IsValidCharAsKey(*(pTmp+i))) {
			length = 0;
			break;
		}
	}

	//キーが無効であれば
	if (length == 0) {
		return nullptr;
	}

	//キーの先頭アドレスとキーの長さを戻す
	*pKeySize = length;
	return pTmp;
}

//pはキーの後ろの＝の一つ後ろ
//＝の後ろから行末までの文字列について、前後の空白・制御文字を取り除いた範囲を返す
const char* _readVal(const char* p, const char* pEOF, int* pValSize) {

	*pValSize = 0;

	const char* pTmp = p;

	while (pTmp < pEOF) {
		//改行コードを見つけたら抜ける
		if (*pTmp == '\n') {
			break;
		}
		//制御コードと空白をスキップする
		if (*pTmp == ' ' || DataUtil::IsControlChar(*pTmp)) {
			++pTmp;
			continue;
		}
		break;
	}

	//行末の一つ手前の文字
	const char* pTerm = SkipToEndLine(pTmp, pEOF)-1;

	while (pTerm > pTmp) {
		//制御コードと空白をスキップする
		if (*pTerm == ' ' || DataUtil::IsControlChar(*pTerm)) {
			--pTerm;
			continue;
		}
		//何かしら文字をみつけたぬける
		break;
	}

	//キーの先頭アドレスとキーの長さを戻す
	*pValSize = (int)(pTerm-pTmp)+1;
	return pTmp;

}

KeyValueData::KeyValueData()
	: m_pData( nullptr )
	, m_pElements(nullptr)
	, m_elementNum( 0 )
	, m_pKeyBuffer(nullptr)
	, m_keyBufferSize(0)
	, m_pValBuffer(nullptr)
	, m_valBufferSize(0)
{
	;
}

KeyValueData::~KeyValueData()
{
	Unload();
}

//テキストデータの先頭アドレスとサイズを指定してデータを構築する
void KeyValueData::Load(const char* pRawData, int RawDataSize)
{
	Debug::Log("KeyValueData:Load データの構築を開始。\n");

	if (pRawData == nullptr || RawDataSize < 1 ) {
		Debug::Log("KeyValueData::Load テキストデータがありません。\n");
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
				Debug::Log("KeyValueData::Load テキストデータがありません。\n");
				return;
			}
		}
	}

	Unload();
	m_pData = pRawData;


	//要素数(キーの数)のカウントと必要なバッファサイズの計算
	m_elementNum = 0;
	m_keyBufferSize = 0;
	m_valBufferSize = 0;

	Element** pElement = &m_pElements;

	const char* p = m_pData;
	const char* pEOF = p + RawDataSize;

	while (p < pEOF) 
	{
		int keySize = 0;
		const char* pKey = _readKey(p,pEOF,&keySize);

		//この行でキーが見つからなければ終端か改行までスキップ
		if (pKey == nullptr) {
			p = SkipToEndLine(p, pEOF)+1;
			continue;
		}

		*pElement = new Element();
		(*pElement)->pKey = (unsigned short)(pKey - m_pData);
		(*pElement)->sizeKey = keySize;
		(*pElement)->pVal = 0;
		(*pElement)->sizeVal = 0;
		(*pElement)->csv = nullptr;
		(*pElement)->pNext = nullptr;

		if (m_keyBufferSize < keySize) {
			m_keyBufferSize = keySize;
		}

		//キーが見つかっていれば同じ行内に=があるのでそこまでpをすすめる
		p = DataUtil::FindAsciiCharInLine(pKey,pEOF,'=');

		//=の次に勧める
		++p;

		//CSVの可能性がある.
		//pを進めていって、最初にヒットした文字（空白・制御コード除く）が { であればCSVとする
		{
			CSVData* pCSV = nullptr;
			const char* pHead = p;
			const char* pTail = nullptr;

			while (pHead < pEOF) {
				if (*pHead == ' ' || DataUtil::IsControlChar(*pHead)) {
					++pHead;
				}
				break;
			}

			//開きカッコを見つけていたら 
			if (*pHead == '{') {

				(*pElement)->csv = new CSVData();

				//カッコの次にすすめて
				++pHead;
				//閉じカッコを探す
				pTail = pHead;
				while (pTail < pEOF) {
					if (*pTail == '}') {
						break;
					}
					++pTail;
				}

				//ちゃんと閉じられていることを確認してCSVDataに解析させる
				if (*pTail == '}') {
					int csvSize = (int)(pTail - pHead) - 1;
					(*pElement)->csv->Load(pHead + 1, csvSize);
				}
				p = pTail;
			}
		}

		//CSVとしての値でなければ＝の後ろの文字列を値として取得
		if((*pElement)->csv == nullptr){
			int valSize = 0;
			const char* pVal = _readVal(p,pEOF,&valSize);

			(*pElement)->pVal = (unsigned short)(pVal - m_pData);
			(*pElement)->sizeVal = (unsigned short)valSize;

			if (m_valBufferSize < valSize) {
				m_valBufferSize = valSize;
			}
		}

		//pは次の行に.
		p = SkipToEndLine(p, pEOF)+1;

		++m_elementNum;
		pElement = &((*pElement)->pNext);
	}

	if (m_keyBufferSize > 0) {
		++m_keyBufferSize;
		m_pKeyBuffer = new char[m_keyBufferSize];
	}
	if (m_valBufferSize > 0) {
		++m_valBufferSize;
		m_pValBuffer = new char[m_valBufferSize];
	}

	Debug::Logf("KeyValueData::Load データの構築完了。要素数:%d\n",m_elementNum);
}

//解放
void KeyValueData::Unload()
{
	m_pData = nullptr;
	
	{
		Element* p = m_pElements;
		while(p) {
			Element* pNext = p->pNext;
			SAFE_DELETE(p->csv);
			SAFE_DELETE(p);
			p = pNext;
		}
		m_pElements = nullptr;
	}

	m_elementNum = 0;

	m_keyBufferSize = 0;
	SAFE_DELETE_ARRAY(m_pKeyBuffer);

	m_valBufferSize = 0;
	SAFE_DELETE_ARRAY(m_pValBuffer);
}

//データの要素数を返す
int KeyValueData::GetKeyCount()const {
	return m_elementNum;
}

//指定したキーのデータを整数値として返す
int KeyValueData::GetInt(const char* key)const {

	Element* pElem = _find(key);
	if (pElem == nullptr) {
		return 0;
	}

	memcpy_s(m_pValBuffer, m_valBufferSize, &m_pData[pElem->pVal],pElem->sizeVal);
	m_pValBuffer[pElem->sizeVal] = 0;

	return atoi(m_pValBuffer);
}
int KeyValueData::GetInt(const string& key)const {
	return GetInt(key.c_str());
}

//指定した番号のデータを実数値として返す
float KeyValueData::GetFloat(const char* key)const {
	Element* pElem = _find(key);
	if (pElem == nullptr) {
		return 0.0f;
	}
	memcpy_s(m_pValBuffer, m_valBufferSize, &m_pData[pElem->pVal],pElem->sizeVal);
	m_pValBuffer[pElem->sizeVal] = 0;

	return (float)atof(m_pValBuffer);
}
float KeyValueData::GetFloat(const string& key)const {
	return GetFloat(key.c_str());
}

//指定した番号のデータを文字列として返す(string型に詰めて返すVer)
void KeyValueData::GetString(const char* key, std::string* pString)const {

	if (pString == nullptr) {
		return;
	}

	Element* pElem = _find(key);
	if (pElem == nullptr) {
		return ;
	}
	
	memcpy_s(m_pValBuffer, m_valBufferSize, &m_pData[pElem->pVal],pElem->sizeVal);
	m_pValBuffer[pElem->sizeVal] = 0;

	*pString = m_pValBuffer;
}
void KeyValueData::GetString(const string& key, std::string* pString)const {
	GetString(key.c_str(),pString);
}

//指定した番号のデータを文字列として返す(配列に詰めて返すVer）
void KeyValueData::GetString(const char* key, char* pBuffer, int bufferSize)const {

	if (pBuffer == nullptr || bufferSize == 0) {
		return;
	}

	Element* pElem = _find(key);
	if (pElem == nullptr) {
		return ;
	}

	int userBufferSize = pElem->sizeVal + 1; 
	if ( userBufferSize > bufferSize  ) {
		userBufferSize = bufferSize;
	}

	memcpy_s(pBuffer, bufferSize, &(m_pData[pElem->pVal]),(size_t)userBufferSize-1);
	pBuffer[userBufferSize-1] = 0;
}
void KeyValueData::GetString(const string& key, char* pBuffer, int bufferSize)const {
	GetString(key.c_str(),pBuffer,bufferSize);
}

//CSVデータの取得
CSVData* KeyValueData::GetCSVData(const char* key)const {
	Element* pElem = _find(key);
	if (pElem == nullptr) {
		return nullptr;
	}
	return pElem->csv;
}
CSVData* KeyValueData::GetCSVData(const string& key)const {
	return GetCSVData(key.c_str());
}

//指定した番号のキーを文字列として返す
void KeyValueData::GetKey(int index, std::string* pString)const {

	if (index < 0 || index >= m_elementNum) {
		return;
	}
	
	if (pString == nullptr) {
		return;
	}

	Element* p = m_pElements;
	for (int i = 0; i < index; ++i) {
		p = p->pNext;
	}
	memcpy_s(m_pKeyBuffer, m_keyBufferSize, &p->pKey, p->sizeKey);
	m_pKeyBuffer[p->sizeKey] = 0;
	*pString = m_pKeyBuffer;

}
void KeyValueData::GetKey(int index, char* pBuffer, int bufferSize)const {

	if (pBuffer == nullptr || bufferSize == 0) {
		return;
	}

	if (index < 0 || index >= m_elementNum) {
		return;
	}
	
	Element* p = m_pElements;
	for (int i = 0; i < index; ++i) {
		p = p->pNext;
	}

	int userBufferSize = p->sizeKey + 1; 
	if ( userBufferSize > bufferSize  ) {
		userBufferSize = bufferSize;
	}

	memcpy_s(pBuffer, bufferSize, &(m_pData[p->pKey]),(size_t)userBufferSize-1);
	pBuffer[userBufferSize-1] = 0;
}

//解析したデータをコンソールに出力する（デバッグ用）
void KeyValueData::PrintKeyValueData()const {
	
	Element* p = m_pElements;
	for (int i = 0; i < m_elementNum; ++i) {

		memcpy_s(m_pKeyBuffer, m_keyBufferSize, &(m_pData[p->pKey]), p->sizeKey);
		m_pKeyBuffer[p->sizeKey] = 0;

		cout << m_pKeyBuffer << "=";

		if (p->csv) {
			p->csv->PrintCSVData();
		}
		else {
			memcpy_s(m_pValBuffer, m_valBufferSize, &m_pData[p->pVal],p->sizeVal);
			m_pValBuffer[p->sizeVal] = 0;
			cout << m_pValBuffer << endl;
		}

		p = p->pNext;
	}
}

KeyValueData::Element* KeyValueData::_find(const char* pKey)const {

	if (pKey == nullptr) {
		return nullptr;
	}

	Element* p = m_pElements;

	while(p) {

		memcpy_s(m_pKeyBuffer, m_keyBufferSize, &(m_pData[p->pKey]), p->sizeKey);
		m_pKeyBuffer[p->sizeKey] = 0;
		
		if (strcmp(m_pKeyBuffer, pKey) == 0) {
			return p;
		}
		p = p->pNext;
	}

	return nullptr;
}
