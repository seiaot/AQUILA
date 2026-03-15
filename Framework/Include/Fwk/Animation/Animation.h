#pragma once

#include <string>
#include <vector>
#include <functional>
#include <string>

#include "Fwk/Graphics/SpriteTypes.h"

//クラスのメンバ関数をアニメーションのイベントのコールバックにするときのマクロ
#define CreateAnimationEventCallback(_func) std::bind(&_func, this, std::placeholders::_1)

enum class AnimationEventType
{
    Begin,      //アニメーション再生が開始した
    Completed,  //アニメーション再生が完了した（最後まで再生して止まった）
    Looped,     //アニメーション再生がループした
    Stopped,    //アニメーション再生が停止した（中断した）
    KeyframeProgressed,   //次のキーフレームに進んだ
    Paused,     //一時停止
    Unpaused,   //一時停止解除
};

struct AnimationEvent {
    AnimationEventType EventType;
    const std::string& Name;
    float Time;
    int PropertyIndex;
    int KeyFrameIndex;
};

enum class AnimationPropertyType
{
    None = -1,
    UV,
    Scale,
    Rotation,
    Color,
    Alpha,
    Translate,
    Pivot,
    AddColor,

    Num
};

struct AnimationParam
{
    float uv[4];
    float rotDeg;
    float scale[2];
    float color[3];
    float alpha;
    float translate[2];
    float pivot[2];
    float addColor[3];
};

struct KeyFrame
{
    float TimeSec;
    float Values[4];
};

struct AnimationProperty
{
    AnimationPropertyType Type = AnimationPropertyType::None;
    std::vector<KeyFrame> KeyFrames;
};

class Animation 
{
public:

    Animation();
    ~Animation();

    void Init();
    void Term();

    void Update(float deltaTime);
    void Start();
    void Pause(bool bPause);
    void Stop();

    void SetName(const char* name);
    void SetName(const std::string name);
    void Clear();
        
    void SetLoop(bool bLoop);
    bool IsLoop()const;

    void SetAnimSpeed(float AnimSpeed);
    float GetAnimSpeed()const;

    const AnimationParam& GetAnimParam()const;
    const std::string& GetName()const;

    bool IsPlaying()const;
    bool IsPausing()const;

    void AddProperty(const AnimationProperty& animProperty);

    void SetEventCallback(std::function<void(const AnimationEvent&)> func);
    void ClearEventCallback();

    bool HasProperty(AnimationPropertyType prop)const;

private:

    void _updateAnim(float timeSec);
    size_t _getKeyFrameIndex(const std::vector<KeyFrame>& keyFrames, float timeSec);
    void _initAnimationParam(AnimationParam* pParam);

    void _sendEvent(AnimationEventType EventType, float Time, int PropertyIndex = -1, int KeyFrameIndex = -1);

private:

    std::string m_Name;
    AnimationParam m_CurrentAnimParam;
    std::vector<AnimationProperty> m_AnimProperties;
    std::vector<int> m_CurrentKeyFrameIndex;
    std::function<void(const AnimationEvent&)> m_EventCallback;
   
    float m_AnimSpeed;
    float m_CurrentTime;
    float m_EndTime;
    bool m_IsExec;
    bool m_IsPause;
    bool m_IsLoop;
};

//各種アニメーションの簡易作成関数
void CreateAnimationUV(Animation& pAnim, const char* name, int keyFrameNum, float totalTime, bool bLoop, const float uvs[][4]);
void CreateAnimationUV(Animation& pAnim, const char* name, int keyFrameNum, float totalTime, bool bLoop, const UVRect uvs[]);
void CreateAnimationScale(Animation& pAnim, const char* name, int keyFrameNum, float totalTime, bool bLoop, const float scale[][2]);
void CreateAnimationRotation(Animation& pAnim, const char* name, int keyFrameNum, float totalTime, bool bLoop, const float rotDeg[]);
void CreateAnimationColor(Animation& pAnim, const char* name, int keyFrameNum, float totalTime, bool bLoop, const float color[][3]);
void CreateAnimationAlpha(Animation& pAnim, const char* name, int keyFrameNum, float totalTime, bool bLoop, const float alpha[] );
void CreateAnimationTranslate(Animation& pAnim, const char* name, int keyFrameNum, float totalTime, bool bLoop, const float scale[][2] );
void CreateAnimationSpriteCell(Animation& pAnim, const char* name, int keyFrameNum, float totalTime, bool bLoop, const SpriteCell cells[]);
void CreateAnimationAdditionalColor(Animation& pAnim, const char* name, int keyFrameNum, float totalTime, bool bLoop, const float color[][3]);

void CreateAnimationUV(Animation& pAnim, const char* name, int keyFrameNum, float keyTimes[], bool bLoop, const float uvs[][4]);
void CreateAnimationUV(Animation& pAnim, const char* name, int keyFrameNum, float keyTimes[], bool bLoop, const UVRect uvs[]);
void CreateAnimationScale(Animation& pAnim, const char* name, int keyFrameNum, float keyTimes[], bool bLoop, const float scale[][2]);
void CreateAnimationRotation(Animation& pAnim, const char* name, int keyFrameNum, float keyTimes[], bool bLoop, const float rotDeg[]);
void CreateAnimationColor(Animation& pAnim, const char* name, int keyFrameNum, float keyTimes[], bool bLoop, const float color[][3]);
void CreateAnimationAlpha(Animation& pAnim, const char* name, int keyFrameNum, float keyTimes[], bool bLoop, const float alpha[] );
void CreateAnimationTranslate(Animation& pAnim, const char* name, int keyFrameNum, float keyTimes[], bool bLoop, const float scale[][2] );
void CreateAnimationSpriteCell(Animation& pAnim, const char* name, int keyFrameNum, float keyTimes[], bool bLoop, const SpriteCell cells[]);
void CreateAnimationAdditionalColor(Animation& pAnim, const char* name, int keyFrameNum, float keyTimes[], bool bLoop, const float color[][3]);

//既存のアニメーションに別プロパティのアニメーションを追加する
void AddAnimationUV(Animation& pAnim, int keyFrameNum, float totalTime, const float uvs[][4]);
void AddAnimationUV(Animation& pAnim, int keyFrameNum, float totalTime, const UVRect uvs[]);
void AddAnimationScale(Animation& pAnim, int keyFrameNum, float totalTime, const float scale[][2]);
void AddAnimationRotation(Animation& pAnim, int keyFrameNum, float totalTime, const float rotDeg[]);
void AddAnimationColor(Animation& pAnim, int keyFrameNum, float totalTime, const float color[][3]);
void AddAnimationAlpha(Animation& pAnim, int keyFrameNum, float totalTime, const float alpha[]);
void AddAnimationTranslate(Animation& pAnim, int keyFrameNum, float totalTime, const float translate[][2]);
void AddAnimationPivot(Animation& pAnim, int keyFrameNum, float totalTime, const float pivots[][2]);
void AddAnimationSpriteCell(Animation& pAnim, int keyFrameNum, float totalTime, const SpriteCell cells[]);
void AddAnimationAdditionalColor(Animation& pAnim, int keyFrameNum, float totalTime, const float color[][3]);

void AddAnimationUV(Animation& pAnim, int keyFrameNum, float keyTimes[], const float uvs[][4]);
void AddAnimationUV(Animation& pAnim, int keyFrameNum, float keyTimes[], const UVRect uvs[]);
void AddAnimationScale(Animation& pAnim, int keyFrameNum, float keyTimes[], const float scale[][2]);
void AddAnimationRotation(Animation& pAnim, int keyFrameNum, float keyTimes[], const float rotDeg[]);
void AddAnimationColor(Animation& pAnim, int keyFrameNum, float keyTimes[], const float color[][3]);
void AddAnimationAlpha(Animation& pAnim, int keyFrameNum, float keyTimes[], const float alpha[]);
void AddAnimationTranslate(Animation& pAnim, int keyFrameNum, float keyTimes[], const float translate[][2]);
void AddAnimationPivot(Animation& pAnim, int keyFrameNum, float keyTimes[], const float pivots[][2]);
void AddAnimationSpriteCell(Animation& pAnim, int keyFrameNum, float keyTimes[], const SpriteCell cells[]);
void AddAnimationAdditionalColor(Animation& pAnim, int keyFrameNum, float keyTimes[], const float color[][3]);