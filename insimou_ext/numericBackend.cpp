//
//  numericBackend.cpp
//  insimou
//
//  Created by Benedikt privat on 16.01.21.
//

#include "numericBackend.hpp"
#include <iostream>

NumericBackend::NumericBackend(){
    input.fill(0);
    action.fill(0);
    weight.fill(0);
}


void NumericBackend::setInput(float observation[], int length){
    for (int i = 0; i < length; i++){
        input[i] = observation[i];
        //find maximum neuron
        if (observation[i] > observation[lastmaxindex]) {
            lastmaxindex = i;
        }
    }
}

void NumericBackend::coreloop(){
    lastaction = input.at(lastmaxindex)* weight.at(lastmaxindex);
}

void NumericBackend::setFeedback(float errsig){
    //update by adding the error
    auto dw = weight.at(lastmaxindex)+float(copysign(1.0, (float)(lastaction)) * errsig * learningrate);
//std::cout <<dw<< std::endl;
    weight.at(lastmaxindex) = dw;
}

float* NumericBackend::getWeights(){
    return weight.data();
}
