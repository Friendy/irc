#include "User.hpp"

/*CONSTRUCTORS*/
// User::User(){}


User::User(int fd) : _fd(fd), _isregistered(false), _passgiven(false), _quitstatus(0){};

//Assignment operator:
User &User::operator=(User const &original)
{
	if (this != &original)
	{
		this->_user = original._user;
		this->_nick = original._nick;
		this->_hostmask = original._hostmask;
	}
	return(*this);
}

User::User(User const &original) : _fd(original._fd)
{
	*this = original;
}


User::User(int fd, const std::string &hostmask) : _fd(fd), _hostmask(hostmask), _isregistered(false), _passgiven(false), _quitstatus(0){
    std::cout << "User created with fd: " << fd << " and hostmask: " << hostmask << std::endl;
    memset(&_address, 0, sizeof(_address));
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

void User::setAddress(struct sockaddr_in addr)
{
	_address = addr;
}

const std::string User::getNick()
{
	return(this->_nick);
}

const std::string User::getHost()
{
	return(this->_hostmask);
}

const std::string User::getFullName()
{
	std::string name;
	name = getNick();
	name.append("!");
	name.append(getUser());
	name.append("@");
	name.append(getHost());
	return(name);
}

int User::getFd()
{
	return(this->_fd);
}

int User::getPollInd()
{
	return(this->_pollInd);
}

void User::setPollInd(int i)
{
	_pollInd = i;
}

void User::setPollPtr(pollfd *pfd)
{
	_fdPtr = pfd;
}

const std::string User::getLastMsg()
{
	return(this->_last_msg);
}

void User::setLastMsg(std::string msg)
{
	_last_msg = msg;
}


// const std::string User::getMsg(std::string msg)
// {
// 	_last_msg = "";
// 	_last_msg = msg;
// 	if (*(_last_msg.rbegin()) == '\n')
// 		_last_msg.erase(_last_msg.length() - 1);
// 	if (*(_last_msg.rbegin()) == 13)
// 		_last_msg.erase(_last_msg.length() - 1);
// 	if (_last_msg != "")
// 		std::cout << "Recieved: " << _last_msg << "\n";
// 	return (_last_msg);
// }

// Command User::parseMsg()
// {
// 	size_t start;
// 	// std::string word;

// 	start = _last_msg.find_first_not_of(' '); //ignore starting spaces
// 	Command com(get_next_word(_last_msg, start));
// 	while (start != std::string::npos)
// 	{
// 		start = _last_msg.find(' ', start + 1);
// 		if (start != std::string::npos)
// 		{
// 			start = _last_msg.find_first_not_of(' ', start + 1);
// 			com.setParam(get_next_word(_last_msg, start));
// 		}
// 	}
// 	_last_msg = "";
// 	return (com);
// }

// void User::processMsg()
// {
// 	if (_last_msg != "")
// 	{
// 		Command com = parseMsg();
// 		// std::cout << "com: " << com.getCommand() << "\n";
// 		// if (com.paramCount() != 0)
// 		// 	std::cout << "par: " << com.getParam(0) << "\n";
// 		if (_isregistered == false)
// 			registerUser(com);
// 	}
// }


void User::registerUser()
{
	_isregistered = true;
}

bool User::isRegistered()
{
	return(_isregistered);
}

bool User::passGiven()
{
	return(_passgiven);
}

void User::setNick(std::string nick)
{
	this->_nick = nick;
}

void User::givePass()
{
	_passgiven = true;
}

void User::setQuitStatus(int status)
{
	_quitstatus = status;
}

int User::getQuitStatus()
{
	return(_quitstatus);
}

void User::joinChannel(Channel *channel) {
    _channels.push_back(channel);
    // Kanala katılma işlemleri
}

/*DESTRUCTOR*/
User::~User(){}
