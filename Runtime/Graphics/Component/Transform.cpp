#include "Transform.h"


//	构造Transform
Transform::Transform(std::string _name = "Transform") : Component(_name)
{
	position3f = { 0,0,0 };
	position = Vector3::zero;
	eulerangle = Vector3::zero;
	scale = Vector3::one;
	forward = Vector3(0, 0, -1);
	quaternion = Quaternion::FromEuler(eulerangle.x, eulerangle.y, eulerangle.z);
}

Transform::Transform(Vector3 pos, std::string _name = "Transform") : Component(_name)
{
	position3f = { pos.x, pos.y, pos.z };
	position = pos;
	eulerangle = Vector3::zero;
	scale = Vector3::one;
	forward = Vector3(0, 0, -1);
	quaternion = Quaternion::FromEuler(eulerangle.x, eulerangle.y, eulerangle.z);
}

Transform::Transform(Vector3 pos, Vector3 rot, std::string _name = "Transform") : Component(_name)
{
	position3f = { pos.x, pos.y, pos.z };
	position = pos;
	eulerangle = rot;
	scale = Vector3::one;
	forward = Vector3(0, 0, -1);
	quaternion = Quaternion::FromEuler(eulerangle.x, eulerangle.y, eulerangle.z);
}

Transform::Transform(Vector3 pos, Vector3 rot, Vector3 scale, std::string _name = "Transform") : Component(_name)
{
	position3f = { pos.x, pos.y, pos.z };
	position = pos;
	eulerangle = rot;
	scale = scale;
	forward = Vector3(0, 0, -1);
	quaternion = Quaternion::FromEuler(eulerangle.x, eulerangle.y, eulerangle.z);
}


//	释放
Transform::~Transform()
{
}

//	更新
void Transform::Tick(const GameTimer& gt)
{
	//	设置方向
	XMMATRIX rot_dir = XMMatrixRotationX((eulerangle.x / 180) * Mathf::Pi) * XMMatrixRotationY((eulerangle.y / 180) * Mathf::Pi) * XMMatrixRotationZ((eulerangle.z / 180) * Mathf::Pi);

	rotateForward = Vector3(0, 0, -1);
	XMVECTOR lightDir = rotateForward.ToSIMD();
	lightDir = XMVector3TransformNormal(lightDir, rot_dir);

	forward = Vector3(lightDir);
}

/// <summary>
/// Set Position
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
void Transform::SetPosition(float x, float y, float z)
{
	position = Vector3(x, y, z);
	position3f = XMFLOAT3(x, y, z);
}

void Transform::SetPosition(Vector3& pos)
{
	position = pos;
	position3f = XMFLOAT3(pos.x, pos.y, pos.z);
}

/// <summary>
/// SetEulerAngle
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
void Transform::SetEulerangle(float x, float y, float z)
{
	eulerangle = Vector3(x, y, z);
}

void Transform::SetEulerangle(Vector3& euler)
{
	eulerangle = euler;
}

/// <summary>
/// SetScale
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
void Transform::SetScale(float x, float y, float z)
{
	scale = Vector3(x, y, z);
}

void Transform::SetScale(Vector3& scl)
{
	scale = scl;
}

void Transform::AddPosition(Vector3 addPos)
{
	position = position + addPos;
	position3f = XMFLOAT3(position.x, position.y, position.z);
}

void Transform::AddEulerangle(Vector3 addAngle)
{
	eulerangle = eulerangle + addAngle;
}

void Transform::AddScale(Vector3 addScale)
{
	scale = scale + addScale;
}

//	获取四元数
Quaternion Transform::GetQuaternion()
{
	quaternion = Quaternion::FromEuler(eulerangle.x, eulerangle.y, eulerangle.z);
	return quaternion;
}

//	获取世界矩阵
float4x4 Transform::GetWorldMatrix()
{
	mWorld = float4x4(GetWorldXMMatrix());
	return mWorld;
}

//	获取世界矩阵
XMFLOAT4X4 Transform::GetWorldXMMatrix()
{
	XMFLOAT4X4 Mat = Mathf::Identity4x4();

	//	位移
	XMMATRIX world = XMLoadFloat4x4(&Mat);
	world = XMMatrixTranslation(position.x, position.y, position.z);

	//// 旋转
	XMMATRIX RotMat = XMMatrixRotationX((eulerangle.x / 180) * Pi) * XMMatrixRotationY((eulerangle.y / 180) * Pi) * XMMatrixRotationZ((eulerangle.z / 180) * Pi);
	world = world * RotMat;

	//	缩放
	world = world * XMMatrixScaling(scale.x, scale.y, scale.z);
	XMStoreFloat4x4(&Mat, world);

	return Mat;
}