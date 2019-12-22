#ifndef USER_H_
#define USER_H_

#include <unordered_set>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <string>
#include <vector>

class Watchable;
class Session;

using namespace std;

class User{
public:
    User(std::string  name);
    virtual ~User() = default;
    virtual Watchable* getRecommendation(Session& s) = 0;
    virtual void updateTagHistory(Watchable* watching) =0;
    std::string& getName();// const;
    void setName(std::string);// const;
    std::vector<Watchable*>& get_history();
    void initNotYetSeen(Session& sess);
    vector<Watchable*>& getNYSeenContent();
    void updateHistory(Watchable* toInsert);
    void updateNotYetSeen(Watchable* watched);
    virtual User* clone()=0;
protected:
    std::vector<Watchable*> history;
private:
    string name;
    vector<Watchable*> notYetSeenContent;
};



class LengthRecommenderUser : public User {
public:
    LengthRecommenderUser(const std::string& name);
    virtual Watchable* getRecommendation(Session& s);
    virtual void updateTagHistory(Watchable* watch);
    User* clone();
private:
};

class RerunRecommenderUser : public User {
public:
    RerunRecommenderUser(const std::string& name);
    virtual Watchable* getRecommendation(Session& s);
    virtual void updateTagHistory(Watchable* watch);
    User* clone();
private:
    size_t lastRecommendedIndex;
};



class GenreRecommenderUser : public User {
public:
    GenreRecommenderUser(const std::string& name);
    virtual Watchable* getRecommendation(Session& s);
    virtual void updateTagHistory(Watchable* watching);
    //vector<pair<int, string>> getTagHistory();
    User* clone();
private:
    vector< pair<int, string> > tagHistory;
};



#endif















