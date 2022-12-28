//
// Created by Administrator on 2022/12/10.
//

#ifndef ANALYSE_SHADER_SHADER_H
#define ANALYSE_SHADER_SHADER_H
#pragma once
#include "ShaderStruct.h"

class Shader {

public:
	Shader(std::string name = "New Shader");
	~Shader();

	//	set name
	void set_name(std::string name);
	//	get name
	const std::string& get_name()const;

	//  set property state
	void set_property_state(State& state);
	//  get property state
	const State get_property_state()const;

	//  set property state vector
	void clear_property_state_key_vector();
	void push_back_property_state_key_vector(std::string vec);
	//  get property state vector
	const vector<std::string> get_property_state_key_vector()const;

	//  set property state map
	void clear_property_state_map();
	void push_back_property_state_map(std::string key, std::string value);
	//  get property state map value
	const std::string get_property_state_map_value(std::string key);

	//  set tags
	void set_tag(ShaderTags& tag);
	//  get tags
	const ShaderTags get_tag()const;

	//  set rasterizer
	void set_rasterizer_mode(ShaderRasterizerMode& mode);
	//  get rasterizer
	const ShaderRasterizerMode get_rasterizer_mode()const;

	//  set blend
	void set_blend_mode(ShaderBlendMode& mode);
	//  get blend
	const ShaderBlendMode get_blend_mode()const;

	//  set depth stencil
	void set_depth_stencil_mode(ShaderDepthStencilMode& mode);
	//  get depth stencil
	const ShaderDepthStencilMode get_depth_stencil_mode()const;

private:
	//  name
	std::string name;
	//  property
	State property_state;
	//  property key
	std::vector<string> property_state_key_vector;
	//  property map
	std::map<std::string, std::string> property_state_map;
	//  tags
	ShaderTags tags;
	//  rasterizer
	ShaderRasterizerMode rasterizer_mode;
	//  blend
	ShaderBlendMode blend_mode;
	// depth stencil
	ShaderDepthStencilMode depth_stencil_mode;
};


#endif //ANALYSE_SHADER_SHADER_H
