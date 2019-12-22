#include "../include/json.hpp"
#include "../include/Watchable.h"
#include "../include/Session.h"
#include <iostream>
#include <string>
#include<sstream>
#include<fstream>

using namespace std;

int main(int argc, char** argv) {
    Session s(argv[1]);
    s.start();
    return 0;
}

