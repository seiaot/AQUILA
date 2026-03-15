#pragma once

#include <DirectXMath.h>
#include <string>
#include <vector>
using namespace std;

#include "Lib/Math/Vector.h"
using namespace Lib::Math;

//注視点モード
enum class FocusMode {
    None,
    Absolute, //絶対位置
    Relative, //相対位置
};

//描画用カメラ
class Camera 
{
public:

    Camera();
    ~Camera();

    void SetPosition(const Vector2f& position);
    void SetPosition(float x, float y);
    void SetPosition(const Vector3f& position);
    void SetPosition(float x, float y, float z);
    Vector3f GetPosition()const;

    DirectX::XMMATRIX GetViewMatrix()const;

    void SetPerspective(float viewWidth, float viewHeight, float nearZ, float farZ, float fov);
    void SetOrtho(float viewWidth, float viewHeight, float nearZ, float farZ);
    static void SetDefaultOrtho(float viewWidth, float viewHeight, float nearZ, float farZ);
    
    DirectX::XMMATRIX GetProjectionMatrix()const;

    void SetFocus(const Vector3f& focus, FocusMode mode );
    FocusMode GetFocusMode()const;
    const Vector3f& GetFocus()const;

    void SetActive(bool isActive);
    bool IsActive()const;

    //描画対象のレイヤーを加える
    void AddRenderLayer(const string& layerName);
    //描画対象のレイヤーを外す
    void RemoveRenderLayer(const string& layerName);
    //描画対象のレイヤーを取得する
    const std::vector<string>& GetRenderLayers()const;
    //指定したレイヤーを描画対象としているかを返す
    bool ContainsRenderLayer(const string& layerName)const;

    //名前
    void SetName(const string& name);
    const string& GetName()const;

    //表示(描画)順
    void SetOrder(int order);
    int GetOrder()const;

private:

    bool m_isActive;
    string m_name;
    std::vector<string> m_targetLayers;
    int m_order;

    Vector3f m_position;
    Vector3f m_focus;
    FocusMode m_focusMode;

    DirectX::XMMATRIX mProjMtx;

private:
    //プロジェクション行列用
    static float m_defaultViewWidth;
    static float m_defaultViewHeight;
    static float m_defaultNearZ;
    static float m_defaultFarZ;
};