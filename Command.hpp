#ifndef COMMAND_HPP
# define COMMAND_HPP
# include <string>
# include <iostream>
# include <vector>

class Command {

private:
	std::string _command;
	std::vector<std::string> _parameters;

public:
	std::string getCommand();
	const std::string getParam(int n);
	std::vector<std::string> getParams();
	void setParam(std::string param);
	void replaceCommand(std::string com);
	size_t paramCount();

	Command(std::string command);
	Command();
	Command(Command const &original);
	Command &operator=(Command const &original);
	~Command();
};

#endif
