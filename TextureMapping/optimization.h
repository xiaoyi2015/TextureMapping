#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include "vector4.h"
#include <vector>

class Optimization
{
public:
	Optimization(vector<Vector4> set);
	~Optimization();

	void InitColor();

private:
	vector<Vector4> pointColorSet;
};


#endif