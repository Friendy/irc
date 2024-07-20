#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>
#include <iostream>
#include "User.hpp"

class Channel {
public:
    Channel();
    Channel(const std::string &name);
    Channel &operator=(Channel const &original);
    Channel(Channel const &original);
    ~Channel();

    const std::string getName() const;
    void setName(std::string name);
    const std::map<int, User*>& getUsers() const;
    void setUsers(const std::map<int, User*>& users);
    void addUser(User &user);
    void removeUser(User &user);

    bool isOperator(User &user) const;
    void addOperator(User &user);
    void removeOperator(User &user);

    void setPassword(const std::string& password);
    std::string getPassword() const;

    void setUserLimit(int limit);
    int getUserLimit() const;

    void setInviteOnly(bool inviteOnly);
    bool isInviteOnly() const;
    bool isInvited(User &user) const;
    void removeInvite(User &user);
    void addInvite(User &user);

    void setTopicRestrict(bool restrict);
    bool isTopicRestrict() const;

    bool isUserInChannel(const User& user) const;
    bool isEmpty() const;

    const std::string& getTopic() const;
    void setTopic(const std::string& topic);
private:
    std::string _name;
    std::map<int, User*> _users;
    std::map<std::string, User*> _operators;
    std::map<std::string, User*> _invitedUsers;
    std::string _password;
    int _userLimit;
    bool _inviteOnly;
    bool _topicRestrict;
    std::string _topic;
};

#endif
