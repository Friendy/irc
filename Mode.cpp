#include "IrcServ.hpp"

std::string IrcServ::fMode(std::vector<std::string> params, User &user)
{
    std::cout << "fMode called with params: ";
    for (size_t i = 0; i < params.size(); ++i)
    {
        std::cout << i <<  params[i] <<" ";
    }
    std::cout << std::endl;

    if (params.size() < 2)
        return buildNotice("Not enough parameters for MODE", ERR_NEEDMOREPARAMS);

    std::string channelName = params[0];
    std::string mode = params[1];

    if (_channels.find(channelName) == _channels.end())
        return buildNotice("No such channel", ERR_NOSUCHCHANNEL);

    Channel* channel = _channels[channelName];

    // Only operators can set modes
    if (!channel->isOperator(user))
        return buildNotice("You're not a channel operator", ERR_CHANOPRIVSNEEDED);

    if (mode == "+i")
        channel->setInviteOnly(true);
    else if (mode == "-i")
        channel->setInviteOnly(false);
    else if (mode == "+t")
        channel->setTopicRestrict(true);
    else if (mode == "-t")
        channel->setTopicRestrict(false);
    else if (mode == "+k")
    {
        if (params.size() < 3)
            return buildNotice("Not enough parameters for MODE +k", ERR_NEEDMOREPARAMS);
        channel->setPassword(params[2]);
    }
    else if (mode == "-k")
        channel->setPassword("");
    else if (mode == "+l")
    {
        if (params.size() < 3)
            return buildNotice("Not enough parameters for MODE +l", ERR_NEEDMOREPARAMS);
        unsigned int userLimit = static_cast<unsigned int>(atoi(params[2].c_str()));
        if (userLimit <= channel->getUsers().size())
            return buildNotice("User limit must be greater than the current number of users", ERR_NEEDMOREPARAMS);
        channel->setUserLimit(atoi(params[2].c_str()));
    }
    else if (mode == "-l")
        channel->setUserLimit(0);
    else if (mode == "+o")
    {
        if (params.size() < 3)
            return buildNotice("Not enough parameters for MODE +o", ERR_NEEDMOREPARAMS);
        if (_nicks.find(params[2]) == _nicks.end())
            return buildNotice("No such nick", ERR_NOSUCHNICK);
        channel->addOperator(*_users[_nicks[params[2]]]);
    }
    else if (mode == "-o")
    {
        if (params.size() < 3)
            return buildNotice("Not enough parameters for MODE -o", ERR_NEEDMOREPARAMS);
        if (_nicks.find(params[2]) == _nicks.end())
            return buildNotice("No such nick", ERR_NOSUCHNICK);
        channel->removeOperator(*_users[_nicks[params[2]]]);
    }
    else
        return buildNotice("Unknown mode", ERR_UNKNOWNCOMMAND);

    return buildNotice("Mode set successfully", 0);
}
