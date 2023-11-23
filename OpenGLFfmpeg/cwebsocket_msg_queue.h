/***********************************************************************************************
created: 		2019-12-27

author:			chensong

purpose:		cnet_msg_queue
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
#ifndef _C_MSG_WEBSOCKET_QUEUE_H_
#define _C_MSG_WEBSOCKET_QUEUE_H_
#include <mutex>
#include "cnet_define.h"
namespace chen {
	class cwebsocket_msg
	{
	public:
		cwebsocket_msg()
			: m_session_id(0)
			, m_next_msg_ptr(NULL)
			, m_msg_id(0)
			, m_buffer_ptr(NULL)
			, m_data_size(0)
			, m_remote_ip("")
			, m_remote_port(0)
		{}
		~cwebsocket_msg(){}
	public:
		cwebsocket_msg * get_next_msg() const { return m_next_msg_ptr; }
		void set_next_msg(cwebsocket_msg * ptr) { m_next_msg_ptr = ptr; }
		const uint16_t get_msg_id() const { return m_msg_id; }
		void set_msg_id(uint16_t msg_id) { m_msg_id = msg_id; }
		const uint64_t get_session_id() const { return m_session_id; }
		void set_session_id(uint64_t value) { m_session_id = value; }
		void set_buffer(unsigned char  * buffer, uint32_t data_size);
		unsigned char  *   get_buffer() const { return m_buffer_ptr; }
		uint32_t    get_data_size()   { return m_data_size; }
		void set_remote_ip(const char * ip) { m_remote_ip = ip; }
		const std::string &get_remote_ip() const { return m_remote_ip; }
		void set_remote_port(uint16_t port) { m_remote_port = port; }
		uint16_t get_remote_port() const { return m_remote_port; }
		void free();
	private:
		cwebsocket_msg(const cwebsocket_msg&);
		cwebsocket_msg& operator=(const cwebsocket_msg&);
	private:

		uint64_t						m_session_id;
		cwebsocket_msg*					m_next_msg_ptr;
		uint16_t							m_msg_id;
		unsigned char  * 							m_buffer_ptr;
		uint32_t						m_data_size;
		std::string						m_remote_ip;
		uint16_t							m_remote_port;
	};
	 
	class cwebsocket_msg_queue
	{
	private:
		typedef std::mutex								clock_type;
		typedef std::lock_guard<clock_type>				clock_guard;
	public:
		cwebsocket_msg_queue();
		~cwebsocket_msg_queue();
		void enqueue(cwebsocket_msg*p);
		cwebsocket_msg* dequeue();
		void clear();
	private:
		cwebsocket_msg_queue(const cwebsocket_msg_queue&);
		cwebsocket_msg_queue& operator=(const cwebsocket_msg_queue&);
	private:
		cwebsocket_msg*			m_head_ptr;
		cwebsocket_msg*			m_tail_ptr;
		clock_type		m_lock;
	};
}//!namespace chen


#endif // !#define _C_MSG_WEBSOCKET_QUEUE_H_