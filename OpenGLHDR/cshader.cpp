/***********************************************************************************************
created: 		2023-11-15

author:			chensong

purpose:		camera

输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
************************************************************************************************/


#include "cshader.h"
#include <string>
#include "cutil.h"
#include <sstream>
#include <istream>
#include <ostream>
#include <iostream>
#include <cstring>
namespace chen {

	// Reads a text file and outputs a string with everything in the text file
	  std::string get_file_contents(const char* filename)
	{
		std::ifstream in(filename, std::ios::binary);
		if (in)
		{
			std::string contents;
			in.seekg(0, std::ios::end);
			contents.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();
			return(contents);
		}
 		throw(errno);
	}


	cshader::cshader(const char* vertexFile, const char* fragmentFile)
	{
		// Read vertexFile and fragmentFile and store the strings
		std::string vertexCode = get_file_contents(vertexFile);
		std::string fragmentCode = get_file_contents(fragmentFile);

		// Convert the shader source strings into character arrays
		const char* vertexSource = vertexCode.c_str();
		const char* fragmentSource = fragmentCode.c_str();

		// Create Vertex Shader Object and get its reference
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		// Attach Vertex Shader source to the Vertex Shader Object
		glShaderSource(vertexShader, 1, &vertexSource, NULL);
		// Compile the Vertex Shader into machine code
		glCompileShader(vertexShader);
		// Checks if Shader compiled succesfully
		//_compile_errors(vertexShader, "VERTEX");

		// Create Fragment Shader Object and get its reference
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		// Attach Fragment Shader source to the Fragment Shader Object
		glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
		// Compile the Vertex Shader into machine code
		glCompileShader(fragmentShader);
		// Checks if Shader compiled succesfully
		//_compile_errors(fragmentShader, "FRAGMENT");

		// Create Shader Program Object and get its reference
		programID = glCreateProgram();
		// Attach the Vertex and Fragment Shaders to the Shader Program
		glAttachShader(programID, vertexShader);
		glAttachShader(programID, fragmentShader);
		// Wrap-up/Link all the shaders together into the Shader Program
		glLinkProgram(programID);
		// Checks if Shaders linked succesfully
		//_compile_errors(programID, "PROGRAM");
		glDetachShader(programID, vertexShader);
		glDetachShader(programID, fragmentShader);
		//glDetachShader(programID, geometryShader);
		// Delete the now useless Vertex and Fragment Shader objects
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
	cshader::cshader(const char* vertexFile, const char* fragmentFile, const char* geometryFile)
	{
		// Read vertexFile and fragmentFile and store the strings
		std::string vertexCode = get_file_contents(vertexFile);
		std::string fragmentCode = get_file_contents(fragmentFile);
		std::string geometryCode = get_file_contents(geometryFile);

		// Convert the shader source strings into character arrays
		const char* vertexSource = vertexCode.c_str();
		const char* fragmentSource = fragmentCode.c_str();
		const char* geometrySource = geometryCode.c_str();

		// Create Vertex Shader Object and get its reference
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		check_error();
		// Attach Vertex Shader source to the Vertex Shader Object
		glShaderSource(vertexShader, 1, &vertexSource, NULL);
		check_error();
		// Compile the Vertex Shader into machine code
		glCompileShader(vertexShader);
		check_error();
		// Checks if Shader compiled succesfully
		_compile_errors(vertexShader, "VERTEX");

		// Create Fragment Shader Object and get its reference
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		check_error();
		// Attach Fragment Shader source to the Fragment Shader Object
		glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
		check_error();
		// Compile the Fragment Shader into machine code
		glCompileShader(fragmentShader);
		// Checks if Shader compiled succesfully
		_compile_errors(fragmentShader, "FRAGMENT");

		// Create Geometry Shader Object and get its reference
		GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		check_error();
		// Attach Geometry Shader source to the Fragment Shader Object
		glShaderSource(geometryShader, 1, &geometrySource, NULL);
		check_error();
		// Compile the Geometry Shader into machine code
		glCompileShader(geometryShader);
		// Checks if Shader compiled succesfully
		//_compile_errors(geometryShader, "GEOMETRY");

		// Create Shader Program Object and get its reference
		programID = glCreateProgram();
		// Attach the Vertex and Fragment Shaders to the Shader Program
		glAttachShader(programID, vertexShader);
		glAttachShader(programID, fragmentShader);
		glAttachShader(programID, geometryShader);
		check_error();
		// Wrap-up/Link all the shaders together into the Shader Program
		glLinkProgram(programID);
		// Checks if Shaders linked succesfully
		//_compile_errors(programID, "PROGRAM");
		check_error();
		glDetachShader(programID, vertexShader);
		check_error();
		glDetachShader(programID, fragmentShader);
		check_error();
		glDetachShader(programID, geometryShader);
		check_error();
		// Delete the now useless Vertex and Fragment Shader objects
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glDeleteShader(geometryShader);
		check_error();
	}


	// Checks if the different Shaders have compiled properly
	void cshader::_compile_errors(unsigned int shader, const char* type)
	{
		// Stores status of compilation
		GLint hasCompiled;
		// Character array to store error message in
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
			if (hasCompiled == GL_FALSE)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
			if (hasCompiled == GL_FALSE)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << infoLog << std::endl;
			}
		}
	}
	cshader::~cshader()
	{
		glDeleteProgram(programID);
	}
	void cshader::activate()
	{
		glUseProgram(programID);
	}
}