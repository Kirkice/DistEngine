/* $Header: /common/Render/src/D3D12Error.cpp	          8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : D3D12Error.cpp											  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#include "../../Core/include/PCH.h"
#include "../../Core/include/Expections.h"

#include "../../Render/include/DXRHelper.h"
#include "../../Render/include/D3D12.h"

namespace render
{
	static const uint64 MaxSubObjDescSize = sizeof(D3D12_HIT_GROUP_DESC);

	void StateObjectBuilder::Init(uint64 maxSubObjects)
	{
		Assert_(maxSubObjects > 0);

		MaxSubObjects = maxSubObjects;
		SubObjectData.Init(maxSubObjects * MaxSubObjDescSize, 0);

		D3D12_STATE_SUBOBJECT defSubObj = { };
		SubObjects.Init(maxSubObjects, defSubObj);
	}

	const D3D12_STATE_SUBOBJECT* StateObjectBuilder::AddSubObject(const void* subObjDesc, uint64 subObjDescSize, D3D12_STATE_SUBOBJECT_TYPE type)
	{
		Assert_(subObjDesc != nullptr);
		Assert_(subObjDescSize > 0);
		Assert_(type < D3D12_STATE_SUBOBJECT_TYPE_MAX_VALID);
		Assert_(subObjDescSize <= MaxSubObjDescSize);
		Assert_(NumSubObjects < MaxSubObjects);

		const uint64 subObjOffset = NumSubObjects * MaxSubObjDescSize;
		memcpy(SubObjectData.Data() + subObjOffset, subObjDesc, subObjDescSize);

		D3D12_STATE_SUBOBJECT& newSubObj = SubObjects[NumSubObjects];
		newSubObj.Type = type;
		newSubObj.pDesc = SubObjectData.Data() + subObjOffset;

		NumSubObjects += 1;

		return &newSubObj;
	}

	void StateObjectBuilder::BuildDesc(D3D12_STATE_OBJECT_TYPE type, D3D12_STATE_OBJECT_DESC& desc)
	{
		desc.Type = type;
		desc.NumSubobjects = uint32(NumSubObjects);
		desc.pSubobjects = NumSubObjects ? SubObjects.Data() : nullptr;
	}

	ID3D12StateObject* StateObjectBuilder::CreateStateObject(D3D12_STATE_OBJECT_TYPE type)
	{
		D3D12_STATE_OBJECT_DESC desc = { };
		BuildDesc(type, desc);

		ID3D12StateObject* stateObj = nullptr;
		DXCall(d3d12::Device->CreateStateObject(&desc, IID_PPV_ARGS(&stateObj)));

		return stateObj;
	}
}