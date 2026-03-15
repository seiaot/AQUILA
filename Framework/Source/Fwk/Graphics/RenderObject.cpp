#include "Fwk/Graphics/RenderObject.h"

namespace Fwk{

RenderObject::RenderObject()
	: mPriority(0)
	, mRenderLayer("default")
	, mIsVisible(true)
{

}

RenderObject::~RenderObject() {
	;
}

int RenderObject::GetPriority()const{
	return mPriority;
}

void RenderObject::SetPriority(int priority){
	mPriority = priority;
}

bool RenderObject::IsVisible()const{
	return mIsVisible;
}

void RenderObject::SetVisible(bool isVisible){
	mIsVisible = isVisible;
}

void RenderObject::SetRenderLayer(const string& layerName){
	mRenderLayer = layerName;
}
const string& RenderObject::GetRenderLayer()const{
	return mRenderLayer;
}

}
