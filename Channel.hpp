#ifndef CHANNEL_HPP
# define CHANNEL_HPP
# include <string>
# include <iostream>
# include <vector>
# include "User.hpp"

class Channel {

private:
	std::string _name;
	std::vector<User> _users;

public:
	const std::string getName();
	void setName(std::string name);
	const std::vector<User> getUsers();
	void setUsers(std::vector<User> users);

	Channel();
	Channel(Channel const &original);
	Channel &operator=(Channel const &original);
	~Channel();
};

#endif
