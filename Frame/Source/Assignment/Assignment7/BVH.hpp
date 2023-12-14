#pragma once

#include "Bounds3.hpp"
#include "Intersection.hpp"
#include "Object.hpp"
#include "Ray.hpp"
#include "Vector.hpp"

#include <atomic>
#include <ctime>
#include <memory>
#include <vector>

struct BVHBuildNode
{
    Bounds3 bounds;
    BVHBuildNode *left;
    BVHBuildNode *right;
    Object *object;
    float area;

public:
    BVHBuildNode()
    {
        bounds = Bounds3{};
        left = nullptr;
        right = nullptr;
        object = nullptr;
    }
};

class BVHAccel
{
public:
    BVHAccel(std::vector<Object *> p, int maxPrimsInNode = 1);
    BVHBuildNode *recursiveBuild(std::vector<Object *>objects);

    Intersection Intersect(const Ray &ray) const;
    Intersection getIntersection(BVHBuildNode *node, const Ray &ray)const;

    void Sample(Intersection &pos, float &pdf);
    void getSample(BVHBuildNode *node, float p, Intersection &pos, float &pdf);

    BVHBuildNode *root;
    const int maxPrimsInNode;
    std::vector<Object *> primitives;
};
