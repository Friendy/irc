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
	std::map<const std::string, std::string (IrcServ::*)(std::vector<std::string> params, User &user)> _commands;
	int _listenfd;
	std::string _pass;
	pollfd	_userPoll[SOMAXCONN];
	nfds_t 	_activePoll;

	//TODO: implement send queue
	//TODO: update introductory message: should contain:
	//TODO: user count, sever name and version
	//TODO: nick history


	/* ******Connection related functions********** */
	void setupSocket(const char* protname, long port_tmp, struct addrinfo *addr_info);

	/* ******Message sending functions ********** */
	void send_msg(int fd, std::string msg);
	void send_msg(std::string msg);

	/* ******General message processing functions********** */
	std::string processMsg(User &user, std::string msg);
	Command parseMsg(const std::string msg);

	/* ******Command functions****** */
	std::string fPass(std::vector<std::string> params, User &user);
	std::string fUser(std::vector<std::string> params, User &user);
	std::string fNick(std::vector<std::string> params, User &user);

	/* ******Helper functions****** */
	void create_hint(struct addrinfo *hint);
	std::string addNewLine(std::string &str);
	void trimMsg(std::string &msg);
	std::string get_next_word(std::string str, size_t start);

public:
	void recieve_msg();

	/* ******Connection related functions********** */
	void server_start(const char* protname, const char* port, const char* hostname);
	void accept_client();

	/* ******Helper functions****** */
	void print_fds();
	void print_users();

	IrcServ();
	IrcServ(std::string pass);
	IrcServ(IrcServ const &original);
	IrcServ &operator=(IrcServ const &original);
	~IrcServ();
};

#endif
