#pragma once
#include "Mathf.h"

namespace Mathf
{
	struct Transform
	{
		Vector3 Position, EulerAngles, Scale;
		Vector3 Right, Up, Forward;
		Quaternion rotation;
		float4x4 localToWorldMatrix, worldToLocalMatrix;

		Transform();
		Transform(Vector3 Position, Vector3 EulerAngles, Vector3 Scale);

		Vector3 GetPosition();
		Vector3 GetEulerAngles();
		Vector3 GetScale();
		Vector3 GetRight();
		Vector3 GetUp();
		Vector3 GetForward();

		Quaternion GetRotation();

		float4x4 GetLocalToWorldMatrix();
		float4x4 GetWorldToLocalMatrix();
	};
}