//
//  PlaceCellLayer.hpp
//  insimou
//
//  Created by Benedikt privat on 14.03.21.
//

#ifndef PlaceCellLayer_hpp
#define PlaceCellLayer_hpp

#include <stdio.h>
#include <stdio.h>
#include <array>
#include "settings.h"




class PlaceCellLayer {
private:
    std::array<std::array<float, INPUTDIM>, INPUTDIM> positions;
    std::array<float, INPUTDIM> distance_pc;
    double vq_decay = 1;
    float sigmafactor = 1;
    std::array<int, INPUTDIM> vector_quantization(std::array<float, INPUTDIM> observation, std::array<float, INPUTDIM> dist2);
public:
    PlaceCellLayer(std::array<int, INPUTDIM>min,std::array<int, INPUTDIM> max, std::array<int, INPUTDIM> res);
    std::array<int, INPUTDIM> activation(std::array<float, INPUTDIM> observation);
};



#endif /* PlaceCellLayer_hpp */

