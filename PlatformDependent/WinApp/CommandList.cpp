/***********************************************************************************************
 ***						W I N A P P  ---  D I S T E N G I N E                            ***
 ***********************************************************************************************
 *                                                                                             *
 *                                   Project Name : WinApp									   *
 *                                                                                             *
 *                                   File Name : CommandList.cpp                               *
 *                                                                                             *
 *                                    Programmer : Kirk                                        *
 *                                                                                             *
 *                                     Date : 2022/8/4                                         *
 *                                                                                             *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "CommandList.h"

CommandList::CommandList(CommandListType type, ComPtr<ID3D12Device> pDevice)
	: CommandList(type, pDevice, nullptr)
{
}

CommandList::CommandList(CommandListType type, ComPtr<ID3D12Device> pDevice, ComPtr<ID3D12PipelineState> pInitialState)
	: _type(type)
	, _device(pDevice)
	, _initialState(pInitialState)
{
	static const std::map<CommandListType, D3D12_COMMAND_LIST_TYPE> typeToNative =
	{
		{CommandListType::Direct,   D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT},
		{CommandListType::Bundle,   D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_BUNDLE},
		{CommandListType::Compute,  D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE},
		{CommandListType::Copy,     D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COPY},
	};
	D3D12_COMMAND_LIST_TYPE nativeType = typeToNative.at(type);
	ThrowIfFailed(pDevice->CreateCommandAllocator(nativeType, IID_PPV_ARGS(&_allocator)));
	ThrowIfFailed(pDevice->CreateCommandList(0, nativeType, _allocator.Get(), pInitialState.Get(), IID_PPV_ARGS(&_commandList)));
}

void CommandList::Reset()
{
	ThrowIfFailed(_allocator->Reset());
	ThrowIfFailed(_commandList->Reset(_allocator.Get(), _initialState.Get()));
}

void CommandList::Close()
{
	ThrowIfFailed(_commandList->Close());
}

CommandListType CommandList::GetType() const
{
	return _type;
}

ComPtr<ID3D12GraphicsCommandList> CommandList::GetInternal() const
{
	return _commandList.Get();
}
