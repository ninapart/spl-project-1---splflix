
#include <string>
#include <iostream>
#include <unordered_map>
#include "../include/User.h"
#include "../include/Session.h"
#include "../include/Action.h"
#include "../include/Watchable.h"
#include <vector>

class Session;

using namespace std;

//BASEACTION

//CONSTRUCTOR
BaseAction::BaseAction() : errorMsg(),status(PENDING) {}

ActionStatus BaseAction::getStatus() const {
    return status;
}

void BaseAction::complete() {
    status = COMPLETED;
}


void BaseAction::error(const string &errorMsg) {
    status = ERROR;
    cout << "Error: " + errorMsg << endl;
}

string BaseAction::enum_to_string(ActionStatus status) const {
    switch (status) {
        case COMPLETED:
            return "COMPLETED";
        case PENDING:
            return "PENDING";
        case ERROR:
            return "ERROR";
    }
    return "";
}

User *BaseAction::findUser(string &name, unordered_map<std::string, User *> &userMap) {
    auto username = userMap.find(name);
    if (username != userMap.end())
        return username->second;
    else
        return nullptr;
}


//CREATEUSER

// CONSTRUCTORS

void CreateUser::act(Session &sess) {
    string &username = (sess.getInput())[1];
    //name verification
    auto name = sess.getUsermap().find(username);
    if (name != sess.getUsermap().end()) {
        error("this name already exist");
        return;
    }
    string &user_algo = (sess.getInput())[2];
    User *user;
    if (user_algo == "len" ) {//the user choose length algorithm
        user = new LengthRecommenderUser(username);
        user->initNotYetSeen(sess);
        (sess.getUsermap()).insert({username, user});
        complete();
    } else if (user_algo == "rer") {//the user choose rerun algo
        user = new RerunRecommenderUser(username);
        user->initNotYetSeen(sess);
        (sess.getUsermap()).insert({username, user});
        complete();
    } else if (user_algo == "gen") {
        user = new GenreRecommenderUser(username);
        user->initNotYetSeen(sess);
        (sess.getUsermap()).insert({username,user});
        complete();
    } else {
        error("no such algorithm");
    }
}


string CreateUser::toString() const {
    return "Create User  " + enum_to_string(getStatus());
}

BaseAction *CreateUser::clone() {
    return new CreateUser(*this);
}


// CHANGEACTIVEUSER


void ChangeActiveUser::act(Session &sess) {
    string &username = (sess.getInput())[1];
    auto name = sess.getUsermap().find(username);
    if (name == sess.getUsermap().end()) {
        error("no such username");
    }
    // else {
    sess.setActiveUser(findUser(username, sess.getUsermap()));
    complete();
    // }
}

string ChangeActiveUser::toString() const {
    return "Change Active User  " + enum_to_string(getStatus());
}

BaseAction *ChangeActiveUser::clone() {
    return new ChangeActiveUser(*this);
}


//DELETE USER


void DeleteUser::act(Session &sess) {
    string &username = (sess.getInput())[1];
    unordered_map<string,User*>& mm=sess.getUsermap();
    auto name = mm.find(username);
    if (name == mm.end()) {
        error("no such username to delete");
    }
    delete(mm[username]);
    (mm).erase(username);
    complete();
}

string DeleteUser::toString() const {
    return "DeleteUser " + enum_to_string(getStatus());
}

BaseAction *DeleteUser::clone() {
    return new DeleteUser(*this);
}

//DUPLICATEUSER


void DuplicateUser::act(Session &sess) {
    string &userToDuplicateName = (sess.getInput())[1];
    if (sess.getUsermap().find(userToDuplicateName) == sess.getUsermap().end()) {
        error("no such username to duplicate");
        return;
    }
    string &userToCreate = (sess.getInput())[2];
    if (sess.getUsermap().find(userToCreate) != sess.getUsermap().end()) {
        error("this username already exist");
        return;
    }

    User *srcUser = sess.getUsermap().find(userToDuplicateName)->second;
    User *targetU = srcUser->clone();
    targetU->setName(userToCreate);
    sess.getUsermap().insert({userToCreate, targetU});

}

string DuplicateUser::toString() const {
    return "DuplicateUser " + enum_to_string(getStatus());
}

BaseAction *DuplicateUser::clone() {
    return new DuplicateUser(*this);
}

// PRINTCONTENTLIST


void PrintContentList::act(Session &sess) {
    for (size_t i = 0; i < sess.getContent().size(); ++i)
        std::cout << to_string(sess.getContent()[i]->getId()) + ". " + (sess.getContent()[i])->toString() << endl;
    complete();
}

string PrintContentList::toString() const {
    return "PrintContentList " + enum_to_string(getStatus());
}

BaseAction *PrintContentList::clone() {
    return new PrintContentList(*this);
}



void PrintWatchHistory::act(Session &sess) {
    cout << "Watch history for " + sess.getActiveUser()->getName() << endl;
    User *user = findUser(sess.getActiveUser()->getName(), sess.getUsermap());
    for (size_t i = 0; i < user->get_history().size(); ++i) {
        cout << to_string(i + 1) + ". " + user->get_history()[i]->recommendToString() << endl;
    }
    complete();
}

string PrintWatchHistory::toString() const {
    return "PrintWatchHistory " + enum_to_string(getStatus());
}

BaseAction *PrintWatchHistory::clone() {
    return new PrintWatchHistory(*this);
}

//WATCH

void Watch::act(Session &sess) {
    size_t id = stol((sess.getInput())[1]);
    if (id < 1 || id > sess.getContent().size()) {
        error("no such content to see");
    } else {
        Watchable *current = sess.getContent()[id - 1];
        bool watching = true;
        while (watching) {
            cout << "Watching " + current->recommendToString() << endl;
            sess.getActiveUser()->updateHistory(current);
            sess.getActiveUser()->updateTagHistory(current);
            sess.getActiveUser()->updateNotYetSeen(current);
            Watchable *next = current->getNextWatchable(sess);
            if (!next) {
                complete();
                return;
            }

            cout << "We recommend watching " + next->recommendToString() + ", continue watching? [y/n]" << endl;
            string input;
            bool checkInput = false;
            while(!checkInput){
                getline(cin, input);
                if (input == "y") {
                    current = next;
                    input = "";
                    checkInput = true;
                } else if (input == "n") {
                    watching = false;
                    input = "";
                    checkInput = true;
                } else
                    error("no such possibility.. just [y] like yellow or [n] like never)");
            }
        }
        complete();
    }
}

string Watch::toString() const {
    return "Watch " + enum_to_string(getStatus());
}

BaseAction *Watch::clone() {
    return new Watch(*this);
}



void PrintActionsLog::act(Session &sess) {
    if (sess.getActionsLog().empty())
        error("no actions to see");
    for (size_t i = 0; i < sess.getActionsLog().size(); i++) {
        size_t pos = sess.getActionsLog().size() - 1 - i;
        cout << (sess.getActionsLog()[pos])->toString() << endl;
    }
    complete();
}


string PrintActionsLog::toString() const {
    return "PrintActionsLog " + enum_to_string(getStatus());
}

BaseAction *PrintActionsLog::clone() {
    return new PrintActionsLog(*this);
}

void Exit::act(Session &sess) {
    sess.setSessionOpen(false);
    complete();
}

string Exit::toString() const {
    return "Exit " + enum_to_string(getStatus());
}

BaseAction *Exit::clone() {
    return new Exit(*this);
}


