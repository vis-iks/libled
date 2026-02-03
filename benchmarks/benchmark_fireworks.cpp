#include <iostream>
#include <vector>
#include <chrono>
#include <memory>
#include <numeric>
#include <iomanip>
#include "core/Canvas.h"
#include "effects/ParticleEffects.h"
#include "core/GraphicsConstants.h"

using namespace libled;

int main() {
    std::cout << "Starting Fireworks Benchmark..." << std::endl;

    // 1. Setup
    Canvas canvas;
    canvas.Resize(DISPLAY_WIDTH, DISPLAY_HEIGHT);

    auto fireworks = std::make_shared<FireworksEffect>();

    // 2. Loop
    const int TOTAL_FRAMES = 5000;
    const int BLOCK_SIZE = 100;

    uint32_t currentTime = 0;
    const uint32_t dt = 16; // ~60fps

    std::vector<double> blockTimes;

    auto startTotal = std::chrono::high_resolution_clock::now();

    for (int frame = 0; frame < TOTAL_FRAMES; ++frame) {
        auto t1 = std::chrono::high_resolution_clock::now();

        fireworks->Render(canvas, currentTime);
        currentTime += dt;

        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> ms = t2 - t1;

        // Aggregate statistics
        static double currentBlockSum = 0;
        currentBlockSum += ms.count();

        if ((frame + 1) % BLOCK_SIZE == 0) {
            double avg = currentBlockSum / BLOCK_SIZE;
            blockTimes.push_back(avg);
            std::cout << "Frames " << (frame + 1 - BLOCK_SIZE) << "-" << frame
                      << ": " << std::fixed << std::setprecision(3) << avg << " ms/frame" << std::endl;
            currentBlockSum = 0;
        }
    }

    auto endTotal = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> totalSec = endTotal - startTotal;

    std::cout << "Total time: " << totalSec.count() << "s" << std::endl;

    // Check for degradation
    if (blockTimes.size() > 1) {
        double first = blockTimes.front();
        double last = blockTimes.back();
        std::cout << "Start Avg: " << first << " ms/frame" << std::endl;
        std::cout << "End Avg:   " << last << " ms/frame" << std::endl;
        // Simple heuristic: if end is significantly slower than start (ignoring initialization noise if needed)
        // Since we know fixed performance is ~0.01ms, anything growing towards 0.1ms is bad.
        if (last > first * 2.0) {
             std::cout << "PERFORMANCE DEGRADATION DETECTED." << std::endl;
        } else {
             std::cout << "Performance stable." << std::endl;
        }
    }

    return 0;
}
