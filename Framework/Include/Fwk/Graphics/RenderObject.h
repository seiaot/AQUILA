#pragma once

#include <string>
using namespace std;

#include "Lib/Graphics/Graphics.h"
#include "RenderDef.h"
#include "IDrawable.h"

using namespace Lib::Graphics;

namespace Fwk
{

class RenderObject : public IDrawable
{
public:

	RenderObject();
	virtual ~RenderObject();

	virtual int GetPriority()const;
	virtual void SetPriority(int priority);
	
	virtual RENDER_PASS GetRenderPass()const=0;
	virtual RENDER_MODE GetRenderMode()const=0;
	
	virtual bool IsVisible()const;
	virtual void SetVisible(bool isVisible);

	void SetRenderLayer(const string& layerName);
	const string& GetRenderLayer()const;

private:

	int mPriority;
	string mRenderLayer;
	bool mIsVisible;
};

}