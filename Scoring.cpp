#include "Scoring.h"

bool RaySphereIntersection(const Ray &ray, const Sphere &sphere)
{
	glm::vec3 oc = ray.origin - sphere.center;
	float a = glm::dot(ray.direction, ray.direction);
	float b = 2.0f * glm::dot(oc, ray.direction);
	float c = glm::dot(oc, oc) - sphere.radius * sphere.radius;
	float discriminant = b * b - 4 * a * c;
	if (discriminant < 0)
		return false;
	else
		return true;
}

