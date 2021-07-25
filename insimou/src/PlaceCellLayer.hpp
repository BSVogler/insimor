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


using position = std::vector<double>;
class PlaceCellLayer {
private:
    int numPos;
    std::vector<position> positions;
    std::vector<double> distance_pc;//distance of this place cells along this dimension  when initialized
    double vq_decay = 1;
    float sigmafactor = 1;
    void vector_quantization(std::vector<double> observation, std::vector<std::array<double, INPUTDIM>> dist2);
public:
    PlaceCellLayer(std::vector<double> min, std::vector<double> max, std::vector<int> res);
    std::vector<double> activation(std::vector<double> observation);
    int numCells();
};



#endif /* PlaceCellLayer_hpp */

