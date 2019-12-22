
#include <utility>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "../include/Action.h"
#include <algorithm>
#include "../include/User.h"
#include <string>
#include "../include/Session.h"
#include "../include/Watchable.h"
#include<iostream>



using namespace std;

//constructor
User::User(string  name) : history(), name(move(name)),notYetSeenContent(){
}
//methods

string& User :: getName() {
    return name;
}

void User::initNotYetSeen(Session& sess) {
    for (size_t i = 0; i < sess.getContent().size(); ++i){
        notYetSeenContent.push_back(sess.getContent()[i]) ;
    }
}


vector<Watchable*>& User::get_history() {
    return history;
}

vector<Watchable*>& User :: getNYSeenContent(){
    return notYetSeenContent;
}


void User::updateHistory(Watchable* toInsert) {
    history.push_back(toInsert);
}


void User::updateNotYetSeen(Watchable* watched) {
    for (size_t i=0; i <notYetSeenContent.size(); i++){
        if (notYetSeenContent[i]->getId() == watched->getId()){
            notYetSeenContent.erase(notYetSeenContent.begin()+(i));
        }
    }
}

void User::setName(std::string otherName) {
    name=otherName;
}


//***************LengthRecommenderUser************

LengthRecommenderUser::LengthRecommenderUser(const std::string& name) : User(name){}


Watchable* LengthRecommenderUser::getRecommendation(Session& s){

    int length_sum=0;
    int counter=0;
    for(size_t i=0; i <s.getActiveUser()->get_history().size(); ++i){
        length_sum += get_history()[i]->getLength();
        counter++;
    }
    long length_mean = length_sum/counter;
    if (getNYSeenContent().empty())
        return nullptr;
    long shortestDistance = abs((getNYSeenContent()[0]->getLength())-length_mean);
    Watchable *curr;
    Watchable *recommendedWatchable = getNYSeenContent()[0];

    for (size_t i=0; i<getNYSeenContent().size(); i++) {
        curr=getNYSeenContent()[i];
        long distanceFromAverage = abs(curr->getLength() - length_mean);
          if (distanceFromAverage < shortestDistance){
                shortestDistance = distanceFromAverage;
                recommendedWatchable = getNYSeenContent()[i];
                }
            }
            return recommendedWatchable;
        }



void LengthRecommenderUser:: updateTagHistory(Watchable* watching){
    return;
}

User *LengthRecommenderUser::clone() {
    return new LengthRecommenderUser(*this);
}


//***************RerunRecommenderUser************

RerunRecommenderUser::RerunRecommenderUser(const std::string &name):User(name), lastRecommendedIndex(0) {}

Watchable* RerunRecommenderUser::getRecommendation(Session &s) {
    //if history is empty return null TODO:check what to return if there's nothing to recommend
    if (get_history().empty())
        return nullptr;
    Watchable *toReturn = get_history()[lastRecommendedIndex];
    lastRecommendedIndex = (lastRecommendedIndex+1);
    return toReturn;
}


void RerunRecommenderUser:: updateTagHistory(Watchable* watching){}

User *RerunRecommenderUser::clone() {
    return new RerunRecommenderUser(*this);
}

//***************GenreRecommenderUser************

GenreRecommenderUser::GenreRecommenderUser(const std::string &name) : User(name){
}

Watchable* GenreRecommenderUser::getRecommendation(Session &s) {
// TODO: check what to recommend if history is empty
    if (s.getActiveUser()->get_history().empty())
        return nullptr;
    for (auto & i : tagHistory) {
        string mostPopularTag = i.second;
        for (auto curr : getNYSeenContent()) {
            for (size_t j = 0; j < curr->getTags().size(); j++) {
                if (curr->getTags()[j] == mostPopularTag)
                    return curr;
            }
        }
    }
    //if no suitable content was found, return nullptr
    return nullptr;
}

void GenreRecommenderUser ::updateTagHistory(Watchable* watching) {
    for (size_t i=0; i<watching->getTags().size(); i++) {
        string currTag = watching->getTags()[i];
            bool found = false;
            for (auto & j : tagHistory) {
                if(j.second == currTag) {
                    j.first--;
                    found = true;
                    break;
                }
            }
            if(!found) {
                pair<int,string> p1={-1, move(currTag)};
                tagHistory.push_back(move(p1));
            }
    }
    sort(tagHistory.begin(), tagHistory.end());
}

User *GenreRecommenderUser::clone() {
    return new GenreRecommenderUser(*this);
}



