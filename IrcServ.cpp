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
	hint->ai_addr = 0;
	hint->ai_addrlen = 0;
	hint->ai_canonname = 0;
	hint->ai_flags = 0;
	hint->ai_next = 0;
}

/* sending a message to a user */
void IrcServ::send_msg(int fd, std::string msg)
{
	std::cout << fd << " print fd" << "\n";
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

void IrcServ::accept_client()
{
	int fd;
	socklen_t dest_len;
	struct sockaddr dest_addr;

	fd = accept(_listenfd, &dest_addr, &dest_len);
	if (fd == -1)
	Err::handler(1, "new socket not created", "");
	_users[fd] = new User(fd);
	_users[fd]->setAddress(dest_addr);
	send_msg(fd, "Hello\n");
	send_msg("Let's welcome a new client\n");
}

void IrcServ::server_start(const char* protname, const char* port, const char* hostname)
{
	struct protoent *prot_struct;
	int isbound;
	int isset;
	int err;
	// struct in_addr *ip_struct;
	struct addrinfo *addr_info; //addrinfo structure
	struct addrinfo hint;
	
/* filling out address info structure
some members of this structure will later be used
as arguments for other functions
addr_info->ai_addr
*/
	create_hint(&hint);
	err = getaddrinfo(hostname, port, &hint, &addr_info);
	if (err != 0)
		Err::handler(1, "can't get info for ", hostname);

/* getting protocol (filling out protocol structure)
to get protocol number that will be used later
*/
	prot_struct = getprotobyname(protname);
	if (prot_struct == NULL)
		Err::handler(1, "no protocol ", protname);

/* creating socket */
	_listenfd = socket(PF_INET, SOCK_STREAM, prot_struct->p_proto);
	if (_listenfd  == -1)
		Err::handler(1, "socket not created", "");

/* binding socket to ip address and port */
// std::cout << "len: " << addr_info->ai_addrlen << "\n";
	isbound = bind(_listenfd, addr_info->ai_addr, addr_info->ai_addrlen);
	if (isbound == -1)
		Err::handler(1, "not bound", "");

	// std::cout << "pr: " << sock_struct.sa_len << "\n";

/* listening */
	isset = listen(_listenfd, 128);
	if (isset == -1)
		Err::handler(1, "fail", "");

//to do save fds in the class and watch their count as many clients connect

}

void IrcServ::setopt(int *sockfd, int level, int option, int optval)
{
	int err;
// socklen_t optlen;

err = setsockopt(*sockfd, level, option, &optval, sizeof(optval));
// err = getsockopt(dest_sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen);
if (err < 0)
	Err::handler(1, "no option change", "");
}

/*DESTRUCTOR*/
IrcServ::~IrcServ(){}
