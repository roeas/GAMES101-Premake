//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"
#include "Utils.hpp"

void Scene::buildBVH()
{
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    // 这里原本的逻辑大概是随机采样场景中的一盏光源，
    // 估计是因为他们混用了 Light 和 Emissive Mash 的概念才写得出来那么脑溢血的代码。
    // 由于当前场景里只存在唯一一盏光源，我们可以改得稍微 Trick 一点。
    for (const auto &obj : objects)
    {
        if (obj->hasEmit())
        {
            obj->Sample(pos, pdf);
            break;
        }
    }
}

bool Scene::trace(
    const Ray &ray,
    const std::vector<Object *> &objects,
    float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k)
    {
        float tNearK = std::numeric_limits<float>::max();
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear)
        {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }

    return (nullptr != *hitObject);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, uint32_t depth) const
{
    Vector3f lightDirect{ 0.0f };
    Vector3f lightEnvironment{ 0.0f };
    Intersection inter = intersect(ray);

    if (!inter.happened)
    {
        // 射线不与场景相交。
        return Vector3f{ 0.0f };
    }

    if (inter.m->hasEmission())
    {
        // 射线与光源相交。
        return inter.m->getEmission();
    }

    Vector3f rayDir = ray.direction;
    Material *material = inter.m;
    Vector3f normal = inter.normal.normalized();
    Vector3f position = inter.coords;

    // 1. 直接光
    if (MaterialType::MIRROR != inter.m->getType())
    {
        float pdfLight;
        Intersection interLight;
        sampleLight(interLight, pdfLight);

        Vector3f positionLight = interLight.coords;
        Vector3f lightDir = (positionLight - position).normalized();
        float distanceToLight = (positionLight - position).norm();
        float distanceToInter = intersect(Ray{ position, lightDir }).distance;

        // 这里有点抽象，EPSILON 大概取 0.00001f 结果就会出现黑条纹，原理未知。
        if (Utils::FloatEqual(distanceToInter, distanceToLight, 0.0001f))
        {
            // 着色点与光源之间无阻挡。
            Vector3f brdfLight = material->eval(rayDir, lightDir, normal);
            float NdotL = dotProduct(normal, lightDir);
            float NdotL_Light = dotProduct(interLight.normal.normalized(), -lightDir);
            lightDirect = interLight.emit * brdfLight * NdotL * NdotL_Light / std::pow(distanceToLight, 2) / pdfLight;
        }
    }

    // 2. 间接光
    constexpr float RussianRoulette = 0.8;
    if (get_random_float() < RussianRoulette)
    {
        Vector3f rayOutDir = material->sample(rayDir, normal).normalized();
        Ray rayOut(position, rayOutDir);
        Intersection interNext = intersect(rayOut);

        if (interNext.happened &&
            (MaterialType::MIRROR == inter.m->getType() || !interNext.m->hasEmission()))
        {
            // 下一条射线未命中场景则停止递归。
            // 击中光源也停止递归，因为光源的直接贡献已经在直接光的部分计算过了。
            // 但是完美镜面材质因为不直接采样光源，故将直接光的计算移至此部分。
            Vector3f brdfNext = material->eval(rayDir, rayOutDir, normal);
            float pdfNext = material->pdf(rayDir, rayOutDir, normal);
            if (pdfNext >= 0.0001f)
            {
                float cosTheta = dotProduct(rayOutDir, normal);
                lightEnvironment = castRay(rayOut, depth + 1) * brdfNext * cosTheta / pdfNext / RussianRoulette;
            }
        }
    }

    return lightDirect + lightEnvironment;
}
