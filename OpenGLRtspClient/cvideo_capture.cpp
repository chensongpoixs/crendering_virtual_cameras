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
#include <QString>
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "postproc.lib")
#pragma comment(lib, "avfilter.lib")
#include <QTimer>
//#include <QTest>
#define MAX_AUDIO_FRAME_SIZE (192000)

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
		// rtsp协议设置tcp  rtsp_transport
		AVDictionary *option = NULL;
		av_dict_set(&option, "rtsp_transport", "tcp", 0);
		// 1. 打开解封装上下文
		int ret = avformat_open_input(
			&ic, //解封装上下文
			url,  //文件路径
			NULL, //指定输入格式 h264,h265, 之类的， 传入NULL则自动检测
			&option); //设置参数的字典
		if (ret != 0)
		{
			printf("%s\n", ffmepgerror(ret));
			return false;
		}
		if (option)
		{
			av_dict_free(&option);
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
			else if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
			{
				if (!audio_stream_)
				{
					audio_stream_ = stream;
					audio_stream_index_ = i;
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


		const AVCodec* audio_codec = avcodec_find_decoder(audio_stream_->codecpar->codec_id);
		if (!audio_codec)
		{
			printf("can't find codec, audio codec id:%d\n", audio_stream_->codecpar->codec_id);
			return false;
		}

		//5.创建解码器上下文
		if (!(codec_ctx = avcodec_alloc_context3(codec)))
		{
			printf("avcodec_alloc_context3 failed,\n");
			return false;
		}
		//5.创建解码器上下文
		if (!(audio_codec_ctx = avcodec_alloc_context3(audio_codec)))
		{
			printf("avcodec_alloc_context3 audio_ failed,\n");
			return false;
		}


		//6.从输入流复制编解码器参数到输出编解码器上下文
		if ((ret = avcodec_parameters_to_context(codec_ctx, video_stream->codecpar)) < 0) {
			printf("Failed to copy %s video codec parameters to decoder context\n",
				av_get_media_type_string(video_stream->codecpar->codec_type));
			return false;
		}
		if ((ret = avcodec_parameters_to_context(audio_codec_ctx, audio_stream_->codecpar)) < 0) {
			printf("Failed to copy %s audio codec parameters to decoder context\n",
				av_get_media_type_string(audio_stream_->codecpar->codec_type));
			return false;
		}

		//7. 打开解码器上下文 */
		if ((ret = avcodec_open2(codec_ctx, codec, nullptr)) < 0) {
			printf("Failed to open %s codec\n",
				av_get_media_type_string(video_stream->codecpar->codec_type));
			return false;
		}

		if ((ret = avcodec_open2(audio_codec_ctx, audio_codec, nullptr)) < 0) {
			printf("Failed to open %s audio codec\n",
				av_get_media_type_string(audio_stream_->codecpar->codec_type));
			return false;
		}
		// ffmpeg -i audio1.mp3 -f s16le audio1.pcm
		// ffplay -ar 44100 -ac 2 -f s16le -i audio1.pcm
	/*	该命令的作用是使用ffplay播放频率为44100Hz，双通道，16位、小端的音频文件audio1.pcm。

			注：
			- i 表示指定的输入文件
			- f 表示强制使用的格式
			- ar 表示播放的音频数据的采样率
			- ac 表示播放的音频数据的通道数

			如下图所示，出现如下画面和能听到*/
		//创建一个frame接收解码之后的帧数据
		frame = av_frame_alloc();
		audio_frame_ = av_frame_alloc();
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





		//设置转码参数
		  out_channel_layout_ = audio_codec_ctx->channel_layout;
		 out_sample_fmt_ = AV_SAMPLE_FMT_S16;
		  out_sample_rate_ = audio_codec_ctx->sample_rate;
		  out_channels_ = av_get_channel_layout_nb_channels(out_channel_layout_);
		//printf("out rate : %d , out_channel is: %d\n",out_sample_rate,out_channels);

		audio_out_buffer_ = (uint8_t*)av_malloc(MAX_AUDIO_FRAME_SIZE * 2);

		audio_swr_ctx_ = swr_alloc_set_opts(NULL,
			out_channel_layout_,
			out_sample_fmt_,
			out_sample_rate_,
			audio_codec_ctx->channel_layout,
			audio_codec_ctx->sample_fmt,
			audio_codec_ctx->sample_rate,
			0, NULL);

		swr_init(audio_swr_ctx_);

	
		init_audio_device();



		audio_thread_ = std::thread(&cvideo_capture::_audio_pthread, this);
		return true;
	}
	void cvideo_capture::init_audio_device()
	{

		 QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
		 info_ = std::move(info);
		fmt_.setSampleRate( audio_codec_ctx->sample_rate );
		fmt_.setSampleSize(16/*audio_codec_ctx->frame_size*/);
		fmt_.setChannelCount(audio_codec_ctx->channels);
		fmt_.setCodec("audio/pcm");
		fmt_.setByteOrder(QAudioFormat::LittleEndian);
		fmt_.setSampleType(QAudioFormat::UnSignedInt);
		
		if (!info.isFormatSupported(fmt_))
		{
			qDebug() << "输出设备不支持该格式，不能播放音频";
			return  ;
		}
		audio_ = new QAudioOutput(fmt_, qApp);  // 注意：这里QAudioOutput必须使用指针，否则不能播放
		//audio_->setVolume(100);
		  io_ = audio_->start();
		int size = audio_->periodSize();     // 这是每个周期防止缓冲区欠载和确保不间断播放所需的数据量。
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
			avcodec_flush_buffers(codec_ctx);
			avcodec_free_context(&codec_ctx);
			codec_ctx = NULL;
		}
		if (audio_codec_ctx)
		{
			avcodec_flush_buffers(audio_codec_ctx);
			avcodec_free_context(&audio_codec_ctx);
			audio_codec_ctx = NULL;
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
		
		out_frame = nullptr;
		do
		{
			
			AVPacket* pkt = av_packet_alloc();
			av_init_packet(pkt);
		
			//读取一帧压缩数据
			ret = av_read_frame(ic,  pkt);
			if (ret != AVERROR_EOF && pkt->stream_index != video_stream_index &&
				pkt->stream_index != audio_stream_index_)
			{
				av_packet_unref(pkt);
				av_packet_free(&pkt);
				pkt = NULL;
				continue;
			}
			if (ret < 0)
			{
				//判断是否读取到结尾，读取到结尾seek到第一帧
				if (ret == AVERROR_EOF)
				{
					av_init_packet(pkt);
				}
				else
				{
					printf("av_read_frame error:%s\n", ffmepgerror(ret));
					return -1;
				}
			}

			//压缩帧数据据发送到解码线程
			if (pkt->stream_index == video_stream_index)
			{
				ret = avcodec_send_packet(codec_ctx,  pkt);
				av_packet_unref(pkt);
				av_packet_free(&pkt);
				pkt = NULL;
				if (ret < 0) {
					printf("Error submitting a packet for video decoding (%s)\n", ffmepgerror(ret));
					return -1;
				}
				ret = avcodec_receive_frame(codec_ctx, frame);
				
				if (ret >= 0)
				{
					out_frame = /*std::move*/(frame);
					out_frame->nb_samples = 0;
					return 1;
				}
			}
			else if (pkt->stream_index == audio_stream_index_)
			{
				{
					std::lock_guard<std::mutex> lk(audio_lock_);
					audio_queue_.emplace_back(std::move(pkt));
					pkt = NULL;
					cond_.notify_one();
					
					frame->nb_samples = 10;
					out_frame = /*std::move*/(frame);
					return 1;
				}
#if 0
				ret = avcodec_send_packet(audio_codec_ctx, &pkt);
				av_packet_unref(&pkt);
				if (ret < 0) {
					printf("Error submitting a packet for audio decoding (%s)\n", ffmepgerror(ret));
					return -1;
				}
				ret = avcodec_receive_frame(audio_codec_ctx, frame);

				if (ret >= 0)
				{
					if (av_sample_fmt_is_planar(audio_codec_ctx->sample_fmt)) {
						int len = swr_convert(audio_swr_ctx_,
							&audio_out_buffer_,
							MAX_AUDIO_FRAME_SIZE * 2,
							(const uint8_t**)frame->data,
							frame->nb_samples);
						if (len <= 0) 
						{
							continue;
						}
						//qDebug("convert length is: %d.\n",len);

						int out_size = av_samples_get_buffer_size(0,
							out_channels_,
							len,
							out_sample_fmt_,
							1);
						//qDebug("buffer size is: %d.",dst_bufsize);

						sleep_time_ = (out_sample_rate_ * 16 * 2 / 8) / out_size;

						if (audio_->bytesFree() < out_size) {
							 //QTest::qSleep(sleep_time_);
							//QThread::sleep(sleep_time_);
							io_->write((char*)audio_out_buffer_, out_size);
						}
						else {
							io_->write((char*)audio_out_buffer_, out_size);
						}
						//将数据写入PCM文件
						//fwrite(audio_out_buffer,1,dst_bufsize,file);
					}
#endif 
#if 0
					
					//else 
					{
						int data_size = av_get_bytes_per_sample(audio_codec_ctx->sample_fmt);
						if (data_size < 0) {
							/* This should not occur, checking just for paranoia */
							fprintf(stderr, "Failed to calculate data size\n");
							//exit(1);
						}
						else
						{
							int audio_total_size = frame->nb_samples * data_size * audio_codec_ctx->ch_layout.nb_channels;
							static uint8_t * p = new uint8_t[audio_total_size * 8];
							uint32_t size_index = 0;
							//QByteArray qbuf(audio_total_size, 0);
#if 0
							for (int i = 0; i < frame->nb_samples; i++)
							{
								for (int ch = 0; ch < audio_codec_ctx->ch_layout.nb_channels; ch++)
								{
									/*static FILE * out_file_ptr = fopen("chensong.pcm", "wb+");
									if (out_file_ptr)
									{
										fwrite((const uint8_t *)(frame->data[ch] + data_size * i), 1, data_size, out_file_ptr);
										fflush(out_file_ptr);
									}*/
									memcpy(p + size_index, (const uint8_t *)(frame->data[ch] + data_size * i), data_size);
									size_index += data_size;
									//io_->write((const char *)(frame->data[ch] + data_size * i), data_size);
									//QByteArray tempbuf((const char *)(frame->data[ch] + data_size * i), data_size);
									//QString qstr;
									//qstr.fill(frame->data[ch] + data_size * i, data_size);
									//qbuf.append(QString(frame->data[ch] + data_size * i, data_size));
									//qbuf += qbuf.append(tempbuf);
									//tempbuf.remove()
								//	io_->write(tempbuf.mid(0, data_size));
								}
							}
							qint64 write_len = 0;
							//int32_t size_q = audio_->periodSize();
							//while (write_len < size_index)
							{
								/*while (audio_->bytesFree() < size_index)
								{
									Sleep(1);
								}*/
								
								//int32_t w_len = size_q > (size_index - write_len) ? (size_index - write_len) : size_q;
								  write_len += io_->write((const char *)(p /*+ write_len*/), size_index);
							}
							/*while (audio_->bytesFree() < audio_->periodSize())
							{
								Sleep(1);
							}


							while (audio_->bytesFree() > audio_->periodSize())
							{
							}
							qint64 write_len = io_->write((const char *)p, size_index);*/
#else  //
							

							//io_->write(reinterpret_cast<const char*>(frame->data[0]),
							//	frame->nb_samples * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16));
#endif //
							//qbuf.remove(0, audio_total_size);
									//fwrite(frame->data[ch] + data_size * i, 1, data_size, outfile);
						}
						
					}
					out_frame = /*std::move*/(frame);
					return 1;
					}

#endif 
					
				


				//if ()
			}
			if (pkt)
			{
				av_packet_free(&pkt);
				pkt = NULL;
			}
			//if (pkt->)
				//读取到结尾
			if (ret == 0 )
			{
			}
			else if (ret == AVERROR_EOF)
			{
				out_frame = nullptr;
				return 0;
			}
			else if (  ret != AVERROR(EAGAIN))
			{
				printf("Error submitting a packet for decoding (%s)\n", ffmepgerror(ret));
				return -1;
			}


		} while (true);

		return -1;
	}

	int cvideo_capture::retrieve(AVFrame*& out_frame)
	{



		AVFrame* srcFrame = nullptr;
	
		
		while (true)
		{
			int ret = grab_frame(srcFrame);
			if (ret < 0)
			{
				return ret;
			}

			if (frame->nb_samples > 0)
			{
//#define AUDIO_PCM (1)

#if 0// AUDIO_PCM

				static FILE * out_audio_file_ptr = fopen("test.pcm", "wb+");
				if (out_audio_file_ptr)
				{
					int data_size = av_get_bytes_per_sample(audio_codec_ctx->sample_fmt);
					if (data_size < 0) {
						/* This should not occur, checking just for paranoia */
						fprintf(stderr, "Failed to calculate data size\n");
						//	exit(1);
					}
					else
					{
						for (int i = 0; i < frame->nb_samples; i++)
						{
							for (int ch = 0; ch < audio_codec_ctx->ch_layout.nb_channels; ch++)
							{
								fwrite(frame->data[ch] + data_size * i, 1, data_size, out_audio_file_ptr);
							}
						}

						fflush(out_audio_file_ptr);
					}
				}
#endif 
				continue;
			}
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
			return ret;
		}
		 
		return -1;
	}
	void cvideo_capture::_audio_pthread()
	{
		AVPacket* packet = NULL;
		while (true)
		{
			{
				std::unique_lock<std::mutex> lk(audio_lock_);
				cond_.wait(lk, [this]() {return audio_queue_.size() > 0; });
			}
			{
				std::lock_guard<std::mutex> lk(audio_lock_);
				if (!audio_queue_.empty())
				{
					 packet =  std::move(audio_queue_.front());
					audio_queue_.pop_front();
				}
			}

			int ret = avcodec_send_packet(audio_codec_ctx, packet);
			av_packet_unref(packet);
			av_packet_free(&packet);
			packet = NULL;
			if (ret < 0) {
				printf("Error submitting a packet for audio decoding (%s)\n", ffmepgerror(ret));
				continue;
			}
			ret = avcodec_receive_frame(audio_codec_ctx, audio_frame_);

			if (ret >= 0)
			{
				if (av_sample_fmt_is_planar(audio_codec_ctx->sample_fmt)) {
					int len = swr_convert(audio_swr_ctx_,
						&audio_out_buffer_,
						MAX_AUDIO_FRAME_SIZE * 2,
						(const uint8_t**)audio_frame_->data,
						audio_frame_->nb_samples);
					if (len <= 0)
					{
						continue;
					}
					//qDebug("convert length is: %d.\n",len);

					int out_size = av_samples_get_buffer_size(0,
						out_channels_,
						len,
						out_sample_fmt_,
						1);
					//qDebug("buffer size is: %d.",dst_bufsize);

					sleep_time_ = (out_sample_rate_ * 16 * 2 / 8) / out_size;

					if (audio_->bytesFree() < out_size) {
						//QTest::qSleep(sleep_time_);
					  // QThread::sleep(sleep_time_);
						Sleep(1);
						io_->write((char*)audio_out_buffer_, out_size);
					}
					else {
						io_->write((char*)audio_out_buffer_, out_size);
					}
					//将数据写入PCM文件
					//fwrite(audio_out_buffer,1,dst_bufsize,file);
				}
			}
			 
		}

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