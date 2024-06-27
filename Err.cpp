#include "Err.hpp"
#include <cerrno>
#include <cstring>    // strerror
#include <unistd.h>   // _exit
/*CONSTRUCTORS*/
Err::Err()
{
}

//Assignment operator:
Err &Err::operator=(Err const &original)
{
	if (this != &original)
		this->_type = original._type;
	return(*this);
}

Err::Err(Err const &original)
{
	*this = original;
}

/*FUNCTIONS*/
int Err::getType() const
{
	return(this->_type);
}

void Err::setType(int type)
{
	this->_type = type;
}

void Err::handler(int exit_stat, std::string msg, std::string val)
{
	std::cout << msg << val << "\n";
	if (errno != 0)
		std::cout << strerror(errno) << " " << errno << "\n";
	_exit(exit_stat);
}

/*DESTRUCTOR*/
Err::~Err(){}
