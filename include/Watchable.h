#ifndef WATCHABLE_H_
#define WATCHABLE_H_

#include <string>
#include <vector>
class Session;

using namespace std;


class Watchable{
public:
    Watchable(long id, int length, const std::vector<std::string>& tags);
    virtual ~Watchable() = default;
    virtual std::string toString() const = 0;
    virtual string recommendToString() const = 0;
    virtual Watchable* getNextWatchable(Session& sess) const = 0;
    //getters
    virtual long getId() const ;
    virtual int getLength() const;
    virtual std::vector<std::string> getTags() const;
    virtual string getName() const = 0;
    virtual string tagsToString(vector<string>) const;
    virtual Watchable* clone()=0;
private:
    const long id;
    int length;
    std::vector<std::string> tags;

};

class Movie : public Watchable{
public:
    Movie(long id, const std::string& name, int length, const std::vector<std::string>& tags);
    virtual std::string toString() const;
    virtual Watchable* getNextWatchable(Session&) const;
    virtual string getName() const ;
    string recommendToString() const;
    Watchable* clone();
private:
    std::string name;
};


class Episode: public Watchable{
public:
    Episode(long id, const std::string& seriesName,int length, int season, int episode ,const std::vector<std::string>& tags);
    virtual std::string toString() const;
    virtual Watchable* getNextWatchable(Session&) const;
    virtual string getName() const;
    string recommendToString() const;
    Watchable* clone();
private:
    std::string seriesName;
    int season;
    int episode;
    long nextEpisodeId;


};

#endif