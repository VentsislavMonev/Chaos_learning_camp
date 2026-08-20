#include "CRT_render.hpp"

int main()
{
    CRT_render renderer("../scene5.crtscene");
    renderer.render("output5.ppm");
}