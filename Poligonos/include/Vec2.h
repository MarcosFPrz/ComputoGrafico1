#pragma once
#include <iostream>
#include <vector>

class Vec2
{
public:
	float v[2];
	void x(float val) {
		v[0] = val;}
	float x() { return v[0]; }
};

