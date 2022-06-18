#include "Transform.h"


//	构造Transform
Transform::Transform()
{
	position3f = { 0,0,0 };
	position = Vector3::zero;
	eulerangle = Vector3::zero;
	scale = Vector3::one;
	forward = Vector3(0, 0, -1);
	quaternion = Quaternion::FromEuler(eulerangle.x, eulerangle.y, eulerangle.z);
}

Transform::Transform(Vector3 pos)
{
	position3f = { pos.x, pos.y, pos.z };
	position = pos;
	eulerangle = Vector3::zero;
	scale = Vector3::one;
	forward = Vector3(0, 0, -1);
	quaternion = Quaternion::FromEuler(eulerangle.x, eulerangle.y, eulerangle.z);
}

Transform::Transform(Vector3 pos, Vector3 rot)
{
	position3f = { pos.x, pos.y, pos.z };
	position = pos;
	eulerangle = rot;
	scale = Vector3::one;
	forward = Vector3(0, 0, -1);
	quaternion = Quaternion::FromEuler(eulerangle.x, eulerangle.y, eulerangle.z);
}

Transform::Transform(Vector3 pos, Vector3 rot, Vector3 scale)
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


//	向量值传递给数组
void Transform::SetVector2Array()
{
	positionArray[0] = position.x;
	positionArray[1] = position.y;
	positionArray[2] = position.z;

	eulerangleArray[0] = eulerangle.x;
	eulerangleArray[1] = eulerangle.y;
	eulerangleArray[2] = eulerangle.z;

	scaleArray[0] = scale.x;
	scaleArray[1] = scale.y;
	scaleArray[2] = scale.z;
}

//	数组值传递给向量
void Transform::SetArray2Vector()
{
	position = { positionArray[0], positionArray[1], positionArray[2] };
	eulerangle = { eulerangleArray[0], eulerangleArray[1], eulerangleArray[2] };
	scale = { scaleArray[0] ,scaleArray[1], scaleArray[2] };
}

//	更新
void Transform::tick(const GameTimer& gt)
{
	GameObject::tick(gt);

	//	设置方向
	XMMATRIX rot_dir = XMMatrixRotationX((eulerangle.x / 180) * Mathf::Pi) * XMMatrixRotationY((eulerangle.y / 180) * Mathf::Pi) * XMMatrixRotationZ((eulerangle.z / 180) * Mathf::Pi);

	XMVECTOR dir = forward.ToSIMD();
	forward = Vector3(XMVector3TransformNormal(dir, rot_dir));

	SetArray2Vector();
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