
#include "../include/Watchable.h"
#include <string>

#include <vector>
#include "../include/Session.h"

using namespace std;

//WATCHABLE
//constructor
Watchable::Watchable(long id, int length, const std::vector<std::string>& tags):
        id(id), length(length), tags(tags) { }


//getters
long Watchable::getId() const {
    return id;
}
int Watchable::getLength() const {
    return length;
}
std::vector<std::string> Watchable::getTags() const {
    return  tags;
}
string Watchable:: tagsToString(vector<string> tags) const{
    string toReturn;
    if(!tags.empty()) {
        for (const auto & tag : tags) {
            toReturn +=", "+ tag;
        }
        toReturn = toReturn.substr(2);
    }
    return toReturn;
}

//MOVIE
//constructor
Movie::Movie(long id, const string &name, int length, const std::vector<std::string> &tags) : Watchable(id, length, tags), name(name){}


string Movie::toString() const {
    string tags_string = tagsToString(getTags());
    return getName() +" "+ to_string(getLength())+"  minutes ["+tags_string+"]";
}

string Movie::recommendToString() const {
    string tags_string = tagsToString(getTags());
    return getName();
}

Watchable *Movie::getNextWatchable(Session & sess) const {
    return sess.getActiveUser()->getRecommendation(sess);
}

string Movie:: getName() const{
    return name;
}
Watchable *Movie::clone() {
    return new Movie(*this);
}




//EPISODE

Episode::Episode(long id, const string& seriesName,int length, int season, int episode, const std::vector<std::string> &tags) :
                 Watchable(id, length, tags), seriesName(seriesName), season(season), episode(episode){}


std::string Episode::toString() const{
    string tags_string = tagsToString(getTags()) ;
    return getName()+" S"+ to_string(season)+"E"+to_string(episode)+" "+to_string(getLength())+" minutes ["+tags_string+"]" ;
}

string Episode::recommendToString() const {
   return getName()+" S"+ to_string(season)+"E"+to_string(episode);
}

Watchable* Episode::getNextWatchable(Session& sess) const{
    //check if there's more content in content vector, and if the next element in content vector is of the same series
    if (sess.getContent()[sess.getContent().size()-1]->getId()>getId() && sess.getContent()[getId()]->getName() == seriesName)
        return sess.getContent()[getId()];
    return sess.getActiveUser()->getRecommendation(sess);
}

string Episode:: getName() const{
    return seriesName;
}

Watchable *Episode::clone() {
    return new Episode(*this);
}


