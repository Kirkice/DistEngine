#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "GameObject.h"
#include "../Core/Mathf/Mathf.h"

using namespace Mathf;

class Transform : public GameObject
{
public:
	//	��������
	Vector3 position;
	XMFLOAT3 position3f;

	//	ŷ����
	Vector3 eulerangle;

	//	�ߴ�
	Vector3 scale;

	Vector3 forward;


	//	�������� ����
	float positionArray[3] = { 0.0f, 0.0f, 0.0f };
	//	ŷ���� ����
	float eulerangleArray[3] = { 0.0f, 0.0f, 0.0f };
	//	�ߴ� ����
	float scaleArray[3] = { 1.0f, 1.0f, 1.0f };

private:
	//	��Ԫ��
	Quaternion quaternion;

	//	�������
	float4x4 mWorld = float4x4();

public:
	Transform();
	Transform(Vector3 pos);
	Transform(Vector3 pos, Vector3 rot);
	Transform(Vector3 pos, Vector3 rot, Vector3 scale);
	~Transform();

	//	����ֵ���ݸ�����
	void SetVector2Array();

	//	����ֵ���ݸ�����
	void SetArray2Vector();

	void Tick();

	//	��ȡ��Ԫ��
	Quaternion GetQuaternion();

	//	��ȡ�������
	float4x4 GetWorldMatrix();

	//	��ȡ�������
	XMFLOAT4X4 GetWorldXMMatrix();

private:

};

#endif // TRANSFORM_H