#include <vector>
#include <string>
#include <fstream>
#include <cassert>
#include <random>
#include <optional>
#include <filesystem>

#include <particlesystem/priorityqueue.h>
#include <particlesystem/particle.h>
#include <particlesystem/collisionsystem.h>

#include <rendering/window.h>

#include <fmt/format.h>

using namespace particlesystem;

/**
 * To test toss, deleteMin, heapify, percolateDown, isMinHeap
 */
void test1PriorityQueue();

/**
 * To test insert, deleteMin, isMinHeap
 */
void test2PriorityQueue();

/**
 * Read particles for the simulation from file
 */
std::vector<Particle> read_particles(const std::filesystem::path& file);

/**
 * To run the simulation
 */
void runSimulation();

int main() {
#ifdef TEST_PRIORITY_QUEUE
    //test1PriorityQueue();  // test toss, deleteMin, heapify, isMinHeap
    test2PriorityQueue();  // test insert, deleteMin, isMinHeap
#endif

#ifndef TEST_PRIORITY_QUEUE
    runSimulation();
#endif
}

/**
 * Read particles for the simulation from file
 */
std::vector<Particle> read_particles(const std::filesystem::path& file) {
    std::ifstream is(file);
    if (!is) {
        return {};
    }

    int n_particles;
    is >> n_particles;  // read number of particles

    std::vector<Particle> particles;
    particles.reserve(n_particles);

    double rx, ry;
    double vx, vy;
    double radius;
    double mass;
    float r, g, b;
    for (int i = 0; i < n_particles; ++i) {
        is >> rx >> ry >> vx >> vy;
        is >> radius >> mass;
        is >> r >> g >> b;
        particles.push_back(Particle{.r = {rx, ry},
                                     .v = {vx, vy},
                                     .radius = radius,
                                     .mass = mass,
                                     .color = {r / 255.0f, g / 255.0f, b / 255.0f}});
    }
    return particles;
}

void runSimulation() {
    std::cout << "Particles file (with absolut path): ";  // billiards10.txt, diffusion.txt, sam4.txt, brownian.txt
    std::string name;
    std::cin >> name;

     std::string path_to{"../collisionsystem/data/"}; // modify this path, if needed (Mac)
     std::filesystem::path particlesFile = path_to + name;
    //std::filesystem::path particlesFile = name;
    auto theParticles = read_particles(particlesFile);

    if (std::size(theParticles) == 0) {
        fmt::print("No particles\n");
        return;
    }

    // create collision system
    CollisionSystem system{std::move(theParticles)};

    // Some initializations for rendering
    rendering::Window window(850, 850, rendering::Window::UseVSync::No);
    system.renderCallback = [&](std::span<Particle> particles) {
        window.beginFrame();
        window.clear({0, 0, 0, 1});
        window.drawParticles(particles);
        window.endFrame();
    };
    system.abortCallback = [&]() { return window.shouldClose(); };

    fmt::print("Simulations starts ...\n");
    system.simulate(10000, 10);  // simulate//10
}

/**
 * To test toss, deleteMin, heapify, percolateDown, isMinHeap
 */
void test1PriorityQueue() {
    constexpr int minItem = 10000;
    constexpr int maxItem = 99999;
    PriorityQueue<int> h;

    fmt::print("Test1: toss, deleteMin, heapify, isMinHeap\n");

    for (int i = 37; i != 0; i = (i + 37) % maxItem) {
        if (i >= minItem) {
            fmt::print("Tossing {}\n", i);
            h.toss(i);
        }
    }

    fmt::print("\n\n");

    for (int i = minItem; i < maxItem; ++i) {
        int x = h.deleteMin();
        fmt::print("{} deleted from queue\t\t Queue Size: {:10}\n", x, h.size());
        if (x != i) {
            fmt::print("Oops! Error after delete of {}\n", i);
        }
    }
    fmt::print("Successful test...\n");
}

/**
 * To test insert, deleteMin, isMinHeap
 */
void test2PriorityQueue() {
    constexpr int minItem = 10000;
    constexpr int maxItem = 99999;
    PriorityQueue<int> h;

    fmt::print("Test2: insert, deleteMin, isMinHeap\n");

    for (int i = 37; i != 0; i = (i + 37) % maxItem) {
        if (i >= minItem) {
            fmt::print("Inserting {}\n", i);
            h.insert(i);
        }
    }

    fmt::print("\n\n");

    for (int i = minItem; i < maxItem; ++i) {
        int x = h.deleteMin();
        fmt::print("{} deleted from queue\t\t Queue Size: {:10}\n", x, h.size());
        if (x != i) {
            fmt::print("Oops! Error after delete of {}\n", i);
        }
    }
    fmt::print("Successful test...\n");
}
