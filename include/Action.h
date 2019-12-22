#ifndef ACTION_H_
#define ACTION_H_
#include <string>
#include "User.h"
#include <unordered_map>
class Session;


using namespace std;

enum ActionStatus{
    PENDING, COMPLETED, ERROR
};


class BaseAction{
public:
    BaseAction();
    virtual ~BaseAction() = default;
    ActionStatus getStatus() const;
    virtual void act(Session& sess)=0;
    virtual std::string toString() const=0;
    string  enum_to_string(ActionStatus status) const;
    virtual BaseAction* clone()=0;

protected:
    void complete();
    void error(const std::string& errorMsg);
    virtual User* findUser(string& name, unordered_map<std::string, User*>& userMap);

private:
    std::string errorMsg;
    ActionStatus status;
};

class CreateUser  : public BaseAction {
public:
    CreateUser() = default;
    virtual void act(Session& sess);
    virtual std::string toString() const;
    BaseAction* clone();
private:
};


class ChangeActiveUser : public BaseAction {
public:
    ChangeActiveUser() = default;
    virtual void act(Session& sess);
    virtual std::string toString() const;
    BaseAction* clone();

};

class DeleteUser : public BaseAction {
public:
    DeleteUser() = default;
    virtual void act(Session & sess);
    virtual std::string toString() const;
    BaseAction* clone();

};

class DuplicateUser : public BaseAction {
public:
    DuplicateUser() = default;
    virtual void act(Session & sess);
    virtual std::string toString() const;
    BaseAction* clone();
};

class PrintContentList : public BaseAction {
public:
    PrintContentList() = default;
    virtual void act (Session& sess);
    virtual std::string toString() const;
    BaseAction* clone();
};


class PrintWatchHistory : public BaseAction {
public:
    PrintWatchHistory() = default;
    virtual void act(Session &sess);
    virtual std::string toString() const;
    BaseAction* clone();
};


class Watch : public BaseAction {
public:
    Watch() = default;
    virtual void act(Session& sess);
    virtual std::string toString() const;
    BaseAction* clone();
};


class PrintActionsLog : public BaseAction {
public:
    PrintActionsLog() = default;
    virtual void act(Session& sess);
    virtual std::string toString() const;
    BaseAction* clone();
};

class Exit : public BaseAction {
public:
    Exit() = default;
    virtual void act(Session& sess);
    virtual std::string toString() const;
    BaseAction* clone();
};

#endif
