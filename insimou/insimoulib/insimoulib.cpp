//
//  insimoulib.cpp
//  insimoulib
//
//  Created by Benedikt privat on 09.01.21.
//

#include <iostream>
#include "insimoulib.hpp"
#include "insimoulibPriv.hpp"

void insimoulib::HelloWorld(const char * s)
{
    insimoulibPriv *theObj = new insimoulibPriv;
    theObj->HelloWorldPriv(s);
    delete theObj;
};

void insimoulibPriv::HelloWorldPriv(const char * s) 
{
    std::cout << s << std::endl;
};

