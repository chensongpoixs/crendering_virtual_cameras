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

#include "cvideo_capture.h"

#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "swscale")

namespace chen {

	static char g_errorbuffer[AV_ERROR_MAX_STRING_SIZE];
	char* ffmepgerror(int errCode)
	{
		return av_make_error_string(g_errorbuffer, AV_ERROR_MAX_STRING_SIZE, errCode);
	}


	cvideo_capture::~cvideo_capture()
	{
		close();
	}

	void cvideo_capture::destroy()
	{
		close();
	}

	bool cvideo_capture::open(const char* url, PixelFormatType fmt)
	{
		close();
		is_opened = false;

		formatType = fmt;

		// 1. 打开解封装上下文
		int ret = avformat_open_input(
			&ic, //解封装上下文
			url,  //文件路径
			NULL, //指定输入格式 h264,h265, 之类的， 传入NULL则自动检测
			NULL); //设置参数的字典
		if (ret != 0)
		{
			printf("%s\n", ffmepgerror(ret));
			return false;
		}
		//2.读取文件信息
		ret = avformat_find_stream_info(ic, NULL);
		if (ret < 0)
		{
			printf("%s\n", ffmepgerror(ret));
			return false;
		}
		//3.获取目标流索引
		for (unsigned int i = 0; i < ic->nb_streams; i++)
		{
			AVStream* stream = ic->streams[i];
			if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
			{
				if (!video_stream)
				{
					video_stream_index = i;
					video_stream = stream;
				}
				else
				{
					stream->discard = AVDISCARD_ALL;
				}
			}
			else
			{
				stream->discard = AVDISCARD_ALL;
			}
		}
		//4.查找解码器
		const AVCodec* codec = avcodec_find_decoder(video_stream->codecpar->codec_id);
		if (!codec)
		{
			printf("can't find codec, codec id:%d\n", video_stream->codecpar->codec_id);
			return false;
		}
		//5.创建解码器上下文
		if (!(codec_ctx = avcodec_alloc_context3(codec)))
		{
			printf("avcodec_alloc_context3 failed,\n");
			return false;
		}

		//6.从输入流复制编解码器参数到输出编解码器上下文
		if ((ret = avcodec_parameters_to_context(codec_ctx, video_stream->codecpar)) < 0) {
			printf("Failed to copy %s codec parameters to decoder context\n",
				av_get_media_type_string(video_stream->codecpar->codec_type));
			return false;
		}

		//7. 打开解码器上下文 */
		if ((ret = avcodec_open2(codec_ctx, codec, nullptr)) < 0) {
			printf("Failed to open %s codec\n",
				av_get_media_type_string(video_stream->codecpar->codec_type));
			return false;
		}
		//创建一个frame接收解码之后的帧数据
		frame = av_frame_alloc();
		is_opened = true;
		width = video_stream->codecpar->width;
		height = video_stream->codecpar->height;

		//检测是否支持当前视频的像素格式
		AVPixelFormat pixfmt = (AVPixelFormat)video_stream->codecpar->format;
		bool support = pixfmt == PIX_FMT_YUV420P ||
			pixfmt == PIX_FMT_YUYV422 ||
			pixfmt == PIX_FMT_YUV422P ||
			pixfmt == PIX_FMT_YUV444P ||
			pixfmt == PIX_FMT_GRAY ||
			pixfmt == PIX_FMT_YUVJ420P ||
			pixfmt == PIX_FMT_YUVJ422P ||
			pixfmt == PIX_FMT_YUVJ444P ||
			pixfmt == PIX_FMT_NV12 ||
			pixfmt == PIX_FMT_NV21 ||
			pixfmt == PIX_FMT_RGB ||
			pixfmt == PIX_FMT_BGR ||
			pixfmt == PIX_FMT_ARGB ||
			pixfmt == PIX_FMT_RGBA ||
			pixfmt == PIX_FMT_ABGR ||
			pixfmt == PIX_FMT_BGRA;

		//不支持根据formatType进行设置
		if (formatType == PIX_FMT_AUTO)
		{
			formatType = support ? (PixelFormatType)pixfmt : PIX_FMT_BGRA;
		}


		return true;
	}

	void cvideo_capture::close()
	{
		if (ic)
		{
			avformat_close_input(&ic);
			ic = NULL;
		}
		if (codec_ctx)
		{
			avcodec_free_context(&codec_ctx);
			codec_ctx = NULL;
		}
		if (sws_ctx)
		{
			sws_freeContext(sws_ctx);
			sws_ctx = NULL;
		}
		//sws_ctx = nullptr;
		if (frame)
		{
			av_frame_free(&frame);
			frame = NULL;
		}
		if (sws_frame)
		{
			av_frame_free(&sws_frame);
			sws_frame = NULL;
		}
	}

	int cvideo_capture::grab_frame(AVFrame*& out_frame)
	{
		if (!is_opened)
		{
			return -2;
		}
		int ret = 0;
		//定义AVPacket用来存储压缩的帧数据
		AVPacket pkt;
		av_init_packet(&pkt);
		out_frame = nullptr;
		do
		{
			ret = avcodec_receive_frame(codec_ctx, frame);
			out_frame = frame;
			if (ret >= 0)
			{
				return 1;
			}
			//读取到结尾
			if (ret == AVERROR_EOF)
			{
				out_frame = nullptr;
				return 0;
			}
			else if (ret != AVERROR(EAGAIN))
			{
				printf("Error submitting a packet for decoding (%s)\n", ffmepgerror(ret));
				return -1;
			}

			//读取一帧压缩数据
			ret = av_read_frame(ic, &pkt);
			if (ret != AVERROR_EOF && pkt.stream_index != video_stream_index)
			{
				av_packet_unref(&pkt);
				continue;
			}
			if (ret < 0)
			{
				//判断是否读取到结尾，读取到结尾seek到第一帧
				if (ret == AVERROR_EOF)
					av_init_packet(&pkt);
				else
				{
					printf("av_read_frame error:%s\n", ffmepgerror(ret));
					return -1;
				}
			}

			//压缩帧数据据发送到解码线程
			ret = avcodec_send_packet(codec_ctx, &pkt);
			av_packet_unref(&pkt);
			if (ret < 0) {
				printf("Error submitting a packet for decoding (%s)\n", ffmepgerror(ret));
				return -1;
			}


		} while (true);

		return -1;
	}

	int cvideo_capture::retrieve(AVFrame*& out_frame)
	{
		AVFrame* srcFrame = nullptr;
		int ret = grab_frame(srcFrame);
		if (ret > 0)
		{
			//判断解码出来的像素格式与目标像素格式是否相同
			if (srcFrame->format == formatType)
			{
				out_frame = srcFrame;
			}
			else
			{
				//像素格式转换
				if (!sws_frame)
				{
					sws_frame = av_frame_alloc();
					sws_frame->format = (AVPixelFormat)formatType;
					sws_frame->width = srcFrame->width;
					sws_frame->height = srcFrame->height;
					int ret = av_frame_get_buffer(sws_frame, 0);
					if (ret)
					{
						printf("av_frame_get_buffer failed: %s\n", ffmepgerror(ret));
						return -1;
					}
				}

				sws_ctx = sws_getCachedContext(
					sws_ctx,									//传NULL会新创建 如果和之前相同会直接返回
					srcFrame->width, srcFrame->height,		// 输入的宽高
					(AVPixelFormat)srcFrame->format,		// 输入的格式
					sws_frame->width, sws_frame->height,	// 输出的宽高
					(AVPixelFormat)sws_frame->format,		// 输出的格式
					SWS_POINT,								// 尺寸变换的算法
					0, 0, 0
				);

				int ret = sws_scale(
					sws_ctx,
					srcFrame->data,				//输入数据
					srcFrame->linesize,			//输入行大小 考虑对齐
					0,							//从0开始
					srcFrame->height,			//输入的高度
					sws_frame->data,			//输出的数据 
					sws_frame->linesize			//输出的大小
				);
				if (ret <= 0)
				{
					printf("sws_scale failed: %s", ffmepgerror(ret));
					return -1;
				}
				sws_frame->pts = srcFrame->pts;
				sws_frame->colorspace = srcFrame->colorspace;
				sws_frame->color_range = srcFrame->color_range;
				out_frame = sws_frame;
			}

		}
		else
		{
			out_frame = nullptr;
		}
		return ret;
	}

	bool cvideo_capture::seek(double percentage)
	{
		if (!is_opened)
		{
			return false;
		}
		int64_t ts = ic->duration * percentage;
		int ret = av_seek_frame(ic, -1, ts, AVSEEK_FLAG_FRAME);
		if (ret < 0)
		{
			printf("Seek error: %s", ffmepgerror(ret));
			return false;
		}
		avcodec_flush_buffers(codec_ctx);
		return true;
	}

}