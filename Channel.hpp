#ifndef CHANNEL_HPP
# define CHANNEL_HPP
# include <string>
# include <iostream>
# include <vector>
# include "User.hpp"

class Channel {

private:
	std::string _name;
	std::vector<User*> _users;

public:
	const std::string getName();
	void setName(std::string name);
    const std::vector<User*>& getUsers() const;
    void setUsers(const std::vector<User*>& users);
	void addUser(User &user);

	Channel();
	Channel(const std::string &name);
	Channel(Channel const &original);
	Channel &operator=(Channel const &original);
	~Channel();
};

#endif
