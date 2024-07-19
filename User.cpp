#include "User.hpp"

/*CONSTRUCTORS*/
// User::User(){}


User::User(int fd) : _fd(fd), _isregistered(false), _passgiven(false), _quitstatus(0)
{
	_lastactivitytm = time(NULL);
	_pingsendtm = 0;
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
	_pingsendtm = 0;
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

const std::string User::getLastMsg()
{
	return(this->_last_msg);
}

void User::setLastMsg(std::string msg)
{
	_last_msg = msg;
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

void User::joinChannel(Channel *channel) {
    _channels.push_back(channel);
    // Kanala katılma işlemleri
}

void User::saveLastActivity()
{
	_lastactivitytm = time(NULL);
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
	time_t curr_time;
	curr_time = time(NULL);
	return(difftime(curr_time, _lastactivitytm));
}

double User::timeSincePing()
{
	time_t curr_time;
	curr_time = time(NULL);
	return(difftime(curr_time, _pingsendtm));
}


/*DESTRUCTOR*/
User::~User(){}
