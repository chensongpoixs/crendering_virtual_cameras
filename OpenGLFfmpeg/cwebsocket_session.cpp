/***********************************************************************************************
created: 		2022-08-09

author:			chensong

purpose:		cwebsocket_session
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

#include "cwebsocket_session.h"
#include "cwebsocket_msg_queue.h"
#include "cwebsocket_server_mgr.h"
#include "cnet_type.h"
#include <string>

//#include "clog.h"
namespace chen {
	cwebsocket_session::cwebsocket_session(cwebsocket_server_mgr *  websocket_server, uint64_t session_id, boost::asio::ip::tcp::socket  socket)
		: m_websocket_server_mgr_ptr(websocket_server)
		, m_session_id(session_id)
		, m_ws(std::move(socket))
		, m_strand(m_ws.get_executor())
		, m_recv_buffer(NULL)
		, m_recv_buffer_size(1024 * 1024)
		, m_writeing_atomic(false)
		, m_send_buffer_list()
		, m_sending_buffer_list()
	{}
	/*cwebsocket_session::cwebsocket_session(boost::asio::ip::tcp::socket socket)
	{
	}*/
	cwebsocket_session::~cwebsocket_session()
	{
		NORMAL_EX_LOG("[session_id = %u]", m_session_id);
		m_websocket_server_mgr_ptr = NULL;
		if (m_recv_buffer)
		{
		//	::free(   m_recv_buffer);
			m_recv_buffer = NULL;
		}
		m_sending_buffer_list.clear();
		m_send_buffer_list.clear();
	}
	void cwebsocket_session::run()
	{
		// Accept the websocket handshake
		m_ws.async_accept(
			boost::asio::bind_executor( m_strand, std::bind( &cwebsocket_session::_handle_accept, this, std::placeholders::_1)));
	}
	bool cwebsocket_session::send_msg(uint64_t session_id, const void * msg_ptr, uint32_t msg_size)
	{
		
		if (m_session_id != session_id/* || 0 == msg_id || ENSS_Open != m_status*/)
		{
			return false;
		}
		
		{
			//可能在异步发送数据的包
			clock_guard lock(m_mutex);
			//boost::asio::buffer bufferw(msg_ptr, msg_size);
			//std::string data(msg_ptr, msg_size);
 			m_send_buffer_list.emplace_back(std::string((char *)msg_ptr, msg_size));
		}

		bool write_bool = false;

		if (!m_writeing_atomic.compare_exchange_strong(write_bool, true))
		{
			return true;
		}
		{
			clock_guard lock(m_mutex);
			m_sending_buffer_list.swap(m_send_buffer_list);
		}
		m_ws.text(m_ws.got_text());
		m_ws.async_write(
			boost::asio::buffer( m_sending_buffer_list.front(), m_sending_buffer_list.front().size()),
			boost::asio::bind_executor(
				m_strand,
				std::bind(
					&cwebsocket_session::_handle_write,
					this,
					std::placeholders::_1,
					std::placeholders::_2)));
		return true;
	}
	bool cwebsocket_session::close(uint64_t session_id)
	{

		boost::system::error_code ec;
		//m_ws.async_close(boost::asio::ip::tcp::socket::shutdown_both, ec);
		//m_ws.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
		if (m_ws.is_open())
		{
			boost::beast::websocket::close_reason reason;
			m_ws.async_close(reason, std::bind(&cwebsocket_session::_handle_close, this, std::placeholders::_1));
			return true;
		}
		_release();
		return true;
	}
	void cwebsocket_session::_handle_accept(boost::system::error_code ec)
	{
		if (ec)
		{
			
			ERROR_EX_LOG("exit accept failed ec !!!z");
			_release();
			return;
		}

		// Read a message
		//boost::beast::multi_buffer buffer;
	
		m_ws.async_read(
			m_buffer, 
			boost::asio::bind_executor(
				m_strand,
				std::bind(
					&cwebsocket_session::_handle_read,
					this,
					std::placeholders::_1,
					std::placeholders::_2)));
	}
	 
	void cwebsocket_session::_handle_read(boost::system::error_code ec, std::size_t bytes_transferred)
	{

		boost::ignore_unused(bytes_transferred);

		// This indicates that the session was closed
		if (ec == boost::beast::websocket::error::closed)
		{
			_release();
			ERROR_EX_LOG("exit closed !!!");
			return;
		}

		if (ec)
		{
			ERROR_EX_LOG("exit read closed !!!");
			//fail(ec, "read")
			_release();
			return;
		}

		/*
		if (error || bytes_transferred == 0)
		{
			
			m_reading_atomic.store(false);
			//LOG_ERROR << "_handle_read =" << error.value();
			_close();
			return;
		}

		if (m_status != ENSS_Open)
		{
			m_reading_atomic.store(false);
			_close();
			return;
		}
		*/


		// 业务逻辑
		cwebsocket_msg * websocket_msg_ptr = new cwebsocket_msg();
		if (!websocket_msg_ptr)
		{
			// wrate
			//return;
		}
		else
		{
			//std::ostringstream cmd;
			//cmd << m_buffer;
			//boost::asio::mutable_buffer::mutable_buffer p = m_buffer.data();
			/*boost::beast::const_buffers_type<char> p = */// m_buffer.data();
			std::stringstream ss;
			ss << boost::beast::buffers(m_buffer.data());
			websocket_msg_ptr->set_buffer((unsigned char *)ss.str().c_str(), ss.str().length());
			websocket_msg_ptr->set_session_id(m_session_id);
			m_websocket_server_mgr_ptr->msg_push(websocket_msg_ptr);
		}
		
		//std::cout << m_buffer.member() << std::endl;;
		m_buffer.consume(m_buffer.size());
	//	m_ws.text(m_ws.got_text());
		/*m_ws.async_read_some(
			boost::asio::buffer(m_recv_buffer, m_recv_buffer_size),
			boost::asio::bind_executor(
				m_strand,
				std::bind(
					&cwebsocket_session::_handle_read,
					this,
					std::placeholders::_1,
					std::placeholders::_2)));*/
		m_ws.async_read (
			m_buffer,
			boost::asio::bind_executor(
				m_strand,
				std::bind(
					&cwebsocket_session::_handle_read,
					this,
					std::placeholders::_1,
					std::placeholders::_2)));
			// Echo the message
	/*	ws_.text(ws_.got_text());
		ws_.async_write(
			buffer_.data(),
			boost::asio::bind_executor(
				strand_,
				std::bind(
					&session::on_write,
					shared_from_this(),
					std::placeholders::_1,
					std::placeholders::_2)));*/

	}
	void cwebsocket_session::_handle_write(boost::system::error_code ec, std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		if (ec)
		{
			//return fail(ec, "write");
			m_sending_buffer_list.clear();
			//m_send_buffer_list.clear();
			_release();
			return;
		}

		/*if (error || bytes_transferred == 0)
		{
			m_writeing_atomic.store(false);
			_close();
			return;
		}

		if (m_status != ENSS_Open)
		{
			m_writeing_atomic.store(false);
			_close();
			return;
		}*/
		if (m_sending_buffer_list.size() > 1)
		{
			m_sending_buffer_list.pop_front();
			m_ws.text(m_ws.got_text());
			m_ws.async_write(
				boost::asio::buffer(m_sending_buffer_list.front(), m_sending_buffer_list.front().size()),
				boost::asio::bind_executor(
					m_strand,
					std::bind(
						&cwebsocket_session::_handle_write,
						this,
						std::placeholders::_1,
						std::placeholders::_2)));
		}
		else if (!m_sending_buffer_list.empty() && !m_send_buffer_list.empty())
		{
			m_sending_buffer_list.pop_front();
			
			{
				clock_guard lock(m_mutex);
				m_sending_buffer_list.swap(m_send_buffer_list);
			}
			m_ws.text(m_ws.got_text());
			m_ws.async_write(
				boost::asio::buffer(m_sending_buffer_list.front(), m_sending_buffer_list.front().size()),
				boost::asio::bind_executor(
					m_strand,
					std::bind(
						&cwebsocket_session::_handle_write,
						this,
						std::placeholders::_1,
						std::placeholders::_2)));

		}
		else if (!m_sending_buffer_list.empty())
		{
			m_sending_buffer_list.pop_front();
			m_writeing_atomic.store(false);
		}
		else
		{
			m_writeing_atomic.store(false);
		}
		// Clear the buffer
		//buffer_.consume(buffer_.size());

		// Do another read
		//do_read();
	}
	void cwebsocket_session::_release()
	{
		
		//boost::asio::ip::tcp::socket::shutdown_both
		if (m_ws.is_open())
		{
			boost::system::error_code ec;
			//m_ws.async_close(  ec);
			boost::beast::websocket::close_reason reason;
			m_ws.async_close(reason, std::bind( &cwebsocket_session::_handle_close, this, std::placeholders::_1 ) );
				return;
		}
		 m_websocket_server_mgr_ptr->post_disconnect(this);;
	}
	void cwebsocket_session::_handle_close(boost::system::error_code ec)
	{
		
		m_websocket_server_mgr_ptr->post_disconnect(this);;
		//m_session_id = 0;
	}
}

 