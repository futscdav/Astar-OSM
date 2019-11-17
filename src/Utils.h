#ifndef _UTILS_GUARD
#define _UTILS_GUARD

#include "Node.h"

// distance between two nodes in m, takes curvature of Earth into account
double EuclideanDistanceInMetres(Node& Node1, Node& Node2);
// same as EuclideanDistanceInMeters, but divided by 1000.0
double EuclideanDistanceInKm(Node& Node1, Node& Node2);

#endif