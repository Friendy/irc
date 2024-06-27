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
#include <vector>
#include <map>
#include "Err.hpp"
#include "Channel.hpp"
#include "User.hpp"
#include <stdlib.h>

class IrcServ {

private:
	std::vector<int> _fds;
	std::map<const int, User *> _users;
	struct sockaddr _dest_addr;
	int _listenfd;
	std::string _pass;

	void create_hint(struct addrinfo *hint);
	void setopt(int *sockfd, int level, int option, int optval);
	void send_msg(int fd, std::string msg);
	void send_msg(std::string msg);
	void setupSocket(const char* protname, long port_tmp);

public:
	// int getData() const;
	// void setData(int data);
	void server_start(const char* protname, const char* port, const char* hostname);
	void print_fds();
	void print_users();
	void accept_client();

	IrcServ();
	IrcServ(std::string pass);
	IrcServ(IrcServ const &original);
	IrcServ &operator=(IrcServ const &original);
	~IrcServ();
};

#endif
