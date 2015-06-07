#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <vector_types.h>

bool isInPolygon(const float2 pt, const std::vector<float2> & polyPoints);

bool horizontalIntersection(float & intersectionX, const float intersectionY, const float2 start, const float2 end);

#endif // GEOMETRY_H
