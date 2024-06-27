#include "IrcServ.hpp"

/*CONSTRUCTORS*/
IrcServ::IrcServ()
{
	// std::cout << "class created" << std::endl;
}

IrcServ::IrcServ(std::string pass)
{
	_pass = pass;
}

//Assignment operator:
IrcServ &IrcServ::operator=(IrcServ const &original)
{
	if (this != &original)
		this->_pass = original._pass;
	return(*this);
}

IrcServ::IrcServ(IrcServ const &original)
{
	*this = original;
}

/*FUNCTIONS*/
// int IrcServ::getData() const
// {
// 	return(this->_data);
// }

// void IrcServ::setData(int data)
// {
// 	this->_data = data;
// }

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


void IrcServ::send_msg(int fd, std::string msg)
{
	std::cout << fd << " print fd" << "\n";
	ssize_t bytes_sent = send(fd, msg.data(), msg.length(), 0);
	if (bytes_sent == -1)
		Err::handler(1, "sending message failed: ", msg);
	else
		std::cout << "Sent: " << msg << "\n";
}

void IrcServ::send_msg(std::string msg)
{
	std::vector<int>::iterator it;
	for (it = _fds.begin(); it < _fds.end(); ++it)
		send_msg(*it, msg);
}

void IrcServ::print_fds()
{
	std::vector<int>::iterator it;
	for (it = _fds.begin(); it < _fds.end(); ++it)
		std::cout << "fd: " << *it << "\n";
	std::cout << "end\n";
}

void IrcServ::print_users()
{
	std::map<const int, User *>::iterator it;
	for (it = _users.begin(); it != _users.end(); ++it)
		std::cout << "fd: " << it->first << " - " << it->second->getFd() << "\n";
	std::cout << "end\n";
}

void IrcServ::accept_client()
{
    int fd;
    socklen_t dest_len = sizeof(_dest_addr);

    fd = accept(_listenfd, (struct sockaddr *)&_dest_addr, &dest_len);
    if (fd == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            Err::handler(1, "new socket not created", strerror(errno));
        }
        // If errno is EAGAIN or EWOULDBLOCK, just return and try again later
        return;
    }

    _fds.push_back(fd);
    _users[fd] = new User(fd);
    send_msg(fd, "Hello");
}

void IrcServ::setupSocket(const char* protname, long port_tmp, struct addrinfo *addr_info) {
	struct protoent *prot_struct;
	int optVal = 1;
	int optLen = sizeof(optVal);
	int isbound;

	std::cout << " test1" << "\n";
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
	memset(hint.sin_zero, '\0', sizeof(hint.sin_zero));

	//addr_info->ai_addrlen = 0;
	
	isbound = bind(_listenfd, addr_info->ai_addr, addr_info->ai_addrlen);
	if (isbound == -1)
		Err::handler(1, "not bound", "");
	if (listen(_listenfd, SOMAXCONN) == -1) {
		Err::handler(1, "listen error", "");
	}
}

void IrcServ::server_start(const char* protname, const char* port, const char* hostname)
{
	// struct protoent *prot_struct;
	// int isbound;
	//int isset;
	int err;
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

}

void IrcServ::setopt(int *sockfd, int level, int option, int optval)
{
	int err;
// socklen_t optlen;
std::cout << " test" << "\n";
err = setsockopt(*sockfd, level, option, &optval, sizeof(optval));
// err = getsockopt(dest_sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen);
if (err < 0)
	Err::handler(1, "no option change", "");
}

/*DESTRUCTOR*/
IrcServ::~IrcServ(){}
