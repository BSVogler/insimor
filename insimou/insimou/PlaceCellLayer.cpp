//
//  PlaceCellLayer.cpp
//  insimou
//
//  Created by Benedikt privat on 14.03.21.
//

#include "PlaceCellLayer.hpp"
#include <math.h>       /* pow */
#include <numeric>
#include <iostream>
//using position = std::vector<float>;

PlaceCellLayer::PlaceCellLayer(std::vector<float> min,
                               std::vector<float> max,
                               std::vector<int> res) :
                                distance_pc(min.size())
{
    //calculate total number of positions
    this->numPos = res[0];
    this->distance_pc = std::vector<float>();
    for (int dim=0; dim<this->distance_pc.size(); ++dim){
        this->distance_pc[dim] = abs((max[dim] - min[dim])) / (res[dim] - 1);
        std::cout << this->distance_pc[dim] <<", ";
        numPos *= res[dim];
    }
    this->positions.reserve(numPos);
    //distance between centers covered by the pc per dimension

    //fill list of all equidistance placed neurons
    position  pos = position(min.size());
    for (int i=0; i<numPos; ++i){
        int currentDimIterator=0;
        this->positions.push_back(pos);//alternatively set with = and use resize in init
        for (int dim=0; dim < this->positions.size(); ++dim){
            //scale
            this->positions[i][dim] = pos[dim] * this->distance_pc[dim] + min[dim];
            //std::cout << pos[dim] << "("<< this->positions[i][dim] << ")"<<", ";
        }
        //std::cout << std::endl;
        //iterate
        while(pos[currentDimIterator]+1 == res[currentDimIterator]){
            //übertrag
            pos[currentDimIterator] = 0;
            currentDimIterator++;
        }
        pos[currentDimIterator]++;
    }
}

int PlaceCellLayer::numCells(){
    return numPos;
}

/*return activation per neuron by calcualting the distance to the observation  in input space*/
std::vector<float> PlaceCellLayer::activation(position observation){
    auto scaleddistance = std::vector<float>();
    scaleddistance.reserve(numPos);
    float distancesum = 0;
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
        for (auto neuron : this->positions){
            ++i;
            // calculate norm(observation-dim), why L2 norm
            float norm = 0;
            for (int dim=0; dim < INPUTDIM; ++dim){
                float dist =float(neuron[dim] - observation[dim]) / this->distance_pc[dim];
                norm += dist*dist;
            }
            //std::cout <<"n("<<i<<"): "<<norm<<std::endl;
            norm =sqrt(norm);
            scaleddistance.push_back(norm);
            distancesum += norm;
        }
    }
    //normalize and exp so that near neurosn are exponentialyl weighted more
    std::cout <<"sum scaleddistance"<<distancesum<<std::endl;
    for (auto neurondist : scaleddistance) {
        //std::cout << neurondist << ", ";
        neurondist = exp(-neurondist / distancesum);
    }
    //std::cout<<std::endl;
    
    return scaleddistance;
}


void PlaceCellLayer::vector_quantization(position observation, std::vector<float> dist2) {
    // exponentially decrease strength of vq
    this->vq_decay *= 1 - this->vq_decay;
    //changeamount = vq_learning_scale * np.exp(-dist2 / self.vq_decay)
//    self.positions += (observation - self.positions) * changeamount[:, np.newaxis]
    
}

