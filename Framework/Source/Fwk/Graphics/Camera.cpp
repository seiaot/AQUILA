#include "Fwk/Graphics/Camera.h"

#include "Lib/Util/Log.h"

//プロジェクション行列用
float Camera::m_defaultViewWidth = 0.0f;
float Camera::m_defaultViewHeight = 0.0f;
float Camera::m_defaultNearZ = 0.0f;
float Camera::m_defaultFarZ = 1000.0f;

Camera::Camera()
    : m_isActive(true)
    , m_name("main")
    , m_targetLayers()
    , m_order(0)
    , m_position(0.0f, 0.0f, -10.0f)
    , m_focus(0.0f, 0.0f, 10.0f)
    , m_focusMode(FocusMode::Relative)
{
    mProjMtx = DirectX::XMMatrixOrthographicLH(
        m_defaultViewWidth, 
        m_defaultViewHeight, 
        m_defaultNearZ, 
        m_defaultFarZ
    );
}

Camera::~Camera()
{
    ;
}

void Camera::SetPosition(const Vector2f& position) {
    m_position.x = position.x;
    m_position.y = position.y;
}
void Camera::SetPosition(float x, float y) {
    m_position.x = x;
    m_position.y = y;
}
void Camera::SetPosition(const Vector3f& position) {
    m_position = position;
}
void Camera::SetPosition(float x, float y, float z) {
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
}
Vector3f Camera::GetPosition()const {
    return m_position;
}

DirectX::XMMATRIX Camera::GetViewMatrix()const 
{
	DirectX::XMVECTOR eyePos = DirectX::XMVectorSet(m_position.x, m_position.y, m_position.z, 0.0f);
    DirectX::XMVECTOR targetPos = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);;
    if (m_focusMode == FocusMode::Absolute) {
        targetPos = DirectX::XMVectorSet(m_focus.x, m_focus.y, m_focus.z, 0.0f);
    }
    else {
        targetPos = DirectX::XMVectorSet(m_position.x + m_focus.x, m_position.y + m_focus.y, m_position.z + m_focus.z, 0.0f);
    }

    
    //同じ座標だったらLockAtできない
    XMVECTOR epsilon = DirectX::XMVectorReplicate(1e-5f);
    if (DirectX::XMVector4NearEqual(eyePos, targetPos, epsilon)) {
        Debug::Logf("カメラの位置と注視点が同値になっています\n");
        return DirectX::XMMatrixIdentity();
    }

    DirectX::XMVECTOR upward = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    return  DirectX::XMMatrixLookAtLH(eyePos, targetPos, upward);
}

void Camera::SetOrtho(float viewWidth, float viewHeight, float nearZ, float farZ) {
    mProjMtx = DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ);
}

void Camera::SetPerspective(float viewWidth, float viewHeight, float nearZ, float farZ, float fov)
{
    mProjMtx = DirectX::XMMatrixPerspectiveFovLH(
        XMConvertToRadians(fov),
        viewWidth / viewHeight, 
        nearZ, 
        farZ
    );
}

DirectX::XMMATRIX Camera::GetProjectionMatrix()const {
    return mProjMtx;
}

void Camera::SetFocus(const Vector3f& focus, FocusMode mode) {
    m_focus = focus;
    m_focusMode = mode;
}
FocusMode Camera::GetFocusMode()const {
    return m_focusMode;
}
const Vector3f& Camera::GetFocus()const {
    return m_focus;
}


void Camera::SetDefaultOrtho(float viewWidth, float viewHeight, float nearZ, float farZ)
{
    m_defaultViewWidth = viewWidth;
    m_defaultViewHeight = viewHeight;
    m_defaultNearZ = nearZ;
    m_defaultFarZ = farZ;
}


void Camera::SetActive(bool isActive) {
    m_isActive = isActive;
}
bool Camera::IsActive()const {
    return m_isActive;
}

//描画対象のレイヤーを加える
void Camera::AddRenderLayer(const string& layerName) {
    if (ContainsRenderLayer(layerName)) {
        return;
    }
    m_targetLayers.push_back(layerName);
}

//描画対象のレイヤーを外す
void Camera::RemoveRenderLayer(const string& layerName) {
    
    int index = -1;
    for (int i = 0; i < m_targetLayers.size(); ++i) {
        if (m_targetLayers[i]._Equal(layerName)) {
            index = i;
            break;
        }
    }

    if (index < 0) {
        return;
    }

    m_targetLayers.erase(m_targetLayers.begin() + index);
}

//描画対象のレイヤーを取得する
const std::vector<string>& Camera::GetRenderLayers()const {
    return m_targetLayers;
}

//指定したレイヤーを描画対象としているかを返す
bool Camera::ContainsRenderLayer(const string& layerName)const {
    for (int i = 0; i < m_targetLayers.size(); ++i) {
        if (m_targetLayers[i]._Equal(layerName)) {
            return true;
        }
    }
    return false;
}


void Camera::SetName(const string& name) {
    m_name = name;
}

const string& Camera::GetName()const {
    return m_name;
}

//表示(描画)順
void Camera::SetOrder(int order) {
    m_order = order;
}

int Camera::GetOrder()const {
    return m_order;
}