#pragma once

#include <map>

#ifdef _DEBUG
#include "Fwk/Graphics/LineSegments.h"
#include <climits>
#endif

#include "Collider.h"
#include "CollisionTypes.h"

using namespace Lib::Math;
using namespace std;

namespace Fwk {
namespace Collision {

class CollisionManager
{
	struct Element {
		Collider* pCollider;
		bool bPendingRemove;
		Element* pPrev;
		Element* pNext;

		void InsertNext(Element* p);
		void InsertPrev(Element* p);
		void Remove();
	};


	struct OverlapInfo{
		CollisionEventType eventType;
		bool isCollide;
	};

public:

	CollisionManager();
	~CollisionManager();

	void Init(int BufferSize);

	void Term();

	void Register(Collider* pCollider);
	void Unregister(Collider* pCollider);

	void ProcCollisions();

#ifdef _DEBUG
	void DrawDebug();
#endif

private:

	Element* _findElement(Collider* pCollider);

	void _procPendingRemoveElements();

private:

	Element* mpBuffer;
	int mBufferSize;

	Element mUsingTop;
	Element mUsingTail;

	Element mFreeTop;
	Element mFreeTail;

	std::map<Element*, std::map<Element*,OverlapInfo>*> mOverlappedCollisions;

#ifdef _DEBUG
	LineSegments mDebugOutline;
#endif
};

}
}
