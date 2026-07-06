#include <fstream>
/// Output image resolution
static const int imageWidth = 1920;
static const int imageHeight = 1080;
static const int maxColorComponent = 255;

int main() 
{
    std::ofstream ppmFileStream("parabol.ppm", std::ios::out | std::ios::binary);
    ppmFileStream << "P3\n";
    ppmFileStream << imageWidth << " " << imageHeight << "\n";
    ppmFileStream << maxColorComponent << "\n";    

    double a = 200.0;
    double b = 120.0;

    double centerX = imageWidth / 2.0;
    double centerY = imageHeight / 2.0;

    for (int y = 0; y < imageHeight; ++y)
    {
        for (int x = 0; x < imageWidth; ++x)
        {
            double dx = (x - centerX);
            double dy = (y - centerY);

            double value = (dx * dx) / (a * a) - (dy * dy) / (b * b);

            if (value > 1)
            {
                ppmFileStream << 0   << " ";
                ppmFileStream << 255 << " ";
                ppmFileStream << 0   << " ";
                ppmFileStream << "\t";  
            }
            else
            {
                ppmFileStream << 0   << " ";
                ppmFileStream << 0   << " ";
                ppmFileStream << 255 << " ";
                ppmFileStream << "\t";  
            }
        }
        ppmFileStream << "\n";
    }

    ppmFileStream.close();
    return 0;
}