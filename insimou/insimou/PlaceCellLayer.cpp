//
//  PlaceCellLayer.cpp
//  insimou
//
//  Created by Benedikt privat on 14.03.21.
//

#include "PlaceCellLayer.hpp"
#include <math.h>       /* pow */
#include<numeric>

using position = std::array<float, INPUTDIM>;

PlaceCellLayer::PlaceCellLayer(std::array<float, INPUTDIM> min, std::array<float, INPUTDIM> max, std::array<int, INPUTDIM> res){
    //calculate total number of positions
    this->numPos = res[0];
    for (int dim=1; dim<INPUTDIM; ++dim){
        this->distance_pc[dim] = abs((max[dim] - min[dim])) / (res[dim] - 1);
        numPos *= res[dim];
    }
    this->positions.reserve(numPos);
    //distance between centers covered by the pc per dimension

    //fill list of all equidistance placed neurons
    auto pos = position();
    for (int i=0; i<numPos; ++i){
        int currentDimIterator=0;
        //copy array for new point
        std::array<float, INPUTDIM> newpos = pos;
        while(newpos[currentDimIterator]+1 == res[currentDimIterator]){
            //übertrag
            newpos[currentDimIterator] = 0;
            currentDimIterator++;
        }
        newpos[currentDimIterator]++;
        for (int dim=0;dim<INPUTDIM;++dim){
            this->positions[i][dim] = pos[dim] * this->distance_pc[dim] + min[dim];
        }
    }
}

std::vector<float> PlaceCellLayer::activation(position observation){
    /*return activation per neuron by calcualting the distance to the observation  in input space*/
    auto scaleddistance = std::vector<float>();
    scaleddistance.reserve(numPos);
    if (false and vq_learning_scale > 0){
        // changes every time, so cannot be cached
//        auto rezsigma = this->sigmafactor / this->distance_pc;
//        auto dists2 = np.linalg.norm((this->positions - observation) * rezsigma, axis=1)** 2;
//        input_activation = dists2;  // use average distance;
        //this->vector_quantization(observation, dists2);
    } else {
        //use lp2 norm, weighted by dimensionality density
        int i=0;
        //calcualte distance for each neuron in scaleddistance
        for (std::vector<float>::iterator neuron=scaleddistance.begin(); neuron != scaleddistance.end(); ++neuron){
            ++i;
            // calculate norm(observation-dim), why L2 norm
            float norm = 0;
            for (int dim=0; dim < INPUTDIM; ++dim){
                norm += (this->positions[i][dim] - observation[dim]) / this->distance_pc[dim];
            }
            *neuron = sqrt(norm);
        }
    }
    //normalize and exp so that near neurosn are exponentialyl weighted more
    auto sum = accumulate(scaleddistance.begin(),scaleddistance.end(),0);
    for (std::vector<float>::iterator neurondist=scaleddistance.begin(); neurondist != scaleddistance.end(); ++neurondist) {
        *neurondist = exp(-*neurondist / sum);
    }
    
    return scaleddistance;
}


void PlaceCellLayer::vector_quantization(position observation, std::array<float, INPUTDIM> dist2) {
    // exponentially decrease strength of vq
    this->vq_decay *= 1 - this->vq_decay;
    //changeamount = vq_learning_scale * np.exp(-dist2 / self.vq_decay)
//    self.positions += (observation - self.positions) * changeamount[:, np.newaxis]
    
}

