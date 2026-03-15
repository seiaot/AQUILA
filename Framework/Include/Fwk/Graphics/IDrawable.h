#pragma once

#include "Lib/Graphics/Graphics.h"


using namespace Lib::Graphics;

class Camera;

namespace Fwk
{

class IDrawable
{
public:

	IDrawable() = default;
	virtual ~IDrawable() = default;

	virtual void OnDraw(IGraphics* g,Camera* pCamera) = 0;
};

}