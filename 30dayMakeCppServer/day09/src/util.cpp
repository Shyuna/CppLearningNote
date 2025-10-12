#include "util.h"
#include <iostream>
#include <cstdlib>

void errif(bool condition, const std::string errmsg){
    if(condition){
        std::cerr<<errmsg<<std::endl;
        exit(EXIT_FAILURE);
    }
}