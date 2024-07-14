#ifndef DEFINES_HPP
# define DEFINES_HPP

//message types
# define PRIVMSG 100
# define NOTICE 200

//reply and error codes for messages
# define RPL_WELCOME 1
# define ERR_UNKNOWNCOMMAND 421
# define ERR_ALREADYREGISTRED 462
# define ERR_NOTREGISTERED 451
// # define
# define ERR_NEEDMOREPARAMS 461
# define ERR_PASSWDMISMATCH 464
# define ERR_NONICKNAMEGIVEN 431
# define ERR_ERRONEUSNICKNAME 432 // characters not allowed
# define ERR_NICKNAMEINUSE 433
// # define

#endif
