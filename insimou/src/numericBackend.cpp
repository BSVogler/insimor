//
//  numericBackend.cpp
//  insimou
//
//  Created by Benedikt privat on 16.01.21.
//

#include "numericBackend.hpp"
#include <iostream>
#include <random>
#include <thread>
#include <chrono>

NumericBackend::NumericBackend(std::vector<double> min,
                               std::vector<double> max,
                               std::vector<int> res):
    placecelllayer(min, max, res),
    lastmaxindex(-1),
    activationdirty(true),
    observationdirty(1)
{
    std::random_device rd;
    
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(-0.5, 0.5);
    
    //start with zero input and output
    observation.resize(min.size());//if not 0 wanted use std::fill
    action.resize(5);
    //init with random weights
    int numcells = this->placecelllayer.numCells();
    weight.reserve(numcells);
    std::cout <<"NUM WEIGHTS: "<<numcells <<std::endl;
    for (int i =0; i <numcells ; i++){
        auto b = dist(e2);
        weight.push_back(b);
    }
    outputs.resize(numcells);
}

void NumericBackend::setWeights(double weights[]){
    //assume incoming vector has same length as num cells
    observationmtx.lock();
    for (int i = 0; i < this->weight.size(); i++){
        this->weight[i] = weights[i];
    }
    std::cout<<"set weights"<<std::endl;
//    for (int i = 0; i < this->weight.size(); i++){
//        std::cout << i<<":" << weights[i] << ",";
//    }
//    std::cout << std::endl;
    observationmtx.unlock();
}


void NumericBackend::setObservation(double observation[], int length){
    observationmtx.lock();
    this->observation.resize(length);
    for (int dim = 0; dim < length; dim++){
        this->observation[dim] = observation[dim];
    }
    this->observationdirty = 1;
    observationmtx.unlock();
}

void NumericBackend::setActivation(double activations[], int length){
    //just a setter function
    observationmtx.lock();
    this->activations.resize(length);
    for (int dim = 0; dim < length; dim++){
        this->activations[dim] = activations[dim];
    }
    this->activationdirty = true;
    //std::cout<<"setactivation"<<std::endl;
    observationmtx.unlock();
}

void NumericBackend::coreloop(){
    //calling multiple times causes side effects
    
    //only set the last action when loop is once through
    if (this->activationdirty || this->observationdirty==1) {
        observationdirty=2;
        observationmtx.lock();
        std::vector<double> activations;
        //either the activation was set directly or we need to compute it
        if (this->activationdirty){
            activations = this->activations;
        } else {
            //todo cache until dirty
            if (lastmaxindex > -1){
                lastaction = observation.at(lastmaxindex)* weight.at(lastmaxindex);//action is det. by weight
            }
            
            //get activation of all input layer neurons
            //using this->activations will cause a segfault, using move semantics
            activations = this->placecelllayer.activation(this->observation);
        }
        //observationmtx.unlock();
        
        //core
        //lateral inhibition causes one hot encoding, find maximum
        lastmaxindex = int(std::distance(activations.begin(), std::max_element(activations.begin(), activations.end())));
        //std::cout<<"lastmaxindex "<<lastmaxindex<<std::endl;
        //todo only works on pole balancing
        //rate should only be a scalar value
        this->action[0] = int(copysign(1.0, this->weight[lastmaxindex]) == 1); // 0 or 1
        activationdirty = false;
        observationdirty=0;
        observationmtx.unlock();
    } else {
//std::cout<<"waiting "<<std::endl;
//observationmtx.unlock();
        
        //no new input so chill ab it before checking again
        //will only get new input once in a while so sleep a little bit
        //this is called spinning with blocking, it is better to use threading //http://www.albahari.com/threading/part2.aspx#_Signaling_with_Event_Wait_Handles
        
        //std::this_thread::sleep_for(std::chrono::milliseconds(3));
    }

}

void NumericBackend::setFeedback(double errsig){
    //update by adding the error
    if (lastmaxindex > -1){
        //only one place cell is active
        auto delta = double(copysign(1.0, (double)(lastaction)) * errsig * learningrate);
        auto neww = weight.at(lastmaxindex) + delta;
        //std::cout << delta;
        //clip
        neww = std::max(-gvwmax, std::min(neww, gvwmax));
        weight.at(lastmaxindex) = neww;
        //std::cout << delta<<std::endl;
    }
}

double* NumericBackend::getWeights(){
//    float sum = 0;
//    for (auto& n : weight)
//        sum += n;
//    std::cout<<sum<<std::endl;
//    for (int i=0;i<5;i++)
//        std::cout<<weight[i]<<",";
//    std::cout<<std::endl;
    
    //wait here till have cleaned data
    if (observationdirty!=0){
        observationmtx.lock();
    }
    auto returnres = weight.data();
    observationmtx.unlock();
    return returnres;
    
}

double* NumericBackend::getOutputs(){
    //wait here till have cleaned data
    if (observationdirty!=0){
        observationmtx.lock();
    }
    for (int i=0; i<this->weight.size(); i++) {
        if (i==lastmaxindex){
            outputs[i] = weight[i];
        } else {
            outputs[i]=0;
        }
    }
    observationmtx.unlock();
    return outputs.data();
}

float* NumericBackend::getActions(){
    //wait here till have cleaned data
    if (observationdirty!=0){
        observationmtx.lock();
    }
    auto returnres = action.data();
    observationmtx.unlock();
    return returnres;
}
