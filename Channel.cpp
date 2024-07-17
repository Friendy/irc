#include "Channel.hpp"

Channel::Channel() : _userLimit(0), _inviteOnly(false), _topicRestrict(false) {}

Channel::Channel(const std::string &name) : _name(name), _userLimit(0), _inviteOnly(false), _topicRestrict(false) {}

Channel &Channel::operator=(Channel const &original) {
    if (this != &original) {
        _name = original._name;
        _users = original._users;
        _operators = original._operators;
        _password = original._password;
        _userLimit = original._userLimit;
        _inviteOnly = original._inviteOnly;
        _topicRestrict = original._topicRestrict;
    }
    return *this;
}

Channel::Channel(Channel const &original) {
    *this = original;
}

Channel::~Channel() {}

const std::string Channel::getName() const {
    return _name;
}

void Channel::setName(std::string name) {
    _name = name;
}

const std::map<int, User*>& Channel::getUsers() const {
    return _users;
}

void Channel::setUsers(const std::map<int, User*>& users) {
    _users = users;
}

void Channel::addUser(User &user) {
    _users[user.getFd()] = &user;
}

void Channel::removeUser(User &user) {
    _users.erase(user.getFd());
}

bool Channel::isOperator(User &user) const {
    return _operators.find(user.getNick()) != _operators.end();
}

void Channel::addOperator(User &user) {
    _operators[user.getNick()] = &user;
}

void Channel::removeOperator(User &user) {
    _operators.erase(user.getNick());
}

void Channel::setPassword(const std::string& password) {
    _password = password;
}

std::string Channel::getPassword() const {
    return _password;
}

void Channel::setUserLimit(int limit) {
    _userLimit = limit;
}

int Channel::getUserLimit() const {
    return _userLimit;
}

void Channel::setInviteOnly(bool inviteOnly) {
    _inviteOnly = inviteOnly;
}

bool Channel::isInviteOnly() const {
    return _inviteOnly;
}

void Channel::setTopicRestrict(bool restrict) {
    _topicRestrict = restrict;
}

bool Channel::isTopicRestrict() const {
    return _topicRestrict;
}
