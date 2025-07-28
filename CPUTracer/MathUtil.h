#include <random>
#ifndef MATHUTIL_H
#define MATHUTIL_H


// Constants
#define UTIL_CONST
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double DegreesToRadians(double degrees) {
    return degrees * pi / 180.0;
}
/// <summary>
/// Returns [0,1]
/// </summary>
/// <returns></returns>
inline double RandomDouble()
{
    thread_local std::mt19937 generator(std::random_device{}());
    thread_local std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(generator);
}
inline double RandomDouble(double min, double max)
{
    return min + RandomDouble() * (max - min);
}

#endif