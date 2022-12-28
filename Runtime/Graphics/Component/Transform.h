#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "GameObject.h"
#include "../Core/Mathf/Mathf.h"

using namespace Mathf;

class Transform : public Component
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
	Vector3 rotateForward = Vector3(0, 0, -1);

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
	Transform(std::string _name);
	Transform(Vector3 pos, std::string _name);
	Transform(Vector3 pos, Vector3 rot, std::string _name);
	Transform(Vector3 pos, Vector3 rot, Vector3 scale, std::string _name);
	~Transform();

	void Tick(const GameTimer& gt);

	void SetPosition(Vector3& pos);
	void SetPosition(float x, float y, float z);

	void SetEulerangle(Vector3& euler);
	void SetEulerangle(float x, float y, float z);

	void SetScale(Vector3& scale);
	void SetScale(float x, float y, float z);

	void AddPosition(Vector3 addPos);
	void AddEulerangle(Vector3 addAngle);
	void AddScale(Vector3 scale);

	//	��ȡ��Ԫ��
	Quaternion GetQuaternion();

	//	��ȡ�������
	float4x4 GetWorldMatrix();

	//	��ȡ�������
	XMFLOAT4X4 GetWorldXMMatrix();

private:

};

#endif // TRANSFORM_H