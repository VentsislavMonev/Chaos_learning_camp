#include "CRT_render.hpp"
#include <chrono>
#include <iostream>

int main()
{
    // i was trying with different bucket sizes 
    try
    {
        CRT_render renderer("scene5.crtscene");
        
        // First render
        auto start1 = std::chrono::steady_clock::now();
        renderer.render("output5_10.ppm");
        auto end1 = std::chrono::steady_clock::now();

        auto time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1);

        // // First render
        // start1 = std::chrono::steady_clock::now();
        // renderer.render("output5_1.ppm", 16);
        // end1 = std::chrono::steady_clock::now();

        // auto time2 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1);

        // // Second render
        // start1 = std::chrono::steady_clock::now();
        // renderer.render("output5_2.ppm",32);
        // end1 = std::chrono::steady_clock::now();

        // auto time3 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1);

        // // Second render
        // start1 = std::chrono::steady_clock::now();
        // renderer.render("output5_3.ppm",64);
        // end1 = std::chrono::steady_clock::now();

        // auto time4 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1);

        // // Second render
        // start1 = std::chrono::steady_clock::now();
        // renderer.render("output5_4.ppm",128);
        // end1 = std::chrono::steady_clock::now();

        // auto time5 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1);

        // // Second render
        // start1 = std::chrono::steady_clock::now();
        // renderer.render("output5_5.ppm",256);
        // end1 = std::chrono::steady_clock::now();

        // auto time6 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1);

        std::cout << "render8():   " << time1.count() << " ms\n";
        // std::cout << "render16():  " << time2.count() << " ms\n";
        // std::cout << "render32():  " << time3.count() << " ms\n";
        // std::cout << "render64():  " << time4.count() << " ms\n";
        // std::cout << "render128(): " << time5.count() << " ms\n";
        // std::cout << "render256(): " << time6.count() << " ms\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cin.get();
    }
}