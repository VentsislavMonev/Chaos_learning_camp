#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <cmath>
#include <random>
#include <string>

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0]
                  << " <height> <width> <output.bin>\n";
        return 1;
    }

    const uint64_t H = std::stoull(argv[1]);
    const uint64_t W = std::stoull(argv[2]);
    const std::string path = argv[3];

    std::ofstream f(path, std::ios::binary | std::ios::trunc);
    if (!f)
    {
        std::cerr << "Cannot open " << path << "\n";
        return 1;
    }

    f.write(reinterpret_cast<const char*>(&H), sizeof(H));
    f.write(reinterpret_cast<const char*>(&W), sizeof(W));

    const std::size_t N = static_cast<std::size_t>(H) * static_cast<std::size_t>(W);

    std::mt19937_64 rng(1337);
    std::uniform_real_distribution<double> alphaVar(0.45, 0.65);
    std::uniform_real_distribution<double> betaVar(0.08, 0.12);

    // alpha
    {
        for (std::size_t k = 0; k < N; ++k)
        {
            double a = alphaVar(rng);
            f.write(reinterpret_cast<const char*>(&a), sizeof(a));
        }
    }

    // beta
    {
        for (std::size_t k = 0; k < N; ++k)
        {
            double b = betaVar(rng);
            f.write(reinterpret_cast<const char*>(&b), sizeof(b));
        }
    }

    // temp
    {
        for (uint64_t i = 0; i < H; ++i)
        {
            for (uint64_t j = 0; j < W; ++j)
            {
                double v = i*j;
                f.write(reinterpret_cast<const char*>(&v), sizeof(v));
            }
        }
    }

    if (!f)
    {
        std::cerr << "Write error.\n";
        return 1;
    }

    std::cout << "Generated rectangular heatshield: "
              << H << " x " << W << " -> " << path << "\n";

    return 0;
}