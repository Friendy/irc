#ifndef USER_HPP
# define USER_HPP
# include <string>
# include <iostream>
#include <sys/socket.h>
#include "Command.hpp"
#include <cstring>
#include <netinet/in.h>
#include <poll.h>

class User {

private:
	std::string _user;
	std::string _nick;
	std::string _host;
	std::string _last_msg; //last recieved message
	const int _fd;
	std::string _hostmask;
	struct sockaddr _addr;
	bool _isregistered;
	bool _passgiven;
	struct sockaddr_in _address;
	bool _quitted;
	int _pollInd; //index of user fd in poll array
	pollfd *_fdPtr;//pointer to pollfd structure

public:
	const std::string getUser();
	const std::string getLastMsg();
	void setUser(std::string user);
	void setAddress(struct sockaddr_in addr);
	void setPollInd(int i);
	void setPollPtr(pollfd *pfd);
	const std::string getNick();
	const std::string getHost();
	const std::string getFullName();
	void setNick(std::string nick);
	int getFd();
	int getPollInd();
	// const std::string getMsg(std::string msg);
	void setLastMsg(std::string msg);
	void registerUser();
	bool isRegistered();
	bool passGiven();
	void givePass();
	void quitted();
	bool hasquitted();
	// Command parseMsg();

	// User();
	User(int fd);
	User(int fd, const std::string &hostmask);
	User(User const &original);
	User &operator=(User const &original);
	~User();
};

#endif
