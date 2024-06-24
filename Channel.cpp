#include "Channel.hpp"

/*CONSTRUCTORS*/
Channel::Channel()
{
	std::cout << "class created" << std::endl;
}

//Assignment operator:
Channel &Channel::operator=(Channel const &original)
{
	if (this != &original)
	{
		this->_name = original._name;
		this->_users = original._users;
	}
	return(*this);
}

Channel::Channel(Channel const &original)
{
	*this = original;
}

/*FUNCTIONS*/
const std::string Channel::getName()
{
	return(this->_name);
}

void Channel::setName(std::string name)
{
	this->_name = name;
}

const std::vector<User> Channel::getUsers()
{
	return(this->_users);
}

void Channel::setUsers(std::vector<User> users)
{
	this->_users = users;
}

/*DESTRUCTOR*/
Channel::~Channel(){}
