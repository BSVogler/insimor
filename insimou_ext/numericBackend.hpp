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

class NumericBackend {
private:
    std::array<float, INPUTDIM> observation;
    std::array<float, INPUTDIM> action;
    std::vector<float> weight;
    int lastmaxindex = 0;
    float lastactivation = 0;
    int lastaction = 0;
    const float learningrate = 1;
    const float gvwmax = 2; //todoobtain from python settings
    PlaceCellLayer placecelllayer;
    
public:
    NumericBackend(std::array<float, INPUTDIM> min, std::array<float, INPUTDIM> max, std::array<int, INPUTDIM> res);
    void setObservation(float observation[], int length);
    void coreloop();
    void setFeedback(float errsig);
    float* getWeights();
};



#endif /* numericBackend_hpp */
