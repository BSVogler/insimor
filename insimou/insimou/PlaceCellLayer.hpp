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


using position = std::vector<float>;
class PlaceCellLayer {
private:
    int numPos;
    std::vector<position> positions;
    std::vector<float> distance_pc;//distance of this place cells along this dimension  when initialized
    double vq_decay = 1;
    float sigmafactor = 1;
    void vector_quantization(std::vector<float> observation, std::vector<float> dist2);
public:
    PlaceCellLayer(std::vector<float> min, std::vector<float> max, std::vector<int> res);
    std::vector<float> activation(std::vector<float> observation);
    int numCells();
};



#endif /* PlaceCellLayer_hpp */

