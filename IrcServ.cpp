#include "IrcServ.hpp"

/*CONSTRUCTORS*/
IrcServ::IrcServ()
{
}

IrcServ::IrcServ(std::string pass) : _server_name("irc.server.com")
{
	_pass = pass;
	_actionQ.push(ACCEPT);
	_actionQ.push(RECEIVE);
	_actionQ.push(SEND);
	_startInd = 0;
	_commands["PASS"] = &IrcServ::fPass;
	_commands["USER"] = &IrcServ::fUser;
	_commands["NICK"] = &IrcServ::fNick;
	_commands["PING"] = &IrcServ::fPing;
	_commands["QUIT"] = &IrcServ::fQuit;
	_commands["MODE"] = &IrcServ::fMode;
	_commands["PRIVMSG"] = &IrcServ::fPriv;
	_commands["UNKNOWN"] = &IrcServ::fUnknown;
	_commands["JOIN"] = &IrcServ::fjoin;
	_codes[RPL_WELCOME] = "RPL_WELCOME";
	_codes[ERR_UNKNOWNCOMMAND] = "ERR_UNKNOWNCOMMAND";
	_codes[ERR_ALREADYREGISTRED] = "ERR_ALREADYREGISTRED";
	_codes[ERR_NOTREGISTERED] = "ERR_NOTREGISTERED";
	_codes[ERR_NEEDMOREPARAMS] = "ERR_NEEDMOREPARAMS";
	_codes[ERR_NOSUCHCHANNEL] = "ERR_NOSUCHCHANNEL";
	_codes[ERR_CHANOPRIVSNEEDED] = "ERR_CHANOPRIVSNEEDED";
	_codes[ERR_NOSUCHNICK] = "ERR_NOSUCHNICK";
	_codes[ERR_BADCHANNELKEY] = "ERR_BADCHANNELKEY";
}

//Assignment operator:
IrcServ &IrcServ::operator=(IrcServ const &original)
{
	if (this != &original)
	{
		this->_pass = original._pass;
		this->_commands = original._commands;
		this->_listenfd = original._listenfd;
		this->_codes = original._codes;
		this->_users = original._users;
		this->_nicks = original._nicks;
		this->_msgQ = original._msgQ;
		this->_activePoll = original._activePoll;
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
	_userPoll[_activePoll].events = POLLIN | POLLOUT;
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
	_users[fd]->setPollInd(_activePoll - 1);
	_users[fd]->setPollPtr(&_userPoll[_activePoll - 1]);

	Message response(buildNotice("Please provide the password: PASS <password>", 0), &_userPoll[_activePoll - 1]);
	_msgQ.push(response);

    std::cout << "Client connected" << std::endl;
}

void IrcServ::setupSocket(const char* protname, long port_tmp, struct addrinfo *addr_info) {
	struct protoent *prot_struct;
	int optVal = 1;
	int optLen = sizeof(optVal);
	int isbound;

	prot_struct = getprotobyname(protname);
	if (prot_struct == NULL)
		Err::handler(1, "no protocol ", protname);
	_listenfd = socket(PF_INET, SOCK_STREAM, prot_struct->p_proto);
	if (_listenfd  == -1)
		Err::handler(1, "socket not created", "");
	if (setsockopt(_listenfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&optVal, optLen) == -1) {
		Err::handler(1, "setsockopt error", "");
	}
	fcntl(_listenfd, F_SETFL, O_NONBLOCK);
	if (_listenfd == -1) {
		Err::handler(1, "setsockopt error", "");
	}
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	port_tmp = (int) port_tmp;
	inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
	
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
    int err;
    int pollReturn;
    struct addrinfo *addr_info;
    struct addrinfo hint;
    long port_tmp = strtol(port, NULL, 0);

    if (port_tmp <= 0 || port_tmp > 65535)
        Err::handler(1, "invalid port: ", port);
    create_hint(&hint);
    err = getaddrinfo(hostname, port, &hint, &addr_info);
    if (err != 0)
        Err::handler(1, "can't get info for ", hostname);

    setupSocket(protname, port_tmp, addr_info);

    int action;
    while (1)
    {
        pollReturn = poll(_userPoll, _activePoll, 5000);
        if (pollReturn == -1)
            Err::handler(1, "poll error", "");
        action = getAction();
        switch (action)
        {
            case ACCEPT:
                accept_client();
                break;
            case RECEIVE:
                recieve_msg();
                break;
            case SEND:
                sendQueue();
                break;
            default:
                ;
        }
    }
}


bool IrcServ::readyToAction(int action)
{
    pollfd *fd;
    switch (action)
    {
        case ACCEPT:
            fd = &_userPoll[0];
            break;
        case RECEIVE:
            setRecvFd();
            return (_curRecvFd != 0);
        case SEND:
            fd = getFirstSend();
            break;
        default:
            return false;
    }
    if (fd == NULL)
        return false;
    if (action == SEND)
        return(fd->revents & POLLOUT);
    return(fd->revents & POLLIN);
}


int IrcServ::getAction()
{
    int action = _actionQ.front();
    int max = 2;
    int count = 0;
    int ready;
    while (!(ready = readyToAction(action)) && count <= max)
    {
        _actionQ.pop();
        _actionQ.push(action);
        count++;
        if (count < 3)
            action = _actionQ.front();
    }
    if (!ready)
        return EMPTY;
    _actionQ.pop();
    _actionQ.push(action);
    std::cout << "Action ready: " << action << std::endl;
    return action;
}



void IrcServ::delete_user(User *user)
{
	int fd;

	fd = user->getFd();
	_nicks.erase(user->getNick());
	delete(user);
	_users.erase(fd);
}

std::string IrcServ::buildPriv(const std::string msg, std::string from, std::string to)
{
	std::stringstream ss;
	std::string new_msg;

	ss << ":" << from << " PRIVMSG " << to << " :" << msg;
	std::getline(ss, new_msg);
	return new_msg;
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

std::string IrcServ::welcome(User user)
{
	std::stringstream ss;
	std::string new_msg;

	ss << ":" << _server_name << " 001 " << user.getNick();
	ss << " :" << "Welcome to the Internet Relay Network " << user.getFullName();
	std::getline(ss, new_msg);
	return (new_msg);
}

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

void IrcServ::setRecvFd()
{
	nfds_t ind = _startInd;
	bool ready;

	while(!(ready = _userPoll[ind].revents & POLLIN) && ind <= _activePoll)
		ind++;
	if (ready == true)
	{
		if (ind == _activePoll)
			_startInd = 0;
		else
			_startInd = ind + 1;
		_curRecvFd = _userPoll[ind].fd;
		return;
	}
	else if (_startInd != 0)
	{
		ind = 0;
		while(!(ready = _userPoll[ind].revents & POLLIN) && ind < _startInd)
			ind++;
		if (ready == true)
		{
			_startInd = ind + 1;
			_curRecvFd = _userPoll[ind].fd;
			return;
		}
	}
	_curRecvFd = 0;
}

void IrcServ::recieve_msg()
{
    char buf[512];
    bzero(buf, 512);
    std::string msg;
    User *currentUser;

    currentUser = _users[_curRecvFd];
    recv(_curRecvFd, buf, 512, 0);
    msg = std::string(buf);
    std::cout << "Received message: " << msg << std::endl;
   	processMsg(*currentUser, msg);
    bzero(buf, 512);
    if (currentUser->hasquitted())
        delete_user(currentUser);
}

void IrcServ::sendQueue()
{
    std::cout << "sendQueue called" << std::endl;
    if (!_msgQ.empty())
    {
        Message *response = &_msgQ.front();
        std::cout << "Processing message: " << response->getMsg() << std::endl;
        if (response->sendMsg() == 0) // if sending failed, the message is pushed to the queue again
        {
            std::cout << "Failed to send message, requeuing: " << response->getMsg() << std::endl;
            _msgQ.push(*response);
            _msgQ.pop(); // Pop the current message after pushing the failed one back to the queue
        }
        else
        {
            std::cout << "Message sent successfully: " << response->getMsg() << std::endl;
            _msgQ.pop(); // Pop the message from the queue after successful sending
        }
    }
}


Message IrcServ::processMsg(User &user, std::string msg)
{
    Message response("");
    std::istringstream stream(msg);
    std::string line;

    while (std::getline(stream, line))
    {
        trimMsg(line);
        if (line.empty())
            continue;

        user.setLastMsg(line);
        Command com = parseMsg(line);
        std::cout << "Command received: " << com.getCommand() << " with parameters: ";
        for (size_t i = 0; i < com.getParams().size(); ++i)
            std::cout << com.getParams()[i] << " ";
        std::cout << std::endl;

        if (com.getCommand() == "CAP")
        {
            std::cout << "CAP command received: " << line << std::endl;
            response.setMsg("CAP * LS :multi-prefix");
            response.addFd(&_userPoll[user.getPollInd()]);
            _msgQ.push(response);
            std::cout << "CAP response added to message queue" << std::endl;
            break;
        }

        std::cout << "Received from " << user.getFd() << ": " << line << std::endl;
        if (com.getCommand() == "NOTICE")
        {
            std::cout << "NOTICE command received: " << line << std::endl;
            break;
        }
        if (com.getCommand() == "JOIN")
        {
            std::cout << "JOIN command received: " << line << std::endl;
            response = fjoin(com.getParams(), user);
            std::cout << "JOIN command processed: " << response.getMsg() << std::endl;
			break;
        }

        if (com.getCommand() == "PRIVMSG")
        {
            std::string privResponse = fPriv(com.getParams(), user);
            if (privResponse != "")
                _msgQ.push(Message(privResponse, &_userPoll[user.getPollInd()]));
            std::cout << "PRIVMSG response added to message queue" << std::endl;
			break;
        }
        else
        {
            response.addFd(&_userPoll[user.getPollInd()]);
        }

        if (_commands.find(com.getCommand()) == _commands.end())
            com.replaceCommand("UNKNOWN");

        response.setMsg((this->*_commands[com.getCommand()])(com.getParams(), user));
        _msgQ.push(response);
        std::cout << com.getCommand() << " response added to message queue" << std::endl;
    }
    return response;
}



bool compare_until_cr(const std::string &a, const std::string &b) {
    size_t i = 0;
    for (; i < b.length(); ++i) {
        std::cout << "Comparing a[" << i << "] = " << static_cast<int>(a[i]) << " with b[" << i << "] = " << static_cast<int>(b[i]) << std::endl;
        if (a[i] == '\r') {
            std::cout << "CR found in 'a', stopping comparison" << std::endl;
            return true;
        }
        if (a[i] != b[i]) {
            std::cout << "Characters do not match, comparison failed" << std::endl;
            return false;
        }
    }
    std::cout << "End of 'b' reached, all characters matched" << std::endl;
    return true; 
}

std::string IrcServ::fPass(std::vector<std::string> params, User &user)
{
	if (user.passGiven())
		return(buildNotice("You may not reregister", ERR_ALREADYREGISTRED));
	if (params.empty())
		return(buildNotice("PASS :Not enough parameters", ERR_NEEDMOREPARAMS));
	std::string pass_param = params[0];
	if (compare_until_cr(pass_param, _pass))
	{
		user.givePass();
		return(buildNotice("The password is correct. Now please provide your nick: NICK <nick>.", 0));
	}
	else
		return(buildNotice("The password is incorrect. Please try again!", ERR_PASSWDMISMATCH));
}

std::string IrcServ::fNick(std::vector<std::string> params, User &user)
{
	std::string oldnick;

	oldnick = user.getNick();
	if (params.empty())
		return(buildNotice("", ERR_NONICKNAMEGIVEN));
	if (!user.passGiven())
		return(buildNotice("Please provide the password first: PASS <password>", 0));
	if (_nicks.find(params[0]) != _nicks.end())
		return(buildNotice("Nickname is already in use!", ERR_NICKNAMEINUSE));
	else
	{
		user.setNick(params[0]);
		_nicks[params[0]] = user.getFd();
	}
	if (oldnick != "")
		return(buildNotice("Your nick has been changed", 0));
	else
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
	std::cout << "param " << params[0] << "\n";
	user.setUser(params[0]);
	user.registerUser();
	return(welcome(user));
}

std::string IrcServ::fPing(std::vector<std::string> params, User &user)
{
	return("PONG irc.server.com");
}

void IrcServ::check_user()
{

}

std::string IrcServ::fjoin(std::vector<std::string> params, User &user) {
    if (!user.isRegistered()) {
        return buildNotice("You are not registered to join", ERR_NOTREGISTERED);
    }
    if (params.empty()) {
        return buildNotice("No channel name given", ERR_NEEDMOREPARAMS);
    }

    std::string channelName = params[0];
    Channel* channel;
    std::string responseMessage;

    if (_channels.find(channelName) == _channels.end()) {
        channel = new Channel(channelName);
        if (params.size() == 2) {
            channel->setPassword(params[1]);
            channel->setInviteOnly(true);
        }
        channel->addUser(user);
        channel->addOperator(user);
        _channels[channelName] = channel;
        responseMessage = "Created and joined channel " + channelName;
    } else {
        channel = _channels[channelName];

        if (channel->isInviteOnly() && !channel->isOperator(user)) {
            return buildNotice("Cannot join channel, invite only", ERR_CHANOPRIVSNEEDED);
        }

        if (channel->getPassword() != "" && (params.size() < 2 || channel->getPassword() != params[1])) {
            return buildNotice("Invalid channel key", ERR_BADCHANNELKEY);
        }

        if (channel->getUserLimit() > 0 && channel->getUsers().size() >= channel->getUserLimit()) {
            return buildNotice("Channel is full", ERR_NEEDMOREPARAMS);
        }

        channel->addUser(user);
        responseMessage = "Joined channel " + channelName;
    }

    // Notify other users in the channel about the new user
    std::map<int, User*> users = channel->getUsers();
    for (std::map<int, User*>::iterator it = users.begin(); it != users.end(); ++it) {
        if (it->second->getNick() != user.getNick()) {
            std::string joinMsg = buildPriv("has joined the channel", user.getFullName(), channelName);
            _msgQ.push(Message(joinMsg, it->second->getPollfd()));
        }
    }

    // Update channel info for all users
    for (std::map<int, User*>::iterator it = users.begin(); it != users.end(); ++it) {
        std::string channelInfo = buildPriv("Channel info updated", user.getFullName(), channelName);
        _msgQ.push(Message(channelInfo, it->second->getPollfd()));
    }

    return buildNotice(responseMessage, 0);
}







std::string IrcServ::fPriv(std::vector<std::string> params, User &user) {
    if (params.size() < 2) {
        return buildNotice("Invalid PRIVMSG format", ERR_NEEDMOREPARAMS);
    }

    std::string target = params[1]; // target channel or user
    std::string message = params[2]; // message content

    std::cout << "Target: " << target << std::endl;
    std::cout << "Message: " << message << std::endl;

    // Check if the target is a channel
    if (_channels.find(target) != _channels.end()) {
		 std::cout << " 1 " << std::endl;
        Channel* channel = _channels[target];
        std::map<int, User*> users = channel->getUsers();
        for (std::map<int, User*>::iterator it = users.begin(); it != users.end(); ++it) {
            if (it->second->getNick() != user.getNick()) {
                std::string privMsg = buildPriv(message, user.getFullName(), target);
                _msgQ.push(Message(privMsg, it->second->getPollfd()));
            }
        }
        return " ";
    } 
    // Check if the target is a user
    else if (_nicks.find(target) != _nicks.end()) {
		std::cout << " 2 " << std::endl;
        std::string privMsg = buildPriv(message, user.getFullName(), target);
        _msgQ.push(Message(privMsg, getPollfd(target)));
        return "";
    } 
    // If target is neither a channel nor a user
    else {
		std::cout << " 3 " << std::endl;
        return buildNotice("No such nick/channel", ERR_NOSUCHNICK);
    }
}





std::string IrcServ::fUnknown(std::vector<std::string> params, User &user)
{
	std::string str("421 ERR_UNKNOWNCOMMAND ");
	str.append(params[0]);
	return(str);
}

std::string IrcServ::fQuit(std::vector<std::string> params, User &user)
{
	user.quitted();
	close(user.getFd());
	return("");
}

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
		start = str.find_first_not_of(' ', start);
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

pollfd* IrcServ::getPollfd(std::string nick) {
    if (_nicks.find(nick) == _nicks.end()) {
        std::cerr << "Nick not found: " << nick << std::endl;
        return NULL;
    }
    User* user = _users[_nicks[nick]];
    if (!user) {
        std::cerr << "User pointer is null for nick: " << nick << std::endl;
        return NULL;
    }
    return &_userPoll[user->getPollInd()];
}


pollfd *IrcServ::getFirstSend()
{
	if (!_msgQ.empty())
		return(_msgQ.front().getPollfd());
	else
		return(NULL);
}

/*DESTRUCTOR*/
IrcServ::~IrcServ(){}
