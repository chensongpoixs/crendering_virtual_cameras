#version 330 core

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inTexCoords;

out vec2 texCoords;
//varying vec2 texCoords;
void main()
{
    gl_Position = vec4(inPos.x, inPos.y  , 0.0, 1.0); 
    //inTexCoords.y *= -1;
	texCoords = inTexCoords;
	//texCoords.y *= -1;

	// 旋转图片
	//texCoords.y *= -1;
}  