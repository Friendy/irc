/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrubina <mrubina@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/15 17:28:20 by mrubina           #+#    #+#             */
/*   Updated: 2024/06/28 17:58:33 by mrubina          ###   ########.fr       */
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
	_exit(0);
}


int main(int argc, char *argv[])
{
	signal(SIGINT, sig_handler);
	if (argc != 3)
		Err::handler(1, "wrong number of arguments: ", "please, use port and password");
	IrcServ serv(argv[2]);
	serv.server_start("tcp", argv[1], "localhost");
	while (1)
	{
		serv.accept_client();
		// serv.print_users();
		// sleep(1);
	}
}

