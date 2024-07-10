#include "Message.hpp"

/*CONSTRUCTORS*/
Message::Message(){};

Message::Message(std::string msg) : _msg(msg){};

Message::Message(std::string msg, int fd) : _msg(msg)
{
	_fds.push_back(fd);
}

//Assignment operator:
Message &Message::operator=(Message const &original)
{
	if (this != &original)
	{
		this->_fds = original._fds;
		this->_msg = original._msg;
	}
	return(*this);
}

Message::Message(Message const &original)
{
	*this = original;
}

/*FUNCTIONS*/

const std::string Message::getMsg()
{
	return(this->_msg);
}

void Message::setMsg(std::string msg)
{
	this->_msg = msg;
}

void Message::addFd(int fd)
{
	_fds.push_back(fd);
}

//sending a message to one fd
void Message::sendMsg(int fd)
{
	addNewLine();
	ssize_t bytes_sent = send(fd, _msg.data(), _msg.length(), 0);
	if (bytes_sent == -1)
		Err::handler(0, "sending message failed: ", _msg);
	else
		std::cout << "Sent: " << _msg << "\n";
}

void Message::addNewLine()
{
	_msg.insert(_msg.end(), '\n');
}

//sending a message to all fds
// void Message::sendToAll()
// {
// 	ssize_t bytes_sent = send(fd, _msg.data(), _msg.length(), 0);
// 	if (bytes_sent == -1)
// 		Err::handler(1, "sending message failed: ", _msg);
// 	else
// 		std::cout << "Sent: " << _msg << "\n";
// }

/*DESTRUCTOR*/
Message::~Message(){}
