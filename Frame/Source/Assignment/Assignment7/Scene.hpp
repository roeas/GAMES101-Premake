#pragma once

#include "BVH.hpp"
#include "Object.hpp"
#include "Ray.hpp"
#include "Vector.hpp"

#include <vector>

class Scene
{
public:
    // setting up options
    int width = 1280;
    int height = 960;
    double fov = 40;

    BVHAccel *bvh;
    std::vector<Object *> objects;

    Scene(int w, int h) : width(w), height(h) {}
    void Add(Object *object) { objects.emplace_back(object); }

    Intersection intersect(const Ray& ray) const;
    void buildBVH();
    Vector3f castRay(const Ray &ray, uint32_t depth) const;
    void sampleLight(Intersection &pos, float &pdf) const;
    bool trace(const Ray &ray, const std::vector<Object*> &objects, float &tNear, uint32_t &index, Object **hitObject);
};