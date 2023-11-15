# crendering_virtual_cameras


OpenGL基础操作 渲染虚拟相机  


[1、VBO](OpenGLVBO)

[2、EBO 和顶点颜色 使用 shader ](OpenGLEBO)

[3、OpenGL texcoord 纹理贴图](OpenGLTexcoord) 

[4、模型矩阵、视图矩阵、投影矩阵的坐标系的转换](OpenGLMat)

[5、 虚拟相机 ](OpenGLVirtualCameras)

[6、 场景漫游 ](OpenGLSceneRoaming)

[7、 解析3D Object 模型 ](OpenGLParse3DObjectModel)


# 二、OpenGL概念

1、 什么是VBO

VBO全名顶点缓冲对象（Vertex Buffer Object）,他主要的作用就是可以一次性的发送一大批顶点数据到显卡上，而不是每个顶点发送一次。我们知道CPU传送数据给GPU其实是比较耗费时间的，所以尽可能的一次性把需要的顶点数据全部传给GPU，这样顶点着色器几乎能立即访问到顶点，有助于加快顶点着色器效率。
 
如何创建VBO

就目前所有游戏引擎来说VBO的机制已经是基础了，我们看下VBO在Opengl中是如何创建的。
首先，我们需要在Opengl生成一个缓冲类型的ID。
 
```
unsigned int VBO;
glGenBuffers(1, &VBO);

```

 这里我们创建了一个缓冲的ID，当然了你也可以通过数组来批量创建一系列的VBO的ID，
 
```
unsigned int VBO[3];
glGenBuffers(3,VBO);
```


接下来，我们要将这个ID绑定给指定类型来告诉Opengl这个缓冲是什么类型的；

```
glBindBuffer(GL_ARRAY_BUFFER, VBO);
```


这里我们将VBO赋予了GL_ARRAY_BUFFER的类型，告诉Opengl这个VBO变量是一个顶点缓冲对象。

注意从这一刻起，我们使用的任何（在GL_ARRAY_BUFFER目标上的）缓冲调用都会用来配置当前绑定的缓冲(VBO)

至此对于VBO的声明就结束了，其实也是蛮简单的，我们回顾下：

1）生成一个缓冲类型的ID；

2）指定ID的缓冲类型为GL_ARRAY_BUFFER；

接下来我们看下，如何给VBO赋值，我们假定有如下三角形的顶点数据：

```
float vertices[] = {
		// 位置              // 颜色
		0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
		0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
	};
```

Opengl通过glBufferData接口来复制顶点数据到缓冲中供Opengl使用，对于上面的三角形顶点数据，代码就可能是这样的：

glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
 
第一个参数表示目标的缓冲类型，这里指当前绑定到GL_ARRAY_BUFFER上的顶点缓冲对象，第二个参数表示数据大小（字节为单位），第三个参数表示我们实际发出的数据。第四个参数GL_STATIC_DRAW表示Opengl如何处理上传的数据，Opengl中一共有三种类型：

GL_STATIC_DRAW ：数据不会或几乎不会改变。
GL_DYNAMIC_DRAW：数据会被改变很多。
GL_STREAM_DRAW ：数据每次绘制时都会改变。
到这一步，我们的数据就已经上传到GPU上去了。一个VBO过程也就结束了。
那这里引申出一个问题，我们看到vertices中有注释，表明这个数组里面有两种数据，一种是顶点坐标，一种是顶点颜色，那都放在一个float的数组中，Opengl如何来区分他们呢？这里就要用到glVertexAttribPointer这个接口了，该接口就是帮助Opengl解释如何处理数据的。我们来看下对于vertices的数据处理的代码：

```
	// 位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 颜色属性
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
```


我们分析下这段代码，首先我们来解释下glVertexAttribPointer的各个参数的意义。

第一个参数表示我们希望数据中哪部分数据放在对应的Location位置上，这个可能主要体现在shader部分，我们看下顶点着色器的代码：

```
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
```

我们根据变量名大致能猜出来location为0的是顶点坐标，location为1的是颜色值，这里的location就是对应了上面代码种glVertexAttribPointer第一个参数，location没有准确的位置定义，并没有说location为0的一定要是顶点坐标属性，也可以是颜色或者uv坐标属性，只是常规来说，我们习惯将坐标放在第一个位置。

glVertexAttribPointer第二个参数表示属性大小，坐标和颜色的大小都是3，所以这里填3；

第三个参数表示数据类型；

第四个参数表示我们是否希望数据标准化。如果我们设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间。我们把它设置为GL_FALSE；

第五个参数表示步长，数据之间的间隔，我们这里不管坐标还是颜色都是3个分量，所以坐标和坐标数据之间隔了6个float字节，颜色和颜色之间隔了6个float字节；

第六个参数表示偏移量，即在一段数据中，指定的数据偏移多少位置开始。在这里，坐标数据都是每段数据的起始位置，所以偏移量是0，而颜色数据在坐标数据之后，坐标数据有3个分量，所以每个颜色数据偏移三个float字节开始算；

每次设定好一个location的值之后，记得要开启对应的位置数据glEnableVertexAttribArray，因为Opengl默认是全关闭的。

经过glVertexAttribPointer执行之后，shader中对应layout的位置数据就有对应的值了。
 
 
2、什么VAO

VAO全名顶点数组对象(Vertex Array Object)，每当我们绘制一个物体的时候都必须重复这一过程。这看起来可能不多，但是如果有超过5个顶点属性，上百个不同物体呢（这其实并不罕见）。绑定正确的缓冲对象，为每个物体配置所有顶点属性很快就变成一件麻烦事。VAO的作用就是顶点属性调用都会储存在这个VAO中。这样的好处就是，当配置顶点属性指针时，你只需要将那些调用执行一次，之后再绘制物体的时候只需要绑定相应的VAO就行了。简单来说有点类似复用的概念。
 
如何执行VAO

VAO的生成和绑定都和VBO很类似；

```
unsigned int VAO;
glGenVertexArrays(1, &VAO);
glBindVertexArray(VAO);
```

其过程也是先从生成一个ID开始，然后将ID绑定到顶点数组对象上。任何随后的顶点属性调用都会储存在这个VAO中。这样的好处就是，当配置顶点属性指针时，你只需要将那些调用执行一次，之后再绘制物体的时候只需要绑定相应的VAO就行了。这使在不同顶点数据和属性配置之间切换变得非常简单，只需要绑定不同的VAO就行了。概括来说就是从绑定之后起，我们应该绑定和配置对应的VBO和属性指针，之后解绑VAO供之后使用。

一个顶点数组对象会储存以下这些内容：

glEnableVertexAttribArray和glDisableVertexAttribArray的调用。

通过glVertexAttribPointer设置的顶点属性配置。

通过glVertexAttribPointer调用与顶点属性关联的顶点缓冲对象。

我们来举个例子，我们还是使用上面三角形数据：

```
float vertices[] = {
		// 位置              // 颜色
		0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
		0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
	};
```

借用GLM的数学库，我们生成2个位置向量：


```
glm::vec3 traPositions[] = {
				glm::vec3(0.0f,  0.0f,  0.0f),
				glm::vec3(0.5f,  0.5f, 0.5f),
			};
```

生成VBO和VAO的ID：

```
unsigned int VBO, VAO;
glGenVertexArrays(1, &VAO);
glBindVertexArray(VAO);
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
```

复制顶点数据到缓冲：

```
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
```

分别处理位置和颜色的数据：

```
// 位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 颜色属性
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//解除VAO绑定
	glBindVertexArray(0);
```

生成一个model的矩阵变量（这个model矩阵做用就是model->world的转变矩阵）

```
glm::mat4 model = glm::mat4(1.0f);
```

创建model是方便做测试，在世界坐标中绘制不同位置的三角形，其在顶点着色器的作用如下代码：

```
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

uniform mat4 model;

void main()
{
    gl_Position = model*vec4(aPos, 1.0);
    ourColor = aColor;
}
```


我们的片段着色器很简单，仅仅是把传入的颜色直接输出：


```
#version 330 core
out vec4 FragColor;

in vec3 ourColor;

void main()
{
    FragColor = vec4(ourColor, 1.0f);
}
```

好，至此准备工作都完成了，我们开始做渲染：

```
while (!glfwWindowShouldClose(window))
	{
		...
		glBindVertexArray(VAO);
		for (int i = 0; i < 2; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, traPositions[i]);
			outShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
		...
	}
```