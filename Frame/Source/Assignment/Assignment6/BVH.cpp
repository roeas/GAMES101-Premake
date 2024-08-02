#include <algorithm>
#include <cassert>
#include "BVH.hpp"

BVHAccel::BVHAccel(std::vector<Object*> p, int maxPrimsInNode,
                   SplitMethod splitMethod)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)), splitMethod(splitMethod),
      primitives(std::move(p))
{
    time_t start, stop;
    time(&start);
    if (primitives.empty())
        return;

    root = recursiveBuild(primitives);

    time(&stop);
    double diff = difftime(stop, start);
    int hrs = (int)diff / 3600;
    int mins = ((int)diff / 60) - (hrs * 60);
    int secs = (int)diff - (hrs * 3600) - (mins * 60);

    printf(
        "\rBVH Generation complete: \nTime Taken: %i hrs, %i mins, %i secs\n\n",
        hrs, mins, secs);
}

BVHBuildNode* BVHAccel::recursiveBuild(std::vector<Object*> objects)
{
    BVHBuildNode* pNode = new BVHBuildNode();

    // 所有物体的包围盒集合。
    Bounds3 bounds;
    for (int i = 0; i < objects.size(); ++i)
    {
        bounds = Union(bounds, objects[i]->getBounds());
    }

    // 只存在一个物体，建立叶节点。
    if (objects.size() == 1)
    {
        pNode->bounds = objects[0]->getBounds();
        pNode->object = objects[0];
        pNode->left = nullptr;
        pNode->right = nullptr;

        return pNode;
    }
    // 存在两个物体，建立其 left 和 right 叶节点。
    else if (objects.size() == 2)
    {
        pNode->left = recursiveBuild(std::vector{ objects[0] });
        pNode->right = recursiveBuild(std::vector{ objects[1] });
        pNode->bounds = Union(pNode->left->bounds, pNode->right->bounds);

        return pNode;
    }
    else
    {
        // 由所有物体的质心组成的包围盒。
        Bounds3 centroidBounds;
        for (int i = 0; i < objects.size(); ++i)
        {
            centroidBounds = Union(centroidBounds, objects[i]->getBounds().Centroid());
        }

        // 最长的维度。
        switch (centroidBounds.maxExtent())
        {
        case 0:
            // X 轴。
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2)
            {
                // 将所有物体按照其包围盒的质心的 X 轴排序，Y 轴和 Z 轴的 case 同理。
                return f1->getBounds().Centroid().x < f2->getBounds().Centroid().x;
            });
            break;
        case 1:
            // Y 轴。
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2)
            {
                return f1->getBounds().Centroid().y < f2->getBounds().Centroid().y;
            });
            break;
        case 2:
            // Z 轴。
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2)
            {
                return f1->getBounds().Centroid().z < f2->getBounds().Centroid().z;
            });
            break;
        }

        const auto &begin = objects.begin();
        const auto &end = objects.end();

#define ENABLE_SAH 1

#if ENABLE_SAH

        // 一份比较清晰的 SAH 介绍：https://zhuanlan.zhihu.com/p/50720158

        // 划分方式的总数。
        constexpr uint8_t SlashCount = 8;
        constexpr float SlashCountInv = 1.0f / static_cast<float>(SlashCount);
        const float SC = centroidBounds.SurfaceArea();

        // 用于记录最优的划分方式。
        uint8_t minCostIndex = SlashCount / 2;
        float minCost = std::numeric_limits<float>::infinity();

        for (uint8_t index = 1; index < SlashCount; ++index)
        {
            const auto &target = objects.begin() + (objects.size() * index * SlashCountInv);
            auto leftObjects = std::vector<Object *>(begin, target);
            auto rightObjects = std::vector<Object *>(target, end);

            // 分别计算划分之后两部分包围盒的表面积。
            Bounds3 leftBounds, rightBounds;
            for (const auto &obj : leftObjects)
            {
                leftBounds = Union(leftBounds, obj->getBounds().Centroid());
            }
            for (const auto &obj : rightObjects)
            {
                rightBounds = Union(rightBounds, obj->getBounds().Centroid());
            }

            float SA = leftBounds.SurfaceArea();
            float SB = rightBounds.SurfaceArea();
            float a = leftObjects.size();
            float b = rightObjects.size();
            float cost = (SA * a + SB * b) / SC + 0.125f;

            if (cost < minCost)
            {
                // 更新更优的划分方式。
                minCost = cost;
                minCostIndex = index;
            }
        }

        const auto &target = objects.begin() + (objects.size() * minCostIndex * SlashCountInv);

#else // ENABLE_SAH

        // 基本的 BVH 划分方式，按数量从中间一分为二。
        const auto &target = objects.begin() + (objects.size() / 2);

#endif // ENABLE_SAH

        auto leftObjects = std::vector<Object *>(begin, target);
        auto rightObjects = std::vector<Object *>(target, end);

        pNode->left = recursiveBuild(leftObjects);
        pNode->right = recursiveBuild(rightObjects);
        pNode->bounds = Union(pNode->left->bounds, pNode->right->bounds);
    }

    return pNode;
}

Intersection BVHAccel::Intersect(const Ray& ray) const
{
    Intersection isect;
    if (!root)
        return isect;
    isect = BVHAccel::getIntersection(root, ray);
    return isect;
}

Intersection BVHAccel::getIntersection(BVHBuildNode* node, const Ray& ray) const
{
    std::array<int, 3> dirIsNeg{
        static_cast<int>(ray.direction.x < 0.0f),
        static_cast<int>(ray.direction.y < 0.0f),
        static_cast<int>(ray.direction.z < 0.0f)
    };
    if (!node->bounds.IntersectP(ray, ray.direction_inv, std::move(dirIsNeg)))
    {
        return Intersection{};
    }

    // 叶节点
    if (node->left == nullptr && node->right == nullptr)
    {
        return node->object->getIntersection(ray);
    }

    Intersection leaf1 = getIntersection(node->left, ray);
    Intersection leaf2 = getIntersection(node->right, ray);
    
    return leaf1.distance < leaf2.distance ? leaf1 : leaf2;
}
