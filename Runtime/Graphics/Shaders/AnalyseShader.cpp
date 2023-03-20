//
// Created by Administrator on 2022/12/5.
//

#include "AnalyseShader.h"

void analyse_shader::load(std::string folder_path, std::vector<std::unique_ptr<Shader>>& shader_vector)
{
	get_shader_files(folder_path, shader_file_paths);

	analyse(shader_vector);
}

void analyse_shader::analyse(std::vector<std::unique_ptr<Shader>>& shader_vector)
{
	if (shader_file_paths.size() <= 0)
		return;

	for (int i = 0; i < shader_file_paths.size(); ++i)
	{
		ifstream srcFile(shader_file_paths[i], ios::in);    //以文本模式打开txt文件
		string combine = "";
		string x;
		while (getline(srcFile, x))    //数字读取方式
		{
			combine.append(x);
		}
		analyse_code(combine, shader_vector);
		srcFile.close();
	}
}

void analyse_shader::analyse_code(std::string combine, std::vector<std::unique_ptr<Shader>>& shader_vector)
{
	//  第一层关键字分割
	vector<string> part_vector;
	split(part_vector, combine, is_any_of("---"), token_compress_on);

	std::string name_string = "";
	std::string input_data_string = "";
	std::string tag_string = "";
	std::string rasterizer_mode_string = "";
	std::string blend_mode_string = "";
	std::string depth_stencil_mode = "";

	if (part_vector.size() == 6)
	{
		name_string = trim_copy(part_vector[0]);
		input_data_string = trim_copy(part_vector[1]);
		tag_string = trim_copy(part_vector[2]);
		rasterizer_mode_string = trim_copy(part_vector[3]);
		blend_mode_string = trim_copy(part_vector[4]);
		depth_stencil_mode = trim_copy(part_vector[5]);
	}
	else
	{
		cout << "error shader code!" << endl;
		return;
	}

	auto shader = std::make_unique<Shader>();

	int analyse_shader_part_index = 0;
	//  Get Name
	analyse_name_part(name_string, shader, analyse_shader_part_index++);

	//  Get InputData
	analyse_input_data_part(input_data_string, shader, analyse_shader_part_index++);

	//  Get TagsData
	analyse_tags_part(tag_string, shader, analyse_shader_part_index++);

	//  Get Raster Mode
	analyse_raster_mode_part(rasterizer_mode_string, shader, analyse_shader_part_index++);

	//  Get Blend Mode
	analyse_blend_mode_part(blend_mode_string, shader, analyse_shader_part_index++);

	//  Get Depth Stencil Mode
	analyse_depth_stencil_mode_part(depth_stencil_mode, shader, analyse_shader_part_index++);

	shader_vector.push_back(std::move(shader));
}

void analyse_shader::set_d3d12_blend(D3D12_BLEND& blend_mode, std::string compare_string, std::string key, std::string value)
{
	if (equals(trim_copy(key), compare_string))
	{
		if (equals(trim_copy(value), "D3D12_BLEND_ZERO"))
			blend_mode = D3D12_BLEND_ZERO;
		else if (equals(trim_copy(value), "D3D12_BLEND_ONE"))
			blend_mode = D3D12_BLEND_ONE;
		else if (equals(trim_copy(value), "D3D12_BLEND_SRC_COLOR"))
			blend_mode = D3D12_BLEND_SRC_COLOR;
		else if (equals(trim_copy(value), "D3D12_BLEND_INV_SRC_COLOR"))
			blend_mode = D3D12_BLEND_INV_SRC_COLOR;
		else if (equals(trim_copy(value), "D3D12_BLEND_SRC_ALPHA"))
			blend_mode = D3D12_BLEND_SRC_ALPHA;
		else if (equals(trim_copy(value), "D3D12_BLEND_INV_SRC_ALPHA"))
			blend_mode = D3D12_BLEND_INV_SRC_ALPHA;
		else if (equals(trim_copy(value), "D3D12_BLEND_DEST_ALPHA"))
			blend_mode = D3D12_BLEND_DEST_ALPHA;
		else if (equals(trim_copy(value), "D3D12_BLEND_INV_DEST_ALPHA"))
			blend_mode = D3D12_BLEND_INV_DEST_ALPHA;
		else if (equals(trim_copy(value), "D3D12_BLEND_DEST_COLOR"))
			blend_mode = D3D12_BLEND_DEST_COLOR;
		else if (equals(trim_copy(value), "D3D12_BLEND_INV_DEST_COLOR"))
			blend_mode = D3D12_BLEND_INV_DEST_COLOR;
		else if (equals(trim_copy(value), "D3D12_BLEND_SRC_ALPHA_SAT"))
			blend_mode = D3D12_BLEND_SRC_ALPHA_SAT;
		else if (equals(trim_copy(value), "D3D12_BLEND_BLEND_FACTOR"))
			blend_mode = D3D12_BLEND_BLEND_FACTOR;
		else if (equals(trim_copy(value), "D3D12_BLEND_INV_BLEND_FACTOR"))
			blend_mode = D3D12_BLEND_INV_BLEND_FACTOR;
		else if (equals(trim_copy(value), "D3D12_BLEND_SRC1_COLOR"))
			blend_mode = D3D12_BLEND_SRC1_COLOR;
		else if (equals(trim_copy(value), "D3D12_BLEND_INV_SRC1_COLOR"))
			blend_mode = D3D12_BLEND_INV_SRC1_COLOR;
		else if (equals(trim_copy(value), "D3D12_BLEND_SRC1_ALPHA"))
			blend_mode = D3D12_BLEND_SRC1_ALPHA;
		else if (equals(trim_copy(value), "D3D12_BLEND_INV_SRC1_ALPHA"))
			blend_mode = D3D12_BLEND_INV_SRC1_ALPHA;
	}
}

void analyse_shader::set_d3d12_blend_op(D3D12_BLEND_OP& blend_mode, std::string compare_string, std::string key, std::string value)
{
	if (equals(trim_copy(key), compare_string))
	{
		if (equals(trim_copy(value), "D3D12_BLEND_OP_ADD"))
			blend_mode = D3D12_BLEND_OP_ADD;
		else if (equals(trim_copy(value), "D3D12_BLEND_OP_SUBTRACT"))
			blend_mode = D3D12_BLEND_OP_SUBTRACT;
		else if (equals(trim_copy(value), "D3D12_BLEND_OP_REV_SUBTRACT"))
			blend_mode = D3D12_BLEND_OP_REV_SUBTRACT;
		else if (equals(trim_copy(value), "D3D12_BLEND_OP_MIN"))
			blend_mode = D3D12_BLEND_OP_MIN;
		else
			blend_mode = D3D12_BLEND_OP_MAX;
	}
}

void analyse_shader::set_d3d12_logic_op(D3D12_LOGIC_OP& blend_mode, std::string compare_string, std::string key, std::string value)
{
	if (equals(trim_copy(key), compare_string))
	{
		if (equals(trim_copy(value), "D3D12_LOGIC_OP_CLEAR"))
			blend_mode = D3D12_LOGIC_OP_CLEAR;
		else if (equals(trim_copy(value), "D3D12_LOGIC_OP_SET"))
			blend_mode = D3D12_LOGIC_OP_SET;
		else if (equals(trim_copy(value), "D3D12_LOGIC_OP_COPY"))
			blend_mode = D3D12_LOGIC_OP_COPY;
		else if (equals(trim_copy(value), "D3D12_LOGIC_OP_COPY_INVERTED"))
			blend_mode = D3D12_LOGIC_OP_COPY_INVERTED;
		else if (equals(trim_copy(value), "D3D12_LOGIC_OP_NOOP"))
			blend_mode = D3D12_LOGIC_OP_NOOP;
		else if (equals(trim_copy(value), "D3D12_LOGIC_OP_AND"))
			blend_mode = D3D12_LOGIC_OP_AND;
		else if (equals(trim_copy(value), "D3D12_LOGIC_OP_NAND"))
			blend_mode = D3D12_LOGIC_OP_NAND;
		else if (equals(trim_copy(value), "D3D12_LOGIC_OP_OR"))
			blend_mode = D3D12_LOGIC_OP_OR;
		else if (equals(trim_copy(value), "D3D12_LOGIC_OP_NOR"))
			blend_mode = D3D12_LOGIC_OP_NOR;
		else if (equals(trim_copy(value), "D3D12_LOGIC_OP_XOR"))
			blend_mode = D3D12_LOGIC_OP_XOR;
		else if (equals(trim_copy(value), "D3D12_LOGIC_OP_EQUIV"))
			blend_mode = D3D12_LOGIC_OP_EQUIV;
		else if (equals(trim_copy(value), "D3D12_LOGIC_OP_AND_REVERSE"))
			blend_mode = D3D12_LOGIC_OP_AND_REVERSE;
		else if (equals(trim_copy(value), "D3D12_LOGIC_OP_AND_INVERTED"))
			blend_mode = D3D12_LOGIC_OP_AND_INVERTED;
		else if (equals(trim_copy(value), "D3D12_LOGIC_OP_OR_REVERSE"))
			blend_mode = D3D12_LOGIC_OP_OR_REVERSE;
		else
			blend_mode = D3D12_LOGIC_OP_OR_INVERTED;
	}
}

void analyse_shader::set_d3d12_color_write_enable(D3D12_COLOR_WRITE_ENABLE& write_enable, std::string compare_string, std::string key, std::string value)
{
	if (equals(trim_copy(key), compare_string))
	{
		if (equals(trim_copy(value), "D3D12_COLOR_WRITE_ENABLE_RED"))
			write_enable = D3D12_COLOR_WRITE_ENABLE_RED;
		else if (equals(trim_copy(value), "D3D12_COLOR_WRITE_ENABLE_GREEN"))
			write_enable = D3D12_COLOR_WRITE_ENABLE_GREEN;
		else if (equals(trim_copy(value), "D3D12_COLOR_WRITE_ENABLE_BLUE"))
			write_enable = D3D12_COLOR_WRITE_ENABLE_BLUE;
		else if (equals(trim_copy(value), "D3D12_COLOR_WRITE_ENABLE_ALPHA"))
			write_enable = D3D12_COLOR_WRITE_ENABLE_ALPHA;
		else
			write_enable = D3D12_COLOR_WRITE_ENABLE_ALL;
	}
}

void analyse_shader::set_d3d12_comparison_func(D3D12_COMPARISON_FUNC& comparison_func, std::string compare_string, std::string key, std::string value)
{
	if (equals(trim_copy(key), compare_string))
	{
		if (equals(trim_copy(value), "D3D12_COMPARISON_FUNC_NEVER"))
			comparison_func = D3D12_COMPARISON_FUNC_NEVER;
		else if (equals(trim_copy(value), "D3D12_COMPARISON_FUNC_LESS"))
			comparison_func = D3D12_COMPARISON_FUNC_LESS;
		else if (equals(trim_copy(value), "D3D12_COMPARISON_FUNC_EQUAL"))
			comparison_func = D3D12_COMPARISON_FUNC_EQUAL;
		else if (equals(trim_copy(value), "D3D12_COMPARISON_FUNC_LESS_EQUAL"))
			comparison_func = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		else if (equals(trim_copy(value), "D3D12_COMPARISON_FUNC_GREATER"))
			comparison_func = D3D12_COMPARISON_FUNC_GREATER;
		else if (equals(trim_copy(value), "D3D12_COMPARISON_FUNC_NOT_EQUAL"))
			comparison_func = D3D12_COMPARISON_FUNC_NOT_EQUAL;
		else if (equals(trim_copy(value), "D3D12_COMPARISON_FUNC_GREATER_EQUAL"))
			comparison_func = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		else
			comparison_func = D3D12_COMPARISON_FUNC_ALWAYS;
	}
}

void analyse_shader::erase_enter(std::string& str)
{
	str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
}

std::string analyse_shader::character_segmentation(std::string source) {
	int len = source.size();
	char buf[1024];
	char buf1[1024] = { 0 };
	char tar1[10] = { "{" };
	char tar2[10] = { "}" };
	sprintf(buf, "%s", source.c_str());
	int ib = 0;
	for (int i = 0; i < len; i++)
	{
		if (buf[i] == tar1[0]) {

			for (int ii = i; i < len; i++)
			{
				if (buf[i + 1] == tar2[0])
				{
					break;
				}
				if (buf[i + 1] != tar1[0])
				{
					buf1[ib] = buf[i + 1];
					ib++;
				}
			}
		}
	}
	return buf1;
}

void analyse_shader::analyse_name_part(std::string name_string, std::unique_ptr<Shader>& shader, int index)
{
	//  Judge Content Name
	string::size_type stringIdx = name_string.find(shader_key_word[index]);
	//  Content "Name"
	if (stringIdx != string::npos)
	{
		vector<string> name_vector;
		//  Split (';')
		split(name_vector, name_string, is_any_of(";"), token_compress_on);
		if (name_vector.size() > 0)
		{
			//  Split ("=")
			name_string = name_vector[0];
			name_vector.clear();
			split(name_vector, name_string, is_any_of("="), token_compress_on);
			if (name_vector.size() > 1)
			{   //  Set Name
				shader->set_name(trim_copy(name_vector[1]));
			}
		}
	}
}

void analyse_shader::analyse_input_data_part(std::string input_data_string, std::unique_ptr<Shader>& shader, int index)
{
	shader->clear_property_state_key_vector();
	shader->clear_property_state_map();

	//    State shader_state;

	//  Judge Content InputData
	string::size_type stringIdx = input_data_string.find(shader_key_word[index]);
	//  Content "InputData"
	if (stringIdx != string::npos)
	{
		//  Get{} String
		input_data_string = character_segmentation(input_data_string);

		vector<string> input_data_vector;
		//  Split (';')
		split(input_data_vector, input_data_string, is_any_of(";"), token_compress_on);
		if (input_data_vector.size() > 0)
		{
			vector<string> property_data_vector;

			for (int i = 0; i < input_data_vector.size(); i++)
			{
				int input_data_vector_index = 0;
				std::string current_line_string = input_data_vector[i];
				//  Content Texture
				string::size_type texIdx = current_line_string.find(input_data_key_word[input_data_vector_index++]);
				if (texIdx != string::npos)
				{
					//  Split (":")
					property_data_vector.clear();
					split(property_data_vector, current_line_string, is_any_of(":"), token_compress_on);
					if (property_data_vector.size() > 0)
					{
						//  Split ("=")
						std::string new_current_line_string = property_data_vector[1];
						property_data_vector.clear();
						split(property_data_vector, new_current_line_string, is_any_of("="), token_compress_on);
						if (property_data_vector.size() > 1)
						{
							shader->push_back_property_state_key_vector(trim_copy(property_data_vector[0]));
							shader->push_back_property_state_map(trim_copy(property_data_vector[0]), "Texture");
							std::cout << trim_copy(property_data_vector[0]) << ":  ";
							std::cout << trim_copy(property_data_vector[1]) << std::endl;
							//  set state
//                            shader_state.add_property<string>(trim_copy(property_data_vector[0]));
//                            shader_state.set_property<string>(trim_copy(property_data_vector[0]),trim_copy(property_data_vector[1]));
						}
					}
				}

				//  Content Color
				string::size_type colorIdx = current_line_string.find(input_data_key_word[input_data_vector_index++]);
				if (colorIdx != string::npos)
				{
					//  Split (":")
					property_data_vector.clear();
					split(property_data_vector, current_line_string, is_any_of(":"), token_compress_on);
					if (property_data_vector.size() > 0)
					{
						//  Split ("=")
						std::string new_current_line_string = property_data_vector[1];
						property_data_vector.clear();
						split(property_data_vector, new_current_line_string, is_any_of("="), token_compress_on);
						if (property_data_vector.size() > 1)
						{
							shader->push_back_property_state_key_vector(property_data_vector[0]);
							shader->push_back_property_state_map(trim_copy(property_data_vector[0]), "Color");
							std::cout << trim_copy(property_data_vector[0]) << ":  ";
							std::cout << trim_copy(property_data_vector[1]) << std::endl;
							//  set state
//                            shader_state.add_property<string>(trim_copy(property_data_vector[0]));
//                            shader_state.set_property<string>(trim_copy(property_data_vector[0]),trim_copy(property_data_vector[1]));
						}
					}
				}

				//  Content Int
				string::size_type intIdx = input_data_vector[i].find(input_data_key_word[input_data_vector_index++]);
				if (intIdx != string::npos)
				{
					//  Split (":")
					property_data_vector.clear();
					split(property_data_vector, current_line_string, is_any_of(":"), token_compress_on);
					if (property_data_vector.size() > 0)
					{
						//  Split ("=")
						std::string new_current_line_string = property_data_vector[1];
						property_data_vector.clear();
						split(property_data_vector, new_current_line_string, is_any_of("="), token_compress_on);
						if (property_data_vector.size() > 1)
						{
							shader->push_back_property_state_key_vector(trim_copy(property_data_vector[0]));
							shader->push_back_property_state_map(trim_copy(property_data_vector[0]), "Int");
							std::cout << trim_copy(property_data_vector[0]) << ":  ";
							std::cout << trim_copy(property_data_vector[1]) << std::endl;
							//  set state
//                            shader_state.add_property<string>(trim_copy(property_data_vector[0]));
//                            shader_state.set_property<string>(trim_copy(property_data_vector[0]),trim_copy(property_data_vector[1]));
						}
					}
				}

				//  Content Float
				string::size_type floatIdx = input_data_vector[i].find(input_data_key_word[input_data_vector_index]);
				if (floatIdx != string::npos)
				{
					//  Split (":")
					property_data_vector.clear();
					split(property_data_vector, current_line_string, is_any_of(":"), token_compress_on);
					if (property_data_vector.size() > 0)
					{
						//  Split ("=")
						std::string new_current_line_string = property_data_vector[1];
						property_data_vector.clear();
						split(property_data_vector, new_current_line_string, is_any_of("="), token_compress_on);
						if (property_data_vector.size() > 1)
						{
							shader->push_back_property_state_key_vector(trim_copy(property_data_vector[0]));
							shader->push_back_property_state_map(trim_copy(property_data_vector[0]), "Float");
							std::cout << trim_copy(property_data_vector[0]) << ":  ";
							std::cout << trim_copy(property_data_vector[1]) << std::endl;
							//  set state
//                            shader_state.add_property<string>(trim_copy(property_data_vector[0]));
//                            shader_state.set_property<string>(trim_copy(property_data_vector[0]),trim_copy(property_data_vector[1]));
						}
					}
				}

			}
		}
	}
}

void analyse_shader::analyse_tags_part(std::string tag_string, std::unique_ptr<Shader>& shader, int index) {
	//  Judge Content TagsData
	string::size_type stringIdx = tag_string.find(shader_key_word[index]);
	//  Content "TagsData"
	if (stringIdx != string::npos) {
		//  Get{} String
		tag_string = character_segmentation(tag_string);

		vector <string> tag_vector;
		//  Split (';')
		split(tag_vector, tag_string, is_any_of(";"), token_compress_on);
		if (tag_vector.size() > 0)
		{
			vector <string> tags_data_vector;
			ShaderTags tags;

			for (int i = 0; i < tag_vector.size(); i++)
			{
				int tags_key_word_index = 0;
				std::string current_line_string = tag_vector[i];

				//  Content RenderPass
				string::size_type render_pass_Idx = tag_vector[i].find(tags_key_word[tags_key_word_index++]);
				if (render_pass_Idx != string::npos)
				{
					//  Split ("=")
					tags_data_vector.clear();
					split(tags_data_vector, current_line_string, is_any_of("="), token_compress_on);
					if (tags_data_vector.size() > 0)
					{
						if (equals(trim_copy(tags_data_vector[0]), "RenderPass"))
							tags.RenderPass = trim_copy(tags_data_vector[1]);
					}
				}

				//  Content RenderType
				string::size_type render_type_Idx = tag_vector[i].find(tags_key_word[tags_key_word_index]);
				if (render_type_Idx != string::npos)
				{
					//  Split ("=")
					tags_data_vector.clear();
					split(tags_data_vector, current_line_string, is_any_of("="), token_compress_on);
					if (tags_data_vector.size() > 0)
					{
						if (equals(trim_copy(tags_data_vector[0]), "RenderType"))
							tags.RenderType = trim_copy(tags_data_vector[1]);
					}
				}
			}

			shader->set_tag(tags);
		}
	}
}

void analyse_shader::analyse_raster_mode_part(std::string raster_mode_string, std::unique_ptr<Shader>& shader, int index)
{
	//  Judge Content RasterizerMode
	string::size_type stringIdx = raster_mode_string.find(shader_key_word[index]);
	//  Content "RasterizerMode"
	if (stringIdx != string::npos) {
		//  Get{} String
		raster_mode_string = character_segmentation(raster_mode_string);

		vector <string> raster_mode_vector;
		//  Split (';')
		split(raster_mode_vector, raster_mode_string, is_any_of(";"), token_compress_on);
		if (raster_mode_vector.size() > 0)
		{
			vector <string> raster_mode_data_vector;
			ShaderRasterizerMode rasterizer_mode;

			for (int i = 0; i < raster_mode_vector.size(); i++)
			{
				int rasterizer_mode_key_word_index = 0;
				std::string current_line_string = raster_mode_vector[i];

				//  Content FillMode
				string::size_type fill_mode_Idx = raster_mode_vector[i].find(rasterizer_mode_key_word[rasterizer_mode_key_word_index++]);
				if (fill_mode_Idx != string::npos)
				{
					//  Split ("=")
					raster_mode_data_vector.clear();
					split(raster_mode_data_vector, current_line_string, is_any_of("="), token_compress_on);
					if (raster_mode_data_vector.size() > 0)
					{
						if (equals(trim_copy(raster_mode_data_vector[0]), "FillMode"))
						{
							if (equals(trim_copy(raster_mode_data_vector[1]), "D3D12_FILL_MODE_WIREFRAME"))
								rasterizer_mode.FILL_MODE = D3D12_FILL_MODE_WIREFRAME;
							else
								rasterizer_mode.FILL_MODE = D3D12_FILL_MODE_SOLID;
						}
					}
				}

				//  Content CullMode
				string::size_type cull_mode_Idx = raster_mode_vector[i].find(rasterizer_mode_key_word[rasterizer_mode_key_word_index]);
				if (cull_mode_Idx != string::npos)
				{
					//  Split ("=")
					raster_mode_data_vector.clear();
					split(raster_mode_data_vector, current_line_string, is_any_of("="), token_compress_on);
					if (raster_mode_data_vector.size() > 0)
					{
						if (equals(trim_copy(raster_mode_data_vector[0]), "CullMode"))
						{
							if (equals(trim_copy(raster_mode_data_vector[1]), "D3D12_CULL_MODE_NONE"))
								rasterizer_mode.CULL_MODE = D3D12_CULL_MODE_NONE;
							else if (equals(trim_copy(raster_mode_data_vector[1]), "D3D12_CULL_MODE_FRONT"))
								rasterizer_mode.CULL_MODE = D3D12_CULL_MODE_FRONT;
							else
								rasterizer_mode.CULL_MODE = D3D12_CULL_MODE_BACK;
						}
					}
				}
			}

			shader->set_rasterizer_mode(rasterizer_mode);
		}
	}
}

void analyse_shader::analyse_blend_mode_part(std::string blend_mode_string, std::unique_ptr<Shader>& shader, int index)
{
	//  Judge Content BlendMode
	string::size_type stringIdx = blend_mode_string.find(shader_key_word[index]);
	//  Content "BlendMode"
	if (stringIdx != string::npos) {
		//  Get{} String
		blend_mode_string = character_segmentation(blend_mode_string);

		vector <string> blend_mode_vector;
		//  Split (';')
		split(blend_mode_vector, blend_mode_string, is_any_of(";"), token_compress_on);
		if (blend_mode_vector.size() > 0)
		{
			vector <string> blend_mode_data_vector;
			ShaderBlendMode blend_mode;

			for (int i = 0; i < blend_mode_vector.size(); i++)
			{
				int blend_mode_key_word_index = 0;
				std::string current_line_string = blend_mode_vector[i];

				//  Content BlendEnable
				string::size_type blend_enable_Idx = blend_mode_vector[i].find(blend_mode_key_word[blend_mode_key_word_index++]);
				if (blend_enable_Idx != string::npos)
				{
					//  Split ("=")
					blend_mode_data_vector.clear();
					split(blend_mode_data_vector, current_line_string, is_any_of("="), token_compress_on);
					if (blend_mode_data_vector.size() > 0)
					{
						if (equals(trim_copy(blend_mode_data_vector[0]), "BlendEnable"))
						{
							if (equals(trim_copy(blend_mode_data_vector[1]), "true"))
								blend_mode.BlendEnable = true;
							else
								blend_mode.BlendEnable = false;
						}
					}
				}

				//  Content LogicOpEnable
				string::size_type logic_op_enable_Idx = blend_mode_vector[i].find(blend_mode_key_word[blend_mode_key_word_index++]);
				if (logic_op_enable_Idx != string::npos)
				{
					//  Split ("=")
					blend_mode_data_vector.clear();
					split(blend_mode_data_vector, current_line_string, is_any_of("="), token_compress_on);
					if (blend_mode_data_vector.size() > 0)
					{
						if (equals(trim_copy(blend_mode_data_vector[0]), "LogicOpEnable"))
						{
							if (equals(trim_copy(blend_mode_data_vector[1]), "true"))
								blend_mode.LogicOpEnable = true;
							else
								blend_mode.LogicOpEnable = false;
						}
					}
				}

				//  Content SrcBlend
				string::size_type src_blend_Idx = blend_mode_vector[i].find(blend_mode_key_word[blend_mode_key_word_index++]);
				if (src_blend_Idx != string::npos)
				{
					//  Split ("=")
					blend_mode_data_vector.clear();
					split(blend_mode_data_vector, current_line_string, is_any_of("="), token_compress_on);
					if (blend_mode_data_vector.size() > 0)
						set_d3d12_blend(blend_mode.SrcBlend, "SrcBlend", blend_mode_data_vector[0], blend_mode_data_vector[1]);
				}

				//  Content DestBlend
				string::size_type dest_blend_Idx = blend_mode_vector[i].find(blend_mode_key_word[blend_mode_key_word_index++]);
				if (dest_blend_Idx != string::npos)
				{
					//  Split ("=")
					blend_mode_data_vector.clear();
					split(blend_mode_data_vector, current_line_string, is_any_of("="), token_compress_on);
					if (blend_mode_data_vector.size() > 0)
						set_d3d12_blend(blend_mode.DestBlend, "DestBlend", blend_mode_data_vector[0], blend_mode_data_vector[1]);
				}

				//  Content BlendOp
				string::size_type blend_op_Idx = blend_mode_vector[i].find(blend_mode_key_word[blend_mode_key_word_index++]);
				if (blend_op_Idx != string::npos)
				{
					//  Split ("=")
					blend_mode_data_vector.clear();
					split(blend_mode_data_vector, current_line_string, is_any_of("="), token_compress_on);
					if (blend_mode_data_vector.size() > 0)
						set_d3d12_blend_op(blend_mode.BlendOp, "BlendOp", blend_mode_data_vector[0], blend_mode_data_vector[1]);
				}

				//  Content SrcBlendAlpha
				string::size_type src_blend_alpha_Idx = blend_mode_vector[i].find(blend_mode_key_word[blend_mode_key_word_index++]);
				if (src_blend_alpha_Idx != string::npos)
				{
					//  Split ("=")
					blend_mode_data_vector.clear();
					split(blend_mode_data_vector, current_line_string, is_any_of("="), token_compress_on);
					if (blend_mode_data_vector.size() > 0)
						set_d3d12_blend(blend_mode.SrcBlendAlpha, "SrcBlendAlpha", blend_mode_data_vector[0], blend_mode_data_vector[1]);
				}

				//  Content DestBlendAlpha
				string::size_type dest_blend_alpha_Idx = blend_mode_vector[i].find(blend_mode_key_word[blend_mode_key_word_index++]);
				if (dest_blend_alpha_Idx != string::npos)
				{
					//  Split ("=")
					blend_mode_data_vector.clear();
					split(blend_mode_data_vector, current_line_string, is_any_of("="), token_compress_on);
					if (blend_mode_data_vector.size() > 0)
						set_d3d12_blend(blend_mode.DestBlendAlpha, "DestBlendAlpha", blend_mode_data_vector[0], blend_mode_data_vector[1]);
				}

				//  Content BlendOpAlpha
				string::size_type blend_op_alpha_Idx = blend_mode_vector[i].find(blend_mode_key_word[blend_mode_key_word_index++]);
				if (blend_op_alpha_Idx != string::npos)
				{
					//  Split ("=")
					blend_mode_data_vector.clear();
					split(blend_mode_data_vector, current_line_string, is_any_of("="), token_compress_on);
					if (blend_mode_data_vector.size() > 0)
						set_d3d12_blend_op(blend_mode.BlendOpAlpha, "BlendOpAlpha", blend_mode_data_vector[0], blend_mode_data_vector[1]);
				}

				//  Content LogicOp
				string::size_type blend_logic_op_Idx = blend_mode_vector[i].find(blend_mode_key_word[blend_mode_key_word_index++]);
				if (blend_logic_op_Idx != string::npos)
				{
					//  Split ("=")
					blend_mode_data_vector.clear();
					split(blend_mode_data_vector, current_line_string, is_any_of("="), token_compress_on);
					if (blend_mode_data_vector.size() > 0)
						set_d3d12_logic_op(blend_mode.LogicOp, "LogicOp", blend_mode_data_vector[0], blend_mode_data_vector[1]);
				}

				//  Content RenderTargetWriteMask
				string::size_type render_target_write_mask_Idx = blend_mode_vector[i].find(blend_mode_key_word[blend_mode_key_word_index]);
				if (render_target_write_mask_Idx != string::npos)
				{
					//  Split ("=")
					blend_mode_data_vector.clear();
					split(blend_mode_data_vector, current_line_string, is_any_of("="), token_compress_on);
					if (blend_mode_data_vector.size() > 0)
						set_d3d12_color_write_enable(blend_mode.RenderTargetWriteMask, "RenderTargetWriteMask", blend_mode_data_vector[0], blend_mode_data_vector[1]);
				}
			}
			shader->set_blend_mode(blend_mode);
		}
	}
}

void analyse_shader::analyse_depth_stencil_mode_part(std::string depth_stencil_mode_string, std::unique_ptr<Shader>& shader, int index)
{
	//  Judge Content DepthStencilMode
	string::size_type stringIdx = depth_stencil_mode_string.find(shader_key_word[index]);
	//  Content "DepthStencilMode"
	if (stringIdx != string::npos) {
		//  Get{} String
		depth_stencil_mode_string = character_segmentation(depth_stencil_mode_string);

		vector <string> depth_stencil_vector;
		//  Split (';')
		split(depth_stencil_vector, depth_stencil_mode_string, is_any_of(";"), token_compress_on);
		if (depth_stencil_vector.size() > 0)
		{
			vector <string> depth_stencil_data_vector;
			ShaderDepthStencilMode depth_stencil_mode;

			for (int i = 0; i < depth_stencil_vector.size(); i++)
			{
				int depth_stencil_mode_key_word_index = 0;
				std::string current_line_string = depth_stencil_vector[i];

				//  Content DepthEnable
				string::size_type depth_enable_Idx = depth_stencil_vector[i].find(depth_stencil_mode_key_word[depth_stencil_mode_key_word_index++]);
				if (depth_enable_Idx != string::npos)
				{
					//  Split ("=")
					depth_stencil_data_vector.clear();
					split(depth_stencil_data_vector, current_line_string, is_any_of("="), token_compress_on);
					if (depth_stencil_data_vector.size() > 0)
					{
						if (equals(trim_copy(depth_stencil_data_vector[0]), "DepthEnable"))
						{
							if (equals(trim_copy(depth_stencil_data_vector[1]), "true"))
								depth_stencil_mode.DepthEnable = true;
							else
								depth_stencil_mode.DepthEnable = false;
						}
					}
				}

				//  Content DepthWriteMask
				string::size_type depth_write_mask_Idx = depth_stencil_vector[i].find(depth_stencil_mode_key_word[depth_stencil_mode_key_word_index++]);
				if (depth_write_mask_Idx != string::npos)
				{
					//  Split ("=")
					depth_stencil_data_vector.clear();
					split(depth_stencil_data_vector, current_line_string, is_any_of("="), token_compress_on);
					if (depth_stencil_data_vector.size() > 0)
					{
						if (equals(trim_copy(depth_stencil_data_vector[0]), "DepthWriteMask"))
						{
							if (equals(trim_copy(depth_stencil_data_vector[1]), "D3D12_DEPTH_WRITE_MASK_ZERO"))
								depth_stencil_mode.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
							else
								depth_stencil_mode.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
						}
					}
				}

				//  Content DepthWriteMask
				string::size_type comparison_func_Idx = depth_stencil_vector[i].find(depth_stencil_mode_key_word[depth_stencil_mode_key_word_index]);
				if (comparison_func_Idx != string::npos)
				{
					//  Split ("=")
					depth_stencil_data_vector.clear();
					split(depth_stencil_data_vector, current_line_string, is_any_of("="), token_compress_on);
					if (depth_stencil_data_vector.size() > 0)
						set_d3d12_comparison_func(depth_stencil_mode.DepthFunc, "DepthFunc", depth_stencil_data_vector[0], depth_stencil_data_vector[1]);
				}
			}

			shader->set_depth_stencil_mode(depth_stencil_mode);
		}
	}
}