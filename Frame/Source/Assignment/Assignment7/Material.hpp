#pragma once

#include "global.hpp"
#include "Vector.hpp"

#include <cmath>

#define MY_PI 3.1415926535897932384626433832795
#define MY_INV_PI 0.31830988618379067153776752674503

enum class MaterialType
{
    DIFFUSE,
    PBR,
    MIRROR,
};

class Material
{
private:
    Vector3f reflect(const Vector3f &I, const Vector3f &N) const
    {
        return I - 2 * dotProduct(I, N) * N;
    }

    Vector3f refract(const Vector3f &I, const Vector3f &N, const float &ior) const
    {
        float cosi = clamp(-1.0f, 1.0f, dotProduct(I, N));
        float etai = 1.0f, etat = ior;
        Vector3f n = N;
        if (cosi < 0.0f) { cosi = -cosi; }
        else { std::swap(etai, etat); n = -N; }
        float eta = etai / etat;
        float k = 1.0f - eta * eta * (1 - cosi * cosi);

        return k < 0.0f ? 0.0f : eta * I + (eta * cosi - sqrtf(k)) * n;
    }

    void fresnel(const Vector3f &I, const Vector3f &N, const float &ior, float &kr) const
    {
        float cosi = clamp(-1.0f, 1.0f, dotProduct(I, N));
        float etai = 1.0f, etat = ior;
        if (cosi > 0.0f) { std::swap(etai, etat); }
        // Compute sini using Snell's law
        float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
        // Total internal reflection
        if (sint >= 1.0f) { kr = 1.0f; }
        else
        {
            float cost = sqrtf(std::max(0.f, 1 - sint * sint));
            cosi = fabsf(cosi);
            float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
            float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
            kr = (Rs * Rs + Rp * Rp) / 2.0f;
        }
    }

    Vector3f toWorld(const Vector3f &a, const Vector3f &N)
    {
        Vector3f B, C;
        if (std::fabs(N.x) > std::fabs(N.y))
        {
            float invLen = 1.0f / std::sqrt(N.x * N.x + N.z * N.z);
            C = Vector3f(N.z * invLen, 0.0f, -N.x * invLen);
        }
        else
        {
            float invLen = 1.0f / std::sqrt(N.y * N.y + N.z * N.z);
            C = Vector3f(0.0f, N.z * invLen, -N.y * invLen);
        }
        B = crossProduct(C, N);
        return a.x * B + a.y * C + a.z * N;
    }

    Vector3f FresnelSchlick(float cosTheta, Vector3f F0)
    {
        return F0 + (Vector3f{ 1.0f } - F0) * pow(1.0 - cosTheta, 5.0);
    }

    float DistributionGGX(float NdotH, float rough)
    {
        float a = rough * rough;
        float a2 = a * a;
        float denom = NdotH * NdotH * (a2 - 1.0) + 1.0;
        return a2 * MY_INV_PI * denom * denom;
    }

    float Visibility(float NdotV, float NdotL, float rough)
    {
        float f = rough + 1.0;
        float k = f * f * 0.125;
        float ggxV = 1.0 / (NdotV * (1.0 - k) + k);
        float ggxL = 1.0 / (NdotL * (1.0 - k) + k);
        return ggxV * ggxL * 0.25;
    }

    Vector3f LerpVec3(const Vector3f &a, const Vector3f &b, float t)
    {
        return a + (b - a) * t;
    }

public:
    MaterialType m_type;
    Vector3f m_emission;
    Vector3f Kd, Ks;

    Vector3f m_albedo;
    float m_metallic;
    float m_roughness;

    Material(MaterialType t = MaterialType::DIFFUSE, Vector3f e = Vector3f(0.0f, 0.0f, 0.0f))
        : m_type(t), m_emission(std::move(e)) {}

    MaterialType getType() const { return m_type; }
    Vector3f& getEmission() { return m_emission; }
    const Vector3f& getEmission() const { return m_emission; }
    bool hasEmission() const
    {
        if (m_emission.norm() > 0.00001f)
        {
            return true;
        }

        return false;
    }

    // 返回出射光线的方向。
    inline Vector3f sample(const Vector3f &wi, const Vector3f &N);
    // 返回采样的 PDF。
    inline float pdf(const Vector3f &wi, const Vector3f &wo, const Vector3f &N);
    // 返回 BRDF。
    inline Vector3f eval(const Vector3f &wi, const Vector3f &wo, const Vector3f &N);
};

Vector3f Material::sample(const Vector3f &wi, const Vector3f &N)
{
    switch (m_type)
    {
        case MaterialType::DIFFUSE:
        case MaterialType::PBR:
        {
            // uniform sample on the hemisphere
            float x_1 = get_random_float(), x_2 = get_random_float();
            float z = std::fabs(1.0f - 2.0f * x_1);
            float r = std::sqrt(1.0f - z * z), phi = 2 * MY_PI * x_2;
            Vector3f localRay(r * std::cos(phi), r * std::sin(phi), z);
            return toWorld(localRay, N);
            break;
        }
        case MaterialType::MIRROR:
        {
            return reflect(wi, N);
            break;
        }
    }
}

float Material::pdf(const Vector3f &wi, const Vector3f &wo, const Vector3f &N)
{
    if (dotProduct(wo, N) <= 0.0f)
    {
        return 0.0f;
    }

    switch (m_type)
    {
        case MaterialType::DIFFUSE:
        case MaterialType::PBR:
        {
            // uniform sample probability 1 / (2 * PI)
            return 0.15915494309189533576888376337251f;
            break;
        }
        case MaterialType::MIRROR:
        {
            return 1.0f;
            break;
        }
    }
}

Vector3f Material::eval(const Vector3f &wi, const Vector3f &wo, const Vector3f &N)
{
    if (dotProduct(N, wo) <= 0.0f)
    {
        return Vector3f{ 0.0f };
    }

    switch(m_type)
    {
        case MaterialType::DIFFUSE:
        {
            // calculate the contribution of diffuse   model
            return Kd * MY_INV_PI;
            break;
        }
        case MaterialType::PBR:
        {
            Vector3f lightDir = wo.normalized();
            Vector3f viewDir = -wi.normalized();
            Vector3f harfDir = normalize(lightDir + viewDir);

            float NdotV = std::max(dotProduct(N, viewDir), 0.0f);
            float NdotL = std::max(dotProduct(N, lightDir), 0.0f);
            float NdotH = std::max(dotProduct(N, harfDir), 0.0f);
            float HdotV = std::max(dotProduct(harfDir, viewDir), 0.0f);

            Vector3f F0 = LerpVec3(Vector3f{ 0.04f, 0.04f , 0.04f }, m_albedo, m_metallic);
            Vector3f  Fre = FresnelSchlick(HdotV, F0);
            float NDF = DistributionGGX(NdotH, m_roughness);
            float Vis = Visibility(NdotV, NdotL, m_roughness);
            Vector3f specularBRDF = Fre * NDF * Vis;

            Vector3f KD = LerpVec3(Vector3f{ 1.0f } - Fre, Vector3f{ 1.0f }, m_metallic);
            Vector3f diffuseBRDF = m_albedo * MY_INV_PI;

            return KD * diffuseBRDF + specularBRDF;

            break;
        }
        case MaterialType::MIRROR:
        {
            // 听说完美镜面是需要考虑菲涅尔项的，有空再研究吧。
            return Vector3f{ 1.0f , 1.0f , 1.0f };
            break;
        }
    }
}
