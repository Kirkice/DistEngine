#include "Transform.h"


//	����Transform
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


//	�ͷ�
Transform::~Transform()
{
}


//	����ֵ���ݸ�����
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

//	����ֵ���ݸ�����
void Transform::SetArray2Vector()
{
	position = { positionArray[0], positionArray[1], positionArray[2] };
	eulerangle = { eulerangleArray[0], eulerangleArray[1], eulerangleArray[2] };
	scale = { scaleArray[0] ,scaleArray[1], scaleArray[2] };
}

//	����
void Transform::tick(const GameTimer& gt)
{
	GameObject::tick(gt);

	//	���÷���
	XMMATRIX rot_dir = XMMatrixRotationX((eulerangle.x / 180) * Mathf::Pi) * XMMatrixRotationY((eulerangle.y / 180) * Mathf::Pi) * XMMatrixRotationZ((eulerangle.z / 180) * Mathf::Pi);

	XMVECTOR dir = forward.ToSIMD();
	forward = Vector3(XMVector3TransformNormal(dir, rot_dir));

	SetArray2Vector();
}

//	��ȡ��Ԫ��
Quaternion Transform::GetQuaternion()
{
	quaternion = Quaternion::FromEuler(eulerangle.x, eulerangle.y, eulerangle.z);
	return quaternion;
}

//	��ȡ�������
float4x4 Transform::GetWorldMatrix()
{
	mWorld = float4x4(GetWorldXMMatrix());
	return mWorld;
}

//	��ȡ�������
XMFLOAT4X4 Transform::GetWorldXMMatrix()
{
	XMFLOAT4X4 Mat = Mathf::Identity4x4();

	//	λ��
	XMMATRIX world = XMLoadFloat4x4(&Mat);
	world = XMMatrixTranslation(position.x, position.y, position.z);

	//// ��ת
	XMMATRIX RotMat = XMMatrixRotationX((eulerangle.x / 180) * Pi) * XMMatrixRotationY((eulerangle.y / 180) * Pi) * XMMatrixRotationZ((eulerangle.z / 180) * Pi);
	world = world * RotMat;

	//	����
	world = world * XMMatrixScaling(scale.x, scale.y, scale.z);
	XMStoreFloat4x4(&Mat, world);

	return Mat;
}