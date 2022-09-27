/***********************************************************************************************
 ***						W I N A P P  ---  D I S T E N G I N E                            ***
 ***********************************************************************************************
 *                                                                                             *
 *                                   Project Name : WinApp									   *
 *                                                                                             *
 *                                   File Name : CommandList.h                                 *
 *                                                                                             *
 *                                    Programmer : Kirk                                        *
 *                                                                                             *
 *                                     Date : 2022/8/4                                         *
 *                                                                                             *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#pragma once
#include "d3dx12.h"
#include <comdef.h>
#include <fstream>
#include "AppUtils.h"
#include <dxgi1_4.h>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;
using namespace std;

enum class CommandListType
{
	Direct,
	Bundle,
	Compute,
	Copy
};

class CommandList
{
public:
	CommandList(CommandListType type, ComPtr<ID3D12Device> pDevice);
	CommandList(CommandListType type, ComPtr<ID3D12Device> pDevice, ComPtr<ID3D12PipelineState> pInitialState);

	void Reset();
	void Close();

	CommandListType GetType() const;
	ComPtr<ID3D12GraphicsCommandList> GetInternal() const;

private:
	ComPtr<ID3D12GraphicsCommandList>   _commandList = nullptr;
	ComPtr<ID3D12CommandAllocator>      _allocator = nullptr;
	ComPtr<ID3D12Device>                _device = nullptr;
	ComPtr<ID3D12PipelineState>         _initialState = nullptr;
	CommandListType                     _type = CommandListType::Direct;
};