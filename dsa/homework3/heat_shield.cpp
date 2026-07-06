#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <future>
#include <string>
#include <cstring>
#include <cstdint>
#include <iomanip>
#include <stdexcept>
#include <algorithm>

//===================================================
// It will work faster with a thread pool because we 
// wouldn`t start a new thread for every gen
// i didnt had time to make it work
//===================================================

// Load the input binary file

struct Grid 
{
    uint64_t H, W;
    std::vector<double> alpha, beta, data;
};

Grid load(const std::string& path) 
{
    std::ifstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("Cannot open: " + path);

    Grid grid;
    f.read(reinterpret_cast<char*>(&grid.H), sizeof(uint64_t));
    f.read(reinterpret_cast<char*>(&grid.W), sizeof(uint64_t));
    if (!f) throw std::runtime_error("Bad header in: " + path);

    size_t N = grid.H * grid.W;
    grid.alpha.resize(N);
    grid.beta.resize(N);
    grid.data.resize(N);
    
    f.read(reinterpret_cast<char*>(grid.alpha.data()), N * sizeof(double));
    f.read(reinterpret_cast<char*>(grid.beta.data()), N * sizeof(double));
    f.read(reinterpret_cast<char*>(grid.data.data()), N * sizeof(double));
    if (!f) throw std::runtime_error("Bad data in: " + path);

    return grid;
}

// Calculates a new cell in the matrix using the old value and neighbouring values
// from row_begin including that row to row_end not including it
void stencil_rows(
    const double* cur, double* nxt,
    const double* alpha, const double* beta,
    size_t H, size_t W,
    size_t row_begin, size_t row_end)
{
    for (size_t i = row_begin; i < row_end; ++i) 
    {
        for (size_t j = 0; j < W; ++j) 
        {
            double neighbours = 0.0;
            if (i > 0)     neighbours += cur[(i-1)*W + j];  // up
            if (i < H-1)   neighbours += cur[(i+1)*W + j];  // down
            if (j > 0)     neighbours += cur[i*W + (j-1)];  // left
            if (j < W-1)   neighbours += cur[i*W + (j+1)];  // right

            size_t k = i * W + j;
            nxt[k] = alpha[k] * cur[k] + beta[k] * neighbours;
        }
    }
}

// Save a chekpoint as "./chekpoint_i" where i is the current number of checkpoint that is being saved
void save_checkpoint(uint64_t gen, uint64_t H, uint64_t W, std::vector<double> snapshot,int index)
{
    // Compute average with Kahan summation to reduce floating point error
    double sum = 0.0;
    double correction = 0.0;
    for (double v : snapshot) 
    {
        double y = v - correction;
        double t = sum + y;
        correction = (t - sum) - y;
        sum = t;
    }
    double avg = sum / static_cast<double>(snapshot.size());

    std::string filename = "checkpoint_" + std::to_string(index) + ".bin";
    std::ofstream f(filename, std::ios::binary);
    if (!f) 
    { std::cerr << "Cannot write " << filename << "\n"; return; }

    f.write(reinterpret_cast<const char*>(&gen), sizeof(uint64_t));
    f.write(reinterpret_cast<const char*>(&H),sizeof(uint64_t));
    f.write(reinterpret_cast<const char*>(&W),sizeof(uint64_t));
    f.write(reinterpret_cast<const char*>(snapshot.data()), snapshot.size() * sizeof(uint64_t));

    std::cout << "[checkpoint " << index << "]"
              << "  gen="      << gen
              << "  avg_temp=" << std::fixed << std::setprecision(6) << avg
              << "  -> "       << filename << "\n";
}


int main(int argc, char* argv[]) 
{
    // Checking args
    if (argc != 4) 
    {
        std::cerr << "Usage: " << argv[0] << "  N  K  input.bin\n";
        return 1;
    }

    uint64_t N_gen = std::stoull(argv[1]);
    uint64_t K = std::stoull(argv[2]);
    std::string input_path = argv[3];

    if (K == 0 || K > N_gen) 
    {
        std::cerr << "ERROR: need 0 < K <= N\n";
        return 1;
    }

    // Load 
    std::cout << "Loading " << input_path << " ...\n";
    Grid grid = load(input_path);
    size_t H = grid.H;
    size_t W = grid.W;
    size_t CELLS = H * W;
    std::cout << "Grid: " << H << " x " << W << "\n"
              << "Simulating " << N_gen << " generations, "
              << "checkpoint every " << K << " steps.\n\n";

    // Ping pong buffers 
    std::vector<double> buf0(grid.data);
    std::vector<double> buf1(CELLS, 0.0);
    grid.data.clear();
    grid.data.shrink_to_fit();

    double* cur = buf0.data();
    double* nxt = buf1.data();

    const double* alpha = grid.alpha.data();
    const double* beta = grid.beta.data();

    // Thread setup

    // One thread per group of rows

    unsigned nthreads = std::max(1u, std::thread::hardware_concurrency());
    nthreads = static_cast<unsigned>(std::min(static_cast<size_t>(nthreads), H));

    // Which thread handles which rows

    std::vector<size_t> row_begin(nthreads);
    std::vector<size_t> row_end(nthreads);
    size_t base = H / nthreads;
    size_t extra = H % nthreads;
    size_t row = 0;
    for (unsigned i = 0; i < nthreads; ++i) 
    {
        row_begin[i] = row;
        row += base + (i < extra ? 1 : 0);
        row_end  [i] = row;
    }

    // Checkpoint tracking
    // We allow one checkpoint to run in the background while we compute.

    std::future<void> checkpoint_future;
    int checkpoint_index = 0;

    // Main loop
    std::vector<std::thread> workers(nthreads);

    for (uint64_t gen = 1; gen <= N_gen; ++gen) 
    {
        for (unsigned i = 0; i < nthreads; ++i) 
        {
            workers[i] = std::thread(
                stencil_rows,
                cur, nxt, alpha, beta,
                H, W,
                row_begin[i], row_end[i]
            );
        }
        for (auto& th : workers) th.join();

        std::swap(cur, nxt);

        if (gen % K == 0) 
        {
            // If a previous checkpoint is still writing wait for it
            if (checkpoint_future.valid()) checkpoint_future.get();

            ++checkpoint_index;

            // Copy the current temperatures because otherwise other threads could change
            // the data that we are saving

            std::vector<double> snapshot(cur, cur + CELLS);

            // Launch checkpoint on a background thread
            checkpoint_future = std::async(
                std::launch::async,
                save_checkpoint,
                gen, (uint64_t)H, (uint64_t)W,
                std::move(snapshot),
                checkpoint_index
            );
        }
    }

    // Wait for last checkpoint if still running
    if (checkpoint_future.valid()) checkpoint_future.get();

    std::cout << "\nDone. Checkpoints written: " << checkpoint_index << "\n";
    return 0;
}