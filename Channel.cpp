#include "Channel.hpp"

/*CONSTRUCTORS*/
Channel::Channel()
{
	std::cout << "class created" << std::endl;
}

Channel::Channel(const std::string &name){
	//std::cout << "class created" << std::endl;
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

const std::vector<User*>& Channel::getUsers() const {
    return _users;
}

void Channel::setUsers(const std::vector<User*>& users) {
    _users = users;
}

void Channel::addUser(User &user) {
    _users.push_back(&user);
}

/*DESTRUCTOR*/
Channel::~Channel(){}
