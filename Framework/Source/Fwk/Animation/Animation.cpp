#include "Fwk/Animation/Animation.h"

#include <algorithm>

//bool cmpareKeyFrame(const KeyFrame& p0, const KeyFrame& p1) {
//	return p0.TimeSec < p1.TimeSec;
//}

Animation::Animation()
	: m_Name()
	, m_CurrentAnimParam({
		{0.0f,0.0f,1.0f,1.0f}
		,0.0f
		,{1.0f,1.0f}
		,{1.0f,1.0f,1.0f}
		,1.0f
	})
	, m_AnimProperties()
	, m_CurrentKeyFrameIndex()
	, m_EventCallback(nullptr)
	, m_CurrentTime(0.0f)
	, m_EndTime(0.0f)
	, m_AnimSpeed(1.0f)
	, m_IsLoop(false)
	, m_IsExec(false)
	, m_IsPause(false)
{
	;
}

Animation::~Animation()
{
	Term();
}

void Animation::Init()
{
	m_Name.clear();
	m_AnimProperties.clear();
	m_CurrentKeyFrameIndex.clear();
	m_CurrentTime = (0.0f);
	m_EndTime = (0.0f);
	m_AnimSpeed = (1.0f);
	m_IsLoop = (false);
	m_IsExec = (false);
	m_IsPause = (false);
	m_EventCallback = nullptr;

	_initAnimationParam(&m_CurrentAnimParam);
}

void Animation::Term()
{
	Clear();
}

void Animation::Update(float deltaTime) 
{
	if (m_IsPause)
		return;
	if (!m_IsExec)
		return;
	if (m_AnimProperties.size() < 1)
		return;

	m_CurrentTime += deltaTime * m_AnimSpeed;

	//終端時間まで進んだとき
	if (m_CurrentTime >= m_EndTime) 
	{
		//ループしない場合は再生終了
		if (!m_IsLoop || m_EndTime == 0.0f) {
			m_CurrentTime = m_EndTime;
			m_IsExec = false;
		}
		else {
			while (m_CurrentTime >= m_EndTime) {
				m_CurrentTime -= m_EndTime;
			}
			_sendEvent(AnimationEventType::Looped, m_CurrentTime);
		}
	}

	_updateAnim(m_CurrentTime);

	if (!m_IsExec) {
		_sendEvent(AnimationEventType::Completed, m_CurrentTime);
	}
}

void Animation::Start() {
	m_IsExec = true;
	m_IsPause = false;
	m_CurrentTime = 0.0f;

	//アニメーションパラメータを初期化する
	_initAnimationParam(&m_CurrentAnimParam);

	_sendEvent(AnimationEventType::Begin , m_CurrentTime);

	_updateAnim(m_CurrentTime);
}

void Animation::Pause(bool bPause) {

	if (m_IsPause != bPause) {
		if (bPause) {
			_sendEvent(AnimationEventType::Paused, m_CurrentTime);
		}
		else {
			_sendEvent(AnimationEventType::Unpaused, m_CurrentTime);
		}
	}
	m_IsPause = bPause;
}

void Animation::Stop() {
	if (m_IsExec) {
		_sendEvent(AnimationEventType::Stopped, m_CurrentTime);
	}
	m_IsExec = false;
	m_IsPause = false;
	m_CurrentTime = 0.0f;
	_initAnimationParam(&m_CurrentAnimParam);
}

void Animation::SetName(const char* name) {
	if (name == nullptr)
		return;
	m_Name = name;
}

void Animation::SetName(const std::string name) {
	m_Name = name;
}


void Animation::Clear() {
	m_AnimProperties.clear();
}

void Animation::SetLoop(bool bLoop) {
	m_IsLoop = bLoop;
}
bool Animation::IsLoop()const {
	return m_IsLoop;
}

void Animation::SetAnimSpeed(float AnimSpeed) {
	m_AnimSpeed = AnimSpeed;
}
float Animation::GetAnimSpeed()const {
	return m_AnimSpeed;
}

const AnimationParam& Animation::GetAnimParam()const {
	return m_CurrentAnimParam;
}

const std::string& Animation::GetName()const {
	return m_Name;
}

bool Animation::IsPlaying()const {
	return m_IsExec;
}

bool Animation::IsPausing()const {
	return m_IsPause;
}

void Animation::AddProperty(const AnimationProperty& animProperty) {

	if (animProperty.KeyFrames.size() == 0)
		return;

	m_AnimProperties.push_back(animProperty);
	m_CurrentKeyFrameIndex.push_back(0);

	float endTime = animProperty.KeyFrames[animProperty.KeyFrames.size() - 1].TimeSec;

	if (m_EndTime < endTime) {
		m_EndTime = endTime;
	}
}

void Animation::SetEventCallback(std::function<void(const AnimationEvent&)> func) {
	m_EventCallback = func;
}

void Animation::ClearEventCallback() {
	m_EventCallback = nullptr;
}

bool Animation::HasProperty(AnimationPropertyType prop)const {
	int propNum = (int)m_AnimProperties.size();

	for (int i = 0; i < propNum; ++i)
	{
		if (m_AnimProperties[i].Type == prop) {
			return true;
		}
	}

	return false;
}

void Animation::_updateAnim(float timeSec) {

	int propNum = (int)m_AnimProperties.size();

	for (int i = 0; i < propNum; ++i)
	{
		const auto propertyType = m_AnimProperties[i].Type;
		const auto& keyFrames = m_AnimProperties[i].KeyFrames;
		size_t currIndex = _getKeyFrameIndex(keyFrames,timeSec);

		if (currIndex < 0)
			return;

		size_t nextIndex = currIndex + 1;
		if (nextIndex >= keyFrames.size())
			nextIndex = keyFrames.size() - 1;

		const auto& currParam = keyFrames[currIndex].Values;
		const auto& nextParam = keyFrames[nextIndex].Values;

		float r = 0.0f;

		if (currIndex != nextIndex)
		{
			float timeFromCurrentKeyFrame = timeSec - keyFrames[currIndex].TimeSec;
			float timeToNextKeyFrame = keyFrames[nextIndex].TimeSec - keyFrames[currIndex].TimeSec;
			r = timeFromCurrentKeyFrame / timeToNextKeyFrame;
		}

		if (propertyType == AnimationPropertyType::UV)
		{
			m_CurrentAnimParam.uv[0] = currParam[0];
			m_CurrentAnimParam.uv[1] = currParam[1];
			m_CurrentAnimParam.uv[2] = currParam[2];
			m_CurrentAnimParam.uv[3] = currParam[3];
		}

		if (propertyType == AnimationPropertyType::Rotation)
		{
			m_CurrentAnimParam.rotDeg = currParam[0] + (nextParam[0] - currParam[0]) * r;
		}

		if (propertyType == AnimationPropertyType::Scale)
		{
			m_CurrentAnimParam.scale[0] = currParam[0] + (nextParam[0] - currParam[0]) * r;
			m_CurrentAnimParam.scale[1] = currParam[1] + (nextParam[1] - currParam[1]) * r;
		}

		if (propertyType == AnimationPropertyType::Color)
		{
			m_CurrentAnimParam.color[0] = currParam[0] + (nextParam[0] - currParam[0]) * r;
			m_CurrentAnimParam.color[1] = currParam[1] + (nextParam[1] - currParam[1]) * r;
			m_CurrentAnimParam.color[2] = currParam[2] + (nextParam[2] - currParam[2]) * r;
		}
		if (propertyType == AnimationPropertyType::Alpha)
		{
			m_CurrentAnimParam.alpha = currParam[0] + (nextParam[0] - currParam[0]) * r;
		}
		if (propertyType == AnimationPropertyType::Translate)
		{
			m_CurrentAnimParam.translate[0] = currParam[0] + (nextParam[0] - currParam[0]) * r;
			m_CurrentAnimParam.translate[1] = currParam[1] + (nextParam[1] - currParam[1]) * r;
		}

		if (m_CurrentKeyFrameIndex[i] != (int)currIndex) {
			_sendEvent(AnimationEventType::KeyframeProgressed, keyFrames[currIndex].TimeSec, i, (int)currIndex);
			m_CurrentKeyFrameIndex[i] = (int)currIndex;
		}
			
		if (propertyType == AnimationPropertyType::Pivot)
		{
			m_CurrentAnimParam.pivot[0] = currParam[0];
			m_CurrentAnimParam.pivot[1] = currParam[1];
		}
		if (propertyType == AnimationPropertyType::AddColor)
		{
			m_CurrentAnimParam.addColor[0] = currParam[0] + (nextParam[0] - currParam[0]) * r;
			m_CurrentAnimParam.addColor[1] = currParam[1] + (nextParam[1] - currParam[1]) * r;
			m_CurrentAnimParam.addColor[2] = currParam[2] + (nextParam[2] - currParam[2]) * r;
		}
	}
}

size_t Animation::_getKeyFrameIndex(const std::vector<KeyFrame>& keyFrames, float timeSec) {
	
	if (keyFrames.size() == 0)
		return -1;

	for (size_t i = 0; i < keyFrames.size() - 1; ++i) {
		if (timeSec >= keyFrames[i].TimeSec
		 && timeSec < keyFrames[i + 1].TimeSec)
			return i;
	}

	return  keyFrames.size() - 1;
}

void Animation::_initAnimationParam(AnimationParam* pParam) {
	if (pParam == nullptr)
		return;

	pParam->rotDeg = 0.0f;
	pParam->scale[0] = 1.0f;
	pParam->scale[1] = 1.0f;
	pParam->uv[0] = 0.0f;
	pParam->uv[1] = 0.0f;
	pParam->uv[2] = 1.0f;
	pParam->uv[3] = 1.0f;
	pParam->color[0] = 1.0f;
	pParam->color[1] = 1.0f;
	pParam->color[2] = 1.0f;
	pParam->alpha = 1.0f;
	pParam->translate[0] = 0.0f;
	pParam->translate[1] = 0.0f;
	pParam->pivot[0] = 0.5f;
	pParam->pivot[1] = 0.5f;
	pParam->addColor[0] = 0.0f;
	pParam->addColor[1] = 0.0f;
	pParam->addColor[2] = 0.0f;

	for (int i = 0; i < m_CurrentKeyFrameIndex.size(); ++i) {
		m_CurrentKeyFrameIndex[i] = -1;
	}
}

void Animation::_sendEvent(AnimationEventType EventType,  float Time,int PropertyIndex, int KeyFrameIndex) {
	if (m_EventCallback == nullptr) {
		return;
	}

	AnimationEvent sendParam = {
		EventType,
		m_Name,
		Time,
		PropertyIndex,
		KeyFrameIndex,
	};
	m_EventCallback(sendParam);
}

float GetKeyFrameTime(int i, int KeyFrameNum, float totalTime)
{
	int intervalNum = KeyFrameNum - 1;
	return (intervalNum > 0) ? (totalTime * i / (float)intervalNum) : 0.0f;
}

void SetAnimParamUV(KeyFrame* pKf, int i, const  float uvs[][4])
{
	pKf->Values[0] = uvs[i][0];
	pKf->Values[1] = uvs[i][1];
	pKf->Values[2] = uvs[i][2];
	pKf->Values[3] = uvs[i][3];
}

void SetAnimParamUV(KeyFrame* pKf, int i, const UVRect uvs[])
{
	pKf->Values[0] = uvs[i].m[0];
	pKf->Values[1] = uvs[i].m[1];
	pKf->Values[2] = uvs[i].m[2];
	pKf->Values[3] = uvs[i].m[3];
}

void SetAnimParamScale(KeyFrame* pKf, int i, const  float scale[][2])
{
	pKf->Values[0] = scale[i][0];
	pKf->Values[1] = scale[i][1];
}

void SetAnimParamRotation(KeyFrame* pKf, int i, const  float rotDeg[])
{
	pKf->Values[0] = rotDeg[i];
}

void SetAnimParamColor(KeyFrame* pKf, int i, const  float color[][3])
{
	pKf->Values[0] = color[i][0];
	pKf->Values[1] = color[i][1];
	pKf->Values[2] = color[i][2];
}

void SetAnimParamAlpha(KeyFrame* pKf, int i, const  float alpha[])
{
	pKf->Values[0] = alpha[i];
}
void SetAnimParamTranslate(KeyFrame* pKf, int i, const  float translate[][2])
{
	pKf->Values[0] = translate[i][0];
	pKf->Values[1] = translate[i][1];
}

void SetAnimParamPivot(KeyFrame* pKf, int i, const  float pivot[][2])
{
	pKf->Values[0] = pivot[i][0];
	pKf->Values[1] = pivot[i][1];
}

void SetAnimParamAdditionalColor(KeyFrame* pKf, int i, const  float color[][3])
{
	pKf->Values[0] = color[i][0];
	pKf->Values[1] = color[i][1];
	pKf->Values[2] = color[i][2];
}

void CreateAnimationUV(
	Animation& pAnim
	, const char* name
	, int keyFrameNum
	, float totalTime
	, bool bLoop
	, const float uvs[][4]
){
	if (name == nullptr || keyFrameNum <1 )
		return;

	pAnim.Clear();
	pAnim.SetName(name);
	pAnim.SetLoop(bLoop);

	AddAnimationUV(pAnim,keyFrameNum,totalTime,uvs);
}

void CreateAnimationUV(
	Animation& pAnim
	, const char* name
	, int keyFrameNum
	, float totalTime
	, bool bLoop
	, const UVRect uvs[]
) {
	if (name == nullptr || keyFrameNum < 1)
		return;

	pAnim.Clear();
	pAnim.SetName(name);
	pAnim.SetLoop(bLoop);

	AddAnimationUV(pAnim, keyFrameNum, totalTime, uvs);
}

void CreateAnimationScale(
	Animation& pAnim
	, const char* name
	, int keyFrameNum
	, float totalTime
	, bool bLoop
	, const  float scale[][2]
){
	if (name == nullptr || keyFrameNum < 1)
		return;

	pAnim.Clear();
	pAnim.SetName(name);
	pAnim.SetLoop(bLoop);

	AddAnimationScale(pAnim, keyFrameNum, totalTime, scale);
}

void CreateAnimationRotation(
	Animation& pAnim
	,const char* name
	,int keyFrameNum
	,float totalTime
	,bool bLoop
	,const  float rotDeg[]
){
	if (name == nullptr)
		return;

	pAnim.Clear();
	pAnim.SetName(name);
	pAnim.SetLoop(bLoop);

	AddAnimationRotation(pAnim, keyFrameNum, totalTime, rotDeg);
}

void CreateAnimationColor(
	Animation& pAnim
	, const char* name
	, int keyFrameNum
	, float totalTime
	, bool bLoop
	, const float color[][3]
){
	if (name == nullptr)
		return;

	pAnim.Clear();
	pAnim.SetName(name);
	pAnim.SetLoop(bLoop);

	AddAnimationColor(pAnim, keyFrameNum, totalTime, color);
}

void CreateAnimationAlpha(
	Animation& pAnim
	, const char* name
	, int keyFrameNum
	, float totalTime
	, bool bLoop
	, const float alpha[]
){
	if (name == nullptr)
		return;

	pAnim.Clear();
	pAnim.SetName(name);
	pAnim.SetLoop(bLoop);

	AddAnimationAlpha(pAnim, keyFrameNum, totalTime, alpha);
}

void CreateAnimationTranslate(
	Animation& pAnim
	, const char* name
	, int keyFrameNum
	, float totalTime
	, bool bLoop
	, const  float translate[][2]
){
	if (name == nullptr || keyFrameNum < 1)
		return;

	pAnim.Clear();
	pAnim.SetName(name);
	pAnim.SetLoop(bLoop);

	AddAnimationTranslate(pAnim, keyFrameNum, totalTime, translate);
}

void CreateAnimationSpriteCell(
	Animation& pAnim, 
	const char* name, 
	int keyFrameNum, 
	float totalTime, 
	bool bLoop, 
	const SpriteCell cells[]
){
	if (name == nullptr || keyFrameNum < 1)
		return;

	pAnim.Clear();
	pAnim.SetName(name);
	pAnim.SetLoop(bLoop);

	AddAnimationSpriteCell(pAnim,keyFrameNum,totalTime,cells);
}

void CreateAnimationAdditionalColor(
	Animation& pAnim
	, const char* name
	, int keyFrameNum
	, float totalTime
	, bool bLoop
	, const float color[][3]
) {
	if (name == nullptr)
		return;

	pAnim.Clear();
	pAnim.SetName(name);
	pAnim.SetLoop(bLoop);

	AddAnimationAdditionalColor(pAnim, keyFrameNum, totalTime, color);
}

void CreateAnimationUV(
	Animation& pAnim,
	const char* name,
	int keyFrameNum, 
	float keyTimes[], 
	bool bLoop, 
	const float uvs[][4]
){
	if (name == nullptr || keyFrameNum <1 )
		return;

	pAnim.Clear();
	pAnim.SetName(name);
	pAnim.SetLoop(bLoop);

	AddAnimationUV(pAnim,keyFrameNum,keyTimes,uvs);

}

void CreateAnimationUV(
	Animation& pAnim, 
	const char* name, 
	int keyFrameNum, 
	float keyTimes[], 
	bool bLoop, 
	const UVRect uvs[]
){
	if (name == nullptr || keyFrameNum < 1)
		return;

	pAnim.Clear();
	pAnim.SetName(name);
	pAnim.SetLoop(bLoop);

	AddAnimationUV(pAnim, keyFrameNum, keyTimes, uvs);
}

void CreateAnimationScale(
	Animation& pAnim, 
	const char* name, 
	int keyFrameNum, 
	float keyTimes[], 
	bool bLoop, 
	const float scale[][2]
){
	if (name == nullptr || keyFrameNum < 1)
		return;

	pAnim.Clear();
	pAnim.SetName(name);
	pAnim.SetLoop(bLoop);

	AddAnimationScale(pAnim, keyFrameNum, keyTimes, scale);
}

void CreateAnimationRotation(
	Animation& pAnim, 
	const char* name, 
	int keyFrameNum, 
	float keyTimes[], 
	bool bLoop, 
	const float rotDeg[]
){
	if (name == nullptr || keyFrameNum < 1)
		return;

	pAnim.Clear();
	pAnim.SetName(name);
	pAnim.SetLoop(bLoop);

	AddAnimationRotation(pAnim, keyFrameNum, keyTimes, rotDeg);
}

void CreateAnimationColor(
	Animation& pAnim, 
	const char* name, 
	int keyFrameNum, 
	float keyTimes[], 
	bool bLoop, 
	const float color[][3]
){
	if (name == nullptr || keyFrameNum < 1)
		return;

	pAnim.Clear();
	pAnim.SetName(name);
	pAnim.SetLoop(bLoop);

	AddAnimationColor(pAnim, keyFrameNum, keyTimes, color);
}

void CreateAnimationAlpha(
	Animation& pAnim, 
	const char* name, 
	int keyFrameNum, 
	float keyTimes[], 
	bool bLoop, 
	const float alpha[]
){
	if (name == nullptr || keyFrameNum < 1)
		return;

	pAnim.Clear();
	pAnim.SetName(name);
	pAnim.SetLoop(bLoop);

	AddAnimationAlpha(pAnim, keyFrameNum, keyTimes, alpha);
}

void CreateAnimationTranslate(
	Animation& pAnim, 
	const char* name, 
	int keyFrameNum, 
	float keyTimes[], 
	bool bLoop, 
	const float translate[][2]
){
	if (name == nullptr || keyFrameNum < 1)
		return;

	pAnim.Clear();
	pAnim.SetName(name);
	pAnim.SetLoop(bLoop);

	AddAnimationTranslate(pAnim, keyFrameNum, keyTimes, translate);

}

void CreateAnimationSpriteCell(
	Animation& pAnim, 
	const char* name, 
	int keyFrameNum, 
	float keyTimes[], 
	bool bLoop, 
	const SpriteCell cells[]
){
	if (name == nullptr || keyFrameNum < 1)
		return;

	pAnim.Clear();
	pAnim.SetName(name);
	pAnim.SetLoop(bLoop);

	AddAnimationSpriteCell(pAnim, keyFrameNum, keyTimes, cells);
}

void CreateAnimationAdditionalColor(
	Animation& pAnim, 
	const char* name, 
	int keyFrameNum, 
	float keyTimes[], 
	bool bLoop, 
	const float color[][3]
){
	if (name == nullptr || keyFrameNum < 1)
		return;

	pAnim.Clear();
	pAnim.SetName(name);
	pAnim.SetLoop(bLoop);

	AddAnimationAdditionalColor(pAnim, keyFrameNum, keyTimes, color);
}


void AddAnimationUV(Animation& pAnim, int keyFrameNum, float totalTime, const float uvs[][4])
{
	float* pKeyTimes = new float[keyFrameNum];

	for (int i = 0; i < keyFrameNum; ++i){
		pKeyTimes[i] = GetKeyFrameTime(i, keyFrameNum, totalTime);
	}
	AddAnimationUV(pAnim, keyFrameNum, pKeyTimes, uvs);

	delete[] pKeyTimes;
}

void AddAnimationUV(Animation& pAnim, int keyFrameNum, float totalTime, const UVRect uvs[])
{
	float* pKeyTimes = new float[keyFrameNum];

	for (int i = 0; i < keyFrameNum; ++i){
		pKeyTimes[i] = GetKeyFrameTime(i, keyFrameNum, totalTime);
	}
	AddAnimationUV(pAnim, keyFrameNum, pKeyTimes, uvs);

	delete[] pKeyTimes;
}

void AddAnimationScale(Animation& pAnim, int keyFrameNum, float totalTime, const float scale[][2])
{
	float* pKeyTimes = new float[keyFrameNum];

	for (int i = 0; i < keyFrameNum; ++i){
		pKeyTimes[i] = GetKeyFrameTime(i, keyFrameNum, totalTime);
	}
	AddAnimationScale(pAnim, keyFrameNum, pKeyTimes, scale);

	delete[] pKeyTimes;
}
void AddAnimationRotation(Animation& pAnim, int keyFrameNum, float totalTime, const float rotDeg[])
{
	float* pKeyTimes = new float[keyFrameNum];

	for (int i = 0; i < keyFrameNum; ++i){
		pKeyTimes[i] = GetKeyFrameTime(i, keyFrameNum, totalTime);
	}
	AddAnimationRotation(pAnim, keyFrameNum, pKeyTimes, rotDeg);

	delete[] pKeyTimes;
}
void AddAnimationColor(Animation& pAnim, int keyFrameNum, float totalTime, const float color[][3])
{
	float* pKeyTimes = new float[keyFrameNum];

	for (int i = 0; i < keyFrameNum; ++i){
		pKeyTimes[i] = GetKeyFrameTime(i, keyFrameNum, totalTime);
	}
	AddAnimationColor(pAnim, keyFrameNum, pKeyTimes, color);

	delete[] pKeyTimes;
}
void AddAnimationAlpha(Animation& pAnim, int keyFrameNum, float totalTime, const float alpha[])
{
	float* pKeyTimes = new float[keyFrameNum];

	for (int i = 0; i < keyFrameNum; ++i){
		pKeyTimes[i] = GetKeyFrameTime(i, keyFrameNum, totalTime);
	}
	AddAnimationAlpha(pAnim, keyFrameNum, pKeyTimes, alpha);

	delete[] pKeyTimes;
}
void AddAnimationTranslate(Animation& pAnim, int keyFrameNum, float totalTime, const float translate[][2])
{
	float* pKeyTimes = new float[keyFrameNum];

	for (int i = 0; i < keyFrameNum; ++i){
		pKeyTimes[i] = GetKeyFrameTime(i, keyFrameNum, totalTime);
	}
	AddAnimationTranslate(pAnim, keyFrameNum, pKeyTimes, translate);

	delete[] pKeyTimes;
}

void AddAnimationPivot(Animation& pAnim, int keyFrameNum, float totalTime, const float pivots[][2])
{
	float* pKeyTimes = new float[keyFrameNum];

	for (int i = 0; i < keyFrameNum; ++i){
		pKeyTimes[i] = GetKeyFrameTime(i, keyFrameNum, totalTime);
	}
	AddAnimationPivot(pAnim, keyFrameNum, pKeyTimes, pivots);

	delete[] pKeyTimes;
}
void AddAnimationSpriteCell(Animation& pAnim, int keyFrameNum, float totalTime, const SpriteCell cells[])
{
	float* pKeyTimes = new float[keyFrameNum];

	for (int i = 0; i < keyFrameNum; ++i){
		pKeyTimes[i] = GetKeyFrameTime(i, keyFrameNum, totalTime);
	}
	AddAnimationSpriteCell(pAnim, keyFrameNum, pKeyTimes, cells);

	delete[] pKeyTimes;
}
void AddAnimationAdditionalColor(Animation& pAnim, int keyFrameNum, float totalTime, const float color[][3])
{
	float* pKeyTimes = new float[keyFrameNum];

	for (int i = 0; i < keyFrameNum; ++i){
		pKeyTimes[i] = GetKeyFrameTime(i, keyFrameNum, totalTime);
	}
	AddAnimationAdditionalColor(pAnim, keyFrameNum, pKeyTimes, color);

	delete[] pKeyTimes;
}

void AddAnimationUV(Animation& pAnim, int keyFrameNum, float keyTimes[], const float uvs[][4])
{
	AnimationProperty prop;

	prop.Type = AnimationPropertyType::UV;
	prop.KeyFrames.clear();

	for (int i = 0; i < keyFrameNum; ++i)
	{
		KeyFrame kf = {};
		kf.TimeSec = keyTimes[i];
		SetAnimParamUV(&kf, i, uvs);
		prop.KeyFrames.push_back(kf);
	}

	pAnim.AddProperty(prop);
}

void AddAnimationUV(Animation& pAnim, int keyFrameNum, float keyTimes[], const UVRect uvs[])
{
	AnimationProperty prop;

	prop.Type = AnimationPropertyType::UV;
	prop.KeyFrames.clear();

	for (int i = 0; i < keyFrameNum; ++i)
	{
		KeyFrame kf = {};
		kf.TimeSec = keyTimes[i];
		SetAnimParamUV(&kf, i, uvs);
		prop.KeyFrames.push_back(kf);
	}

	pAnim.AddProperty(prop);
}

void AddAnimationScale(Animation& pAnim, int keyFrameNum, float keyTimes[], const float scale[][2])
{
	AnimationProperty prop;

	prop.Type = AnimationPropertyType::Scale;
	prop.KeyFrames.clear();

	for (int i = 0; i < keyFrameNum; ++i)
	{
		KeyFrame kf = {};
		kf.TimeSec = keyTimes[i];
		SetAnimParamScale(&kf, i, scale);
		prop.KeyFrames.push_back(kf);
	}
	pAnim.AddProperty(prop);
}

void AddAnimationRotation(Animation& pAnim, int keyFrameNum, float keyTimes[], const float rotDeg[])
{
	AnimationProperty prop;

	prop.Type = AnimationPropertyType::Rotation;
	prop.KeyFrames.clear();

	for (int i = 0; i < keyFrameNum; ++i)
	{
		KeyFrame kf = {};
		kf.TimeSec = keyTimes[i];
		SetAnimParamRotation(&kf, i, rotDeg);
		prop.KeyFrames.push_back(kf);
	}
	pAnim.AddProperty(prop);
}

void AddAnimationColor(Animation& pAnim, int keyFrameNum, float keyTimes[], const float color[][3])
{
	AnimationProperty prop;

	prop.Type = AnimationPropertyType::Color;
	prop.KeyFrames.clear();

	for (int i = 0; i < keyFrameNum; ++i)
	{
		KeyFrame kf = {};
		kf.TimeSec = keyTimes[i];
		SetAnimParamColor(&kf, i, color);
		prop.KeyFrames.push_back(kf);
	}

	pAnim.AddProperty(prop);
}

void AddAnimationAlpha(Animation& pAnim, int keyFrameNum, float keyTimes[], const float alpha[])
{
	AnimationProperty prop;

	prop.Type = AnimationPropertyType::Alpha;
	prop.KeyFrames.clear();

	for (int i = 0; i < keyFrameNum; ++i)
	{
		KeyFrame kf = {};
		kf.TimeSec = keyTimes[i];
		SetAnimParamAlpha(&kf, i, alpha);
		prop.KeyFrames.push_back(kf);
	}

	pAnim.AddProperty(prop);
}

void AddAnimationTranslate(Animation& pAnim, int keyFrameNum, float keyTimes[], const float translate[][2])
{
	AnimationProperty prop;

	prop.Type = AnimationPropertyType::Translate;
	prop.KeyFrames.clear();

	for (int i = 0; i < keyFrameNum; ++i)
	{
		KeyFrame kf = {};
		kf.TimeSec = keyTimes[i];
		SetAnimParamTranslate(&kf, i, translate);
		prop.KeyFrames.push_back(kf);
	}
	pAnim.AddProperty(prop);
}

void AddAnimationPivot(Animation& pAnim, int keyFrameNum, float keyTimes[], const float pivots[][2])
{
	AnimationProperty prop;

	prop.Type = AnimationPropertyType::Pivot;
	prop.KeyFrames.clear();

	for (int i = 0; i < keyFrameNum; ++i)
	{
		KeyFrame kf = {};
		kf.TimeSec = keyTimes[i];
		SetAnimParamPivot(&kf, i, pivots);
		prop.KeyFrames.push_back(kf);
	}
	pAnim.AddProperty(prop);
}

void AddAnimationSpriteCell(Animation& pAnim, int keyFrameNum, float keyTimes[], const SpriteCell cells[])
{
	AnimationProperty prop_uv;
	prop_uv.Type = AnimationPropertyType::UV;
	prop_uv.KeyFrames.clear();

	AnimationProperty prop_pivot;
	prop_pivot.Type = AnimationPropertyType::Pivot;
	prop_pivot.KeyFrames.clear();

	for (int i = 0; i < keyFrameNum; ++i)
	{
		{
			KeyFrame kf = {};
			kf.TimeSec = keyTimes[i];
			kf.Values[0] = cells[i].texcoord[0];
			kf.Values[1] = cells[i].texcoord[1];
			kf.Values[2] = cells[i].texcoord[2];
			kf.Values[3] = cells[i].texcoord[3];
			prop_uv.KeyFrames.push_back(kf);
		}
		{
			KeyFrame kf = {};
			kf.TimeSec = keyTimes[i];
			kf.Values[0] = cells[i].pivot[0];
			kf.Values[1] = cells[i].pivot[1];
			prop_pivot.KeyFrames.push_back(kf);
		}
	}
	pAnim.AddProperty(prop_uv);
	pAnim.AddProperty(prop_pivot);
}

void AddAnimationAdditionalColor(Animation& pAnim, int keyFrameNum, float keyTimes[], const float color[][3])
{
	AnimationProperty prop;

	prop.Type = AnimationPropertyType::AddColor;
	prop.KeyFrames.clear();

	for (int i = 0; i < keyFrameNum; ++i)
	{
		KeyFrame kf = {};
		kf.TimeSec = keyTimes[i];
		SetAnimParamColor(&kf, i, color);
		prop.KeyFrames.push_back(kf);
	}

	pAnim.AddProperty(prop);
}