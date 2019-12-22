#ifndef SESSION_H_
#define SESSION_H_
#include <unordered_map>
#include <string>
#include<fstream>
#include "Action.h"
#include <vector>
#include "json.hpp"
#include <string>

class User;
class Watchable;

using namespace std;


class Session{
public:
    Session(const std::string &configFilePath);
    virtual ~Session();//destructor
    Session(const Session& other);//copy constructor
    Session(Session &&other);//move constructor
    Session& operator=(Session &&other);
    Session& operator=(Session &other);
    void user_Input();
    void start();
    void setActiveUser(User* user);
    void setSessionOpen(bool value);
    vector<Watchable*> getContent() const;
    vector<string>& getInput();
    User* getActiveUser() const;
    vector<BaseAction*> getActionsLog() const;
    unordered_map<string,User*>& getUsermap();

    void insertDefaultUser();

private:
    vector<Watchable*> content;
    vector<BaseAction*> actionsLog;
    unordered_map<string,User*> userMap;
    User* activeUser;
    vector<string> input;
    bool session_open;

    void copy(const Session &other);
};
#endif
