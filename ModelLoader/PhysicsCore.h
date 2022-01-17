#pragma once
#include "Mathf.h"
#include "d3dApp.h"
#include "Camera.h"
#include "SkinnedData.h"
#include "LoadM3d.h"
#include "FbxLoader.h"
#include "BoundingAABB.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class PhysicsCore : public D3DApp
{

public:
	PhysicsCore(HINSTANCE hInstance);
	PhysicsCore(const PhysicsCore& rhs) = delete;
	PhysicsCore& operator=(const PhysicsCore& rhs) = delete;
	~PhysicsCore();

	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

	void OnKeyboardInput(const GameTimer& gt);

public:

	//Ray 
	bool ScreenPointToRay(BoundingAABB aabb);

	Camera mCamera;
	POINT mLastMousePos;
	bool isMouseDown = false;
};

