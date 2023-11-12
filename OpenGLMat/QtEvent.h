#ifndef _QT_EVENT_H_
#define _QT_EVENT_H_

#include <qevent.h>

class QtEvent : public QEvent
{
public:
	enum  Type
	{
		GL_Renderer = QEvent::User + 1,
	};
public:
	QtEvent(Type type) 
		: QEvent(QEvent::Type(type)){}
	//~QtEvent();

private:

};
 

#endif // QT_EVENT_H_