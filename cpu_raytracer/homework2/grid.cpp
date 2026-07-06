#include <fstream>
#include <random>
/// Output image resolution
static const int imageWidth = 1920;
static const int imageHeight = 1080;
static const int maxColorComponent = 255;

int main() 
{
    std::ofstream ppmFileStream("grid.ppm", std::ios::out | std::ios::binary);
    ppmFileStream << "P3\n";
    ppmFileStream << imageWidth << " " << imageHeight << "\n";
    ppmFileStream << maxColorComponent << "\n";

    int rectangles_count = 13;

    int rectangle_height = imageHeight / rectangles_count;
    int rectangle_width  = imageWidth  / rectangles_count;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> dist(0, maxColorComponent);

    for (int rowIdx = 0; rowIdx < imageHeight; rowIdx++)
    {
        for (int colIdx = 0; colIdx < imageWidth; colIdx++)
        {
            int rect_row = std::min(rowIdx / rectangle_height, rectangles_count - 1);
            int rect_col = std::min(colIdx / rectangle_width, rectangles_count - 1);

            int color = (rect_row * rectangles_count + rect_col) % 6;

            int random_number = dist(gen);

            switch (color)
            {
            case 0:
                ppmFileStream << random_number << " ";
                ppmFileStream << 0   << " ";
                ppmFileStream << 0   << " ";
                ppmFileStream << "\t";    
                break;
            case 1:
                ppmFileStream << 0   << " ";
                ppmFileStream << random_number << " ";
                ppmFileStream << 0   << " ";
                ppmFileStream << "\t";    
                break;
            case 2:
                ppmFileStream << random_number << " ";
                ppmFileStream << random_number << " ";
                ppmFileStream << 0   << " ";
                ppmFileStream << "\t";  
                break;
            case 3:
                ppmFileStream << 0   << " ";
                ppmFileStream << 0   << " ";
                ppmFileStream << random_number << " ";
                ppmFileStream << "\t";    
                break;
            case 4:
                ppmFileStream << random_number << " ";
                ppmFileStream << 0   << " ";
                ppmFileStream << random_number << " ";
                ppmFileStream << "\t";    
                break;
            case 5:
                ppmFileStream << 0   << " ";
                ppmFileStream << random_number << " ";
                ppmFileStream << random_number << " ";
                ppmFileStream << "\t";    
                break;
            
            default:
                break;
            }
        }

        ppmFileStream << '\n';
    }
}