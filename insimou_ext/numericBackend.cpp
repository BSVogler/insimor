//
//  numericBackend.cpp
//  insimou
//
//  Created by Benedikt privat on 16.01.21.
//

#include "numericBackend.hpp"
#include <iostream>

NumericBackend::NumericBackend(std::array<int, INPUTDIM> min, std::array<int, INPUTDIM> max, std::array<int, INPUTDIM> res): placecelllayer(min, max, res)
{
    observation.fill(0);
    action.fill(0);
    weight.fill(0);
}


void NumericBackend::setObservation(float observation[], int length){
    for (int i = 0; i < length; i++){
        observation[i] = observation[i];
        //find maximum neuron
        if (observation[i] > observation[lastmaxindex]) {
            lastmaxindex = i;
        }
    }
}

void NumericBackend::coreloop(){
    //calling multiple times causes side effects
    lastaction = observation.at(lastmaxindex)* weight.at(lastmaxindex);
    
    auto activations = this->placecelllayer.activation(this->observation);
    //lateral inhibition causes one hot encoding
    lastmaxindex = std::distance(activations.begin(), std::max_element(activations.begin(), activations.end()));

    this->lastactivation = this->weight[lastmaxindex];

    //todo only works on pole balancing
    //rate should only be a scalar value
    this->action[0] = int(copysign(1.0, (float)(this->lastactivation)) == 1); // 0 or 1
}

void NumericBackend::setFeedback(float errsig){
    //update by adding the error
    auto neww = weight.at(lastmaxindex)+float(copysign(1.0, (float)(lastaction)) * errsig * learningrate);
    //limit
    neww = std::max(-gvwmax, std::min(neww, gvwmax));
    weight.at(lastmaxindex) = neww;
}

float* NumericBackend::getWeights(){
    return weight.data();
}
