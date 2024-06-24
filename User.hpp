#ifndef USER_HPP
# define USER_HPP
# include <string>
# include <iostream>

class User {

private:
	std::string _user;
	std::string _nick;
	const int _fd;

public:
	const std::string getUser();
	void setUser(std::string user);
	const std::string getNick();
	void setNick(std::string nick);
	int getFd();

	// User();
	User(int fd);
	User(User const &original);
	User &operator=(User const &original);
	~User();
};

#endif
