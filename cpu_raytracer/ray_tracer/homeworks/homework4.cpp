#include <iostream>
#include "CRT_triangle.hpp"

int main() 
{
    CRT_vector vectors[] = 
    {
        CRT_vector(3.5, 0, 0),
        CRT_vector(1.75, 3.5, 0),
        CRT_vector(3, -3, 1),
        CRT_vector(4, 9, 3),
        CRT_vector(-12, 12, -4),
        CRT_vector(-1.75, -1.75, -3),
        CRT_vector(1.75, -1.75, -3),
        CRT_vector(0, 1.75, -3),
        CRT_vector(0, 0, -1),
        CRT_vector(1, 0, 1),
        CRT_vector(-1, 0, 1),
        CRT_vector(0.56, 1.11, 1.23),
        CRT_vector(0.44, -2.368, -0.54),
        CRT_vector(-1.56, 0.15, -1.92)
    };


    // ---- TASK 2 ------------------
    std::cout<< "Task 2:"<< std::endl;

    std::cout<< "Calculate the cross product (AxB) between two vectors:"<<std::endl;
    std::cout<< "A = (3.5, 0, 0) and B = (1.75, 3.5, 0) is: "<< (vectors[0]^vectors[1]) << '\n' << std::endl;

    std::cout<< "Calculate the cross product (AxB) between two vectors:"<<std::endl;
    std::cout<< "A = (3, -3, 1) and B = (4, 9, 3) is: "<< (vectors[2]^vectors[3]) << '\n' << std::endl;

    std::cout<< "Calculate the area of the parallelogram formed by vectors:"<<std::endl;
    std::cout<< "A = (3, -3, 1) and B = (4, 9, 3) is: "<< (vectors[2]^vectors[3]).length() << '\n' << std::endl;

    std::cout<< "Calculate the area of the parallelogram formed by vectors:"<<std::endl;
    std::cout<< "A = (3, -3, 1) and B = (-12, 12, -4) is: "<< (vectors[2]^vectors[4]).length() << '\n' << std::endl;

    std::cout<<"\n\n";

    // ---- TASK 3 ------------------


    CRT_triangle x(vectors[5],vectors[6],vectors[7]);
    CRT_triangle y(vectors[8],vectors[9],vectors[10]);
    CRT_triangle z(vectors[11],vectors[12],vectors[13]);

    std::cout<< "Task 3:"<< std::endl;

    std::cout<< "● Find the normal vector for a triangle with the following vertices:\n" <<
                "○ A = (-1.75, -1.75, -3)\n" <<
                "○ B = (1.75, -1.75, -3)\n" <<
                "○ C = (0, 1.75, -3)\n"<<std::endl;
    std::cout<< x.normal_vector << '\n' << std::endl;

    std::cout<< "● Find the normal vector for a triangle with the following vertices:\n" <<
                "○ A = (0, 0, -1)\n" <<
                "○ B = (1, 0, 1)\n" <<
                "○ C = (-1, 0, 1)\n"<<std::endl;
    std::cout<< y.normal_vector << '\n' << std::endl;

    std::cout<< "● Find the normal vector for a triangle with the following vertices:\n" <<
                "○ A = (0.56, 1.11, 1.23)\n" <<
                "○ B = (0.44, -2.368, -0.54)\n" <<
                "○ C = ((-1.56, 0.15, -1.92)\n"<<std::endl;
    std::cout<< z.normal_vector << '\n' << std::endl;

    std::cout<<"The area of these triangles are:\n"<< 
               "First triangle area: "     << x.normal_vector.length()/2 << '\n' <<
               "Second triangle area: "    << y.normal_vector.length()/2 << '\n' <<
               "Third triangle area: "     << z.normal_vector.length()/2 << std::endl;

}