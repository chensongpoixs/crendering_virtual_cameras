/***********************************************************************************************
created: 		2023-11-13

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



#include "cutil.h"
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sstream>

namespace chen 
{



	char* load_file_context(const char* url)
	{
		char* fileContext = NULL;
		FILE* fp = ::fopen(url, "rb");
		if (fp)
		{
			fseek(fp, 0, SEEK_END);
			int len = ftell(fp);
			if (len > 0)
			{
				rewind(fp);
				fileContext = new char[len + 1];
				fread(fileContext, 1, len, fp);
				fileContext[len] = 0;
			}
		}
		return fileContext;
	}
	void check_error()
	{
		GLenum err(glGetError());
		while (err != GL_NO_ERROR)
		{
			printf("[OpenGL error = %u]\n", err);
			err = glGetError();
		}
	}
	GLuint   CreateGpuProgram(const char* vs, const char* fs)
	{
		/*GLuint vsShader = CompileShader(GL_VERTEX_SHADER, vs);
		GLuint fsShader = CompileShader(GL_FRAGMENT_SHADER, fs);*/
		GLuint vsShader = Compile_sharder(GL_VERTEX_SHADER, vs);
		GLuint fsShader = Compile_sharder(GL_FRAGMENT_SHADER, fs);
		//GLboolean vssh = glIsShader(vsShader);
		//GLboolean fssh = glIsShader(fsShader);
		check_error();
		// 创建shader 程序
		GLuint shaderProgram = glCreateProgram();
		check_error();
		// 着色器附加到着色器程序上面去
		//glGetAttachedShaders(shaderProgram, 5)
		glAttachShader(shaderProgram, vsShader);
		//glLinkProgram(shaderProgram);
		//glDetachShader(shaderProgram, vsShader);
		//glDeleteShader(vsShader);
		check_error();
		//GL_INVALID_OPERATION
		glAttachShader(shaderProgram, fsShader);
		check_error();
		/*GLenum err = glGetError();
		printf("GL_ATTACHED_SHADERS[err = %u]\n", err);*/
		GLint success = GL_TRUE;
		check_error();

		//assert(!glGetError());
		//链接一下着色器程序
		glLinkProgram(shaderProgram);

		check_error();
		//获取编译的状态
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		check_error();
		if (!success)
		{
			char infolog[1024] = { 0 };
			GLsizei loglen = 0;
			//获取错误日志
			glGetProgramInfoLog(shaderProgram, sizeof(infolog), &loglen, infolog);
			printf("[ERROR] Compile   error %s\n", infolog);

			// 编译出错删除shader
			glDeleteProgram(shaderProgram);
		}
		check_error();
		printf("LIne sharder program success Compile Success !!!\n");
		// //删除着色器

		glDetachShader(shaderProgram, fsShader);
		check_error();
		glDetachShader(shaderProgram, vsShader);
		check_error();
		glDeleteShader(fsShader);
		check_error();
		glDeleteShader(vsShader);
		check_error();
		return shaderProgram;
	}
	GLint  Compile_sharder(GLenum shaderType, const char* url)
	{
		char* shaderCode = load_file_context(url);
		/*const char* shaderTypeStr = reinterpret_cast<const char *>("Vertex Shader");
		if (GL_FRAGMENT_SHADER == shaderType)
		{
			shaderTypeStr = reinterpret_cast<const char*>("Fragement Shader");
		}*/
		//创建着色器
		GLuint shader = glCreateShader(shaderType);
		if (!shader)
		{
			throw;
		}
		//GLint size = strlen((const char*)(shaderCode));
		//获取着色器的代码
		glShaderSource(shader, 1, &shaderCode, NULL);
		// 编译着色器
		glCompileShader(shader);

		GLint success = GL_TRUE;

		//获取编译的状态
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			char infolog[1024] = { 0 };
			GLsizei loglen = 0;
			//获取错误日志
			glGetShaderInfoLog(shader, sizeof(infolog), &loglen, infolog);
			printf("[ERROR] Compile  error %s\n", /*shaderTypeStr,*/ infolog);

			// 编译出错删除shader
			glDeleteShader(shader);
		}
		printf("Compile Success !!!\n");
		if (shaderCode)
		{
			delete[]shaderCode;
			shaderCode = NULL;
		}
		return shader;
	}


	GLuint CreateGLBuffer(GLenum target, GLenum usage, GLsizeiptr size, const void* data)
	{
		GLuint BufferID;
		// 创建VBO
		glGenBuffers(
			1, //创建VBO的个数
			&BufferID
		);
		check_error();
		// 绑定一下 
		glBindBuffer(target, BufferID);
		//int size = sizeof(vertices) ;
		check_error();
		glBufferData(
			target, //缓冲类型
			size, //数据量的大小
			data, //顶点数据
			usage
		);
		/**
		GL_STATIC_DRAW ： 数据几乎不会改变
		GL_DYNAMIC_DRAW： 数据可能会发生改变
		GL_STREAM_DRAW: 每次绘制数据都会发生改变
		*/
		check_error();
		glBindBuffer(target, 0);
		return BufferID;
	}

	GLuint CreateGLTexture(GLenum target, int width, int height, GLint internalformat, GLint format, const void* data)
	{
		GLuint texId;
		// 创建2D物理
		glGenTextures(1, &texId);
		glBindTexture(target, texId);
		// 设置环绕方式
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// 设置过lu方式
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(
			target, //纹理类型
			0, // 多级 纹理级别
			internalformat, // 纹理格式
			width,
			 height ,
			0,
			format, //数据格式
			GL_UNSIGNED_BYTE,
			data //图片的rgb数据 
		);
		glGenerateMipmap(target);
		glBindTexture(target, 0);
		return texId;
	}

	


	Mesh* LoadObjModel(const char* url, bool isDropRepeat)
	{
		printf("====================\n");
		char* fileContext = load_file_context(url);
		if (!fileContext)
			return NULL;

		std::vector<VertexAttri> positions, normals, texcoords;
		std::vector<VertexIndex> vertices;
		std::vector<uint32_t> indices;
		std::stringstream strStream(fileContext);
		delete fileContext;

		std::string temp;
		while (!strStream.eof())
		{
			const int MAX_SIZE = 256;
			char lineStr[MAX_SIZE];
			strStream.getline(lineStr, MAX_SIZE);

			std::stringstream lineStream(lineStr);
			if (lineStr[0] == 'v')
			{
				if (lineStr[1] == 't')
				{
					lineStream >> temp;
					VertexAttri attri;
					lineStream >> attri.x;
					lineStream >> attri.y;
					lineStream >> attri.z;
					texcoords.push_back(attri);
				}
				else if (lineStr[1] == 'n')
				{
					lineStream >> temp;
					VertexAttri attri;
					lineStream >> attri.x;
					lineStream >> attri.y;
					lineStream >> attri.z;
					normals.push_back(attri);
				}
				else
				{
					lineStream >> temp;
					VertexAttri attri;
					lineStream >> attri.x;
					lineStream >> attri.y;
					lineStream >> attri.z;
					positions.push_back(attri);
				}
			}
			else if (lineStr[0] == 'f')
			{
				lineStream >> temp;
				for (int i = 0; i < 3; i++)
				{
					lineStream >> temp;
					int pos1 = temp.find_first_of('/');
					std::string vStr = temp.substr(0, pos1);
					int pos2 = temp.find_first_of('/', pos1 + 1);
					std::string tStr = temp.substr(pos1 + 1, pos2 - pos1 - 1);
					std::string nStr = temp.substr(pos2 + 1, temp.length() - pos2 - 1);

					VertexIndex vi;
					//顶点索引信息，
					//顶点对应着顶点位置,法线和纹理坐标
					//该结构体记录着当前点所对应各自的属性容器索引值
					//posIndex --> positions 向量容器中的位置索引
					//texIndex --> texcoords 向量容器中的位置索引
					//norIndex --> normals	 向量容器中的位置索引
					vi.posIndex = atoi(vStr.c_str()) - 1;
					vi.texIndex = atoi(tStr.c_str()) - 1;
					vi.norIndex = atoi(nStr.c_str()) - 1;

					int index = -1;
					//重复检查
					if (isDropRepeat)
					{
						int currentVertexCount = vertices.size();
						for (int k = 0; k < currentVertexCount; k++)
						{
							if (vertices[k].posIndex == vi.posIndex &&
								vertices[k].norIndex == vi.norIndex &&
								vertices[k].texIndex == vi.texIndex)
							{
								index = k;
								break;
							}
						}
					}
					if (index == -1)
					{
						index = vertices.size();
						vertices.push_back(vi);
					}
					//顶点数据必须的
					indices.push_back(index);
				}
			}
		}
		Mesh* mesh = new Mesh();
		mesh->indexCount = indices.size();
		mesh->indices = new uint32_t[mesh->indexCount];
		memcpy(mesh->indices, &indices[0], mesh->indexCount * sizeof(uint32_t));

		mesh->vertexCount = vertices.size();
		mesh->vertices = new Vertex[mesh->vertexCount];
		for (int i = 0; i < mesh->vertexCount; i++)
		{
			//从当前顶点索引容器中取出相应的数据拷贝到Vertex结构体中的对应位置
			//memcpy(&mesh->vertices[i].position, &positions[vertices[i].posIndex], sizeof(float) * 3);
			memcpy(&mesh->vertices[i].position, &positions[vertices[i].posIndex], sizeof(float) * 3);
			memcpy(&mesh->vertices[i].normal, &normals[vertices[i].norIndex], sizeof(float) * 3);
			memcpy(&mesh->vertices[i].texcoord, &texcoords[vertices[i].texIndex], sizeof(float) * 2);
		}
		mesh->faceCount = mesh->indexCount / 3;
		return mesh;
		//char* fileContext = load_file_context(url);

		//if (!fileContext)
		//{
		//	printf("open url = %s fialed !!!\n", url);
		//	return NULL;
		//}
		//std::stringstream strStream(fileContext);
		//delete[] fileContext;
		//fileContext = NULL;
		//static const int MAX_SIZE = 256;
		//
		//std::string temp;
		//
		//std::vector< VertexAttri> positions;
		//std::vector< VertexAttri> normals;
		//std::vector< VertexAttri> texcoords;



		//std::vector<VertexIndex>  vertices;
		//std::vector<uint32_t> indices;
		//// pares obj model 
		//while (!strStream.eof())
		//{
		//	char lineStr[MAX_SIZE] = { 0 };
		//	//获取一行数据
		//	strStream.getline(lineStr, MAX_SIZE);

		//	std::stringstream lineStream(lineStr);
		//	if (lineStr[0] == 'v')
		//	{
		//		if (lineStr[1] == 't')
		//		{
		//			lineStream >> temp;
		//			VertexAttri attri;
		//			lineStream >> attri.x;
		//			lineStream >> attri.y;
		//			lineStream >> attri.z;
		//			texcoords.emplace_back(attri);
		//		}
		//		else if (lineStr[1] == 'n') //发线向量
		//		{
		//			lineStream >> temp;
		//			VertexAttri attri;
		//			lineStream >> attri.x;
		//			lineStream >> attri.y;
		//			lineStream >> attri.z;
		//			normals.emplace_back(attri);
		//		}
		//		else // 顶点坐标
		//		{
		//			lineStream >> temp;
		//			VertexAttri attri;
		//			lineStream >> attri.x;
		//			lineStream >> attri.y;
		//			lineStream >> attri.z;
		//			positions.emplace_back(attri);
		//		}
		//	}
		//	else if (lineStr[0] == 'f')
		//	{
		//		lineStream >> temp;
		//		for (int i = 0; i < 3; ++i)
		//		{
		//			lineStream >> temp;
		//			int pos1 = temp.find_first_of('/');
		//			std::string vStr = temp.substr(0, pos1);
		//			int pos2 = temp.find_first_of( '/', pos1 + 1);
		//			std::string tStr = temp.substr(pos1 + 1, pos2 - pos1 - 1);

		//			std::string nStr = temp.substr(pos2 + 1, temp.length() - pos2 - 1);
		//			VertexIndex vi;
		//			/// <summary>
		//			/// 顶点索引信息
		//			/// 顶点对应着顶点位置、法线和纹理坐标
		//			/// 该结构体记录着当前点所对应各自的属性容器索引值
		//			/// posIndex  --> positions 向量容器中的位置索引
		//			/// texIndex  --> texcoords 向量容器中的位置索引
		//			/// norIndex  --> normals   向量容器中的位置索引
		//			/// </summary>
		//			/// <param name="url"></param>
		//			/// <returns></returns>
		//			vi.posIndex = atoi(vStr.c_str()) - 1;
		//			vi.texIndex = atoi(tStr.c_str()) - 1;
		//			vi.norIndex = atoi(nStr.c_str()) - 1;

		//			int index = -1;
		//			// 重复检查
		//			if (isDropRepeat)
		//			{
		//				int currentVertexCount = vertices.size();
		//				for (int k = 0; k < currentVertexCount; ++k)
		//				{
		//					if (vertices[k].posIndex == vi.posIndex &&
		//						vertices[k].texIndex == vi.texIndex &&
		//						vertices[k].norIndex == vi.norIndex)
		//					{
		//						index = k;
		//						break;
		//					}
		//				}

		//			}
		//			if (index == -1)
		//			{
		//				index = vertices.size();
		//				vertices.push_back(vi);
		//			}
		//			indices.push_back(index);
		//		}
		//	}
		//}

		//Mesh* mesh = new Mesh();
		//if (!indices.empty())
		//{
		//	mesh->indexCount = indices.size();
		//	mesh->indices = new uint32_t[mesh->indexCount];
		//	memcpy(mesh->indices, &indices[0], mesh->indexCount * sizeof(uint32_t));
		//}
		//
		//mesh->vertexCount = vertices.size();
		//mesh->vertices = new Vertex[mesh->vertexCount];
		//for (int i = 0; i < mesh->vertexCount; ++i)
		//{
		//	//mesh->vertices[i].texcoord = vertices[i].texIndex;
		//	memcpy(&mesh->vertices[i].position, &positions[vertices[i].posIndex], sizeof(float) * 3);
		//	//memcpy(&mesh->vertices[i].position, &positions[vertices[i].posIndex], sizeof(float) * 3);
		//	memcpy(&mesh->vertices[i].texcoord, &normals[vertices[i].texIndex], sizeof(float) * 2);
		//	memcpy(&mesh->vertices[i].normal, &texcoords[vertices[i].norIndex], sizeof(float) * 3);
		//}

		//mesh->faceCount = mesh->indexCount / 3;
		return mesh;
	}

}