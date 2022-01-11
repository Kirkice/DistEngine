#include "Transform.h"

namespace Mathf
{
	Transform::Transform()
	{
		//Trans
		Position = Vector3(0, 0, 0);
		EulerAngles = Vector3(0, 0, 0);
		Scale = Vector3(1,1,1);

		//Quater
		rotation = Quaternion::FromEuler(0,0,0);

		//Matrix
		XMMATRIX Rot = XMMatrixRotationX((EulerAngles.x / 180) * Mathf::Pi) * XMMatrixRotationY((EulerAngles.y / 180) * Mathf::Pi) * XMMatrixRotationZ((EulerAngles.z / 180) * Mathf::Pi);

		XMFLOAT4X4 tempMat = Mathf::Identity4x4();
		XMStoreFloat4x4(&tempMat, XMMatrixScaling(Scale.x, Scale.y, Scale.z) * XMMatrixTranslation(Position.x, Position.y, Position.z) * Rot);
		localToWorldMatrix = float4x4(tempMat);

		XMMATRIX InvWorld = XMMatrixInverse(&XMMatrixDeterminant(XMMatrixScaling(Scale.x, Scale.y, Scale.z) * XMMatrixTranslation(Position.x, Position.y, Position.z) * Rot), XMMatrixScaling(Scale.x, Scale.y, Scale.z) * XMMatrixTranslation(Position.x, Position.y, Position.z) * Rot);

		XMStoreFloat4x4(&tempMat, InvWorld);
		worldToLocalMatrix = float4x4(tempMat);
	}

	Transform::Transform(Vector3 mPosition, Vector3 mEulerAngles, Vector3 mScale)
	{
		//Trans
		Position = mPosition;
		EulerAngles = mEulerAngles;
		Scale = mScale;

		//Quater
		rotation = Quaternion::FromEuler(mEulerAngles.x, mEulerAngles.y, mEulerAngles.z);

		//Matrix
		XMMATRIX Rot = XMMatrixRotationX((EulerAngles.x / 180) * Mathf::Pi) * XMMatrixRotationY((EulerAngles.y / 180) * Mathf::Pi) * XMMatrixRotationZ((EulerAngles.z / 180) * Mathf::Pi);

		XMFLOAT4X4 tempMat = Mathf::Identity4x4();
		XMStoreFloat4x4(&tempMat, XMMatrixScaling(Scale.x, Scale.y, Scale.z) * XMMatrixTranslation(Position.x, Position.y, Position.z) * Rot);
		localToWorldMatrix = float4x4(tempMat);

		XMMATRIX InvWorld = XMMatrixInverse(&XMMatrixDeterminant(XMMatrixScaling(Scale.x, Scale.y, Scale.z) * XMMatrixTranslation(Position.x, Position.y, Position.z) * Rot), XMMatrixScaling(Scale.x, Scale.y, Scale.z) * XMMatrixTranslation(Position.x, Position.y, Position.z) * Rot);

		XMStoreFloat4x4(&tempMat, InvWorld);
		worldToLocalMatrix = float4x4(tempMat);
	}

	Vector3 Transform::GetPosition()
	{
		return Position;
	}

	Vector3 Transform::GetEulerAngles()
	{
		return EulerAngles;
	}

	Vector3 Transform::GetScale()
	{
		return Scale;
	}

	Vector3 Transform::GetRight()
	{
		return localToWorldMatrix.Right();
	}

	Vector3 Transform::GetUp()
	{
		return localToWorldMatrix.Up();
	}

	Vector3 Transform::GetForward()
	{
		return localToWorldMatrix.Forward();
	}

	Quaternion Transform::GetRotation()
	{
		return rotation;
	}

	float4x4 Transform::GetLocalToWorldMatrix()
	{
		return localToWorldMatrix;
	}

	float4x4 Transform::GetWorldToLocalMatrix()
	{
		return worldToLocalMatrix;
	}
}