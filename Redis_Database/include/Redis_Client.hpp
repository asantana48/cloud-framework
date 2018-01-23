#ifndef Redis_Client_H
#define Redis_Client_H

#include <iostream>
#include <string>

#include "redox.hpp"


using namespace std;
using redox::Redox;
using redox::Command;
using redox::Subscriber;


void increment_Key();
string get_Key();
void Redis_HMSET(string, string);


#endif
