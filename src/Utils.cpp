#include "Utils.h"
#include <cmath>

#define M_PI 3.14159265358979323846L

// converts angle from degrees to radians
double ToRadians(double Angle) {
	return Angle * M_PI / 180.0;
}

double EuclideanDistanceInMetres(Node& Node1, Node& Node2) {
	const double EarthRadius = 6378L;
	const double Factor1 = 21L;
	double Lat1, Lon1, Lat2, Lon2;
	Lat1 = Node1.GetLatitude();
	Lon1 = Node1.GetLongitude();
	Lat2 = Node2.GetLatitude();
	Lon2 = Node2.GetLongitude();

	// Pointless to calculate, but the comparison every time works out to be slower
	// if (Lat1 == Lat2 && Lon1 == Lon2) {
	// 	return 0;
	// }

	double Angle = ToRadians((Lat1 + Lat2) / 2.0);
	int Factor = (int)((EarthRadius - Factor1 * std::sin(Angle)) * 1000.0L);

	double p1Lat = ToRadians(Lat1);
	double p2Lat = ToRadians(Lat2);
	
	// acos(sin(x1) * sin(x2) + cos(x1) * cos(x2) * cos(y2-y1)) * curvature
	return std::acos(std::sin(p1Lat) * std::sin(p2Lat) + std::cos(p1Lat) * std::cos(p2Lat) * std::cos(ToRadians(Lon2-Lon1))) * Factor;
}

double EuclideanDistanceInKm(Node& Node1, Node& Node2) {
	return EuclideanDistanceInMetres(Node1, Node2)/1000.0L;
}