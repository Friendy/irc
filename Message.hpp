#ifndef MESSAGE_HPP
# define MESSAGE_HPP
# include <string>
# include <iostream>
# include <vector>
# include "User.hpp"
# include "Err.hpp"
# include "defines.hpp"
#include <poll.h>

class Message {

private:
	std::string _msg;
	pollfd* _fdPtr;
	int _isquitmsg;

public:
	const std::string getMsg() const;
	void setMsg(std::string msg);
	void addFd(pollfd *fd);
	int sendMsg();
	void addNewLine();
	pollfd* getPollfd() const;
	int isQuitMsg();
	void setQuitMsg(int status);

	Message();
	Message(std::string msg);
	Message(std::string msg, pollfd *fd);
	Message(Message const &original);
	Message &operator=(Message const &original);
	~Message();
};

#endif