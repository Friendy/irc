#ifndef MESSAGE_HPP
# define MESSAGE_HPP
# include <string>
# include <iostream>
# include <vector>
# include "User.hpp"
# include "Err.hpp"

class Message {

private:
	std::string _msg;
	std::vector<int> _fds;

public:
	const std::string getMsg();
	void setMsg(std::string msg);
	// std::vector<int> getFds();
	void addFd(int fd);
	void sendMsg(int fd);
	void addNewLine();
	// void sendToAll();

	Message();
	Message(std::string msg);
	Message(std::string msg, int fd);
	Message(Message const &original);
	Message &operator=(Message const &original);
	~Message();
};

#endif
