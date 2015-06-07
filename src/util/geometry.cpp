#include "geometry.h"

#include <iostream>

bool isInPolygon(const float2 pt, const std::vector<float2> & polyPoints) {

    int intersectionsLeft = 0;
    int intersectionsRight = 0;

    for (int i=0; i<polyPoints.size(); ++i) {
        float intersectionX;
        if (horizontalIntersection(intersectionX,pt.y,polyPoints[i],polyPoints[(i+1)%polyPoints.size()])) {
            if (intersectionX < pt.x) { ++intersectionsLeft; }
            else { ++intersectionsRight; }
        }
    }
    bool oddLeft = (intersectionsLeft % 2) == 1;
    bool oddRight = (intersectionsRight % 2) == 1;
    if (oddLeft != oddRight) {
        std::cerr << "oddLeft != oddRight" << std::endl;
    }
    return oddLeft;

}

bool horizontalIntersection(float & intersectionX, const float intersectionY, const float2 start, const float2 end) {

    if ( (start.y > intersectionY && end.y >= intersectionY) || (start.y < intersectionY && end.y <= intersectionY) ) {
        return false;
    }
    if (start.x == end.x) {
        intersectionX = end.x;
    } else if (start.x < end.x) {
        double dx = end.x - start.x;
        double dy = end.y - start.y;
        intersectionX = start.x + dx*( (intersectionY - start.y)/dy );
    } else {
        double dx = start.x - end.x;
        double dy = start.y - end.y;
        intersectionX = end.x + dx*( (intersectionY - end.y)/dy );
    }
    return true;

}
