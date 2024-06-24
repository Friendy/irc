/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrubina <mrubina@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/15 17:28:20 by mrubina           #+#    #+#             */
/*   Updated: 2024/06/24 00:58:50 by mrubina          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcServ.hpp"

// socket, setsockopt, getsockname

// inet_addr, inet_ntoa

// getprotobyname, gethostbyname, getaddrinfo, freeaddrinfo,

// bind, connect, listen, accept,
// htons, htonl, ntohs, ntohl, ,

void sig_handler(int)
{
	std::cout << "\nserver was interrupted\n";
	std::cout << "quitting\n";
	exit(0);
}


int main(int argc, char *argv[])
{
	signal(SIGINT, sig_handler);
	IrcServ serv;
	if (argc == 3)
		serv.server_start("tcp", argv[2], "localhost");
	else
		Err::handler(1, "not enough arguments: ", "please, use ip_address and port");
	while (1)
	{

	}
}

