#ifndef CRT_MATRIX_HPP
#define CRT_MATRIX_HPP

#include "CRT_vector.hpp"

static const int DIMENSIONS = 3;

class CRT_matrix
{
    
public:
    float matrix[DIMENSIONS][DIMENSIONS];

    friend CRT_matrix operator*(const CRT_matrix& a, const CRT_matrix& b) 
    {
        CRT_matrix r{};
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                for (int k = 0; k < 3; k++)
                    r.matrix[i][j] += a.matrix[i][k] * b.matrix[k][j];
        return r;
    }

    friend CRT_vector operator*(const CRT_matrix& mat, const CRT_vector& v) 
    {
        return CRT_vector(
            mat.matrix[0][0] * v.x + mat.matrix[0][1] * v.y + mat.matrix[0][2] * v.z,
            mat.matrix[1][0] * v.x + mat.matrix[1][1] * v.y + mat.matrix[1][2] * v.z,
            mat.matrix[2][0] * v.x + mat.matrix[2][1] * v.y + mat.matrix[2][2] * v.z
        );
    }
};



#endif