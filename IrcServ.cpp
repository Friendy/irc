#include "IrcServ.hpp"

/*CONSTRUCTORS*/
IrcServ::IrcServ()
{
}

IrcServ::IrcServ(std::string pass) : _server_name("irc.server.com")
{
	_pass = pass;
	_commands["PASS"] = &IrcServ::fPass;
	_commands["USER"] = &IrcServ::fUser;
	_commands["NICK"] = &IrcServ::fNick;
	_commands["PING"] = &IrcServ::fPing;
	_commands["QUIT"] = &IrcServ::fQuit;
	_commands["PRIVMSG"] = &IrcServ::fPriv;
	_codes[RPL_WELCOME] = "RPL_WELCOME";
	_codes[ERR_UNKNOWNCOMMAND] = "ERR_UNKNOWNCOMMAND";
	_codes[ERR_ALREADYREGISTRED] = "ERR_ALREADYREGISTRED";
	_codes[ERR_NEEDMOREPARAMS] = "ERR_NEEDMOREPARAMS";
	_codes[ERR_PASSWDMISMATCH] = "ERR_PASSWDMISMATCH";
}

//Assignment operator:
IrcServ &IrcServ::operator=(IrcServ const &original)
{
	if (this != &original)
	{
		this->_pass = original._pass;
		this->_commands = original._commands;
		this-> _listenfd = original._listenfd;
	}
	return(*this);
}

IrcServ::IrcServ(IrcServ const &original)
{
	*this = original;
}

/*FUNCTIONS*/

void IrcServ::addToPoll(int fd) {
	if (_activePoll >= SOMAXCONN)
		Err::handler(1, "too many connections", "");
	_userPoll[_activePoll].fd = fd;
	_userPoll[_activePoll].events = POLLIN | POLLOUT;//mrubina: added POLLOUT for sending
	_activePoll++;
}

/* ******Connection related functions********** */
void IrcServ::accept_client() {
    int fd;
    struct sockaddr_in dest_addr;
    socklen_t dest_len = sizeof(dest_addr);
    char host[INET_ADDRSTRLEN];
	
    fd = accept(_listenfd, (struct sockaddr *)&dest_addr, &dest_len);
    if (fd == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            Err::handler(1, "new socket not created", strerror(errno));
        }
        return;
    }
    inet_ntop(AF_INET, &dest_addr.sin_addr, host, INET_ADDRSTRLEN);
    std::string hostmask(host);
    std::cout << "User hostmask: " << hostmask << std::endl;
    std::cout << "User connected on fd: " << fd << std::endl;

    _users[fd] = new User(fd, hostmask);
    _users[fd]->setAddress(dest_addr);

    addToPoll(fd);
	_users[fd]->setPollInd(_activePoll - 1);//mrubina: added index to user for easy access

    send_msg(fd, "Hello\r\n");//will be removed after polling
	// send_msg(fd, "001 RPL_WELCOME \n");

    std::cout << "Client connected" << std::endl;
}


void IrcServ::setupSocket(const char* protname, long port_tmp, struct addrinfo *addr_info) {
	struct protoent *prot_struct;
	int optVal = 1;
	int optLen = sizeof(optVal);
	int isbound;

	// std::cout << " test1" << "\n";
	prot_struct = getprotobyname(protname);
	if (prot_struct == NULL)
		Err::handler(1, "no protocol ", protname);
	_listenfd = socket(PF_INET, SOCK_STREAM, prot_struct->p_proto);
	if (_listenfd  == -1)
		Err::handler(1, "socket not created", "");
	if (setsockopt(_listenfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&optVal, optLen) == -1) {
		Err::handler(1, "setsockopt error", "");
	}
// Set the socket to non-blocking mode using fcntl. This ensures that socket operations (e.g., read, write)
// return immediately if they cannot be completed, instead of blocking the execution.
	fcntl(_listenfd, F_SETFL, O_NONBLOCK);
	if (_listenfd == -1) {
		Err::handler(1, "setsockopt error", "");
	}
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	port_tmp = (int) port_tmp;
	// hint.sin_port = htons(port_tmp);

	//sockaddr_in *ipv4 = (sockaddr_in *)addr_info->ai_addr;
	//inet_pton(addr_info->ai_family, "0.0.0.0", &(ipv4->sin_addr));
	inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
	//memset(hint.sin_zero, '\0', sizeof(hint.sin_zero));

	//addr_info->ai_addrlen = 0;
	
	isbound = bind(_listenfd, addr_info->ai_addr, addr_info->ai_addrlen);
	if (isbound == -1)
		Err::handler(1, "not bound", "");
	if (listen(_listenfd, SOMAXCONN) == -1) {
		Err::handler(1, "listen error", "");
	}
	memset(_userPoll, 0, sizeof(_userPoll));
	_activePoll = 1;
	_userPoll[0].fd	= _listenfd;
	_userPoll[0].events = POLLIN;
}

void IrcServ::server_start(const char* protname, const char* port, const char* hostname)
{
	// struct protoent *prot_struct;
	// int isbound;
	//int isset;
	int err;
	int pollReturn;
	// struct in_addr *ip_struct;
	struct addrinfo *addr_info; //addrinfo structure
	struct addrinfo hint;
	long port_tmp = strtol(port, NULL, 0);
	
/* filling out address info structure
some members of this structure will later be used
as arguments for other functions
addr_info->ai_addr
*/
	if (port_tmp <= 0 || port_tmp > 65535)
		Err::handler(1, "invalid port: ", port);
	create_hint(&hint);
	err = getaddrinfo(hostname, port, &hint, &addr_info);
	if (err != 0)
		Err::handler(1, "can't get info for ", hostname);

/* getting protocol (filling out protocol structure)
to get protocol number that will be used later
*/
	// prot_struct = getprotobyname(protname);
	// if (prot_struct == NULL)
	// 	Err::handler(1, "no protocol ", protname);
	setupSocket(protname, port_tmp, addr_info);

/* creating socket */
	// _listenfd = socket(PF_INET, SOCK_STREAM, prot_struct->p_proto);
	// if (_listenfd  == -1)
	// 	Err::handler(1, "socket not created", "");

/* binding socket to ip address and port */
// std::cout << "len: " << addr_info->ai_addrlen << "\n";
	// isbound = bind(_listenfd, addr_info->ai_addr, addr_info->ai_addrlen);
	// if (isbound == -1)
	// 	Err::handler(1, "not bound", "");

	// std::cout << "pr: " << sock_struct.sa_len << "\n";

/* listening */
	// isset = listen(_listenfd, 128);
	// if (isset == -1)
	// 	Err::handler(1, "fail", "");

//to do save fds in the class and watch their count as many clients connect
	while(1)
	{
		pollReturn = poll(_userPoll, _activePoll, 5000);
		if (pollReturn == -1)
			Err::handler(1, "poll error", "");
		accept_client();
		recieve_msg();
	}

}

//deletes user from the server(after disconnecting it) setting its iterator to the next element
void IrcServ::delete_user(std::map<const int, User *>::iterator &it)
{
	std::map<const int, User *>::iterator del_it = it;
	it++;
	_nicks.erase(del_it->second->getNick());
	delete(del_it->second);
	_users.erase(del_it);

}

/* ******Message sending functions ********** */
/* sending a message to a user */
void IrcServ::send_msg(int fd, std::string msg)
{
	ssize_t bytes_sent = send(fd, msg.data(), msg.length(), 0);
	if (bytes_sent == -1)
		Err::handler(1, "sending message failed: ", msg);
	else
		std::cout << "Sent: " << msg << "\n";
}

/* sending a message to all users */
void IrcServ::send_msg(std::string msg)
{
	std::map<const int, User *>::iterator it;
	for (it = _users.begin(); it != _users.end(); ++it)
		send_msg(it->second->getFd(), msg);
}


std::string IrcServ::buildPriv(const std::string msg, User to)
{
	std::stringstream ss;
	std::string new_msg;

	ss << " PRIVMSG " << to.getNick();
	if (msg != "")
		ss << " :" << msg;
	std::getline(ss, new_msg);
	return (new_msg);
// :irc.server.com NOTICE 462 ERR_ALREADYREGISTRED
// :nick!user@127.0.0.1 PRIVMSG nick :msg

}

std::string IrcServ::buildNotice(const std::string msg, int code)
{
	std::stringstream ss;
	std::string new_msg;

	ss << ":" << _server_name << " NOTICE";
	if (code != 0)
		ss << " " << code  << " " << _codes[code];
	if (msg != "")
		ss << " :" << msg;
	std::getline(ss, new_msg);
	return (new_msg);
}

// std::string IrcServ::buildPriv(const std::string msg, User to, int code)
// {
// 	std::stringstream ss;
// 	std::string new_msg;

// 	ss << " PRIVMSG " << to.getNick();
// 	if (code != 0)
// 		ss << " " << code  << " " << _codes[code];
// 	if (msg != "")
// 		ss << " :" << msg;
// 	std::getline(ss, new_msg);
// 	return (new_msg);
// // :irc.server.com NOTICE 462 ERR_ALREADYREGISTRED
// // :nick!user@127.0.0.1 PRIVMSG nick :msg
// }

std::string IrcServ::welcome(User user)
{
	std::stringstream ss;
	std::string new_msg;

	ss << ":" << _server_name << " 001 " << user.getNick();
	ss << " :" << "Welcome to the Internet Relay Network " << user.getFullName();
	std::getline(ss, new_msg);
	return (new_msg);
}

/* ******General message processing functions********** */
//extracts command and parameters
Command IrcServ::parseMsg(const std::string msg)
{
	size_t start;
	std::string pref;

	start = msg.find_first_not_of(' '); //ignore starting spaces
	if (msg[start] == ':')
	{
		start++;
		pref = get_next_word(msg, start);
	}
	Command com(get_next_word(msg, start));
	if (com.getCommand() == "PRIVMSG")
		com.setParam(pref);
	/* 	//TODO add parsing for join, PRIVMSG
	PRIVMSG #channel :msg
	:nick!user@127.0.0.1 PRIVMSG #channel :msg
	:nick!user@127.0.0.1 PRIVMSG nick :msg
	NOTICE RPL_WELCOME :LAGCHECK 1720120129566735-yeah-:)
	*/
	while (start != std::string::npos)
	{
		start = msg.find(' ', start); //finding first space after previous word
		if (start != std::string::npos)
		{
			start++;
			com.setParam(get_next_word(msg, start));
		}
	}
	return (com);
}

/* for each user receives a message, processes it,
creates response and sends it.
later will be changed after polling: responses will be queued and then sent using another function
*/
void IrcServ::recieve_msg()
{
	char buf[512];
	bzero(buf, 512);
	std::string msg;
	Message response;

	std::map<const int, User *>::iterator it;
	for (it = _users.begin(); it != _users.end(); ++it)
	{
		if ( _userPoll[it->second->getPollInd()].revents & POLLIN)//if pollling showed ready for recieving
		{
			recv(it->second->getFd(), buf, 512, 0);
			msg = std::string(buf);
			// std::cout << "msg: " << msg << "\n";
			// if (msg.find("PRIVMSG") != std::string::npos)
			// 	send_msg(it->second->getFd(), msg);
			response = processMsg(*it->second, msg);
			// std::cout << "msg l: " << response.getMsg().length() << "\n";
			if (response.getMsg() != "" && _userPoll[it->second->getPollInd()].revents & POLLOUT)//ready to send
				response.sendMsg(it->second->getFd());
				// send_msg(it->second->getFd(), addNewLine(response));
			// std::cout << "msg: " << buf << "\n";
			bzero(buf, 512);
			if (it->second->hasquitted())
				delete_user(it);
			if (it ==_users.end()) //this can happen if the last user is deleted
				break;
		}
	}
}

/*
trims message, saves it for the user, executes command using an appropriate function
CAP command ignored because
*/
Message IrcServ::processMsg(User &user, std::string msg)
{
	Message response("");
	Command com;

	trimMsg(msg);
	user.setLastMsg(msg);
	if (msg != "")
	{
		com = parseMsg(msg);
		// std::cout << "com: " << com.getCommand() << "\n";
		if (com.getCommand() == "CAP")
			return(response);
		std::cout << "Recieved: " << msg << "\n";
		if (com.getCommand() == "NOTICE")
			return(response);
		// if (com.getCommand() == "PRIVMSG")
		// 	response.addFd(_nicks[com.getParam(0)]);
		if (_commands.find(com.getCommand()) != _commands.end())
			response.setMsg((this->*_commands[com.getCommand()])(com.getParams(), user));
		else
			response.setMsg("421 ERR_UNKNOWNCOMMAND\n");
		return(response);
	}
	else
		return(response);
}

/* ******Command functions****** */
std::string IrcServ::fPass(std::vector<std::string> params, User &user)
{
	if (user.passGiven())
		return(buildNotice("You may not reregister", ERR_ALREADYREGISTRED));
	if (params.empty())
		return(buildNotice("PASS :Not enough parameters", ERR_NEEDMOREPARAMS));
	if (params[0] == _pass)
	{
		user.givePass();
		return(buildNotice("The password is correct. Now please provide your nick: NICK <nick>.", 0));
	}
	else
		return(buildNotice("The password is incorrect. Please try again", ERR_PASSWDMISMATCH));
}

std::string IrcServ::fNick(std::vector<std::string> params, User &user)
{
	std::string oldnick;

	oldnick = user.getNick();
	if (params.empty())
		return(buildNotice("", ERR_NONICKNAMEGIVEN));
	if (!user.passGiven())
		// return("Please provide the password first: PASS <password>");
		return(buildNotice("Please provide the password first: PASS <password>", 0));
		
	//TODO check if nick characters are all allowed
	if (_nicks.find(params[0]) != _nicks.end())
		// return("Error: Nickname is already in use");
		return(buildNotice("Nickname is already in use!", ERR_NICKNAMEINUSE));
	else
	{
		user.setNick(params[0]);
		_nicks[params[0]] = user.getFd();
	}
	if (oldnick != "")
		return(buildNotice("Your nick has been changed", 0));
		// return("Your nick has been changed");
	else
		// return("Now for the last step, add username: USER <username> <hostname> <servername> <realname>");
		return(buildNotice("Now for the last step, add username: USER <username> 0 * <:realname>", 0));
}

std::string IrcServ::fUser(std::vector<std::string> params, User &user)
{
	if (user.isRegistered())
		return(buildNotice("You may not reregister", ERR_ALREADYREGISTRED));
	if (!user.passGiven())
		return(buildNotice("Please provide the password first: PASS <password>", 0));
	else if (user.getNick() == "")
		return(buildNotice("Please provide your nick first: NICK <nick>.", 0));
	//TODO add more parameters
	user.setUser(params[0]);
	user.registerUser();
	// return("irc.server.com NOTICE 001 RPL_WELCOME");
		return(welcome(user));
		// return(":irc.server.com 001");
		// return(":nick!user@127.0.0.1 PRIVMSG nick :msg");
		// :nick!user@127.0.0.1 PRIVMSG nick :msg
}

std::string IrcServ::fPing(std::vector<std::string> params, User &user)
{
	return("PONG irc.server.com");
}

std::string IrcServ::fPriv(std::vector<std::string> params, User &user)
{
	return(buildPriv(params[2], user));
}

//sets quitted flag and closes user socket afer getting a QUIT message
//the user is deleted from the server list in delete_user function
std::string IrcServ::fQuit(std::vector<std::string> params, User &user)
{
	user.quitted();
	close(user.getFd());
	return("");
}

/* ******Helper functions****** */
void IrcServ::create_hint(struct addrinfo *hint)
{
	hint->ai_socktype = SOCK_STREAM;
	hint->ai_protocol = 0;
	hint->ai_family = AF_INET;
	hint->ai_addr = NULL;
	hint->ai_addrlen = 0;
	hint->ai_canonname = NULL;
	hint->ai_flags = 0;
	hint->ai_next = NULL;
}

void IrcServ::print_fds()
{
	std::map<const int, User *>::iterator it;
	for (it = _users.begin(); it != _users.end(); ++it)
		std::cout << "fd: " << it->second->getFd() << "\n";
	std::cout << "end\n";
}

void IrcServ::print_users()
{
	std::map<const int, User *>::iterator it;
	for (it = _users.begin(); it != _users.end(); ++it)
		std::cout << "fd: " << it->first << " - " << it->second->getFd() << "\n";
	std::cout << "end\n";
}

std::string IrcServ::get_next_word(std::string str, size_t &start)
{
	size_t end;
	std::string sub;

	if (str[start] == ' ')
		start = str.find_first_not_of(' ', start);//finding start of the current word
	end = str.find(' ', start);
	if (end == std::string::npos || str[start] == ':')
	{
		sub = str.substr(start, str.length() - start);
		start = std::string::npos;
	}
	else
	{
		sub = str.substr(start, end - start);
		start = end;
	}
	return (sub);
}

void IrcServ::trimMsg(std::string &msg)
{
	if (*(msg.rbegin()) == '\n')
		msg.erase(msg.length() - 1);
	if (*(msg.rbegin()) == 13)
		msg.erase(msg.length() - 1);
}

std::string IrcServ::addNewLine(std::string &str)
{
	str.insert(str.end(), '\n');
	return(str);
}


/*DESTRUCTOR*/
IrcServ::~IrcServ(){}
