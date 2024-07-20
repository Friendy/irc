#ifndef USER_HPP
# define USER_HPP
# include <string>
# include <iostream>
#include <sys/socket.h>
#include "Command.hpp"
#include <cstring>
#include <netinet/in.h>
#include <poll.h>
#include <ctime>

// Forward declaration of Channel class
class Channel;

class User {

private:
	std::string _user;
	std::string _nick;
	const int _fd;
	std::string _hostmask;
	struct sockaddr _addr;
	bool _isregistered;
	bool _passgiven;
	struct sockaddr_in _address;
	int _quitstatus;
	int _pollInd; //index of user fd in poll array
	pollfd *_fdPtr;//pointer to pollfd structure
	std::vector<Channel*> _channels;
	time_t _lastactivitytm;//time when the last message is recieved
	time_t _pingsendtm;//time when the last ping is sent
	time_t _quitsendtm;//time when the quit msg is sent to the user
	bool _msg_incomplete;
	std::string _buffer;

public:
	const std::string getUser();
	void setUser(std::string user);
	void setAddress(struct sockaddr_in addr);
	void setPollInd(int i);
	void setPollPtr(pollfd *pfd);
	const std::string getNick();
	const std::string getHost();
	const std::string getFullName();
	void setNick(std::string nick);
	int getFd() const;
	int getPollInd();
	pollfd *getPollfd();
	// const std::string getMsg(std::string msg);
	void registerUser();
	bool isRegistered();
	bool passGiven();
	void givePass();
	int getQuitStatus();
	void setQuitStatus(int status);
	void joinChannel(Channel *channel);
	void saveLastPing();
	void saveLastActivity();
	time_t getPingTime();
	time_t getLastActivity();

	void saveQuitSendTime();
	time_t getQuitSendTime();
	double timeSinceQuitSend();
	void setMsgIncomplete(bool status);
	bool isIncomplete();
	std::string msgAppend(std::string msg, int last);
	void clearBuffer();
	time_t getPingTime();

	double timeSinceActivity();
	double timeSincePing();
	

	bool operator<(const User& other) const {
        return this->_fd < other.getFd();
    }
	// Command parseMsg();
    const std::string getName() const;
    const std::string getHostmask() const;  

	// User();
	User(int fd);
	User(int fd, const std::string &hostmask);
	User(User const &original);
	User &operator=(User const &original);
	~User();
};

#endif
