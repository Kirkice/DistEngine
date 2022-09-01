#include "BoundingSphere.h"
#include <vector>

using namespace DistBound;

BoundingSphere::BoundingSphere()
{
	x = y = z = w = 0.0f;
}

BoundingSphere::BoundingSphere(float x_, float y_, float z_, float w_)
{
	x = x_;
	y = y_;
	z = z_;
	w = w_;
}

BoundingSphere::BoundingSphere(const Vector3& xyz, float radius_)
{
	x = xyz.x;
	y = xyz.y;
	z = xyz.z;
	w = radius_;
}

BoundingSphere::BoundingSphere(const XMFLOAT4& xyzw)
{
	x = xyzw.x;
	y = xyzw.y;
	z = xyzw.z; 
	w = xyzw.w;
}

BoundingSphere::BoundingSphere(FXMVECTOR xyzw)
{
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(this), xyzw);
}

BoundingSphere::BoundingSphere(Vector4 vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
	w = vec.w;
}

bool BoundingSphere::IsIntersect(const BoundingSphere& rhs)
{
	float dis = Vector3::Distance(this->GetCenter(), rhs.GetCenter());
	float radiusSum = this->GetRadius() + rhs.GetRadius();
	float radiusSub = abs(this->GetRadius() - rhs.GetRadius());

	if (dis <= radiusSum && dis >= radiusSub)
		return true;
	else
		return false;
}

bool BoundingSphere::IsContain(const BoundingSphere& rhs)
{
	float dis = Vector3::Distance(this->GetCenter(), rhs.GetCenter());
	float radiusSub = abs(this->GetRadius() - rhs.GetRadius());

	if (dis < radiusSub)
		return true;
	else
		return false;
}

bool SolvingQuadratics(double a, double b, double c)
{
	double delta = b * b - 4 * a * c;
	if (delta < 0)
		false;
	else
		return true;
}

bool BoundingSphere::IsIntersectWithRay(Vector3 rayOri, Vector3 rayDir, float length)
{
	Vector3 D = rayOri * length;

	Vector3 Center = this->GetCenter();
	float R = this->GetRadius();
	double a = (D.x * D.x) + (D.y * D.y) + (D.z * D.z);
	double b = (2 * D.x * (rayOri.x - Center.x) + 2 * D.y * (rayOri.y - Center.y) + 2 * D.z * (rayOri.z - Center.z));
	double c = ((rayOri.x - Center.x) * (rayOri.x - Center.x) + (rayOri.y - Center.y) * (rayOri.y - Center.y) + (rayOri.z - Center.z) * (rayOri.z - Center.z)) - R * R;

	return SolvingQuadratics(a, b, c);
}