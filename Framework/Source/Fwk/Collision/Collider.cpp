#include "Fwk/Collision/Collider.h"

namespace Fwk {
	namespace Collision {

		Collider::Collider()
			: mShape({ ShapeType::None })
			, mPosition(0.0f, 0.0f, 0.0f)
			, mTag()
			, mIsActive(true)
			, mActiveDuration(ActiveDuration::Default)
			, mIsCollide(false)
			, mOnOverlapped(nullptr)
			, mOnOverlappedEx(nullptr)
			, mpOwner(nullptr)
			, mGroup(0)
			, mHitGroup(0)
#ifdef _DEBUG
			, mOutlineColor()
#endif
		{
#ifdef _DEBUG
			mOutlineColor[0] = 0.0f;
			mOutlineColor[1] = 1.0f;
			mOutlineColor[2] = 0.0f;
#endif
		}

		Collider::~Collider()
		{
			if (mOnDestroyed != nullptr) {
				mOnDestroyed(this);
			}
		}

		void Collider::SetPosition(float _x, float _y, float _z) {
			mPosition = { _x, _y ,_z };
		}
		void Collider::SetPosition(const Vector3f& _position) {
			mPosition = _position;
		}

		//中心位置の取得
		const Vector3f& Collider::GetPosition()const {
			return mPosition;
		}

		void Collider::SetSphere(const Sphere& _sphere) {
			mShape.sphere = _sphere;
			mShape.shapeType = ShapeType::Sphere;
		}
		void Collider::SetSphere(const Vector3f& _position, float _r) {
			mShape.sphere.position = _position;
			mShape.sphere.radius = _r;
			mShape.shapeType = ShapeType::Sphere;
		}
		void Collider::SetSphere(float _x, float _y, float _z, float _r) {
			mShape.sphere.position = { _x,_y ,_z };
			mShape.sphere.radius = _r;
			mShape.shapeType = ShapeType::Sphere;
		}

		void Collider::SetCube(const Cube& _cube) {
			mShape.cube = _cube;
			mShape.shapeType = ShapeType::Cube;
		}
		void Collider::SetCube(const Vector3f& _position, Sizef _size, Anglef _angle) {
			mShape.shapeType = ShapeType::Cube;
			mShape.cube.position = _position;
			for (int i = 0; i < 3; ++i)
			{
				*(&(mShape.cube.size.w) + i) = *(&(_size.w) + i);
				for (int n = 0; n < 3; ++n)
				{
					*(&(mShape.cube.vOriginDirection[i].x) + n) = (i == n) ? *(&(_size.w) + i) / 2.0f : 0.0f;
				}
			}

			SetCubeRotation(_angle);
		}
		//コリジョンの大きさの設定
		void Collider::SetCubeSize(const Sizef _size)
		{
			mShape.cube.size = _size;
			for (int i = 0; i < 3; ++i)
			{
				mShape.cube.vOriginDirection[i] = mShape.cube.vOriginDirection[i].GetUnitVector() * *(&(_size.w) + i) / 2.0f;
			}
		}

		//立方体の回転を設定
		void Collider::SetCubeRotation(Anglef rot, Vector3f axisX, Vector3f axisY, Vector3f axisZ)
		{
			//形が立方体の時のみ実行
			if (mShape.shapeType != ShapeType::Cube)
			{
				return;
			}

			//回転対象3つ
			for (int i = 0;i < 3;++i)
			{
				//スプライトの回転方向と合わせるため引数に-をつける
				mShape.cube.vDirection[i] = Vector3f::GetRotatedVector3Axis(mShape.cube.vOriginDirection[i], -rot.x, -rot.y, -rot.z, axisX, axisY, axisZ);
			}
		}

		//立方体の回転を追加
		void Collider::AddCubeRotation(float _rx, float _ry, float _rz)
		{
			//形が立方体の時のみ実行
			if (mShape.shapeType != ShapeType::Cube)
			{
				return;
			}

			//回転対象3つ
			for (int i = 0;i < 3;++i)
			{
				mShape.cube.vDirection[i] = Vector3f::GetRotatedVector3Axis(mShape.cube.vDirection[i], -_rx, -_ry, -_rz,mShape.cube.vDirection[0], mShape.cube.vDirection[1], mShape.cube.vDirection[2]);
			}
		}

		//立方体が回転しているか調べる
		bool Collider::IsCubeRotated(Shape _shape) const
		{
			//方向を示すベクトルが変化していなかったら
			if (_shape.cube.vDirection[0].GetUnitVector() == Vector3f(1.0f, 0.0f, 0.0f)
				&& _shape.cube.vDirection[1].GetUnitVector() == Vector3f(0.0f, 1.0f, 0.0f)
				&& _shape.cube.vDirection[2].GetUnitVector() == Vector3f(0.0f, 0.0f, 1.0f)
				)
			{
				return true;
			}

			return false;

		}

		//形状の取得
		ShapeType Collider::GetShapeType()const {
			return mShape.shapeType;
		}
		const Sphere& Collider::GetShapeSphere()const {
			return mShape.sphere;
		}
		const Cube& Collider::GetShapeCube()const {
			return mShape.cube;
		}

		//衝突チェック
		bool Collider::IsOverlapped(Collider& collider)const {

			Shape colA = mShape;
			Shape colB = collider.mShape;

			bool isHit = false;

			//無効設定のコリジョンは処理しない
			if (mIsActive == false || collider.mIsActive == false) {
				return false;
			}

			//コリジョン形状がないものは処理対象外
			if (colA.shapeType == ShapeType::None || colB.shapeType == ShapeType::None) {
				return false;
			}

			//形状によって衝突判定関数を変える
			//球同士
			if (colA.shapeType == ShapeType::Sphere && colB.shapeType == ShapeType::Sphere) {
				colA.sphere.position += mPosition;
				colB.sphere.position += collider.mPosition;
				//円と円の衝突
				isHit = IsSpheresOverlapped(colA.sphere, colB.sphere);
			}
			//立方体同士
			else if (colA.shapeType == ShapeType::Cube && colB.shapeType == ShapeType::Cube) {
				colA.cube.position += mPosition;
				colB.cube.position += collider.mPosition;
				//両方の立方体が回転していなければ
				if (!IsCubeRotated(colA) && !IsCubeRotated(colB))
				{
					//立方体と立方体の衝突
					isHit = IsCubesOverlapped(colA.cube, colB.cube);
				}
				else
				{
					//立方体と立方体(回転済み)の衝突判定
					isHit = IsOBBsOverlapped(colA.cube, colB.cube);
				}
			}
			//球と立方体
			else if (colA.shapeType == ShapeType::Sphere && colB.shapeType == ShapeType::Cube) {
				colA.sphere.position += mPosition;
				colB.cube.position += collider.mPosition;
				//Aが円,Bが矩形の衝突
				//立方体側が回転していたら
				if (!IsCubeRotated(colB))
				{
					isHit = IsSphereAndOBBOverlapped(colA.sphere, colB.cube);
				}
				else
				{
					isHit = IsSphereAndCubeOverlapped(colA.sphere, colB.cube);
				}
			}
			else {
				colA.cube.position += mPosition;
				colB.sphere.position += collider.mPosition;
				//Aが矩形,Bが円の衝突
				//立方体側が回転していたら
				if (!IsCubeRotated(colA))
				{
					isHit = IsSphereAndOBBOverlapped(colB.sphere, colA.cube);
				}
				else
				{
					isHit = IsSphereAndCubeOverlapped(colB.sphere, colA.cube);
				}
			}

			return isHit;
		}

		//名前の設定
		void Collider::SetName(const std::string& _name) {
			mName = _name;
		}
		const std::string& Collider::GetName()const {
			return mName;
		}

		void Collider::SetTag(const std::string& _tag) {
			mTag = _tag;
		}
		const std::string& Collider::GetTag()const {
			return mTag;
		}

		void Collider::SetActive(bool _isActive) {
			mIsActive = _isActive;
		}

		bool Collider::IsActive()const {
			return mIsActive;
		}

		void Collider::SetOwner(void* _pOwner) {
			mpOwner = _pOwner;
		}

		void* Collider::GetOwner()const {
			return mpOwner;
		}

		void Collider::SetCallbackFunction(const std::function<void(const Collider& colA, const Collider& colB)>& _OnOverlapped) {
			mOnOverlapped = _OnOverlapped;
		}

		void Collider::SetCallbackFunctionEx(const std::function<void(const CollisionEvent& collisionEvent)>& _OnOverlapped) {
			mOnOverlappedEx = _OnOverlapped;
		}

		//グループの設定
		void Collider::SetGroup(int _group) {
			mGroup = _group;
		}

		//グループの解除
		void Collider::ClearGroup() {
			mGroup = 0;
		}

		//グループの取得
		unsigned int Collider::GetGroup()const {
			return mGroup;
		}

		//衝突対象グループの追加
		void Collider::AddHitGroup(int _hitGroup) {
			mHitGroup |= (1 << _hitGroup);
		}
		//衝突対象グループの削除
		void Collider::RemoveHitGroup(int _hitGroup) {
			mHitGroup &= ~(1 << _hitGroup);

		}
		//衝突対象グループのクリア
		void Collider::ClearHitGroup() {
			mHitGroup = 0;

		}
		//衝突対象グループの取得
		unsigned int Collider::GetHitGroup()const {
			return mHitGroup;
		}

		//活性期間の設定
		void Collider::SetActiveDuration(ActiveDuration _activationDuration) {
			mActiveDuration = _activationDuration;
		}

#ifdef _DEBUG
		void Collider::GetOutlineVertices(std::vector<XMFLOAT3>& vertices)
		{
			if (mShape.shapeType == ShapeType::Cube)
			{
				XMFLOAT3 p(mPosition.x + mShape.cube.position.x, mPosition.y + mShape.cube.position.y, 0.0f);

				const float w = mShape.cube.size.w;
				const float h = mShape.cube.size.h;

				vertices[0] = XMFLOAT3(p.x - w / 2.0f, p.y + h / 2.0f, 0.0f);
				vertices[1] = XMFLOAT3(p.x + w / 2.0f, p.y + h / 2.0f, 0.0f);
				vertices[2] = XMFLOAT3(p.x + w / 2.0f, p.y - h / 2.0f, 0.0f);
				vertices[3] = XMFLOAT3(p.x - w / 2.0f, p.y - h / 2.0f, 0.0f);

				return;
			}

			if (mShape.shapeType == ShapeType::Sphere) {

				XMFLOAT3 p(mPosition.x + mShape.sphere.position.x, mPosition.y + mShape.sphere.position.y, 0.0f);
				const float r = mShape.sphere.radius;

				const float toRad = PI_F / 180.0f;

				for (int i = 0; i < 32; ++i) {

					const float theta = toRad * (float)(360 * i) / 32.0f;

					vertices[i] =
						XMFLOAT3(
							p.x + cosf(theta) * r,
							p.y + sinf(theta) * r,
							0.0f

						);
				}
				return;
			}

		}

		void Collider::GetOutlineColor(XMFLOAT3* pOutColor) {
			pOutColor->x = mOutlineColor[0];
			pOutColor->y = mOutlineColor[1];
			pOutColor->z = mOutlineColor[2];
		}

#endif

		//デバッグ表示時のラインの色
		void Collider::SetOutlineColor(float r, float g, float b)
		{
#ifdef _DEBUG
			mOutlineColor[0] = r;
			mOutlineColor[1] = g;
			mOutlineColor[2] = b;
#endif
		}
	}
}
