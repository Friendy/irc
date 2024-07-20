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
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include <vector>
#include <map>
#include "Err.hpp"
#include "Channel.hpp"
#include "User.hpp"
#include "defines.hpp"
#include <sys/stat.h>
#include <stdlib.h>
#include <cstring>
#include <cerrno>
#include <poll.h>
#include <queue>
#include "defines.hpp"
#include "Message.hpp"

class IrcServ {

private:
	/* For now each user is a connection
	and this connection's fd is used as a unique key
	in case it's not possible another unique key will be introduced
	 */
	std::map<const int, User *> _users;
	std::map<const std::string, std::string (IrcServ::*)(std::vector<std::string> params, User &user)> _commands;
	std::map<const std::string, int> _nicks;
	std::map<const int, std::string> _codes;
	std::queue<Message> _msgQ;
	int _listenfd;
	const std::string _server_name;
	std::string _pass;
	pollfd	_userPoll[SOMAXCONN];
	nfds_t 	_activePoll;
	std::map<std::string, Channel*> _channels;
	std::queue<int> _actionQ;
	nfds_t 	_startInd;
	nfds_t 	_curRecvFd;


	//TODO: implement send queue
	//TODO: update introductory message: should contain:
	//TODO: user count, sever name and version
	//TODO: nick history
	//TODO PING nick

	/* 
What needs to be done

	1. polling
	2. send queue
	3. nick history
	4. parsing
	commands
	Registration:
	PASS
	USER
	NICK
	Channels:
	JOIN
	KICK
	INVITE
	TOPIC
	Messaging:
	PRIVMSG

	Quiting:
	PART
	QUIT

	Ctrl D for incomplete messages

	 */


	/* ******Connection related functions********** */
	void setupSocket(const char* protname, long port_tmp, struct addrinfo *addr_info);

	/* ******Message sending functions ********** */
	//void send_msg(int fd, std::string msg);
	//void send_msg(std::string msg);
	void sendQueue();
	std::string buildPriv(const std::string msg, std::string from, std::string to);
	std::string welcome(User user);
	std::string buildNotice(const std::string msg, int code);
	std::string buildNoticetest(const std::string msg, int code, User &user, std::string channelName);

	/* ******General message processing functions********** */
	// std::string processMsg(User &user, std::string msg);
	Message processMsg(User &user, std::string msg);
	Command parseMsg(const std::string msg);
	//void delete_user(std::map<const int, User *>::iterator &it);
	void delete_user(User *user);

	/* ******Command functions****** */
	std::string fPass(std::vector<std::string> params, User &user);
	std::string fUser(std::vector<std::string> params, User &user);
	std::string fNick(std::vector<std::string> params, User &user);
	std::string fPing(std::vector<std::string> params, User &user);
	std::string fQuit(std::vector<std::string> params, User &user);
	std::string fPriv(std::vector<std::string> params, User &user);
	std::string fUnknown(std::vector<std::string> params, User &user);
	std::string fjoin(std::vector<std::string> params, User &user);
	std::string fMode(std::vector<std::string> params, User &user);
	std::string fPart(std::vector<std::string> params, User &user);
	std::string fKick(std::vector<std::string> params, User &user);
	std::string fTopic(std::vector<std::string> params, User &user);
	std::string fInvite(std::vector<std::string> params, User &user);
	/* ******Helper functions****** */
	void create_hint(struct addrinfo *hint);
	void trimMsg(std::string &msg);
	std::string get_next_word(std::string str, size_t &start);
	pollfd *getPollfd(std::string nick);
	pollfd *getFirstSend();
	int getAction();
	bool readyToAction(int action);
	void check_user();
	void setRecvFd();

public:
	void recieve_msg();
	void sendToChannel(std::map<User, std::string>* resp, const Channel& channel, const std::string& message, User &user);
	void addResponse(std::map<User, std::string>* resp, const User& receiver, const std::string& respMessage);
	std::string listChannelUsers(const std::string &channelName);

	/* ******Connection related functions********** */
	void server_start(const char* protname, const char* port, const char* hostname);
	void accept_client();
	void addToPoll(int fd);

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
