#include "Message.hpp"

/*CONSTRUCTORS*/
Message::Message(){};

Message::Message(std::string msg) : _msg(msg), _repushed(false){};

Message::Message(std::string msg, pollfd *fd) : _msg(msg), _repushed(false)
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
		this->_repushed = original._repushed;
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

void Message::addFd(pollfd *fd)
{
	_fds.push_back(fd);
}

//sending a message to one fd
int Message::sendMsg(int fd)
{
	addNewLine();
	ssize_t bytes_sent = send(fd, _msg.data(), _msg.length(), 0);
	if (bytes_sent == -1)
	{
		Err::handler(0, "sending message failed: ", _msg);
		return (0);
	}
	else
		std::cout << "Sent to " << fd << ": " << _msg << "\n";
	return(1);
}

//sending a message to all fds
//before sending we check polling result
void Message::sendMsg()
{
	std::vector<pollfd *>::iterator it;
	std::vector<pollfd *>::iterator save_it;
	if (!_fds.empty())
	{
		for (it = _fds.begin(); it != _fds.end(); ++it)
		{
			if ((*it)->revents & POLLOUT)
			{
				if (sendMsg((*it)->fd) == 1)
				{
					save_it = it;
					it++;
					_fds.erase(save_it);
				}
				if (save_it ==_fds.end()) //this can happen if the last element is deleted
					break;
			}
		}
	}
}

void Message::addNewLine()
{
	_msg.insert(_msg.end(), '\n');
}

//if the message is sent not to all its fds it returns true
bool Message::sendIncomplete()
{
	return (!_fds.empty());
}

void Message::setrepushed(bool repushed)
{
	_repushed = repushed;
}

bool Message::isrepushed()
{
	return(_repushed);
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
