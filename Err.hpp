#ifndef ERR_HPP
# define ERR_HPP
# include <string>
# include <iostream>

class Err {

private:
	int _type;

public:
	int getType() const;
	void setType(int type);
	static void handler(int exit_stat, std::string msg, std::string val);

	Err();
	Err(Err const &original);
	Err &operator=(Err const &original);
	~Err();
};

#endif
