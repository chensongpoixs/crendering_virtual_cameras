/***********************************************************************************************
created: 		2023-11-18

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


#ifndef _C_VIDEO_CAPTURE_H_
#define _C_VIDEO_CAPTURE_H_
#include <Windows.h>
#include <stdint.h>
#include <GL/eglew.h>
#include <vector>
#include "ctexture.h" 
extern "C"
{
#include <libavutil/frame.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
}
namespace chen {

	enum PixelFormatType
	{
		PIX_FMT_AUTO = -1,

		PIX_FMT_YUV420P = 0,
		PIX_FMT_YUYV422 = 1,
		PIX_FMT_UYVY422 = 15,
		PIX_FMT_YUV422P = 4,
		PIX_FMT_YUV444P = 5,

		PIX_FMT_GRAY = 8,

		PIX_FMT_YUVJ420P = 12,
		PIX_FMT_YUVJ422P = 13,
		PIX_FMT_YUVJ444P = 14,

		PIX_FMT_NV12 = 23,
		PIX_FMT_NV21 = 24,

		PIX_FMT_RGB = 2,
		PIX_FMT_BGR = 3,

		PIX_FMT_ARGB = 25,
		PIX_FMT_RGBA = 26,
		PIX_FMT_ABGR = 27,
		PIX_FMT_BGRA = 28,
	};


	class cvideo_capture
	{
	public:
		explicit cvideo_capture()
			: is_opened(false)
			, formatType(PIX_FMT_AUTO)
			, width(0)
			, height(0)
			, video_stream_index(-1)
			, video_stream(NULL)
			, ic(NULL)
			, codec_ctx(NULL)
			, frame(NULL)
			, sws_frame(NULL)
			, sws_ctx(NULL)
		{
			::avformat_network_init();
		}
		virtual ~cvideo_capture();

	public:

	public:
		void destroy();
		/**
		 * @param url 文件路径
		* @param fmt 输出像素格式
		* @return 成功返回true 失败返回false
		*/
		bool open(const char* url, PixelFormatType fmt = PIX_FMT_AUTO);


		// 关闭并释放资源
		void close();

		/**
		* 读取一帧视频数据
		* @param out_frame 输出帧数据，原始像素格式
		* @return -1 错误, -2 没有打开、0：获取到结尾，1:获取成功，
		*/
		int grab_frame(AVFrame*& out_frame);

		/**
		* 读取一帧视频数据并转换到目标像素格式
		* @param out_frame 输出帧数据， 目标像素格式， open函数设置
		*/
		int retrieve(AVFrame*& out_frame);


		/**
		* seek到目标位置相近的关键帧
		* @param percentage 目标位置 ， 百分比[0 ~1)
		* @return 成功返回 true
		*/
		bool seek(double percentage);
	public:
	private:
		bool is_opened;
		PixelFormatType formatType ;
		int width;
		int height;

		// 视频流索引
		int  video_stream_index;
		// 视频流
		AVStream* video_stream;

		//接封装上下文
		AVFormatContext* ic;
		// 解码器上下文
		AVCodecContext* codec_ctx;
		
		AVFrame* frame;
		AVFrame* sws_frame;

		//像素格式转换上下文
		SwsContext* sws_ctx;

		

	};









}


#endif // _C_VIDEO_CAPTURE_H_