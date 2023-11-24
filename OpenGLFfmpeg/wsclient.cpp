﻿/***********************************************************************************************
created: 		2022-01-20

author:			chensong

purpose:		assertion macros
'输赢不重要，答案对你们有什么意义才重要。

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
//#include "clog.h"
#include "cbase64.h"
#ifdef _WIN32
    #if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
        #define _CRT_SECURE_NO_WARNINGS // _CRT_SECURE_NO_WARNINGS for sscanf errors in MSVC2013 Express
    #endif
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <fcntl.h>
    #include <WinSock2.h>
    #include <WS2tcpip.h>
    #pragma comment( lib, "ws2_32" )
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/types.h>
    #include <io.h>
    #ifndef _SSIZE_T_DEFINED
        typedef int ssize_t;
        #define _SSIZE_T_DEFINED
    #endif
    #ifndef _SOCKET_T_DEFINED
        typedef SOCKET socket_t;
        #define _SOCKET_T_DEFINED
    #endif
    #ifndef snprintf
        #define snprintf _snprintf_s
    #endif
    #if _MSC_VER >=1600
        // vs2010 or later
        #include <stdint.h>
    #else
        typedef __int8 int8_t;
        typedef unsigned __int8 uint8_t;
        typedef __int32 int32_t;
        typedef unsigned __int32 uint32_t;
        typedef __int64 int64_t;
        typedef unsigned __int64 uint64_t;
    #endif
    #define socketerrno WSAGetLastError()
    #define SOCKET_EAGAIN_EINPROGRESS WSAEINPROGRESS
    #define SOCKET_EWOULDBLOCK WSAEWOULDBLOCK
#else
    #include <fcntl.h>
    #include <netdb.h>
    #include <netinet/tcp.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/socket.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <stdint.h>
    #ifndef _SOCKET_T_DEFINED
        typedef int socket_t;
        #define _SOCKET_T_DEFINED
    #endif
    #ifndef INVALID_SOCKET
        #define INVALID_SOCKET (-1)
    #endif
    #ifndef SOCKET_ERROR
        #define SOCKET_ERROR   (-1)
    #endif
    #define closesocket(s) ::close(s)
    #include <errno.h>
    #define socketerrno errno
    #define SOCKET_EAGAIN_EINPROGRESS EAGAIN
    #define SOCKET_EWOULDBLOCK EWOULDBLOCK
#endif

#include <vector>
#include <string>
//#include "rtc_base/logging.h"
#include "wsclient.h"
#include <iostream>
//#include "cint2str.h"
namespace { // private module-only namespace

using wsclient::WebSocketCallback;

socket_t hostname_connect(const std::string& hostname, int port) {
    struct addrinfo hints;
    struct addrinfo *result;
    struct addrinfo *p;
    int ret;
    socket_t sockfd = INVALID_SOCKET;
    char sport[16];
    ::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;
	/*
	hints.ai_family = AF_INET;     /* Allow IPv4 */
	//hints.ai_flags = AI_PASSIVE;/* For wildcard IP address */
	//hints.ai_protocol = 0;         /* Any protocol */
	//hints.ai_socktype = SOCK_STREAM;
	//*/
    snprintf(sport, 16, "%d", port);
    if ((ret = getaddrinfo(hostname.c_str(), sport, &hints, &result)) != 0)
    {
      ::fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
      return 1;
    }
    for(p = result; p != NULL; p = p->ai_next)
    {
        sockfd = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sockfd == INVALID_SOCKET)
		{ 
			continue; 
		}
        if (::connect(sockfd, p->ai_addr, p->ai_addrlen) != SOCKET_ERROR) 
		{
            break;
        }
        ::closesocket(sockfd);
        sockfd = INVALID_SOCKET;
    }
    ::freeaddrinfo(result);
    return sockfd;
}


class _DummyWebSocket : public wsclient::WebSocket
{
  public:
    void poll(int timeout) { }
    void send(const std::string& message) { }
	void send(const char* message) { }
    void sendBinary(const std::string& message) { }
    void sendBinary(const std::vector<uint8_t>& message) { }
	void sendBinary(const uint8_t* message, size_t len) {}
    void sendPing() { }
    void close() { } 
    readyStateValues getReadyState() const { return CLOSED; }
    void dispatch(WebSocketCallback* callable) { }
};


class _RealWebSocket : public wsclient::WebSocket
{
  public:
    // http://tools.ietf.org/html/rfc6455#section-5.2  Base Framing Protocol
    //
    //  0                   1                   2                   3
    //  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    // +-+-+-+-+-------+-+-------------+-------------------------------+
    // |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
    // |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
    // |N|V|V|V|       |S|             |   (if payload len==126/127)   |
    // | |1|2|3|       |K|             |                               |
    // +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
    // |     Extended payload length continued, if payload len == 127  |
    // + - - - - - - - - - - - - - - - +-------------------------------+
    // |                               |Masking-key, if MASK set to 1  |
    // +-------------------------------+-------------------------------+
    // | Masking-key (continued)       |          Payload Data         |
    // +-------------------------------- - - - - - - - - - - - - - - - +
    // :                     Payload Data continued ...                :
    // + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
    // |                     Payload Data continued ...                |
    // +---------------------------------------------------------------+
    struct wsheader_type {
        unsigned header_size;
        bool fin;
        bool mask;
        enum opcode_type {
            CONTINUATION = 0x0,
            TEXT_FRAME = 0x1,
            BINARY_FRAME = 0x2,
            CLOSE = 8,
            PING = 9,
            PONG = 0xa,
        } opcode;
        int N0;
        uint64_t N;
        uint8_t masking_key[4];
    };

    std::vector<uint8_t> rxbuf;
    std::vector<uint8_t> txbuf;
    std::vector<uint8_t> receivedData;

    socket_t sockfd;
    readyStateValues readyState;
    bool useMask;

    _RealWebSocket(socket_t sockfd, bool useMask) : sockfd(sockfd), readyState(OPEN), useMask(useMask) {
    }

    readyStateValues getReadyState() const {
      return readyState;
    }

    void poll(int timeout) { // timeout in milliseconds
        if (readyState == CLOSED) {
            if (timeout > 0) {
                timeval tv = { timeout/1000, (timeout%1000) * 1000 };
                select(0, NULL, NULL, NULL, &tv);
            }
            return;
        }
        if (timeout != 0) {
            fd_set rfds;
            fd_set wfds;
            timeval tv = { timeout/1000, (timeout%1000) * 1000 };
            FD_ZERO(&rfds);
            FD_ZERO(&wfds);
            FD_SET(sockfd, &rfds);
            if (txbuf.size()) { FD_SET(sockfd, &wfds); }
            select(sockfd + 1, &rfds, &wfds, 0, timeout > 0 ? &tv : 0);
        }
        while (true) {
            // FD_ISSET(0, &rfds) will be true
            int N = rxbuf.size();
            ssize_t ret;
            rxbuf.resize(N + 1500);
            ret = recv(sockfd, (char*)&rxbuf[0] + N, 1500, 0);

            if (ret < 0 && (socketerrno == SOCKET_EWOULDBLOCK || socketerrno == SOCKET_EAGAIN_EINPROGRESS)) {
                rxbuf.resize(N);
                break;
            }
            else if (ret <= 0) {
                rxbuf.resize(N);
                closesocket(sockfd);
                readyState = CLOSED;
                fputs(ret < 0 ? "Connection error!\n" : "Connection closed!\n", stderr);
                break;
            }
            else {
                rxbuf.resize(N + ret);
            }

			//std::ostringstream cmd;
			//std::ostringstream strcmd;
			//for (const  uint8_t& value : rxbuf)
			//{
			//	//printf("%2X")
			//	cmd <<", " << webrtc::hexmem(&value, sizeof(uint8_t)) ;
			//	strcmd << value;
			//	//std::hex(value);
			//}

			//RTC_LOG(LS_INFO) << "websocket recv data = " << cmd.str().c_str() << ", recv data size = " << ret ;
			//RTC_LOG(LS_INFO) << "websocket [recv data = " << strcmd.str().c_str() << "]";

        }
        while (txbuf.size()) {
            int ret = ::send(sockfd, (char*)&txbuf[0], txbuf.size(), 0);

			{
				using namespace chen;
				//NORMAL_EX_LOG("io websocket send len = %u", ret);
			}
            if (false) { } // ??
            else if (ret < 0 && (socketerrno == SOCKET_EWOULDBLOCK || socketerrno == SOCKET_EAGAIN_EINPROGRESS)) {
                break;
            }
            else if (ret <= 0) {
                closesocket(sockfd);
                readyState = CLOSED;
                fputs(ret < 0 ? "Connection error!\n" : "Connection closed!\n", stderr);
                break;
            }
            else {
                txbuf.erase(txbuf.begin(), txbuf.begin() + ret);
            }
        }
        if (!txbuf.size() && readyState == CLOSING) {
            closesocket(sockfd);
            readyState = CLOSED;
			/*if (callable)
			{
				callable->OnClose();
			}*/
        }
    }

    virtual void dispatch(WebSocketCallback* callable) {
        // TODO: consider acquiring a lock on rxbuf...
        while (true) {
            wsheader_type ws;
            if (rxbuf.size() < 2) { return; /* Need at least 2 */ }
            const uint8_t * data = (uint8_t *) &rxbuf[0]; // peek, but don't consume
            ws.fin = (data[0] & 0x80) == 0x80;
            ws.opcode = (wsheader_type::opcode_type) (data[0] & 0x0f);
            ws.mask = (data[1] & 0x80) == 0x80;
            ws.N0 = (data[1] & 0x7f);
            ws.header_size = 2 + (ws.N0 == 126? 2 : 0) + (ws.N0 == 127? 8 : 0) + (ws.mask? 4 : 0);
            if (rxbuf.size() < ws.header_size) { return; /* Need: ws.header_size - rxbuf.size() */ }
            int i = 0;
            if (ws.N0 < 126) {
                ws.N = ws.N0;
                i = 2;
            }
            else if (ws.N0 == 126) {
                ws.N = 0;
                ws.N |= ((uint64_t) data[2]) << 8;
                ws.N |= ((uint64_t) data[3]) << 0;
                i = 4;
            }
            else if (ws.N0 == 127) {
                ws.N = 0;
                ws.N |= ((uint64_t) data[2]) << 56;
                ws.N |= ((uint64_t) data[3]) << 48;
                ws.N |= ((uint64_t) data[4]) << 40;
                ws.N |= ((uint64_t) data[5]) << 32;
                ws.N |= ((uint64_t) data[6]) << 24;
                ws.N |= ((uint64_t) data[7]) << 16;
                ws.N |= ((uint64_t) data[8]) << 8;
                ws.N |= ((uint64_t) data[9]) << 0;
                i = 10;
            }
            if (ws.mask) {
                ws.masking_key[0] = ((uint8_t) data[i+0]) << 0;
                ws.masking_key[1] = ((uint8_t) data[i+1]) << 0;
                ws.masking_key[2] = ((uint8_t) data[i+2]) << 0;
                ws.masking_key[3] = ((uint8_t) data[i+3]) << 0;
            }
            else {
                ws.masking_key[0] = 0;
                ws.masking_key[1] = 0;
                ws.masking_key[2] = 0;
                ws.masking_key[3] = 0;
            }
            if (rxbuf.size() < ws.header_size+ws.N) { return; /* Need: ws.header_size+ws.N - rxbuf.size() */ }

            // We got a whole message, now do something with it:
            if (false) { }
            else if (
                   ws.opcode == wsheader_type::TEXT_FRAME 
                || ws.opcode == wsheader_type::BINARY_FRAME
                || ws.opcode == wsheader_type::CONTINUATION
            ) {
                if (ws.mask) { for (size_t i = 0; i != ws.N; ++i) { rxbuf[i+ws.header_size] ^= ws.masking_key[i&0x3]; } }
                receivedData.insert(receivedData.end(), rxbuf.begin()+ws.header_size, rxbuf.begin()+ws.header_size+(size_t)ws.N);// just feed
                if (ws.fin) {

					if (ws.opcode == wsheader_type::TEXT_FRAME)
					{
						std::string stringMessage(receivedData.begin(), receivedData.end());
						callable->OnMessage(stringMessage);
					}
					else
					{
						callable->OnMessage(receivedData);
					}
                    receivedData.erase(receivedData.begin(), receivedData.end());
                    std::vector<uint8_t> ().swap(receivedData);// free memory
                }
            }
            else if (ws.opcode == wsheader_type::PING) {
                if (ws.mask) { for (size_t i = 0; i != ws.N; ++i) { rxbuf[i+ws.header_size] ^= ws.masking_key[i&0x3]; } }
                std::string data(rxbuf.begin()+ws.header_size, rxbuf.begin()+ws.header_size+(size_t)ws.N);
                sendData(wsheader_type::PONG,(uint8_t*)data.c_str(),data.length());
            }
            else if (ws.opcode == wsheader_type::PONG) { }
            else if (ws.opcode == wsheader_type::CLOSE) { close(); }
            else { fprintf(stderr, "ERROR: Got unexpected WebSocket message.\n"); close(); }

            rxbuf.erase(rxbuf.begin(), rxbuf.begin() + ws.header_size+(size_t)ws.N);
        }
    }

    void sendPing() {
		const char* empty = "";
        sendData(wsheader_type::PING,(uint8_t*)empty, 0);
    }

    void send(const std::string& message) {
        sendData(wsheader_type::TEXT_FRAME, (uint8_t*)message.c_str(), message.size());
    }

	void send(const char* message) {
		sendData(wsheader_type::TEXT_FRAME, (uint8_t*)message,strlen(message));
	}

    void sendBinary(const std::string& message) {
        sendData(wsheader_type::BINARY_FRAME, (uint8_t*)message.c_str(), message.size());
    }

    void sendBinary(const std::vector<uint8_t>& message) {
        sendData(wsheader_type::BINARY_FRAME, &message[0], message.size());
    }

	void sendBinary(const uint8_t* message_begin, size_t message_len) {
		sendData(wsheader_type::BINARY_FRAME, message_begin, message_len);
	}

	void sendData(wsheader_type::opcode_type type,const uint8_t* message_begin, size_t message_len) {
		// TODO:
		// Masking key should (must) be derived from a high quality random
		// number generator, to mitigate attacks on non-WebSocket friendly
		// middleware:
		const uint8_t masking_key[4] = { 0x12, 0x34, 0x56, 0x78 };



		// TODO: consider acquiring a lock on txbuf...
		if (readyState == CLOSING || readyState == CLOSED) { return; }
		std::vector<uint8_t> header;
		header.assign(2 + (message_len >= 126 ? 2 : 0) + (message_len >= 65536 ? 6 : 0) + (useMask ? 4 : 0), 0);
		header[0] = 0x80 | type;
		if (false) {}
		else if (message_len < 126) {
			header[1] = (message_len & 0xff) | (useMask ? 0x80 : 0);
			if (useMask) {
				header[2] = masking_key[0];
				header[3] = masking_key[1];
				header[4] = masking_key[2];
				header[5] = masking_key[3];
			}
		}
		else if (message_len < 65536) {
			header[1] = 126 | (useMask ? 0x80 : 0);
			header[2] = (message_len >> 8) & 0xff;
			header[3] = (message_len >> 0) & 0xff;
			if (useMask) {
				header[4] = masking_key[0];
				header[5] = masking_key[1];
				header[6] = masking_key[2];
				header[7] = masking_key[3];
			}
		}
		else { // TODO: run coverage testing here
			header[1] = 127 | (useMask ? 0x80 : 0);
			header[2] = (message_len >> 56) & 0xff;
			header[3] = (message_len >> 48) & 0xff;
			header[4] = (message_len >> 40) & 0xff;
			header[5] = (message_len >> 32) & 0xff;
			header[6] = (message_len >> 24) & 0xff;
			header[7] = (message_len >> 16) & 0xff;
			header[8] = (message_len >> 8) & 0xff;
			header[9] = (message_len >> 0) & 0xff;
			if (useMask) {
				header[10] = masking_key[0];
				header[11] = masking_key[1];
				header[12] = masking_key[2];
				header[13] = masking_key[3];
			}
		}
		// N.B. - txbuf will keep growing until it can be transmitted over the socket:
		txbuf.insert(txbuf.end(), header.begin(), header.end());

		//write data
		size_t offset = txbuf.size();
		txbuf.resize(offset+message_len);
		for (size_t i = 0; i< message_len; ++i)
		{
			txbuf[offset+i] = message_begin[i];
		}

		if (useMask) {
			size_t message_offset = txbuf.size() - message_len;
			for (size_t i = 0; i != message_len; ++i) {
				txbuf[message_offset + i] ^= masking_key[i & 0x3];
			}
		}
	}


    void close() {
        if(readyState == CLOSING || readyState == CLOSED) { return; }
        readyState = CLOSING;
        uint8_t closeFrame[6] = {0x88, 0x80, 0x00, 0x00, 0x00, 0x00}; // last 4 bytes are a masking key
        std::vector<uint8_t> header(closeFrame, closeFrame+6);
        txbuf.insert(txbuf.end(), header.begin(), header.end());
    }

};


wsclient::WebSocket::pointer from_url(const std::string& url, bool useMask, const std::string& origin) {
    char host[128];
    int port;
	using namespace chen;
	//NORMAL_EX_LOG("url = %s", url.c_str());
    char path[128];
    if (url.size() >= 128) {
      fprintf(stderr, "ERROR: url size limit exceeded: %s\n", url.c_str());
      return NULL;
    }
    if (origin.size() >= 200) {
      fprintf(stderr, "ERROR: origin size limit exceeded: %s\n", origin.c_str());
      return NULL;
    }
    if (false) { }
    else if (sscanf(url.c_str(), "ws://%[^:/]:%d/%s", host, &port, path) == 3) {
    }
    else if (sscanf(url.c_str(), "ws://%[^:/]/%s", host, path) == 2) {
        port = 80;
    }
    else if (sscanf(url.c_str(), "ws://%[^:/]:%d", host, &port) == 2) {
        path[0] = '\0';
    }
    else if (sscanf(url.c_str(), "ws://%[^:/]", host) == 1) {
        port = 80;
        path[0] = '\0';
    }
    else {
        fprintf(stderr, "ERROR: Could not parse WebSocket url: %s\n", url.c_str());
        return NULL;
    }
    //fprintf(stderr, "wsclient: connecting: host=%s port=%d path=/%s\n", host, port, path);
    socket_t sockfd = hostname_connect(host, port);
    if (sockfd == INVALID_SOCKET) {
        fprintf(stderr, "Unable to connect to %s:%d\n", host, port);
        return NULL;
    }
    {
		 
        // XXX: this should be done non-blocking,
		char line[256] = {0};
        int status;
        int i;
        snprintf(line, 256, "GET /%s HTTP/1.1\r\n", path); 
		::send(sockfd, line, strlen(line), 0);
		 
        if (port == 80) {
            snprintf(line, 256, "Host: %s\r\n", host); 
			::send(sockfd, line, strlen(line), 0);
        }
        else {
            snprintf(line, 256, "Host: %s:%d\r\n", host, port); 
			::send(sockfd, line, strlen(line), 0);

        }
		const char * user_agent = "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/95.0.4638.69 Safari/537.36\r\n";

		::send(sockfd, user_agent, strlen(user_agent), 0);
		 
        snprintf(line, 256, "Upgrade: websocket\r\n"); 
		::send(sockfd, line, strlen(line), 0);
		 
        snprintf(line, 256, "Connection: Upgrade\r\n"); 
		::send(sockfd, line, strlen(line), 0);
		 

        if (!origin.empty()) {
            snprintf(line, 256, "Origin: %s\r\n", origin.c_str()); ::send(sockfd, line, strlen(line), 0);
			 
        }
        //snprintf(line, 256, "Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==\r\n");
		std::string websocketkey = chen::base64_encode(std::to_string(::time(NULL)));
		snprintf(line, 256, "Sec-WebSocket-Key: %s==\r\n", websocketkey.c_str());
		::send(sockfd, line, strlen(line), 0);
		 
        snprintf(line, 256, "Sec-WebSocket-Version: 13\r\n"); 
		::send(sockfd, line, strlen(line), 0);
		 
      

		const char * weocketproto = "Sec-WebSocket-Protocol: protoo\r\n";
		::send(sockfd, weocketproto, strlen(weocketproto), 0);
		 

		snprintf(line, 256, "\r\n"); 
		::send(sockfd, line, strlen(line), 0);
		 
        for (i = 0; i < 2 || (i < 255 && line[i-2] != '\r' && line[i-1] != '\n'); ++i) 
		{

			int ret = ::recv(sockfd, line + i, 1, 0);

			if ( ret == 0) 
			{ 
				return NULL; 
			} 
		}
        line[i] = 0;
		 
        if (i == 255)
		{
			fprintf(stderr, "ERROR: Got invalid status line connecting to: %s\n", url.c_str()); 
			return NULL; 
		}
        if (sscanf(line, "HTTP/1.1 %d", &status) != 1 || status != 101) 
		{
			fprintf(stderr, "ERROR: Got bad status connecting to %s: %s", url.c_str(), line); 
			return NULL; 
		}
        // TODO: verify response headers,
        while (true) {
            for (i = 0; i < 2 || (i < 255 && line[i-2] != '\r' && line[i-1] != '\n'); ++i) { if (recv(sockfd, line+i, 1, 0) == 0) { return NULL; } }
            if (line[0] == '\r' && line[1] == '\n') { break; }
        }
    }
    int flag = 1;
    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*) &flag, sizeof(flag)); // Disable Nagle's algorithm
#ifdef _WIN32
    u_long on = 1;
    ioctlsocket(sockfd, FIONBIO, &on);
#else
    fcntl(sockfd, F_SETFL, O_NONBLOCK);
#endif
    //fprintf(stderr, "Connected to: %s\n", url.c_str());
    return wsclient::WebSocket::pointer(new _RealWebSocket(sockfd, useMask));
}

} // end of module-only namespace



namespace wsclient {

WebSocket::pointer WebSocket::create_dummy() {
    static pointer dummy = pointer(new _DummyWebSocket);
    return dummy;
}


WebSocket::pointer WebSocket::from_url(const std::string& url, const std::string& origin) {
    return ::from_url(url, true, origin);
}

WebSocket::pointer WebSocket::from_url_no_mask(const std::string& url, const std::string& origin) {
    return ::from_url(url, false, origin);
}


} // namespace wsclient
