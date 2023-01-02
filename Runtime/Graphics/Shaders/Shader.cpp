//
// Created by Administrator on 2022/12/10.
//

#include "Shader.h"

Shader::Shader(string name)
{
	this->name = name;
}

Shader::~Shader()
{
}

void Shader::set_name(std::string name)
{
	this->name = name;
}

const std::string& Shader::get_name()const
{
	return this->name;
}

void Shader::set_property_state(State& state)
{
	this->property_state = state;
}

const State Shader::get_property_state()const
{
	return this->property_state;
}

void Shader::clear_property_state_key_vector()
{
	this->property_state_key_vector.clear();
}

void Shader::push_back_property_state_key_vector(std::string str)
{
	this->property_state_key_vector.push_back(str);
}

const std::vector<std::string> Shader::get_property_state_key_vector()const
{
	return this->property_state_key_vector;
}

void Shader::clear_property_state_map()
{
	property_state_map.clear();
}

void Shader::push_back_property_state_map(std::string key, std::string value)
{
	property_state_map[key] = std::move(value);
}

const std::string Shader::get_property_state_map_value(std::string key)
{
	return this->property_state_map[key];
}

void Shader::set_tag(ShaderTags& tag)
{
	this->tags = tag;
}

const ShaderTags Shader::get_tag()const
{
	return this->tags;
}

void Shader::set_rasterizer_mode(ShaderRasterizerMode& mode)
{
	this->rasterizer_mode = mode;
}

const ShaderRasterizerMode Shader::get_rasterizer_mode()const
{
	return this->rasterizer_mode;
}

void Shader::set_blend_mode(ShaderBlendMode& mode)
{
	this->blend_mode = mode;
}

const ShaderBlendMode Shader::get_blend_mode()const
{
	return this->blend_mode;
}

void Shader::set_depth_stencil_mode(ShaderDepthStencilMode& mode)
{
	this->depth_stencil_mode = mode;
}

const ShaderDepthStencilMode Shader::get_depth_stencil_mode()const
{
	return this->depth_stencil_mode;
}