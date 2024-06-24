#include "User.hpp"

/*CONSTRUCTORS*/
// User::User(){}


User::User(int fd) : _fd(fd){};

//Assignment operator:
User &User::operator=(User const &original)
{
	if (this != &original)
	{
		this->_user = original._user;
		this->_nick = original._nick;
	}
	return(*this);
}

User::User(User const &original) : _fd(original._fd)
{
	*this = original;
}

/*FUNCTIONS*/
const std::string User::getUser()
{
	return(this->_user);
}

void User::setUser(std::string user)
{
	this->_user = user;
}

const std::string User::getNick()
{
	return(this->_nick);
}

int User::getFd()
{
	return(this->_fd);
}

void User::setNick(std::string nick)
{
	this->_nick = nick;
}

/*DESTRUCTOR*/
User::~User(){}
