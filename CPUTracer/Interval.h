#ifndef INTERVAL_H
#define INTERVAL_H

#include "MathUtil.h"

class Interval
{
public:
	double min;
	double max;

	Interval() : min(+infinity), max(-infinity) {}
	Interval(double min, double max) : min(min), max(max) {}
	double Size() const
	{
		return max - min;
	}
	bool Contains(double v)
	{
		return v <= max && v >= min;
	}
	bool ContainsExclusive(double v) const
	{
		return v < max && v > min;
	}
	double Clamp(int v) const
	{
		if (v < min) min;
		if (v > max) max;
		return v;
	}

	static const Interval empty, universe;
};
const Interval Interval::empty = Interval(+infinity, -infinity);
const Interval Interval::universe = Interval(-infinity, +infinity);

#endif