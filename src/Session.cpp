
#include "../include/Session.h"
#include "../include/Action.h"
#include "../include/json.hpp"
#include "../include/User.h"// !!!!!! check
#include <unordered_map>
#include <string>
#include <vector>
#include<iostream>
#include<sstream>
#include<fstream>
#include "../include/Watchable.h"

class User;

class Watchable;

using namespace std;
using std::make_pair;
//CONSTRUCTOR
Session::Session(const std::string &configFilePath):content(),actionsLog(),userMap(),activeUser(nullptr),input(),session_open(false) {
    //read json file
    using json = nlohmann::json;
    std::ifstream i(configFilePath);
    json j;
    i >> j;
//MOVIES

    int length;
    string name;
    long id=0;
    for (size_t h = 0; h < j["movies"].size(); h++) {
        name = j["movies"][h]["name"];
        length = j["movies"][h]["length"];
        vector<string> tags = j["movies"][h]["tags"];
        content.push_back(new Movie(++id, name, length, tags));
    }
    //EPISODES
    int numOfSeasons;
    int episode;
    for (size_t k = 0; k < j["tv_series"].size(); k++) {
        name = j["tv_series"][k]["name"];
        length = j["tv_series"][k]["episode_length"];
        vector<string> tags = j["tv_series"][k]["tags"];
        numOfSeasons = j["tv_series"][k]["seasons"].size();
        for (int season = 1; season < numOfSeasons + 1; season++) {
            int numOfEpisodes = j["tv_series"][k]["seasons"][season - 1];
            for (episode = 1; episode < numOfEpisodes + 1; ++episode) {
                content.push_back( new Episode(++id, name, length, season, episode, tags));
            }
        }
    }
    activeUser = new LengthRecommenderUser("default");
    activeUser->initNotYetSeen(*this);
    insertDefaultUser();
}

//aiding function for constructor
void Session::insertDefaultUser() {
    getUsermap().insert({getActiveUser()->getName(), getActiveUser()});
}


//copy constructor
Session::Session(const Session& other):content(),actionsLog(),userMap(),activeUser(nullptr),input(),session_open(false) {
    copy(other);

}

void Session::copy(const Session &other) {
    for (auto *watchable: other.getContent()) {
        content.push_back(watchable->clone());
    }
    for (auto *action: other.getActionsLog()) {
        actionsLog.push_back(action->clone());
    }
    for (auto currUser : other.userMap){
        userMap.insert({currUser.first, currUser.second->clone()}); //maybe needs *this
    }
    activeUser = userMap[other.activeUser->getName()];

    for (auto currUserp : userMap){
        User* user=currUserp.second;
        vector<Watchable*>& myhistory=user->get_history();
        for (size_t i = 0; i < myhistory.size(); ++i) {
            myhistory[i]= content[myhistory[i]->getId() - 1];
        }

        vector<Watchable*>& nySeen=user->getNYSeenContent();
        for (size_t i = 0; i < nySeen.size(); ++i) {
            nySeen[i]= content[nySeen[i]->getId() - 1];
        }
    }
    session_open = other.session_open;
}


//move assignment operator

Session &Session::operator=(Session &&other) {
    if (this == &other) {
        return *this;
    }
    for (size_t i = 0; i < content.size(); i++) {
        delete content[i];
    }
    content.clear();

    for (size_t i = 0; i < other.getContent().size(); i++) {
        content.push_back(other.content[i]);
    }
    other.content.clear();

    for (size_t i = 0; i < actionsLog.size(); i++) {
        delete actionsLog[i];
    }
    actionsLog.clear();
    for (size_t i = 0; i < other.getActionsLog().size(); i++) {
        actionsLog.push_back(other.actionsLog[i]);
    }
    other.actionsLog.clear();

    for (auto currUser : userMap){
        delete currUser.second;
    }
    userMap.clear();
    for (auto currUser : other.userMap){
        userMap.insert({currUser.first, currUser.second}); //maybe needs *this
    }
    other.userMap.clear();

    activeUser = userMap[other.activeUser->getName()];
    other.activeUser = nullptr;
    return *this;
}
//copy ass
Session &Session::operator=(Session &other) {
    if (this == &other) {
        return *this;
    }
    for (auto p: content)
        delete(p);
    content.clear();
    for (auto p: actionsLog)
        delete(p);
    actionsLog.clear();
    for (auto p: userMap)
        delete(p.second);
    userMap.clear();
    copy(other);
    return *this;
}


Session::~Session() {
    for (size_t i = 0; i < content.size(); i++) {
        delete content[i];
    }
    content.clear();
    for (size_t i = 0; i < actionsLog.size(); i++) {
        delete actionsLog[i];
    }
    actionsLog.clear();

    for (auto currUser = userMap.begin(); currUser != userMap.end(); ++currUser) {
        delete currUser->second;
       // userMap.erase(currUser->first);
    }
    //userMap.erase(userMap.begin(), userMap.end());
    userMap.clear();
    activeUser = nullptr;

    input.clear();
}

//move constructor
Session::Session(Session &&other): content(move(other.content)), actionsLog(move(other.actionsLog)), userMap(move(other.userMap)),
                             activeUser(other.activeUser), input(move(other.input)), session_open(other.session_open){
    other.activeUser= nullptr;
}



void Session::start() {
    session_open = true;
    cout << "SPLFLIX is now on!" << endl;
    //initialize user
    while (session_open) {
        user_Input();
        BaseAction *action;
        if (input[0] == "createuser")
            action = new CreateUser();
        else if (input[0] == "changeuser")
            action = new ChangeActiveUser();
        else if (input[0] == "deleteuser")
            action = new DeleteUser();
        else if (input[0]=="dupuser")
            action =  new DuplicateUser();
        else if (input[0] == "content")
            action = new PrintContentList();
        else if (input[0] == "watchhist")
            action = new PrintWatchHistory();
        else if (input[0] == "watch")
            action = new Watch();
        else if (input[0] == "log")
            action = new PrintActionsLog();
        else if (input[0]=="exit")
            action = new Exit();
        else
            continue;
        action->act(*this);
        actionsLog.push_back(action);
        input.clear();
    }

}


// create a vector of strings of the user input
void Session::user_Input() {
    string user_in;
    getline(cin, user_in);
    string buf;                 // Have a buffer string
    stringstream ss(user_in);       // Insert the string into a stream

    while (ss >> buf) {
        input.push_back(buf);
    }
}

//*****************************************************setters**************************************************

void Session::setActiveUser(User *user) {
    activeUser = user;
}
void Session::setSessionOpen(bool value) {
    session_open = value;
}

//*****************************************************getters**************************************************

unordered_map<string, User *> &Session::getUsermap() {
    return userMap;
}

vector<BaseAction *> Session::getActionsLog() const{
    return actionsLog;
}

vector<Watchable *> Session::getContent() const {
    return content;
}


vector<string> &Session::getInput() {
    return input;
}

User *Session::getActiveUser() const {
    return activeUser;
}
