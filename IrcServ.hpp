#ifndef IRCSERV_HPP
# define IRCSERV_HPP
# include <string>
# include <iostream>
#include <netdb.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
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
#include <sys/stat.h>
#include <stdlib.h>
#include <cstring>
#include <cerrno>
#include <poll.h>

class IrcServ {

private:
	/* For now each user is a connection
	and this connection's fd is used as a unique key
	in case it's not possible another unique key will be introduced
	 */
	std::map<const int, User *> _users;
	int _listenfd;
	std::string _pass;
	pollfd	_userPoll[SOMAXCONN];
	nfds_t 	_activePoll; 

	void create_hint(struct addrinfo *hint);
	void send_msg(int fd, std::string msg);
	void send_msg(std::string msg);
	void setupSocket(const char* protname, long port_tmp, struct addrinfo *addr_info);

public:
	// int getData() const;
	// void setData(int data);
	void server_start(const char* protname, const char* port, const char* hostname);
	void accept_client();
	void print_fds();
	void print_users();

	IrcServ();
	IrcServ(std::string pass);
	IrcServ(IrcServ const &original);
	IrcServ &operator=(IrcServ const &original);
	~IrcServ();
};

#endif
