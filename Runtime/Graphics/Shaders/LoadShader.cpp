//
// Created by Administrator on 2022/12/6.
//

#include "LoadShader.h"

void get_shader_files(std::string path, std::vector<std::string>& files)
{
	//文件句柄
	intptr_t hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					get_shader_files(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else {
				std::string file_name_str = strlwr(fileinfo.name);

				//  包含shader
				std::string::size_type contant_fbx_idx = file_name_str.find(".shader");
				if (contant_fbx_idx != std::string::npos)
				{
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}