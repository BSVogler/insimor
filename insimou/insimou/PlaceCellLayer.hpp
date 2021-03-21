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
#include <vector>
#include "settings.h"
#include <math.h>


using position = std::array<float, INPUTDIM>;
class PlaceCellLayer {
private:
    int numPos;
    std::vector<position> positions;
    std::array<float, INPUTDIM> distance_pc;//distance of this place cells along this dimension  when initialized
    double vq_decay = 1;
    float sigmafactor = 1;
    void vector_quantization(std::array<float, INPUTDIM> observation, std::array<float, INPUTDIM> dist2);
public:
    PlaceCellLayer(std::array<float, INPUTDIM>min,std::array<float, INPUTDIM> max, std::array<int, INPUTDIM> res);
    std::vector<float> activation(std::array<float, INPUTDIM> observation);
    int numCells();
};



#endif /* PlaceCellLayer_hpp */

