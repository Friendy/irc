#include "Command.hpp"

/*CONSTRUCTORS*/
Command::Command()
{
	// _command = "TEST";
	// std::cout << "class created" << std::endl;
}

Command::Command(std::string command) : _command(command){};

// Command::Command(std_) : _com
// {
// 	// std::cout << "class created" << std::endl;
// }

//Assignment operator:
Command &Command::operator=(Command const &original)
{
	if (this != &original)
	{
		this->_command = original._command;
		this->_parameters = original._parameters;
	}
	return(*this);
}

Command::Command(Command const &original)
{
	*this = original;
}

/*FUNCTIONS*/
std::string Command::getCommand()
{
	return(this->_command);
}

void Command::replaceCommand(std::string com)
{
	if (!_parameters.empty())
		_parameters[0] = _command;
	else
		setParam(_command);
	_command = com;
}

const std::string Command::getParam(int n)
{
	if (!_parameters.empty())
		return(this->_parameters[n]);
	else
		throw std::exception();
}

std::vector<std::string> Command::getParams()
{
	return(_parameters);
}

void Command::setParam(std::string param)
{
	_parameters.push_back(param);
}

size_t Command::paramCount()
{
	if (_parameters.empty())
		return(0);
	else
		return(_parameters.size());
}

// const std::vector<User> Command::getUsers()
// {
// 	return(this->_users);
// }

// void Command::setUsers(std::vector<User> users)
// {
// 	this->_users = users;
// }

/*DESTRUCTOR*/
Command::~Command(){}
