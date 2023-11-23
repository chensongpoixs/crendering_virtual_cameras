/***********************************************************************************************
created: 		2022-08-09

author:			chensong

purpose:		websocket_session
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

#ifndef _C_WEBSOCKET_SESSION_H
#define _C_WEBSOCKET_SESSION_H

#include "cnet_define.h"
#include <mutex>
//#include "cwebsocket_server_mgr.h"
namespace chen {
	class cwebsocket_server_mgr;

	class cwebsocket_session 
	{
	private:
		typedef boost::beast::websocket::stream<boost::asio::ip::tcp::socket>			cws;
		typedef boost::asio::strand< boost::asio::io_context::executor_type>			strand_;
		typedef std::mutex															    clock_type;
		typedef std::lock_guard<clock_type>											    clock_guard;
		typedef boost::beast::multi_buffer												     recv_buffer_;
		typedef std::list<std::string>													send_buffer_list;
		typedef std::atomic_bool 														catomic_bool;
	public:
		cwebsocket_session( cwebsocket_server_mgr * websocket_server,  uint64_t session_id, boost::asio::ip::tcp::socket  socket);
			
		~cwebsocket_session();


	public:
		void run();

		uint64_t get_session_id() { return m_session_id; }
		/**
		* 发送数据包缓存到发送缓冲区中
		* @param   msg_ptr	: 发送数据包
		* @param   size		: 发送数据包的大小
		**/
		bool send_msg(uint64_t session_id,  const void * msg_ptr, uint32_t msg_size);
		//void do_read();


		bool close(uint64_t session_id);
	private:
		void _handle_accept(boost::system::error_code ec);


		void _handle_read( boost::system::error_code ec, std::size_t bytes_transferred);

		void _handle_write( boost::system::error_code ec, std::size_t bytes_transferred);

		void _release();

		void _handle_close(boost::system::error_code ec);
	private:
		cwebsocket_server_mgr *								m_websocket_server_mgr_ptr;
		uint64_t											m_session_id;		// 网络层的会话id
		cws													m_ws;
		strand_												m_strand;
		unsigned char *										m_recv_buffer;
		uint64_t											m_recv_buffer_size;
		catomic_bool										m_writeing_atomic;
		clock_type											m_mutex;
		send_buffer_list									m_send_buffer_list;
		send_buffer_list									m_sending_buffer_list;
		boost::beast::multi_buffer							m_buffer;

	};
}

#endif // _C_WEBSOCKET_SESSION_H