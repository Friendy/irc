#include "Message.hpp"

/*CONSTRUCTORS*/
Message::Message() : _fdPtr(NULL) {};

Message::Message(std::string msg) : _msg(msg), _fdPtr(NULL), _isquitmsg(EXIST), _isping(0) {};


Message::Message(std::string msg, pollfd *fd) : _msg(msg), _isquitmsg(EXIST), _isping(0)
{
	_fdPtr = fd;
}

//Assignment operator:
Message &Message::operator=(Message const &original)
{
	if (this != &original)
	{
		this->_fdPtr = original._fdPtr;
		this->_msg = original._msg;
		this->_isquitmsg = original._isquitmsg;
		this->_isping = original._isping;
	}
	return(*this);
}

Message::Message(Message const &original)
{
	*this = original;
}

/*FUNCTIONS*/

const std::string Message::getMsg() const
{
	return(this->_msg);
}

void Message::setMsg(std::string msg)
{

	this->_msg = msg;
}

void Message::addFd(pollfd *fd)
{
	_fdPtr = fd;
}

pollfd *Message::getPollfd() const
{
	return(_fdPtr);
}

int Message::isQuitMsg()
{
	return(_isquitmsg);
}

void Message::setQuitMsg(int status)
{
	_isquitmsg = status;
}

int Message::isPing()
{
	return(_isping);
}

void Message::setPing()
{
	_isping = 1;
}


//sending a message to one fd
int Message::sendMsg()
{
	if (!_fdPtr)
    {
        std::cerr << "File descriptor is null" << std::endl;
        return 1;
    }
	addNewLine();
	ssize_t bytes_sent = send(_fdPtr->fd, _msg.data(), _msg.length(), 0);
	if (bytes_sent == -1)
	{
		Err::handler(0, "sending message failed: ", _msg);
		return (0);
	}
	else
		std::cout << "Sent to " << _fdPtr->fd << ": " << _msg;
	return(1);
}



//sending a message to all fds
//before sending we check polling result
// void Message::sendMsg()
// {
// 	std::vector<pollfd *>::iterator it;
// 	std::vector<pollfd *>::iterator save_it;
// 	if (!_fds.empty())
// 	{
// 		for (it = _fds.begin(); it != _fds.end(); ++it)
// 		{
// 			if ((*it)->revents & POLLOUT)
// 			{
// 				if (sendMsg((*it)->fd) == 1)
// 				{
// 					save_it = it;
// 					it++;
// 					_fds.erase(save_it);
// 				}
// 				if (save_it ==_fds.end()) //this can happen if the last element is deleted
// 					break;
// 			}
// 		}
// 	}
// }

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