#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "GameObject.h"
#include "../Core/Mathf/Mathf.h"

using namespace Mathf;

class Transform : public Component
{
public:
	//	世界坐标
	Vector3 position;
	XMFLOAT3 position3f;

	//	欧拉角
	Vector3 eulerangle;

	//	尺寸
	Vector3 scale;

	Vector3 forward;
	Vector3 rotateForward = Vector3(0, 0, -1);

	//	世界坐标 数组
	float positionArray[3] = { 0.0f, 0.0f, 0.0f };
	//	欧拉角 数组
	float eulerangleArray[3] = { 0.0f, 0.0f, 0.0f };
	//	尺寸 数组
	float scaleArray[3] = { 1.0f, 1.0f, 1.0f };

private:
	//	四元数
	Quaternion quaternion;

	//	世界矩阵
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

	//	获取四元数
	Quaternion GetQuaternion();

	//	获取世界矩阵
	float4x4 GetWorldMatrix();

	//	获取世界矩阵
	XMFLOAT4X4 GetWorldXMMatrix();

private:

};

#endif // TRANSFORM_H