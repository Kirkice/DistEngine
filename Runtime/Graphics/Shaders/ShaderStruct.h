//
// Created by Administrator on 2022/12/10.
//

#ifndef ANALYSE_SHADER_SHADER_STRUCT_H
#define ANALYSE_SHADER_SHADER_STRUCT_H
#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>
#include <functional>
#include <map>
#include <dxgi1_4.h>
#include <d3d12.h>
#include "../Serialization/Property.h"

//  key word
const std::vector<std::string> shader_key_word =
{
		"Name",
		"InputData",
		"Tags",
		"RasterizerMode",
		"BlendMode",
		"DepthStencilMode",
};

const std::vector<std::string> input_data_key_word =
{
		"Texture",
		"Color",
		"Int",
		"Float",
};

const std::vector<std::string>  tags_key_word =
{
		"RenderPass",
		"RenderType",
};

const std::vector<std::string>  rasterizer_mode_key_word
{
		"FillMode",
		"CullMode",
};

const std::vector<std::string>  blend_mode_key_word =
{
		"BlendEnable",
		"LogicOpEnable",
		"SrcBlend",
		"DestBlend",
		"BlendOp",
		"SrcBlendAlpha",
		"DestBlendAlpha",
		"BlendOpAlpha",
		"LogicOp",
		"RenderTargetWriteMask",
};

const std::vector<std::string>  depth_stencil_mode_key_word =
{
		"DepthEnable",
		"DepthWriteMask",
		"DepthFunc",
		"StencilEnable",
		"StencilReadMask",
		"StencilWriteMask",
};

//  Tags
struct ShaderTags
{
	std::string RenderPass;
	std::string RenderType;
};

//  RasterizerMode
struct ShaderRasterizerMode
{
	D3D12_FILL_MODE FILL_MODE;
	D3D12_CULL_MODE CULL_MODE;
};

//  BlendMode
struct ShaderBlendMode
{
	BOOL BlendEnable;
	BOOL LogicOpEnable;
	D3D12_BLEND SrcBlend;
	D3D12_BLEND DestBlend;
	D3D12_BLEND_OP BlendOp;
	D3D12_BLEND SrcBlendAlpha;
	D3D12_BLEND DestBlendAlpha;
	D3D12_BLEND_OP BlendOpAlpha;
	D3D12_LOGIC_OP LogicOp;
	D3D12_COLOR_WRITE_ENABLE RenderTargetWriteMask;
};

//  DepthStencilMode
struct ShaderDepthStencilMode
{
	BOOL DepthEnable;
	D3D12_DEPTH_WRITE_MASK DepthWriteMask;
	D3D12_COMPARISON_FUNC DepthFunc;
};

#endif //ANALYSE_SHADER_SHADER_STRUCT_H
