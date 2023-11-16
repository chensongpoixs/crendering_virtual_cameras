#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glm/glm.hpp>
//#include<glad/glad.h>
#include <gl/glew.h>
#include<vector>
#include <QVector3D>
// Structure to standardize the vertices used in the meshes
struct Vertex
{
	QVector3D position;
	QVector3D normal;
	QVector3D color;
	QVector2D texUV;
};



class VBO
{
public:
	// Reference ID of the Vertex Buffer Object
	GLuint ID;
	// Constructor that generates a Vertex Buffer Object and links it to vertices
	VBO(std::vector<Vertex>& vertices);

	// Binds the VBO
	void Bind();
	// Unbinds the VBO
	void Unbind();
	// Deletes the VBO
	void Delete();
};

#endif