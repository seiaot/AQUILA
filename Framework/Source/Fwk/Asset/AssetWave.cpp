// ----------------------------------------------------
// Waveアセットクラス
// 
// 
// 
// ----------------------------------------------------
#include "Fwk/Asset/AssetWave.h"
#include "Lib/Util/Log.h"
#include "Lib/LibDef.h"

// マルチメディア関連
#pragma comment ( lib, "winmm.lib" )
#include<mmsystem.h>

// 文字列
#include<string>

namespace Fwk
{

AssetWave::AssetWave() {

}

AssetWave::~AssetWave() {

}

bool AssetWave::Init(const wchar_t* pFilePath) {
	
	HMMIO mmioHandle = nullptr;

	// チャンク情報
	MMCKINFO chunkInfo{};

	// RIFFチャンク用
	MMCKINFO riffChunkInfo{};

	Debug::Logf("Waveファイルの読み込み開始:%ls\n",pFilePath);

	// WAVファイルを開く
	mmioHandle = mmioOpen((LPWSTR)pFilePath,nullptr,MMIO_READ);

	if (!mmioHandle){
		Debug::Logf("エラー：Waveファイルを開けませんでした。%ls\n",pFilePath);
		return false;
	}
	
	// RIFFチャンクに侵入するためにfccTypeにWAVEを設定をする
	riffChunkInfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');

	// RIFFチャンクに侵入する
	if (mmioDescend(
		mmioHandle,		//MMIOハンドル
		&riffChunkInfo,	//取得したチャンクの情報
		nullptr,		//親チャンク
		MMIO_FINDRIFF	//取得情報の種類
	) != MMSYSERR_NOERROR)
	{
		mmioClose(mmioHandle, MMIO_FHOPEN);	
		Debug::Logf("エラー：Riffチャンクに侵入失敗しました。%ls\n",pFilePath);
		return false;
	}

	// 侵入先のチャンクを"fmt "として設定する
	chunkInfo.ckid = mmioFOURCC('f','m','t',' ');
	if (mmioDescend(
		mmioHandle,
		&chunkInfo,
		&riffChunkInfo,
		MMIO_FINDCHUNK
	) != MMSYSERR_NOERROR)
	{
		mmioClose(mmioHandle, MMIO_FHOPEN);	
		Debug::Logf("エラー：fmtチャンクがありません。%ls\n",pFilePath);
		return false;
	}

	// fmtデータの読み込み
	DWORD readSize = mmioRead(
		mmioHandle,						//ハンドル
		(HPSTR)&m_waveData.format,		// 読み込み用バッファ
		chunkInfo.cksize				//バッファサイズ
	);

	if (readSize != chunkInfo.cksize){
		mmioClose(mmioHandle, MMIO_FHOPEN);
		Debug::Logf("エラー：読み込みサイズが一致していません。%ls\n",pFilePath);
		return false;
	}

	// フォーマットチェック
	if (m_waveData.format.wFormatTag != WAVE_FORMAT_PCM){
		mmioClose(mmioHandle, MMIO_FHOPEN);
		Debug::Logf("エラー：Waveフォーマットエラーです。%ls\n",pFilePath);
		return false;
	}

	// fmtチャンクを退出する
	if (mmioAscend(mmioHandle, &chunkInfo, 0) != MMSYSERR_NOERROR){
		mmioClose(mmioHandle, MMIO_FHOPEN);
		Debug::Logf("エラー：fmtチャンク退出失敗。%ls\n",pFilePath);
		return false;
	}

	// 1サンプル当たりのバッファサイズを算出
	m_waveData.format.wBitsPerSample = m_waveData.format.nBlockAlign * 8 /m_waveData.format.nChannels;

	// dataチャンクに侵入
	chunkInfo.ckid = mmioFOURCC('d','a','t','a');
	if (mmioDescend(mmioHandle, &chunkInfo, &riffChunkInfo, MMIO_FINDCHUNK) != MMSYSERR_NOERROR){
		mmioClose(mmioHandle, MMIO_FHOPEN);
		Debug::Logf("エラー：dataチャンク侵入失敗。%ls\n",pFilePath);
		return false;
	}

	// サイズ保存
	m_waveData.size = (size_t)chunkInfo.cksize;

	// dataチャンク読み込み
	m_waveData.buffer= new char[chunkInfo.cksize];
	readSize = mmioRead(mmioHandle, (HPSTR)m_waveData.buffer, chunkInfo.cksize);

	if (readSize != chunkInfo.cksize){
		mmioClose(mmioHandle, MMIO_FHOPEN);
		delete[] m_waveData.buffer;
		Debug::Logf("エラー：dataチャンク読み込み失敗。%ls\n",pFilePath);
		return false;
	}

	// ファイルを閉じる
	mmioClose(mmioHandle, MMIO_FHOPEN);

	return true;
}

void AssetWave::OnTerm() {
	SAFE_DELETE_ARRAY(m_waveData.buffer);
}

const Lib::Audio::WaveData* AssetWave::GetWaveData()const {
	return &m_waveData;
}

}
