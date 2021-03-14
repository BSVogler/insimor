//
//  PlaceCellLayer.cpp
//  insimou
//
//  Created by Benedikt privat on 14.03.21.
//

#include "PlaceCellLayer.hpp"
#include <math.h>       /* pow */

PlaceCellLayer::PlaceCellLayer(std::array<int, INPUTDIM> min, std::array<int, INPUTDIM> max, std::array<int, INPUTDIM> res){
    //distance between centers covered by the pc per dimension
    for (int i=0; i<pow(res,INPUTDIM); ++i){
        //todo create res^INPUTDIM entries
        this->distance_pc[i] = abs((max[i] - min[i])) / (res[i] - 1);
        for (int entry=0; entry<res[i];++entry){
            //for each dimension create a grid, then transform to have all coordinates
            auto pos = np.mgrid[tuple(slice(0., res[dim]) for dim in range(INPUTDIM))].T.reshape(-1,);
            this->positions[i][entry] = pos[entry] * self.distance_pc[i] + min[i];// add offset to each elemen..t
        }
        // shape: (num_cells_per_dim^INPUTDIM, num_cells_per_dim)
    }
    
}

std::array<int, INPUTDIM> PlaceCellLayer::activation(std::array<float, INPUTDIM> observation){
    if (vq_learning_scale > 0){
        // changes every time, so cannot be cached
//        auto rezsigma = this->sigmafactor / this->distance_pc;
//        auto dists2 = np.linalg.norm((this->positions - observation) * rezsigma, axis=1)** 2;
//        input_activation = dists2;  // use average distance;
//        self.vector_quantization(observation, dists2);
    } else {
        //use lp2 norm, weighted by dimensionality density
        auto scaleddistance = (this->positions - observation) / self.distance_pc;  // calculation per dimension
      auto input_activation = np.linalg.norm(scaleddistance, axis=1) ** 2;
    }
    auto vec = exp(-input_activation);
    return vec / sum(vec);  // normalize
}

std::array<int, INPUTDIM> PlaceCellLayer::vector_quantization(std::array<float, INPUTDIM> observation, std::array<float, INPUTDIM> dist2) {
//    if sys.maxsize > 2 ** 32:  # check if 64bit
//        self.vq_decay *= np.float128(1 - gv.vq_decay)  # exponentially decrease strength of vq
//    else:
//        self.vq_decay *= np.float64(1 - gv.vq_decay)  # exponentially decrease strength of vq
//    changeamount = gv.vq_learning_scale * np.exp(-dist2 / self.vq_decay)
//    self.positions += (observation - self.positions) * changeamount[:, np.newaxis]
    
}

