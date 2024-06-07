#ifndef SCORING_H
#define SCORING_H

#include <glm/glm/glm.hpp>
#include <cmath>
#include <time.h>

struct Sphere
{
	glm::vec3 center;
	float radius;
};

struct Ray
{
	glm::vec3 origin;
	glm::vec3 direction;
};
bool RaySphereIntersection(const Ray& ray, const Sphere& sphere);

#endif // !SCORING_H
