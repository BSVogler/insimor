//
//  numericBackend.hpp
//  insimou
//
//  Created by Benedikt privat on 16.01.21.
//

#ifndef numericBackend_hpp
#define numericBackend_hpp


#include <stdio.h>
#include <array>
#include <vector>
#include "PlaceCellLayer.hpp"
#include "settings.h"

class PlaceCellLayer;

//the numeric backend is an abstraction of SNN

class NumericBackend {
public:
    //static NumericBackend* inst();
    void setObservation(float observation[], int length);
    void coreloop();
    void setFeedback(float errsig);
    float* getWeights();
    float* getActions();
    //this should be private and protected

    NumericBackend(std::vector<float> min, std::vector<float> max, std::vector<int> res);
protected:
    NumericBackend ( const NumericBackend& );//prevent copy constructor
private:
    std::vector<float> observation;
    std::vector<float> action;
    std::vector<float> weight;
    int lastmaxindex = 0;
    float lastactivation = 0;
    int lastaction = 0;
    const float learningrate = 1;
    const float gvwmax = 2; //todoobtain from python settings
    PlaceCellLayer placecelllayer;
};



#endif /* numericBackend_hpp */
