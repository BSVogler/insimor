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
#include <mutex>
#include "PlaceCellLayer.hpp"
#include "settings.h"

class PlaceCellLayer;

//the numeric backend is an abstraction of SNN

class NumericBackend {
public:
    //static NumericBackend* inst();
    void setObservation(double observation[], int length);
    void setActivation(double activations[], int length);
    void setWeights(double weights[]);
    void coreloop();
    void setFeedback(double errsig);
    double* getWeights();
    float* getActions();
    //this should be private and protected

    NumericBackend(std::vector<double> min, std::vector<double> max, std::vector<int> res);
protected:
    NumericBackend ( const NumericBackend& );//prevent copy constructor
private:
    std::vector<double> observation;
    std::vector<double> activations;//computed or set actions
    std::vector<float> action;//computed action
    std::vector<double> weight;
    int lastmaxindex = 0;
    int lastaction = 0;
    const float learningrate = 1;
    const double gvwmax = 2; //todoobtain from python settings
    PlaceCellLayer placecelllayer;
    std::mutex observationmtx;//prevent simultaneous accessing and computing of weights or observations
    bool activationdirty; //dirty flag to indicate changes in the analog signal (frame)
    short observationdirty;//todo=0, doing=1, done=2
};



#endif /* numericBackend_hpp */
