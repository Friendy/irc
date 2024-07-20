#include "Message.hpp"

/*CONSTRUCTORS*/
Message::Message() : _fdPtr(NULL) {}

Message::Message(std::string msg) : _msg(msg), _fdPtr(NULL) {}

Message::Message(std::string msg, pollfd *fd) : _msg(msg), _fdPtr(fd) {}

// Assignment operator:
Message &Message::operator=(Message const &original) {
    if (this != &original) {
        this->_fdPtr = original._fdPtr;
        this->_msg = original._msg;
    }
    return *this;
}

Message::Message(Message const &original) {
    *this = original;
}

/*FUNCTIONS*/

const std::string Message::getMsg() const {
    return this->_msg;
}

void Message::setMsg(std::string msg) {
    this->_msg = msg;
}

void Message::addFd(pollfd *fd) {
    _fdPtr = fd;
}

pollfd *Message::getPollfd() const {
    return _fdPtr;
}

// Sending a message to one fd
int Message::sendMsg() {
    if (!_fdPtr) {
        std::cerr << "File descriptor is null" << std::endl;
        return 1;
    }
    addNewLine();
    ssize_t bytes_sent = send(_fdPtr->fd, _msg.c_str(), _msg.length(), 0);
    if (bytes_sent == -1) {
        Err::handler(0, "sending message failed: ", _msg);
        return 0;
    } else {
        std::cout << "Sent to " << _fdPtr->fd << ": " << _msg << std::endl;
    }
    return 1;
}

void Message::addNewLine() {
    if (!_msg.empty() && _msg[_msg.size() - 1] != '\n') {
        _msg.push_back('\n');
    }
}

/*DESTRUCTOR*/
Message::~Message() {}
