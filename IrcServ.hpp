#ifndef IRCSERV_HPP
# define IRCSERV_HPP
# include <string>
# include <iostream>
#include <netdb.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <err.h>
#include <string>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include "Err.hpp"

class IrcServ {

private:
	int _data;

	void create_hint(struct addrinfo *hint);
	void send_msg(int *fd, std::string msg);
	void setopt(int *sockfd, int level, int option, int optval);

public:
	int getData() const;
	void setData(int data);
	void server_start(const char* protname, const char* port, const char* hostname);

	IrcServ();
	IrcServ(IrcServ const &original);
	IrcServ &operator=(IrcServ const &original);
	~IrcServ();
};

#endif
