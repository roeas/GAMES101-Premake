//
// Created by goksu on 2/25/20.
//

#include "Renderer.hpp"
#include "Scene.hpp"
#include "Utils.hpp"

#include <fstream>

inline float deg2rad(const float &deg) { return deg * 0.01745329251994329576923690768489f; }

// The main render function. This where we iterate over all pixels in the image,
// generate primary rays and cast these rays into the scene. The content of the
// framebuffer is saved to a file.
void Renderer::Render(const Scene &scene, const uint32_t spp)
{
    std::cout << "SPP: " << spp << std::endl;

    float scale = tan(deg2rad(scene.fov * 0.5));
    float imageAspectRatio = (float)scene.width / (float)scene.height;
    Vector3f eyePosition(278, 273, -800);

    size_t index = 0;
    std::vector<Vector3f> framebuffer(scene.width * scene.height);

    for (uint32_t heightIndex = 0; heightIndex < scene.height; ++heightIndex)
    {
        for (uint32_t widthIndex = 0; widthIndex < scene.width; ++widthIndex)
        {
            float x = (2.0f * (widthIndex + 0.5f) / (float)scene.width - 1.0f) * imageAspectRatio * scale;
            float y = (1.0f - 2.0f * (heightIndex + 0.5f) / (float)scene.height) * scale;
            Vector3f rayDir = normalize(Vector3f(-x, y, 1.0f));

            #pragma omp parallel for
            for (int sppCount = 0; sppCount < spp; ++sppCount)
            {
                framebuffer[index] += scene.castRay(Ray(eyePosition, rayDir), 0) / (float)spp;
            }
            ++index;
        }
        UpdateProgress(heightIndex / (float)scene.height);
    }
    UpdateProgress(1.0f);

    // save framebuffer to file
    std::string outputPath = Utils::PathFromAsset("output/assigment7.ppm");
    FILE *fp = fopen(outputPath.c_str(), "wb");
    (void)fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);
    for (auto i = 0; i < scene.height * scene.width; ++i)
    {
        static unsigned char color[3];
        color[0] = (unsigned char)(255 * std::pow(clamp(0.0f, 1.0f, framebuffer[i].x), 0.6f));
        color[1] = (unsigned char)(255 * std::pow(clamp(0.0f, 1.0f, framebuffer[i].y), 0.6f));
        color[2] = (unsigned char)(255 * std::pow(clamp(0.0f, 1.0f, framebuffer[i].z), 0.6f));
        fwrite(color, 1, 3, fp);
    }
    fclose(fp);
}
