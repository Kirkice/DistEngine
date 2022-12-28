#include "BoundingOBB.h"


BoundingOBB::BoundingOBB()
{
	m_min = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	m_max = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
}

BoundingOBB::BoundingOBB(Vector3 min, Vector3 max)
{
	m_min = min;
	m_max = max;
}

void BoundingOBB::AddPoint(Vector3 point)
{
	m_min = Vector3::Min(point, m_min);
	m_max = Vector3::Max(point, m_max);
}

void BoundingOBB::AddBoundingOBB(const BoundingOBB& obb)
{
	AddPoint(obb.m_min);
	AddPoint(obb.m_max);
}

Vector3 BoundingOBB::GetMin()
{
	return m_min;
}

Vector3 BoundingOBB::GetMax()
{
	return m_max;
}

Vector3 BoundingOBB::GetCenter()
{
	return (m_min + m_max) * 0.5f;
}

bool BoundingOBB::IsIntersect(const BoundingOBB& obb)
{
	return ((m_min.x >= obb.m_min.x && m_min.x <= obb.m_max.x) || (obb.m_min.x >= m_min.x && obb.m_min.x <= m_max.x)) &&
		((m_min.y >= obb.m_min.y && m_min.y <= obb.m_max.y) || (obb.m_min.y >= m_min.y && obb.m_min.y <= m_max.y)) &&
		((m_min.z >= obb.m_min.z && m_min.z <= obb.m_max.z) || (obb.m_min.z >= m_min.z && obb.m_min.z <= m_max.z));
}

bool BoundingOBB::IsIntersectWithRay(Vector3 rayOri, Vector3 rayDir, float length /* = 100 */)
{
	float tmin = 0.0f;
	float tmax = FLT_MAX;

	rayDir = rayDir * length;

	//The plane perpendicular to x-axie  
	if (abs(rayDir.x) < 0.000001f) //If the ray parallel to the plane  
	{
		//If the ray is not within AABB box, then not intersecting  
		if (rayOri.x < this->GetMin().x || rayOri.x > this->GetMax().x)
			return false;
	}
	else
	{
		//Compute the distance of ray to the near plane and far plane  
		float ood = 1.0f / rayDir.x;
		float t1 = (this->GetMin().x - rayOri.x) * ood;
		float t2 = (this->GetMax().x - rayOri.x) * ood;

		//Make t1 be intersecting with the near plane, t2 with the far plane  
		if (t1 > t2)
		{
			float temp = t1;
			t1 = t2;
			t2 = temp;
		}

		//Compute the intersection of slab intersection intervals  
		if (t1 > tmin) tmin = t1;
		if (t2 < tmax) tmax = t2;

		//Exit with no collision as soon as slab intersection becomes empty  
		if (tmin > tmax) return false;
	}

	//The plane perpendicular to y-axie  
	if (abs(rayDir.y) < 0.000001f) //If the ray parallel to the plane  
	{
		//If the ray is not within AABB box, then not intersecting  
		if (rayOri.y < this->GetMin().y || rayOri.y > this->GetMax().y)
			return false;
	}
	else
	{
		//Compute the distance of ray to the near plane and far plane  
		float ood = 1.0f / rayDir.y;
		float t1 = (this->GetMin().y - rayOri.y) * ood;
		float t2 = (this->GetMax().y - rayOri.y) * ood;

		//Make t1 be intersecting with the near plane, t2 with the far plane  
		if (t1 > t2)
		{
			float temp = t1;
			t1 = t2;
			t2 = temp;
		}

		//Compute the intersection of slab intersection intervals  
		if (t1 > tmin) tmin = t1;
		if (t2 < tmax) tmax = t2;

		//Exit with no collision as soon as slab intersection becomes empty  
		if (tmin > tmax) return false;
	}// end for perpendicular to y-axie  

		//The plane perpendicular to z-axie  
	if (abs(rayDir.z) < 0.000001f) //If the ray parallel to the plane  
	{
		//If the ray is not within AABB box, then not intersecting  
		if (rayOri.z < this->GetMin().z || rayOri.z > this->GetMax().z)
			return false;
	}
	else
	{
		//Compute the distance of ray to the near plane and far plane  
		float ood = 1.0f / rayDir.z;
		float t1 = (this->GetMin().z - rayOri.z) * ood;
		float t2 = (this->GetMax().z - rayOri.z) * ood;

		//Make t1 be intersecting with the near plane, t2 with the far plane  
		if (t1 > t2)
		{
			float temp = t1;
			t1 = t2;
			t2 = temp;
		}

		//Compute the intersection of slab intersection intervals  
		if (t1 > tmin) tmin = t1;
		if (t2 < tmax) tmax = t2;

		//Exit with no collision as soon as slab intersection becomes empty  
		if (tmin > tmax) return false;
	}// end for perpendicular to z-axie  

	//Hit Point Position
	//vcHit->x = origin.x + tmin * dir.x;
	//vcHit->y = origin.y + tmin * dir.y;
	//vcHit->z = origin.z + tmin * dir.z;

	return true;
}