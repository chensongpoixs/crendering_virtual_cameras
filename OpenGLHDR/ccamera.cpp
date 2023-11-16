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
#include "ccamera.h"
#include <QKeyEvent>
namespace chen {
	ccamera::ccamera(int width, int height, QVector3D position)
	{
		this->width = width;
		this->height = height;
		Position = position;


		QVector3D Orientation1  (0.0f, 0.0f, -1.0f);
		Orientation = Orientation1;
		QVector3D Up1(0.0f, 1.0f, 0.0f);
		Up = Up1;
		QMatrix4x4 cameraMatrix1(1.0f, 0.0f, 0.0f, 0.0f,
								 0.0f, 1.0f, 0.0f, 0.0f,
								 0.0f, 0.0f, 1.0f, 0.0f,
								 0.0f, 0.0f, 0.0f, 1.0f);
		cameraMatrix = cameraMatrix1;
	}
	ccamera::~ccamera()
	{
	}
	void ccamera::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
	{
		// Initializes matrices since otherwise they will be the null matrix
		QMatrix4x4 view (1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
		QMatrix4x4 projection = view;// glm::mat4(1.0f);

		// Makes camera look in the right direction from the right position
		 /*view =glm::lookAt*/view.lookAt(Position, Position + Orientation, Up);
		// Adds perspective to the scene
		/*projection = glm::perspective*/ projection.perspective(/*std::radians(FOVdeg)*/FOVdeg * static_cast<float>(0.01745329251994329576923690768489), (float)width / height, nearPlane, farPlane);

		// Sets new camera matrix
		cameraMatrix = projection * view;
	}
	void ccamera::Matrix(cshader* shader, const char* uniform)
	{
		// Exports camera matrix
		glUniformMatrix4fv(glGetUniformLocation(shader->programID, uniform), 1, GL_FALSE,  cameraMatrix.constData());
	}

	void ccamera::Inputs(QKeyEvent* event)
	{
		// Handles key inputs
		if (event->key() == Qt::Key_W && event->type() == QEvent::KeyPress  )
		{
			Position += speed * Orientation;
		}
		if (event->key() == Qt::Key_A && event->type() == QEvent::KeyPress)
		{
			Position += speed *  -Orientation.crossProduct(Orientation, Up).normalized();
			//Position += speed * -glm::normalize(glm::cross(Orientation, Up));
		}
		if (event->key() == Qt::Key_S && event->type() == QEvent::KeyPress)
		{
			Position += speed * -Orientation;
		}
		if (event->key() == Qt::Key_D && event->type() == QEvent::KeyPress)
		{
			Position += speed * Orientation.crossProduct(Orientation, Up).normalized(); 
			//glm::normalize(glm::cross(Orientation, Up));
		}
		if (event->key() == Qt::Key_Space && event->type() == QEvent::KeyPress
			/*glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS*/)
		{
			Position += speed * Up;
		}
		if (event->key() == Qt::Key_Control && event->type() == QEvent::KeyPress
			/*glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS*/)
		{
			Position += speed * -Up;
		}
		if (event->key() == Qt::Key_Shift && event->type() == QEvent::KeyPress
			/*glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS*/)
		{
			speed = 0.4f;
		}
		else if (event->key() == Qt::Key_Shift && event->type() == QEvent::KeyRelease
			/*glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE*/)
		{
			speed = 0.1f;
		}


		
	}

	void ccamera::Inputs(QMouseEvent* event)
	{
		// Handles mouse inputs
		if (event->button() == Qt::LeftButton/*Qt::Key_Shift*/ && event->type() == QEvent::MouseButtonPress
			/*glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS*/)
		{
			// Hides mouse cursor
		//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

			// Prevents camera from jumping on the first click
			if (firstClick)
			{
				//glfwSetCursorPos(window, (width / 2), (height / 2));
				firstClick = false;
			}

			// Stores the coordinates of the cursor
			double mouseX;
			double mouseY;
			// Fetches the coordinates of the cursor
			//glfwGetCursorPos(window, &mouseX, &mouseY);

			// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
			// and then "transforms" them into degrees 
			float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
			float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

			// Calculates upcoming vertical change in the Orientation
			//glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));
//
			// Decides whether or not the next vertical Orientation is legal or not
			//if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
			{
			//	Orientation = newOrientation;
			}

			// Rotates the Orientation left and right
			//Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

			// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
			//glfwSetCursorPos(window, (width / 2), (height / 2));
		}
		else if (
			event->button() == Qt::LeftButton/*Qt::Key_Shift*/ && event->type() == QEvent::MouseButtonPress
			/*glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE*/)
		{
			// Unhides cursor since camera is not looking around anymore
			//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			// Makes sure the next time the camera looks around it doesn't jump
			firstClick = true;
		}
	}
}