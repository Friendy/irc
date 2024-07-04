#include "IrcServ.hpp"

/*CONSTRUCTORS*/
IrcServ::IrcServ()
{
}

IrcServ::IrcServ(std::string pass)
{
	_pass = pass;
	_commands["PASS"] = &IrcServ::fPass;
	_commands["USER"] = &IrcServ::fUser;
	_commands["NICK"] = &IrcServ::fNick;
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
	_userPoll[_activePoll].events = POLLIN;
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

    send_msg(fd, "Hello\n");

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

/* ******General message processing functions********** */
//extracts command and parameters
Command IrcServ::parseMsg(const std::string msg)
{
	size_t start;

	start = msg.find_first_not_of(' '); //ignore starting spaces
	Command com(get_next_word(msg, start));
	while (start != std::string::npos)
	{
		start = msg.find(' ', start); //finding first space after previous word
		if (start != std::string::npos)
		{
			start = msg.find_first_not_of(' ', start);//finding start of the current word
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
	std::string response;

	std::map<const int, User *>::iterator it;
	for (it = _users.begin(); it != _users.end(); ++it)
	{
		// std::cout << "fd: " << it->first << " - " << it->second->getFd() << "\n";
		recv(it->second->getFd(), buf, 512, 0);
		msg = std::string(buf);
		response = processMsg(*it->second, msg);
		if (response != "")
			send_msg(it->second->getFd(), addNewLine(response));
		// std::cout << "msg: " << buf << "\n";
		bzero(buf, 512);
	}
}

/*
trims message, saves it for the user, executes command using an appropriate function
*/
std::string IrcServ::processMsg(User &user, std::string msg)
{
	std::string response;
	Command com;

	trimMsg(msg);
	user.setLastMsg(msg);
	if (msg != "")
	{
		com = parseMsg(msg);
		if (_commands.find(com.getCommand()) != _commands.end())
			response = (this->*_commands[com.getCommand()])(com.getParams(), user);
		else
			response = "command not found";
		return(response);
	}
	else
		return("");
}

/* ******Command functions****** */
std::string IrcServ::fPass(std::vector<std::string> params, User &user)
{
	if (user.passGiven())
			return("You have already provided the correct password");
	if (params[0] == _pass)
	{
		user.givePass();
		return("The password is correct. Now please provide your nick: NICK <nick>.");
	}
	else
		return("The password is incorrect. Please try again");
}

std::string IrcServ::fNick(std::vector<std::string> params, User &user)
{
	std::string oldnick;

	oldnick = user.getNick();
	if (!user.passGiven())
		return("Please provide the password first: PASS <password>");
	//TODO check if this nick already exists
	user.setNick(params[0]);
	if (oldnick != "")
		return("Your nick has been changed");
	else
		return("Now for the last step, add username: USER <username> <hostname> <servername> <realname>");
}

std::string IrcServ::fUser(std::vector<std::string> params, User &user)
{
	if (user.isRegistered())
		return("You are already registered");
	if (!user.passGiven())
		return("Please provide the password first: PASS <password>");
	else if (user.getNick() == "")
		return("Please provide your nick first: NICK <nick>.");
	//TODO check if this username already exists and add more parameters
	user.setUser(params[0]);
	user.registerUser();
		return("You are now registered");
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

std::string IrcServ::get_next_word(std::string str, size_t start)
{
	size_t end;

	end = str.find(' ', start);
	if (end != std::string::npos)
		return (str.substr(start, end - start));
	else
		return (str.substr(start, str.length() - start));
}

void IrcServ::trimMsg(std::string &msg)
{
	if (*(msg.rbegin()) == '\n')
		msg.erase(msg.length() - 1);
	if (*(msg.rbegin()) == 13)
		msg.erase(msg.length() - 1);
	if (msg != "")
		std::cout << "Recieved: " << msg << "\n";
}

std::string IrcServ::addNewLine(std::string &str)
{
	str.insert(str.end(), '\n');
	return(str);
}


/*DESTRUCTOR*/
IrcServ::~IrcServ(){}
