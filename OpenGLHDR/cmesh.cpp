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

#include "cmesh.h"
#include "cebo.h"
#include <QMatrix4x4>
#include <QMatrix>
namespace chen {


	cmesh::cmesh(std::vector <Vertex>  vertices, std::vector <GLuint>  indices, std::vector <ctexture>  textures)
	{
		cmesh::vertices = vertices;
		cmesh::indices = indices;
		cmesh::textures = textures;

		vao.bind();
		// Generates Vertex Buffer Object and links it to vertices
		cvbo VBO(vertices);
		// Generates Element Buffer Object and links it to indices
		cebo EBO(indices);
		// Links VBO attributes such as coordinates and colors to VAO
		vao.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
		vao.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
		vao.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
		vao.LinkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));
		// Unbind all to prevent accidentally modifying them
		vao.unbind();
		VBO.unbind();
		EBO.unbind();
	}

	cmesh::~cmesh()
	{
	}
	void cmesh::draw(cshader* shader, ccamera* camera)
	{
		// Bind shader to be able to access uniforms
		shader->activate();
		vao.bind();


		// Keep track of how many of each type of textures we have
		unsigned int numDiffuse = 0;
		unsigned int numSpecular = 0;

		for (unsigned int i = 0; i < textures.size(); i++)
		{
			std::string num;
			std::string type = textures[i].type;
			if (type == "diffuse")
			{
				num = std::to_string(numDiffuse++);
			}
			else if (type == "specular")
			{
				num = std::to_string(numSpecular++);
			}
			textures[i].texunit(shader, (type + num).c_str(), i);
			textures[i].bind();
		}

		// Take care of the camera Matrix
		glUniform3f(glGetUniformLocation(shader->programID, "camPos"),  camera->Position.x(), camera->Position.y(), camera->Position.z());
		camera->Matrix(shader, "camMatrix");
		 
			// Initialize matrices
		QMatrix4x4 trans(1.0f, 0.0f, 0.0f, 0.0f, 
						 0.0f, 1.0f, 0.0f, 0.0f,
						 0.0f, 0.0f, 1.0f, 0.0f,
						 0.0f, 0.0f, 0.0f, 1.0f); ;// = QMatrix(1.0f);

		
		//trans = 1.0f;
		//trans.setRow(0, 1.0f);
		QMatrix4x4 rot(1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);;// = QMatrix(1.0f);
		QMatrix4x4 sca(1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);;// = QMatrix(1.0f);
		QMatrix4x4 matrix = trans;


		 QVector3D translation(0.0f, 0.0f, 0.0f);
		trans.translate(translation);

		QQuaternion rotation(1.0f, 0.0f, 0.0f, 0.0f);
		//rot = 
		QVector3D scale(1.0f, 1.0f, 1.0f);
		sca.scale(scale);
		//QMatrix4x4::translate(trans, );

		// Push the matrices to the vertex shader
		glUniformMatrix4fv(glGetUniformLocation(shader->programID, "translation"), 1, GL_FALSE, &trans.constData()[0]);
		glUniformMatrix4fv(glGetUniformLocation(shader->programID, "rotation"), 1, GL_FALSE, &rot.constData()[0]);
		glUniformMatrix4fv(glGetUniformLocation(shader->programID, "scale"), 1, GL_FALSE, &sca.constData()[0]);
		glUniformMatrix4fv(glGetUniformLocation(shader->programID, "model"), 1, GL_FALSE, &matrix.constData()[0]);
		// Draw the actual mesh
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}
}