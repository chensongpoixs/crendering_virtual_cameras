/***********************************************************************************************
created: 		2023-11-17

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
#include "cmodel.h"


namespace chen {
	cmodel::cmodel(const char * url)
	{
		mesh = chen::LoadObjModel(url, false/*true*//*EBO 开启就不需要内存空间*/);
		VBO = chen::CreateGLBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, mesh->vertexCount * sizeof(struct chen::Vertex), mesh->vertices);
		EBO = chen::CreateGLBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, mesh->indexCount * sizeof(uint32_t), mesh->indices);
		position = QVector3D(0.0f, 0.0f, 0.0f);
		axis = QVector3D(0.0f, 1.0f, 0.0f);
		scale = QVector3D(1.0f, 1.0f, 1.0f);
	}
	cmodel::~cmodel()
	{
		if (mesh)
		{
			delete mesh->indices;
			delete mesh->vertices;
			delete mesh;
			mesh = NULL;
		}
	}


	 

 
	void cmodel::apply_shader(cshader1* shader)
	{
		this->shader = shader;
		this->shader->Apply();
	}
	void cmodel::set_texture(const char* locationName, ctexture* texture)
	{
		shader->SetTexture2D(locationName, texture);
	}
	void cmodel::set_position(float x, float y, float z)
	{
		position = QVector3D(x, y, z);
	}

	/**
	*
	*/
	void cmodel::set_rotation(float angle, float axisX, float axisY, float axisZ)
	{
		this->angle = angle;
		axis = QVector3D(axisX, axisY, axisZ);
	}

	void cmodel::set_scale(float x, float y, float z)
	{
		scale = QVector3D(x, y, z);
	}

	void cmodel::draw(const float* viewmat, const float* projmat)
	{
		//// 绑定一下 
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// shader --> 启用顶点属性
		glEnableVertexAttribArray(shader->posLocation);
		chen::check_error();
		// 告诉shader 顶点数据排列
		//GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer
		glVertexAttribPointer(
			shader->posLocation, // 顶点属性ID
			3, // 几个数据构成一组
			GL_FLOAT, // 数据类型
			GL_FALSE, // 
			sizeof(float) * 8, // 步长
			(const void*)(sizeof(float) * 0) // 偏移量,第一组数据的起始位置
		);
		chen::check_error();
		// shader --> 启用顶点属性
		glEnableVertexAttribArray(shader->colorLocation);
		chen::check_error();
		// 告诉shader 顶点数据排列
		//GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer
		glVertexAttribPointer(shader->colorLocation, 3,
			GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(sizeof(float) * 3));
		// shader --> 启用顶点属性
		glEnableVertexAttribArray(shader->texcoordLocation);
		chen::check_error();
		// 告诉shader 顶点数据排列
		//GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer
		glVertexAttribPointer(shader->texcoordLocation, 2,
			GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(sizeof(float) * 6));



		chen::check_error();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);


		// 模型矩阵
		QMatrix4x4 modelMat;
		// 视图矩阵
		//QMatrix4x4 viewMat;


		// 移动模型
		modelMat.translate(position);
		// 转动
		 modelMat.rotate(angle, axis);
		// 
		modelMat.scale(scale);

		// 眼睛的位置
		/*QVector3D eyePoint = QVector3D(0, 0, 0);
		viewMat.lookAt(eyePoint, eyePoint + QVector3D(0, 0, -1), QVector3D(0, 1, 0));*/


		//camera.SetRotation(0, 180, 0);

		//设置矩阵ｉＤ
		glUniformMatrix4fv(shader->modelLocation, 1, GL_FALSE, modelMat.constData());
		glUniformMatrix4fv(shader->viewLocation, 1, GL_FALSE, viewmat);
		glUniformMatrix4fv(shader->projLocation, 1, GL_FALSE, projmat);


		// 绑定EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);  //EBO 开启就不需要内存空间
		// EBO绘制的方式 
		glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, NULL);
	}
}