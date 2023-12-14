#include "global.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Triangle.hpp"
#include "Utils.hpp"
#include "Vector.hpp"

#include <chrono>

int main(int argc, char** argv)
{
    // Change the definition here to change resolution
    Scene scene(784, 784);

    Material* red = new Material(MaterialType::DIFFUSE);
    red->Kd = Vector3f(0.63f, 0.065f, 0.05f);
    Material* green = new Material(MaterialType::DIFFUSE);
    green->Kd = Vector3f(0.14f, 0.45f, 0.091f);
    Material* white = new Material(MaterialType::DIFFUSE);
    white->Kd = Vector3f(0.725f, 0.71f, 0.68f);
    Material *light = new Material(MaterialType::DIFFUSE, (
        8.0f * Vector3f(0.747f + 0.058f, 0.747f + 0.258f, 0.747f) +
        15.6f * Vector3f(0.740f + 0.287f, 0.740f + 0.160f, 0.740f) +
        18.4f * Vector3f(0.737f + 0.642f, 0.737f + 0.159f, 0.737f)));
    light->Kd = Vector3f(0.65f);

    Material *pMirror = new Material(MaterialType::MIRROR);

    Material *pPBRPingk = new Material(MaterialType::PBR);
    pPBRPingk->m_albedo = Vector3f{ 1.0f, 0.42f, 0.79f };
    pPBRPingk->m_metallic = 0.1f;
    pPBRPingk->m_roughness = 0.9f;

    MeshTriangle floor(Utils::PathFromAsset("model/cornellbox/floor.obj"), white);
    MeshTriangle shortbox(Utils::PathFromAsset("model/cornellbox/shortbox.obj"), pPBRPingk);
    MeshTriangle tallbox(Utils::PathFromAsset("model/cornellbox/tallbox.obj"), pMirror);
    MeshTriangle left(Utils::PathFromAsset("model/cornellbox/left.obj"), red);
    MeshTriangle right(Utils::PathFromAsset("model/cornellbox/right.obj"), green);
    MeshTriangle light_(Utils::PathFromAsset("model/cornellbox/light.obj"), light);

    scene.Add(&floor);
    scene.Add(&shortbox);
    scene.Add(&tallbox);
    scene.Add(&left);
    scene.Add(&right);
    scene.Add(&light_);

    scene.buildBVH();

    Renderer r;
    auto start = std::chrono::system_clock::now();
    r.Render(scene, 4);
    auto stop = std::chrono::system_clock::now();

    auto time = stop - start;
    std::cout << "Render complete" << std::endl;
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(time).count() << " hours" << std::endl;
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(time).count() << " minutes" << std::endl;
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(time).count() << " seconds" << std::endl;

    return 0;
}
