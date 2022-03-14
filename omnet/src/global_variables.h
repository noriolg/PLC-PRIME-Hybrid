
#ifndef GLOBAL_VARIABLES_H_
#define GLOBAL_VARIABLES_H_


#include <iostream>
#include <string>
#include <map>
using namespace std;


#define INITSELFMSG 1000

#define TESTPHYMSG 2000


std::map<float, float> snrBerMap;

#define PLC_PREFERENCE 0
#define RF_PREFERENCE 1

#define NODE_TYPE_PLC 0
#define NODE_TYPE_RF 1
#define NODE_TYPE_HYBRID 2

#endif /* GLOBAL_VARIABLES_H_ */
