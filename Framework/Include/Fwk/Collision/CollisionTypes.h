#pragma once

namespace Fwk {
namespace Collision {

class Collider;

//活性の期間
enum ActiveDuration {
	Continuous,		//一度活性化したら非活性にするまで活性状態を続ける
	Once,			//一度活性化した後、何かに衝突したら非活性に戻る

	Default = Continuous,
};

//衝突イベントの種類
enum class CollisionEventType {
	Enter,
	Stay,
	Exit,
};

//衝突イベントの情報
struct CollisionEvent {
	Collider* ColliderA;
	Collider* ColliderB;
	CollisionEventType EventType;
};

}
}
