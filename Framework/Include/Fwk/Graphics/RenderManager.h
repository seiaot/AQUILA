#pragma once

#include <Windows.h>

#include "Lib/Graphics/Graphics.h"
#include "Lib/Graphics/ConstantBuffer.h"
#include "Lib/Graphics/RootSignature.h"
#include "Lib/Graphics/PipelineState.h"
#include "Lib/Graphics/ResShader.h"

#include "Fwk/Shape/Shape.h"
#include "Fwk/Graphics/Camera.h"
#include "Fwk/Graphics/Color.h"
#include "Fwk/Graphics/Font.h"
#include "SpriteFont.h"
#include "RenderDef.h"

#if defined(DEBUG) || defined(_DEBUG)
#include "Fwk/Debug/DebugLog.h"
#endif

#include <cstdint>
#include <vector>
#include <map>

using namespace Lib::Graphics;

namespace Fwk {

class RenderObject;

class RenderManager
{
	friend class Framework;

private:
	
	struct Layer {
		std::string name;
		int order = 0;
		bool isActive =false;
		std::vector<RenderObject*> requestBuffer;
	};

private:

	RenderManager(const RenderManager&) = delete;
	void operator = (const RenderManager&) = delete;

	//この辺りはフレームワークにのみ呼ばせる

	RenderManager();
	~RenderManager();

	void Init(
		HWND hwnd,
		uint32_t width,
		uint32_t height,
		uint32_t bufferSize,
		uint32_t DescriptorSize_Resource,
		uint32_t DescriptorSize_Sampler,
		const wchar_t* pFontPath,
		const std::map<string,wstring>& ShadersPath
	);

	void Update(float deltaTime);
	void Term();
	void Render();

public:

	void RequestRender(RenderObject* pDrawable);

	void CreateConstantBuffer(Lib::Graphics::ConstantBuffer& ConstBufferRef);

	void SyncGpuProc();

	Lib::Graphics::IGraphics* GetGraphics();

	void SetClearColor(float r, float g, float b);
	void SetClearColor(const float rgb[]);
	void SetClearColor(const DirectX::XMVECTORF32& _color);

	//-----------------------------------------
	//レイヤーの追加/削除
	//-----------------------------------------
	void AddLayer(const std::string& name);
	void RemoveLayer(const std::string& name);
	void SetLayerOrder(const std::string& name,int order);
	void SetLayerActive (const std::string& name, bool isActive);

	//-----------------------------------------
	//カメラの追加登録
	//-----------------------------------------
	void SetCamera(const Camera& camera);
	void RemoveCamera(const std::string& name);
	Camera& GetCamera();
	Camera& GetCamera(const std::string& name);

	//-----------------------------------------
	//デフォルトカメラの操作
	//-----------------------------------------
	//位置
	void SetCameraPosition(const Vector2f& position);
	void SetCameraPosition(const Vector3f& position);
	Vector3f GetCameraPosition()const;

	//オーダー
	void SetCameraOrder(int order);
	int GetCameraOrder()const;

	//アクティブ
	void SetCameraActive(bool isActive);
	bool IsCameraActive()const;

	//注視点
	void SetCameraFocus(const Vector3f& focus, const FocusMode focusMode);
	FocusMode GetCameraFocusMode()const;
	const Vector3f& GetCameraFocus()const;

	//ビュー行列
	DirectX::XMMATRIX GetCameraViewMatrix()const;

	//プロジェクション行列
	void SetCameraPerspective(float viewWidth, float viewHeight, float nearZ, float farZ, float fov);
	void SetCameraOrtho(float viewWidth, float viewHeight, float nearZ, float farZ);
	DirectX::XMMATRIX GetCameraProjectionMatrix()const;

	//カメラの描画対象レイヤーの操作
	void AddCameraRenderLayer(const std::string& targetLayerName);
	void RemoveCameraRenderLayer(const std::string& targetLayerName);

	//-----------------------------------------
	//カメラ名を指定して操作
	//-----------------------------------------
	//位置
	void SetCameraPosition(const std::string& name, const Vector2f& position);
	void SetCameraPosition(const std::string& name, const Vector3f& position);
	Vector3f GetCameraPosition(const std::string& name)const;

	//オーダー
	void SetCameraOrder(const std::string& name, int order);
	int GetCameraOrder(const std::string& name)const;

	//アクティブ
	void SetCameraActive(const std::string& name, bool isActive);
	bool IsCameraActive(const std::string& name)const;

	//注視点
	void SetCameraFocus(const std::string& name, const Vector3f& focus, const FocusMode focusMode);
	FocusMode GetCameraFocusMode(const std::string& name)const;
	const Vector3f& GetCameraFocus(const std::string& name)const;

	//ビュー行列
	DirectX::XMMATRIX GetCameraViewMatrix(const std::string& name)const;

	//プロジェクション行列
	void SetCameraPerspective(const std::string& name, float viewWidth, float viewHeight, float nearZ, float farZ, float fov);
	void SetCameraOrtho(const std::string& name, float viewWidth, float viewHeight, float nearZ, float farZ);
	DirectX::XMMATRIX GetCameraProjectionMatrix(const std::string& name)const;

	//カメラの描画対象レイヤーの操作
	void AddCameraRenderLayer(const std::string& cameraName,const std::string& targetLayerName);
	void RemoveCameraRenderLayer(const std::string& cameraName,const std::string& targetLayerName);

public:

	void PrintText(const wchar_t* pText);
	void PrintText(const wchar_t* pText, float pos_x, float pos_y);

	void PrintText(const char* pText);
	void PrintText(const char* pText, float pos_x, float pos_y);

	void PrintText(const string& pText);
	void PrintText(const string& pText, float pos_x, float pos_y);

	void SetTextColor(float r, float g, float b);
	void SetTextColor(const float rgb[]);
	void SetTextColor(const DirectX::XMVECTORF32& _color);

	void SetTextSize(float size);

	void DebugLogImpl(const char* format, ...);
	void DebugLogImpl(const string& format, ...);

private:

	void _PrintTextImpl(const wchar_t* pText, float pos_x, float pos_y, const float rgb[], float scale);
	void _PrintTextImpl(const char* pText, float pos_x, float pos_y, const float rgb[], float scale);

private:

	enum class PIPELINE {
		NONE = -1,

		//3d
		DEFAULT,

		//2d
		OPAQUE_2D,
		TRANSPARENT_2D,
		ADD_2D,
		SUB_2D,
		INVERSE_2D,

		//2d_NoTex
		OPAQUE_2D_NoTex,
		TRANSPARENT_2D_NoTex,
		ADD_2D_NoTex,
		SUB_2D_NoTex,
		INVERSE_2D_NoTex,

		//line
		LINE_SEGMENTS,

		COUNT,
	};

private:

	void _postPresent();
	void _clearRequest();

	PIPELINE _getPipelineState(RENDER_PASS pass, RENDER_MODE mode);
	//ROOT_SIGNATURE _getRootSignature(RENDER_PASS pass, RENDER_MODE mode);
	void _onRenderPassChanged(RENDER_PASS pass, RENDER_PASS prevPass, Lib::Graphics::IGraphics* g);

	Camera* _findCamera(const string& name);
	const Camera* _findCameraConst(const string& name)const;
	int _getCameraIndex(const string& name);

	Layer* _findLayer(const string& name);
	int _getLayerIndex(const string& name);

	static bool cmpareDrawReq(RenderObject* p0, RenderObject* p1);
	static bool cmpareCamera(Camera* p0, Camera* p1);
	static bool cmpareLayer(RenderManager::Layer* p0, RenderManager::Layer* p1);

private:

	enum class VtxShader {
		Default,

		COUNT
	};
	enum class FragShader {
		NoTexNoLit,
		TexNoLit,

		COUNT
	};

	bool m_IsInitilized;

	Graphics m_Graphics;

	std::vector<Layer*> m_Layers;
	int m_RequestBufferSize;

	SpriteFont m_SpriteFont;

	RootSignature m_RootSignature[(int)PIPELINE::COUNT];
	PipelineState m_PipelineState[(int)PIPELINE::COUNT];

	ResShader m_VtxShader[(int)VtxShader::COUNT];
	ResShader m_PixShader[(int)FragShader::COUNT];

	std::vector<Camera*> m_Cameras;

	enum {
		TEXT_BUFFER_MAX = 128
	};
	Font m_Text[TEXT_BUFFER_MAX];
	int m_UseTextNum;

	float m_TextColor[3];
	float m_TextScale;

#if defined(DEBUG) || defined(_DEBUG)
	Fwk::DebugLog mDebugLog;
#endif
};

}