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
	std::vector<pollfd *> _fds;
	bool _repushed;

public:
	const std::string getMsg();
	void setMsg(std::string msg);
	// std::vector<int> getFds();
	// void addFd(int fd);
	void addFd(pollfd *fd);
	int sendMsg(int fd);
	void sendMsg();
	void addNewLine();
	bool sendIncomplete();
	void setrepushed(bool repushed);
	bool isrepushed();

	Message();
	Message(std::string msg);
	Message(std::string msg, pollfd *fd);
	Message(Message const &original);
	Message &operator=(Message const &original);
	~Message();
};

#endif
