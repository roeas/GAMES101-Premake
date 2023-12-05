//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"

#include "Utils.hpp"

void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    // 这他妈到底是什么？
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, uint32_t depth) const
{
    Vector3f lightDirect{ 0.0f, 0.0f , 0.0f };
    Vector3f lightEnvironment{ 0.0f, 0.0f , 0.0f };
    Intersection inter = intersect(ray);

    if (!inter.happened)
    {
        // 射线不与场景相交。
        return Vector3f{ 0.0f, 0.0f , 0.0f };
    }

    if (inter.m->hasEmission())
    {
        // 射线与光源相交。
        return inter.m->getEmission();
    }

    // 1. 直接光
    float pdfLight;
    Intersection interLight;
    sampleLight(interLight, pdfLight);

    Vector3f rayDir = ray.direction;
    Material *material = inter.m;
    Vector3f normal = inter.normal.normalized();

    Vector3f position = inter.coords;
    Vector3f positionLight = interLight.coords;
    Vector3f lightDir = (positionLight - position).normalized();
    float distanceToLight = (positionLight - position).norm();
    float distanceToInter = intersect(Ray{ position, lightDir }).distance;

    // 这里有点抽象，EPSILON 大概取 0.00001f 结果就会出现黑条纹，原理未知。
    if(Utils::FloatEqual(distanceToInter, distanceToLight, 0.0001f))
    {
        // 着色点与光源之间无阻挡。
        Vector3f brdfLight = material->eval(rayDir, lightDir, normal);
        float NdotL = dotProduct(normal, lightDir);
        float NdotL_Light = dotProduct(interLight.normal.normalized(), -lightDir);
        lightDirect = interLight.emit * brdfLight * NdotL * NdotL_Light / std::pow(distanceToLight, 2) / pdfLight;
    }

    // 2. 间接光
    if (get_random_float() < RussianRoulette)
    {
        // Material::sample 通过入射角，法线和材质返回一条出射光线。
        Vector3f rayOutDir = material->sample(rayDir, normal).normalized();
        Ray rayOut(position, rayOutDir);
        Intersection interNext = intersect(rayOut);

        if (interNext.happened && !interNext.m->hasEmission())
        {
            // 下一条射线未命中场景则停止递归。
            // 击中光源也停止递归，因为光源的直接贡献已经在直接光的部分计算过了。
            Vector3f brdfNext = material->eval(rayDir, rayOutDir, normal);
            float pdfNext = material->pdf(rayDir, rayOutDir, normal);
            float cosTheta = dotProduct(rayOutDir, normal);
            lightEnvironment = castRay(rayOut, depth + 1) * brdfNext * cosTheta / pdfNext / RussianRoulette;
        }
    }

    return lightDirect + lightEnvironment;
}
