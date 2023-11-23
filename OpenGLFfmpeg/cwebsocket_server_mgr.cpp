/***********************************************************************************************
created: 		2022-08-09

author:			chensong

purpose:		websocket_server_mgr
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


#include "cwebsocket_server_mgr.h"
#include "cwebsocket_session.h"
#include <boost\bind.hpp>
#include <boost/asio.hpp>
//#include "clog.h"
#include "cwebsocket_msg_reserve.h"
#include <string>
#include <cstring>
#include <iostream>
#include <memory>
#include <algorithm>
#include <boost/asio/buffer.hpp>
//#include "clog.h"
namespace chen{

	cwebsocket_server_mgr g_websocket_server_mgr;


	bool cwebsocket_server_mgr::startup(uint32_t thread_num, const char * ip, uint16_t port)
	{
		if (!_start_listen(ip, port, thread_num))
		{
			return false;
		}
		for (uint32_t i = 0; i < thread_num; ++i)
		{
			m_threads.emplace_back(&cwebsocket_server_mgr::_worker_thread, this);
		}

		return true;
	}

	void cwebsocket_server_mgr::shutdown()
	{
		m_shuting.store(true);
		if (m_acceptor_ptr)
		{
			if (m_acceptor_ptr->is_open())
			{
				cerror_code ec;
				m_acceptor_ptr->cancel(ec);
				m_acceptor_ptr->close(ec);
			}
			delete m_acceptor_ptr;
			m_acceptor_ptr = nullptr;
		}

		if (!m_io_service.stopped())
		{
			// 关闭工作线程
			m_io_service.stop();
		}

		for (std::thread& thread : m_threads)
		{
			if (thread.joinable())
			{
				thread.join();
			}
		}
		m_threads.clear();
	}

	void cwebsocket_server_mgr::destroy()
	{
		/*for (cnet_session* session : m_sessions)
		{
			session->destroy();
			delete session;
		}
		m_sessions.clear();
		m_available_sessions.clear();

		for (cconnect_para* para : m_paras)
		{
			delete para;
		}
		m_paras.clear();*/

		/*if (m_msgs)
		{
			cnet_msg *p = m_msgs->dequeue();
			cnet_msg* tmp = nullptr;
			while (p)
			{
				tmp = p;
				p = p->get_next_msg();
				m_pool_ptr->free(tmp);
			}

			delete m_msgs;
			m_msgs = nullptr;
		}*/
	}

	void cwebsocket_server_mgr::msg_push(cwebsocket_msg * msg_ptr)
	{
		m_msgs->enqueue(msg_ptr);
	}

	void cwebsocket_server_mgr::post_disconnect(cwebsocket_session * p)
	{
		m_io_service.post(std::bind(&cwebsocket_server_mgr::_handle_close, this, p));
	}

	void cwebsocket_server_mgr::process_msg()
	{
		cwebsocket_msg *msg_ptr = m_msgs->dequeue();

		while (msg_ptr)
		{
			if (msg_ptr->get_msg_id() == EMIR_Connect)
			{
				//cmsg_connect* p = (cmsg_connect*)msg_ptr->get_buf();
				m_connect_callback(msg_ptr->get_session_id(), msg_ptr->get_remote_ip().c_str(), msg_ptr->get_remote_port());
			}
			else if (msg_ptr->get_msg_id() == EMIR_Disconnect)
			{
				m_disconnect_callback(msg_ptr->get_session_id());
			}
			else
			{
				//std::to_string(2);
				 
				m_msg_callback(msg_ptr->get_session_id(),       msg_ptr->get_buffer(), msg_ptr->get_data_size());
			}
			cwebsocket_msg *tmp_msg = msg_ptr;
			msg_ptr = msg_ptr->get_next_msg();
			 
			tmp_msg->free();
			delete tmp_msg;
			tmp_msg = NULL;
			//tmp_msg->free_me();
		} 
	}

	bool cwebsocket_server_mgr::send_msg(uint64_t sessionId, const void * msg_ptr, uint32_t msg_size)
	{
		clock_guard lock(m_session_mutex);
		CWEBSOCKET_SESSION_MAP::iterator iter =  m_session_map.find(sessionId);
		if (iter != m_session_map.end())
		{
			  
			return iter->second->send_msg(sessionId, msg_ptr, msg_size);
		}
		return false;
	}

	void cwebsocket_server_mgr::close(uint64_t sessionId)
	{
		clock_guard lock(m_session_mutex);
		CWEBSOCKET_SESSION_MAP::iterator iter = m_session_map.find(sessionId);
		if (iter != m_session_map.end())
		{
			  iter->second->close(sessionId );
		}
	}

	void cwebsocket_server_mgr::show_info()
	{
	}

	bool cwebsocket_server_mgr::_start_listen(const std::string & ip, uint16_t port, uint32_t thread_num)
	{
		cendpoint endpoint(boost::asio::ip::address::from_string(ip), port);

		cerror_code ec;
		m_acceptor_ptr->open(endpoint.protocol(), ec);

		if (ec)
		{
			ERROR_EX_LOG("open socket error");
			//LOG_ERROR << "open socket error " << ec.value();
			return false;
		}


		{
			m_acceptor_ptr->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), ec);

			if (ec)
			{
				ERROR_EX_LOG("set_option reuse_address error,");
				//LOG_ERROR << "set_option reuse_address error, " << ec.value();
				return false;
			}
		}

		m_acceptor_ptr->bind(endpoint, ec);

		if (ec)
		{
			ERROR_EX_LOG("bind error");
			//LOG_ERROR << "bind error, " << ec.value();
			return false;
		}

		m_acceptor_ptr->listen(boost::asio::socket_base::max_connections, ec);

		if (ec)
		{
			//LOG_ERROR << "listen error, " << ec.value() << ", port = " << port;
			ERROR_EX_LOG("listen error , %u", port);
			return false;
		}

		for (uint32_t i = 0; i < thread_num; ++i)
		{
			_post_accept();
		}
		NORMAL_LOG("listen on ip= %s, port = %u", ip.c_str(), port);
		//LOG_INFO << "listen on ip=" << ip << " port=" << port;
		return true;
	}

	void cwebsocket_server_mgr::_post_accept()
	{
		if (  m_shuting)
		{
			return;
		}
		if (!m_acceptor_ptr->is_open())
		{
			// ERROR_EX_LOG();
			return;
		}
		
		m_acceptor_ptr->async_accept(m_socket, std::bind(&cwebsocket_server_mgr::_handle_accept, this , std::placeholders::_1) );

	}

	void cwebsocket_server_mgr::_handle_accept(const boost::system::error_code& ec)
	{


		if (ec)
		{
		//	fail(ec, "accept");
			//ERROR_EX_LOG("");
		}
		else
		{

			std::string sClientIp = m_socket.remote_endpoint().address().to_string();
			uint16 uiClientPort = m_socket.remote_endpoint().port();
			// Create the session and run it
			cwebsocket_session* session_ptr = new cwebsocket_session(this, m_client_seeesion, std::move(m_socket));
			 clock_guard lock(m_session_mutex);
			 if (!session_ptr)
			 {
				 // error alloc failed !!!
			 }
			 else if ( !m_session_map.insert(std::make_pair(m_client_seeesion, session_ptr)).second)
			 {
				 // warning  install failed !!!!

			 }
			 if (session_ptr)
			 {
				 cwebsocket_msg * msg_ptr = new cwebsocket_msg();
				 if (msg_ptr)
				 {

					 msg_ptr->set_msg_id(EMIR_Connect);
					 msg_ptr->set_remote_ip(sClientIp.c_str());
					 msg_ptr->set_remote_port(uiClientPort);
					 msg_ptr->set_session_id(m_client_seeesion);
					 msg_push(msg_ptr);
					 ++m_client_seeesion;

				 }
				 session_ptr->run();
				 

				 

			 }

			
		}
		_post_accept();
	}

	void cwebsocket_server_mgr::_handle_close(cwebsocket_session* session_ptr)
	{

		


		cwebsocket_msg * msg_ptr = new cwebsocket_msg();
		if (!msg_ptr)
		{
			WARNING_EX_LOG(" alloc failed !!!!");
			clock_guard lock(m_session_mutex);
			m_session_map.erase(session_ptr->get_session_id());
			delete session_ptr;
			session_ptr = NULL;
			return;
		}
		msg_ptr->set_msg_id(EMIR_Disconnect);
		msg_ptr->set_session_id(session_ptr->get_session_id());
		msg_push(msg_ptr);
		uint64_t session_id = session_ptr->get_session_id();
		
		
		{
			clock_guard lock(m_session_mutex);
			m_session_map.erase(session_id);
			delete session_ptr;
			 session_ptr = NULL;
		}
		/*
		cnet_msg* msg_ptr = create_msg(EMIR_Disconnect, 0);
		if (!msg_ptr)
		{
			LOG_ERROR << "alloc msg EMIR_Disconnect failed, name=" << get_name();
			post_disconnect(session_ptr);
			return;
		}
		const uint32 session_id = session_ptr->get_session_id();
		msg_ptr->set_session_id(session_id);
		session_ptr->reset();
		msg_push(msg_ptr);
		const uint32 index = get_session_index(session_id);
		if (_is_server_index(index))
		{
			_return_session(session_ptr);
			_post_accept();
		}
		else if (m_reconnect_second > 0)
		{
			//重连
			cconnect_para* para_ptr = m_paras[index];
			para_ptr->busy.store(true);
			session_ptr->set_connect_status();
			para_ptr->timer.expires_from_now(boost::posix_time::seconds(m_reconnect_second));
			para_ptr->timer.async_wait(boost::bind(&cnet_mgr::_handle_reconnect, this, index));
		}
		*/
	}

	void cwebsocket_server_mgr::_worker_thread()
	{
		for (;;)
		{
			try
			{
				m_io_service.run();
				//LOG_INFO << m_name << " thread exit";
				break;
			}
			catch (boost::system::system_error& e)
			{
				//LOG_ERROR << "work thread exception, code = " << e.code().value();
			}
		}
	}
	
	cwebsocket_server_mgr::~cwebsocket_server_mgr()
	{
	}
	bool cwebsocket_server_mgr::init()
	{
		
		m_acceptor_ptr = new cacceptor(m_io_service);
		if (!m_acceptor_ptr)
		{
			//LOG_ERROR << "new wacceptor failed";
			return false;
		}
		m_msgs = new cwebsocket_msg_queue();
		if (!m_msgs)
		{
			// error websocket msg ueue
			return false;
		}
		return true;
	}
}