#pragma once

#include <map>
#include <string>

#include "Lib/Graphics/Graphics.h"
#include "Lib/Graphics/ConstantBuffer.h"
#include "Lib/Math/Vector.h"

#include "Fwk/Shape/Shape.h"
#include "RenderObject.h"

using namespace Lib::Graphics;
using namespace Lib::Math;

class LineSegments : public Fwk::RenderObject
{
    LineSegments(const LineSegments&) = delete;
    void operator = (const LineSegments&) = delete;

public:

    LineSegments();
    ~LineSegments();

    void Init(int vtxMax,int indexMax);
    void Term();
    void Update();
    void Draw();

    //------------------------------------------------
    //パラメータ設定系
    //------------------------------------------------

    //位置設定(2D)
    void SetPosition(float x, float y);
    //位置設定(2D)
    void SetPosition(const Vector2f& vPos);

    //頂点座標設定(2D) SetPositionした位置からの相対位置
    void SetVertexPosition(int vtxIndex, float x, float y);
    //頂点座標設定(2D) SetPositionした位置からの相対位置
    void SetVertexPosition(int vtxIndex, const Vector2f& vPos);
    //頂点座標設定(2D) SetPositionした位置からの相対位置
    void SetVertexPosition(int vtxIndex, const XMFLOAT3& vPos);

    //インデックス設定
    void SetIndices(const uint32_t indices[], int indexNum);
    //インデックス設定
    void SetIndices(const std::vector<uint32_t>& indices);

    //回転（Z軸対象）　引数はDegree 0.0f～360.0fで一周
    void SetRotation(float rotDeg);
    //回転（全軸対象）　引数はDegree 0.0f～360.0fで一周
    void SetRotation(float rotDegX, float rotDegY, float rotDegZ);
    //回転　二次元ベクトルが示す向きに回転する
    void SetRotation(const Vector2f& vDirection);
    //スケール設定
    void SetScale(float sx, float sy);
    //スケール設定
    void SetScale(const Vector2f& vScale);

    void SetColor(float r, float g, float b);
    void SetColor(int vertexIndex, float r, float g, float b);

    //X軸フリップ
    void SetFlipX(bool isFlip);

    //Y軸フリップ
    void SetFlipY(bool isFlip);

    void SetTopology(D3D_PRIMITIVE_TOPOLOGY topology);

    //------------------------------------------------
    //パラメータ取得系
    //------------------------------------------------
    //X軸の回転の値を戻す
    float GetRotationX()const;
    //Y軸の回転の値を戻す
    float GetRotationY()const;
    //Z軸の回転の値を戻す
    float GetRotationZ()const;
    //位置情報を返す
    float GetPositionX()const;
    float GetPositionY()const;
    Vector2f GetPosition()const;
    //スケールを返す
    float GetScaleX()const;
    float GetScaleY()const;

    bool IsFlipX()const;
    bool IsFlipY()const;

    //------------------------------------------------
    //implements interface methods.
    //------------------------------------------------
    void OnDraw(IGraphics* g,Camera* pCamera) override;
    //------------------------------------------------
    //implements RenderObject
    //------------------------------------------------
    RENDER_PASS GetRenderPass()const override;
    RENDER_MODE GetRenderMode()const override;


private:

    float m_RotDeg[3];
    
    DirectX::XMMATRIX m_ScaleMtx;
    DirectX::XMMATRIX m_RotMtx;
    DirectX::XMMATRIX m_TransMtx;

    ConstantBuffer m_ConstantBuffer[Graphics::FrameCount];

    Fwk::Shape m_shape;

    bool m_isFlipX;
    bool m_isFlipY;
    bool m_IsInitialized;

    int m_vertexMax;
};