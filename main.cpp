/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrubina <mrubina@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/15 17:28:20 by mrubina           #+#    #+#             */
/*   Updated: 2024/07/21 18:50:37 by mrubina          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcServ.hpp"

void sig_handler(int)
{
	std::cout << "\nserver was interrupted\n";
	std::cout << "quitting\n";
	// system("leaks ircserv");
	_exit(0);
}

int main(int argc, char *argv[])
{
	signal(SIGINT, sig_handler);
	if (argc != 3)
		Err::handler(1, "wrong number of arguments: ", "please, use port and password");
	IrcServ serv(argv[2]);
	serv.server_start("tcp", argv[1], "localhost");
	// system("leaks ircserv");
}

