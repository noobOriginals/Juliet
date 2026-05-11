#include <lib/render.hpp>

// Std includes
#include <iostream>
#include <cmath>
#include <queue>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

// Local includes
#include <util/util.hpp>

using namespace glm;

namespace lib {

// Render

Render::Render(RenderParameters params) {
    image = makeImage(params.screenWidth, params.screenHeight);
    screenW = params.screenWidth;
    screenH = params.screenHeight;
    camPos = params.cameraPos;
    maxBounces = params.maxBounces;
    samplesPerPixel = params.samplesPerPixel;
    tileSize = params.threadTileSize;
    supersampling = params.enableSupersampling;
    gammaCorrection = params.enableGammaCorrection;
    multiThreading = params.enableMultiThreading;
    raytraceCallback = params.raytraceCallback;

    if (params.threadCount == 0) {
        threadCount = std::thread::hardware_concurrency();
    } else {
        threadCount = params.threadCount;
    }

    vec3 dir = normalize(params.cameraLookAt - camPos);
    vec3 right = normalize(cross(dir, params.worldUp));
    vec3 up = normalize(cross(right, dir));

    float32 height = std::tan(util::degToRad(params.vfov * 0.5f)) * params.focalLength;
    float32 width = height * screenW / screenH;

    pixelDeltaW = right * (2.0f * width / screenW);
    pixelDeltaH = up * (2.0f * height / screenH);
    pixelOrigin = camPos + dir * params.focalLength - right * width - up * height + pixelDeltaW * 0.5f + pixelDeltaH * 0.5f;
}

void Render::render() const {
    std::cout << screenW << " x " << screenH << " / " << tileSize << " @SPP " << samplesPerPixel << "\n";

    if (!multiThreading) {
        Pixel* pixels = (Pixel*)image.data.data();
        for (int32 y = 0; y < screenH; y++) {
            for (int32 x = 0; x < screenW; x++) {
                pixels[y * screenW + x] = renderPixel(x, y);
            }
        }
        return;
    }

    std::queue<Tile> workQueue;
    for (int32 y = 0; y < screenH; y += tileSize) {
        for (int32 x = 0; x < screenW; x += tileSize) {
            workQueue.push({x, y, clamp(x + tileSize, 0, screenW), clamp(y + tileSize, 0, screenH)});
        }
    }

    std::mutex queueMutex;

    const int32 totalTiles = ((screenW + tileSize - 1) / tileSize) * ((screenH + tileSize - 1) / tileSize);
    std::atomic<int32> completedTiles(0);

    auto worker = [&]() {
        while (true) {
            Tile tile;
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                if (workQueue.empty()) {
                    return;
                }
                tile = workQueue.front();
                workQueue.pop();
            }
            renderTile(tile);

            int32 done = ++completedTiles;
            int32 pct = done * 100 / totalTiles;
            int32 filled = pct * 0.7;
            std::printf("\r[%-50s] %3d%% (%d/%d tiles)", std::string(filled, '#').append(70 - filled, ' ').c_str(), pct, done, totalTiles);
            std::fflush(stdout);
            if (done == totalTiles) {
                std::printf("\n");
            }
        }
    };

    std::vector<std::thread> threads;
    for (int32 i = 0; i < threadCount; i++) {
        threads.emplace_back(worker);
    }
    for (auto& t : threads) {
        t.join();
    }
}

void Render::save(std::string filepath) const {
    saveImage(filepath.c_str(), image);
}

// Render private helpers

Pixel Render::renderPixel(int32 x, int32 y) const {
    vec3 color(0.0f);
    if (!supersampling) {
        core::Ray ray;
        ray.org = camPos;
        ray.dir = normalize(pixelOrigin + pixelDeltaW * (float32)x + pixelDeltaH * (float32)y - camPos);
        color = raytraceCallback(ray, maxBounces);
    } else {
        vec3 pixel = pixelOrigin + pixelDeltaW * (float32)x + pixelDeltaH * (float32)y;
        for (int32 i = 0; i < samplesPerPixel; i++) {
            vec3 jitter = pixelDeltaW * (util::randomUnit() - 0.5f) + pixelDeltaH * (util::randomUnit() - 0.5f);
            core::Ray ray;
            ray.org = camPos;
            ray.dir = normalize(pixel + jitter - camPos);
            color += raytraceCallback(ray, maxBounces);
        }
        color /= samplesPerPixel;
    }
    if (gammaCorrection) {
        color = util::gammaCorrect(color);
    }
    return makePixel(util::clamp(color, 0.0f, 1.0f));
}

void Render::renderTile(Tile t) const {
    Pixel* pixels = (Pixel*)image.data.data();
    for (int32 y = t.y0; y < t.y1; y++) {
        for (int32 x = t.x0; x < t.x1; x++) {
            pixels[y * screenW + x] = renderPixel(x, y);
        }
    }
}

} // namespace lib