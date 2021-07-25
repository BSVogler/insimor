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

PlaceCellLayer::PlaceCellLayer(std::vector<double> min,
                               std::vector<double> max,
                               std::vector<int> res) :
                                distance_pc(min.size())
{
    //calculate total number of positions
    for (int dim=0; dim<this->distance_pc.size(); ++dim){
        this->distance_pc[dim] = abs((max[dim] - min[dim])) / (res[dim] - 1);
        std::cout << this->distance_pc[dim] <<", ";
        if (dim==0){
            numPos = res[dim];
        } else {
            numPos *= res[dim];
        }
    }
    this->positions.reserve(numPos);
    //distance between centers covered by the pc per dimension

    //fill list of all equidistance placed neurons
    position pos = position(min.size());//current position starting with the zero position
    for (int i=0; i<numPos; ++i){
        int currentDimIterator=0;
        this->positions.push_back(pos);//alternatively set with = and use resize in init
        for (int dim=0; dim < min.size(); ++dim){
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

/*return activation per neuron by calcualting the distance to the observation in input space. Analog in, analog out hence static during one input frame.*/
std::vector<double> PlaceCellLayer::activation(position observation){
    auto scaleddistance = std::vector<double>();
    scaleddistance.reserve(numPos);
    double distancesum = 0;

    // changes every time, so cannot be cached
//        auto rezsigma = this->sigmafactor / this->distance_pc;
//        auto dists2 = np.linalg.norm((this->positions - observation) * rezsigma, axis=1)** 2;
//        input_activation = dists2;  // use average distance;
    //this->vector_quantization(observation, dists2);
    //use lp2 norm, weighted by dimensionality density
    //calculate distance for each neuron in scaleddistance
    auto dist2 = std::vector<std::array<double, INPUTDIM>>();
    dist2.reserve(this->positions.size());
    int neuron_num = 0;
    for (auto neuron : this->positions){
        // calculate ||observation-neuron||_2, why L2 norm?
        dist2[neuron_num] = std::array<double, INPUTDIM>();
        double norm = 0;
        for (int dim=0; dim < INPUTDIM; ++dim){
            double dist = double(neuron[dim] - observation[dim]) / this->distance_pc[dim];
            dist2[neuron_num][dim] =dist*dist;
            norm += dist*dist;
        }
        //std::cout <<"n("<<i<<"): "<<norm<<std::endl;
        norm =sqrt(norm);
        scaleddistance.push_back(norm);
        distancesum += norm;
        neuron_num++;
    }
    //normalize and exp so that near neurons are exponentialyl weighted more
//    std::cout <<"sum scaleddistance "<<distancesum<<std::endl;
    for (auto neurondist : scaleddistance) {
        //std::cout << neurondist << ", ";
        neurondist = exp(-neurondist / distancesum);
    }
    //std::cout<<std::endl;
    if (vq_learning_scale > 0){
        vector_quantization(observation, dist2);
    }
    return scaleddistance;
}


void PlaceCellLayer::vector_quantization(position observation, std::vector<std::array<double, INPUTDIM>> dist2) {
    // exponentially decrease strength of vq
    this->vq_decay *= 1 - this->vq_decay;
    int i = 0;
    for (auto d: dist2){
        for (int dim=0; dim < INPUTDIM; ++dim){
            auto change_amount = vq_learning_scale*exp(-d[dim]/this->vq_decay);
            double delta = (observation[dim] - this->positions[i][dim]) * change_amount;
            this->positions[i][dim] += delta;
        }
        i++;
    }
}


