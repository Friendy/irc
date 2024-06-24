#include "IrcServ.hpp"

/*CONSTRUCTORS*/
IrcServ::IrcServ()
{
	// std::cout << "class created" << std::endl;
}

//Assignment operator:
IrcServ &IrcServ::operator=(IrcServ const &original)
{
	if (this != &original)
		this->_data = original._data;
	return(*this);
}

IrcServ::IrcServ(IrcServ const &original)
{
	*this = original;
}

/*FUNCTIONS*/
int IrcServ::getData() const
{
	return(this->_data);
}

void IrcServ::setData(int data)
{
	this->_data = data;
}

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


void IrcServ::send_msg(int *fd, std::string msg)
{
	ssize_t bytes_sent = send(*fd, msg.data(), msg.length(), 0);
	if (bytes_sent == -1)
		Err::handler(1, "sending message failed: ", msg);
	else
		std::cout << "Sent: " << msg << "\n";
	std::cout << "size: " << msg.length() << "\n";
}

void IrcServ::server_start(const char* protname, const char* port, const char* hostname)
{
	struct protoent *prot_struct;
	int sockfd;
	int dest_sockfd;
	int isbound;
	int isset;
	int err;
	// struct sockaddr sock_struct;
	struct sockaddr dest_addr;
	// struct in_addr *ip_struct;
	struct addrinfo *addr_info; //addrinfo structure
	struct addrinfo hint;
	socklen_t dest_len;

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
	sockfd = socket(PF_INET, SOCK_STREAM, prot_struct->p_proto);
	if (sockfd == -1)
		Err::handler(1, "socket not created", "");

/* binding socket to ip address and port */
// std::cout << "len: " << addr_info->ai_addrlen << "\n";
	isbound = bind(sockfd, addr_info->ai_addr, addr_info->ai_addrlen);
	if (isbound == -1)
		Err::handler(1, "not bound", "");

	// std::cout << "pr: " << sock_struct.sa_len << "\n";

/* listening */
	isset = listen(sockfd, 128);
	if (isset == -1)
		Err::handler(1, "fail", "");
dest_sockfd = accept(sockfd, &dest_addr, &dest_len);
if (dest_sockfd == -1)
	Err::handler(1, "new socket not created", "");
send_msg(&dest_sockfd, "Hello");

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
