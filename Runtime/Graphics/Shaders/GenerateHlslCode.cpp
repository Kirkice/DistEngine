//
// Created by Administrator on 2022/12/11.
//

#include "GenerateHlslCode.h"

//const std::string

const std::string space_string = "*                                                                                             *";
const std::string line_string = "*--------------------------------------------------------------------------------------------*";

std::string get_header_log(std::string name)
{
    std::string out_string = "/* $Header: Assets/Shaders/";
    out_string.append(name);
    out_string.append("                                   ");

    // 基于当前系统的当前日期/时间
    time_t now = time(0);
    tm* ltm = localtime(&now);
    out_string.append(to_string(ltm->tm_mon)).append("/");
    out_string.append(to_string(ltm->tm_mday)).append("/").append(to_string((ltm->tm_year - 100))).append("  ");
    out_string.append(to_string(ltm->tm_hour)).append(":").append(to_string(ltm->tm_min)).append(":").append(to_string(ltm->tm_sec));
    out_string.append(" KirkZhu $ */\n");
    out_string.append("/").append(line_string).append("\n");
    out_string.append(space_string).append("\n");
    out_string.append("*                 Project Name : DistEngine                                                   *\n");
    out_string.append(space_string).append("\n");
    out_string.append("*                    File Name : ").append(name).append(".hlsl                                                    *\n");
    out_string.append(space_string).append("\n");
    out_string.append("*                   Programmer : Kirk Zhu                                                     *\n");
    out_string.append(line_string).append("/\n");
    return out_string;
}

void generate_hlsl_code(std::vector<std::unique_ptr<Shader>>& shader_vector, int register_index)
{
    for (int a = 0; a < shader_vector.size(); a++)
    {
        std::unique_ptr<Shader>& shader = shader_vector[a];
        std::string out_put_code = "";
        out_put_code.append(get_header_log(shader->get_name()));
        out_put_code.append("\n");

        out_put_code.append("#ifndef DIST_").append(boost::to_upper_copy(shader->get_name())).append("_INCLUDE\n");
        out_put_code.append("#define DIST_").append(boost::to_upper_copy(shader->get_name())).append("_INCLUDE\n");
        out_put_code.append("#include \"Core.hlsl\"\n");
        out_put_code.append("\n");

        string struct_data_string = "";
        struct_data_string.append("struct ");
        struct_data_string.append(shader->get_name()).append("Data");
        struct_data_string.append("\n");
        struct_data_string.append("{\n");

        string texture_data_string = "";
        int texture_data_count = 0;
        for (int i = 0; i < shader->get_property_state_key_vector().size(); ++i)
        {
            std::string type_string = shader->get_property_state_map_value(shader->get_property_state_key_vector()[i]);

            //  Textures
            string::size_type TextureIdx = type_string.find("Texture");
            if (TextureIdx != string::npos)
                texture_data_count++;

            //  Color
            string::size_type ColorIdx = type_string.find("Color");
            if (ColorIdx != string::npos)
                struct_data_string.append("    ").append("float4").append("                    ").append(shader->get_property_state_key_vector()[i]).append(";\n");

            //  Int
            string::size_type IntIdx = type_string.find("Int");
            if (IntIdx != string::npos)
                struct_data_string.append("    ").append("int   ").append("                    ").append(shader->get_property_state_key_vector()[i]).append(";\n");

            //  Float
            string::size_type FloatIdx = type_string.find("Float");
            if (FloatIdx != string::npos)
                struct_data_string.append("    ").append("float ").append("                    ").append(shader->get_property_state_key_vector()[i]).append(";\n");
        }

        struct_data_string.append("};\n").append("\n").append("\n").append("\n").append("\n");

        texture_data_string.append("Texture2D ").append("g").append(shader->get_name()).append("Maps");
        texture_data_string.append("[").append(to_string(texture_data_count)).append("]").append("          ").append(": ");
        texture_data_string.append("register(t").append(to_string(register_index)).append(");\n");

        ofstream outfile;
        std::string file_name = "../shaders/";
        file_name.append(shader->get_name());
        file_name.append("_input.hlsl");
        outfile.open(file_name, ios::trunc);
        outfile << out_put_code.append(struct_data_string).append(texture_data_string).append("\n \n #endif");
        outfile.close();
    }
}