#include "User.hpp"

User::User(int fd) : _fd(fd), _isregistered(false), _passgiven(false), _quitstatus(0)
{
	_lastactivitytm = time(NULL);
	_pingsendtm = time(NULL);
	_quitsendtm = 0;
	_pingsent = false;
	_buffer = "";
	_msg_incomplete = false;
}

//Assignment operator:
User &User::operator=(User const &original)
{
	if (this != &original)
	{
		this->_user = original._user;
		this->_nick = original._nick;
		this->_lastactivitytm = original._lastactivitytm;
		this->_pingsendtm = original._pingsendtm;
		this->_quitsendtm = original._quitsendtm;
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
	_lastactivitytm = time(NULL);
	_pingsendtm = time(NULL);
	_quitsendtm = 0;
	_pingsent = false;
	_buffer = "";
	_msg_incomplete = false;
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

int User::getFd() const
{
	return(this->_fd);
}

int User::getPollInd()
{
	return(this->_pollInd);
}

pollfd *User::getPollfd()
{
	return(_fdPtr);
}

void User::setPollInd(int i)
{
	_pollInd = i;
}

void User::setPollPtr(pollfd *pfd)
{
	_fdPtr = pfd;
}

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

void User::setMsgIncomplete(bool status)
{
	_msg_incomplete = status;
}

bool User::isIncomplete()
{
	return(_msg_incomplete);
}

std::string User::msgAppend(std::string msg, int last)
{
	_buffer.append(msg);
	if (last == 1)
		_msg_incomplete = false;
	return(_buffer);
}

void User::setPingSent(bool status)
{
	_pingsent = status;
}

bool User::getPingSent()
{
	return(_pingsent);
}

void User::clearBuffer()
{
	_buffer = "";
}

void User::joinChannel(Channel *channel) {
    _channels.push_back(channel);
}

const std::string User::getName() const {
    return this->_user;
}

const std::string User::getHostmask() const {
    return this->_hostmask;
}

void User::saveLastActivity()
{
	_lastactivitytm = time(NULL);
	_pingsent = false;
}

time_t User::getLastActivity()
{
	return(_lastactivitytm);
}

void User::saveLastPing()
{
	_pingsendtm = time(NULL);
}

time_t User::getPingTime()
{
	return(_pingsendtm);
}

double User::timeSinceActivity()
{
	return(difftime(time(NULL), _lastactivitytm));
}

double User::timeSincePing()
{
	return(difftime(time(NULL), _pingsendtm));
}

void User::saveQuitSendTime()
{
	_quitsendtm = time(NULL);
}

time_t User::getQuitSendTime()
{
	return(_quitsendtm);
}

double User::timeSinceQuitSend()
{
	return(difftime(time(NULL), _quitsendtm));
}


/*DESTRUCTOR*/
User::~User(){}
