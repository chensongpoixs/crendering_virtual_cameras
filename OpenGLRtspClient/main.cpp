#include "OpenGLRtspClient.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OpenGLRtspClient w;
    w.show();
    return a.exec();
}
//#include <QtCore/QCoreApplication>
//#include <QFile>
//#include <QAudioFormat>
//#include <QAudioOutput>
//#include <QDebug>
//
//#include <QtCore/QCoreApplication>
//#include <QFile>
//#include <QAudioFormat>
//#include <QAudioOutput>
//#include <QDebug>
//#include <QThread>
//
//#pragma execution_character_set("utf-8")
//
//int fffmain(int argc, char *argv[])
//{
//	QCoreApplication a(argc, argv);
//
//	QFile file("./chensong.pcm");
//	if (file.open(QIODevice::ReadOnly))
//	{
//		qDebug() << "打开成功！";
//		QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
//		qDebug() << info.supportedSampleTypes();   // 输出支持的样本类型列表。
//		qDebug() << info.supportedByteOrders();    // 输出支持的字节顺序列表
//		qDebug() << info.supportedCodecs();        // 输出可用编码器
//		qDebug() << info.supportedSampleRates();   // 输出支持的采样率
//
//		QAudioFormat fmt;
//		fmt.setSampleRate(44100);   // 设置采样率
//		fmt.setSampleSize(16);      // 设置样本大小
//		fmt.setChannelCount(2);     // 设置使用双通道
//		fmt.setCodec("audio/pcm");  // 设置编解码器
//		fmt.setByteOrder(QAudioFormat::LittleEndian);   // 使用小端
//		fmt.setSampleType(QAudioFormat::UnSignedInt);   // 使用无符号整数样本类型
//
//		if (!info.isFormatSupported(fmt))
//		{
//			qDebug() << "输出设备不支持该格式，不能播放音频";
//			return 0;
//		}
//
//		QAudioOutput* audio = new QAudioOutput(fmt, qApp);  // 注意：这里QAudioOutput必须使用指针，否则不能播放
//		QIODevice* io = audio->start();
//		int size = audio->periodSize();     // 这是每个周期防止缓冲区欠载和确保不间断播放所需的数据量。
//		QByteArray buf = file.readAll();    // 将需要播放的音频数据读到buf中
//		file.close();
//
//		while (!buf.isEmpty())
//		{
//			if (audio->bytesFree() < size)   // 音频缓冲区中可用的空闲字节数，判断缓冲区是否可写入数据。
//			{
//				QThread::msleep(1);
//				continue;
//			}
//
//			io->write(buf.mid(0, size));   // 写入需要播放的数据
//			buf.remove(0, size);
//			qDebug() << buf.count();
//		}
//
//		io->close();
//		delete io;
//		io = nullptr;
//	}
//
//	return a.exec();
//}