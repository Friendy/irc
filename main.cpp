/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrubina <mrubina@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/15 17:28:20 by mrubina           #+#    #+#             */
/*   Updated: 2024/06/22 12:23:43 by mrubina          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <err.h>
#include <string>
#include <iostream>


// socket, setsockopt, getsockname

// inet_addr, inet_ntoa

// getprotobyname, gethostbyname, getaddrinfo, freeaddrinfo,

// bind, connect, listen, accept,
// htons, htonl, ntohs, ntohl, ,

void create_hint(struct addrinfo *hint)
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

void socket_test(const char* protname, const char* ipstr, const char* port, const char* hostname)
{
	std::cout << "****** Testing functions: ******\n";
	struct protoent *prot_struct;
	int sockfd;
	int dest_sockfd;
	int isbound;
	int isset;
	int err;
	struct sockaddr sock_struct;
	struct sockaddr dest_addr;
	struct in_addr *ip_struct;
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
	if (err == -1)
		errx(1, "can't get info for %s", hostname);

/* getting protocol (filling out protocol structure)
to get protocol number that will be used later
*/
	prot_struct = getprotobyname(protname);
	if (prot_struct == NULL)
		errx(1, "no protocol %s", protname);

/* creating socket */
	sockfd = socket(PF_INET, SOCK_STREAM, prot_struct->p_proto);
	if (sockfd == -1)
		errx(1, "socket not created %i ", errno);

/* binding socket to ip address and port */
// std::cout << "len: " << addr_info->ai_addrlen << "\n";
	isbound = bind(sockfd, addr_info->ai_addr, addr_info->ai_addrlen);
	if (isbound == -1)
		errx(1, "not bound %i ", errno);
	std::cout << "pr: " << sock_struct.sa_len << "\n";
// 

/* listening */
	isset = listen(sockfd, 128);
	if (isset == -1)
		errx(1, "fail %i ", errno);

dest_sockfd = accept(sockfd, &dest_addr, &dest_len);
if (dest_sockfd == -1)
		errx(1, "new socket not created %i ", errno);
std::cout << "len: " << dest_len << "\n";
std::cout << "addr: " << ((struct sockaddr_in *)&dest_addr)->sin_addr.s_addr << "\n";
std::cout << "family: " << ((struct sockaddr_in *)&dest_addr)->sin_len << "\n";
// connect(sockfd, &dest_addr, sizeof(struct sockaddr));


}




int main()
{
	 socket_test("tcp", "127.0.0.1", "6667", "localhost");
}


