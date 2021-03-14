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

class NumericBackend {
private:
    std::array<float, INPUTDIM> input;
    std::array<float, INPUTDIM> action;
    std::array<float, INPUTDIM> weight;
    int lastmaxindex = 0;
    int lastaction = 0;
    const float learningrate = 0.1;
    
public:
    NumericBackend();
    void setInput(float observation[], int length);
    void coreloop();
    void setFeedback(float errsig);
    float* getWeights();
};



#endif /* numericBackend_hpp */
