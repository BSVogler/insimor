//
//  numericBackend.hpp
//  insimou
//
//  Created by Benedikt privat on 16.01.21.
//

#ifndef numericBackend_hpp
#define numericBackend_hpp
#define INPUTDIM 10
#define OUTPUTDIM 1

#include <stdio.h>
#include <array>

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
    void setInput(std::array<float, INPUTDIM>* observation);
    void coreloop();
    void setFeedback(float errsig);
};



#endif /* numericBackend_hpp */
