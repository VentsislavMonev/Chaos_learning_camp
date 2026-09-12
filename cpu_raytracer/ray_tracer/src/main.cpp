#include "CRT_render.hpp"

int main()
{
   try
    {
        CRT_render renderer("gun.crtscene");
        renderer.render("output5.ppm");
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cin.get();
    }
}