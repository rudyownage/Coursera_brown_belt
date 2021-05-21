#include "sphere.h"

using namespace std;

namespace Sphere {
	const double PI = 3.1415926535;

	double ConvertDegreesToRadians(double degrees) {
		return degrees * PI / 180.0;
	}

	Point Point::FromDegrees(double latitude, double longitude) {
		return { ConvertDegreesToRadians(latitude), ConvertDegreesToRadians(longitude) };
	}
	const double EARTH_RADIUS = 6'371'000;

	double Distance(Point lhs, Point rhs) {
		Point lhsInRadians = lhs.FromDegrees(lhs.latitude, lhs.longitude);
		Point rhsInRadians = rhs.FromDegrees(rhs.latitude, rhs.longitude);

		return	acos(sin(lhsInRadians.latitude) * sin(rhsInRadians.latitude) + cos(lhsInRadians.latitude) 
					 * cos(rhsInRadians.latitude) * cos(fabs(lhsInRadians.longitude - rhsInRadians.longitude))) * EARTH_RADIUS;
	}
}