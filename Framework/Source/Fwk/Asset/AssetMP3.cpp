// ----------------------------------------------------
// MP3アセットクラス
// 
// 
// 
// ----------------------------------------------------
#include "Fwk/Asset/AssetMP3.h"
#include "Lib/Util/Log.h"
#include "Lib/LibDef.h"

// 文字列
#include<string>

namespace Fwk
{

AssetMP3::AssetMP3()
	: m_waveData()
	, m_pMFSourceReader(nullptr)
	, m_pMFMediaType(nullptr)
{

}

AssetMP3::~AssetMP3() {

}

bool AssetMP3::Init(const wchar_t* pFilePath) {
	
	Term();

	Debug::Logf("MP3ファイルの読み込み開始:%ls\n",pFilePath);

	//ソースリーダーの作成
	MFCreateSourceReaderFromURL(pFilePath, NULL, &m_pMFSourceReader);

	//メディアタイプの取得
	MFCreateMediaType(&m_pMFMediaType);
	m_pMFMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	m_pMFMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	m_pMFSourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, m_pMFMediaType);

	m_pMFMediaType->Release();
	m_pMFMediaType = nullptr;
	m_pMFSourceReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &m_pMFMediaType);

	//オーディオデータ形式の作成
	{
		WAVEFORMATEX* pFormat = nullptr;
		MFCreateWaveFormatExFromMFMediaType(m_pMFMediaType, &pFormat, nullptr);
		m_waveData.format = *pFormat;
		CoTaskMemFree(pFormat);
	}

	//データの読み込み
	std::vector<BYTE> mediaData;
	while (true)
	{
		//読み込む
		IMFSample* pMFSample{ nullptr };
		DWORD dwStreamFlags{ 0 };
		m_pMFSourceReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &dwStreamFlags, nullptr, &pMFSample);

		//ストリームの終了まで読んだ
		if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM){
			break;
		}

		//サンプリングデータを連続したバッファに変換する
		IMFMediaBuffer* pMFMediaBuffer{ nullptr };
		pMFSample->ConvertToContiguousBuffer(&pMFMediaBuffer);

		//バッファをロックしてバッファにアクセスできるようにする
		BYTE* pBuffer{ nullptr };
		DWORD cbCurrentLength{ 0 };
		pMFMediaBuffer->Lock(&pBuffer, nullptr, &cbCurrentLength);

		//読み込みバッファをリサイズしてWaveデータを読み込み
		mediaData.resize(mediaData.size() + cbCurrentLength);
		memcpy(mediaData.data() + mediaData.size() - cbCurrentLength, pBuffer, cbCurrentLength);

		pMFMediaBuffer->Unlock();
		pMFMediaBuffer->Release();
		pMFSample->Release();
	}
	
	m_waveData.buffer = new char[mediaData.size()];
	m_waveData.size = mediaData.size();
	memcpy(m_waveData.buffer, mediaData.data(), m_waveData.size);

	return true;
}

void AssetMP3::OnTerm() {
	SAFE_DELETE_ARRAY(m_waveData.buffer);

	if (m_pMFMediaType != nullptr) {
		m_pMFMediaType->Release();
		m_pMFMediaType = nullptr;
	}


	if (m_pMFSourceReader != nullptr) {
		m_pMFSourceReader->Release();
		m_pMFSourceReader = nullptr;
	}
}

const Lib::Audio::WaveData* AssetMP3::GetWaveData()const {
	return &m_waveData;
}

}
