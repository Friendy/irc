#ifndef MESSAGE_HPP
# define MESSAGE_HPP
# include <string>
# include <iostream>
# include <vector>
# include "User.hpp"
# include "Err.hpp"
#include <poll.h>

class Message {

private:
	std::string _msg;
	// std::vector<pollfd *> _fds;
	pollfd* _fdPtr;

public:
	const std::string getMsg() const;
	void setMsg(std::string msg);
	// std::vector<int> getFds();
	// void addFd(int fd);
	void addFd(pollfd *fd);
	int sendMsg();
	void addNewLine();
	pollfd* getPollfd() const;

	Message();
	Message(std::string msg);
	Message(std::string msg, pollfd *fd);
	Message(Message const &original);
	Message &operator=(Message const &original);
	~Message();
};

#endif