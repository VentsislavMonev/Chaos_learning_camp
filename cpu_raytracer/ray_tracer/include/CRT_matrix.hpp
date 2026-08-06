#ifndef CRT_MATRIX_HPP
#define CRT_MATRIX_HPP

#include "CRT_vector.hpp"

// this shouldnt be changed its in a variable so it isnt a magic number
static const int DIMENSIONS = 3;

class CRT_matrix
{
    
public:
    CRT_matrix() noexcept
    {   
        matrix[0][0]=1.0f;matrix[0][1]=0.0f;matrix[0][2]=0.0f;
        matrix[1][0]=0.0f;matrix[1][1]=1.0f;matrix[1][2]=0.0f;
        matrix[2][0]=0.0f;matrix[2][1]=0.0f;matrix[2][2]=1.0f;
    }
    CRT_matrix( float a00,float a01,float a02,
                float a10,float a11,float a12,
                float a20,float a21,float a22) noexcept
    {
        matrix[0][0]=a00;matrix[0][1]=a01;matrix[0][2]=a02;
        matrix[1][0]=a10;matrix[1][1]=a11;matrix[1][2]=a12;
        matrix[2][0]=a20;matrix[2][1]=a21;matrix[2][2]=a22;
    }

    friend CRT_matrix operator*(const CRT_matrix& a, const CRT_matrix& b) noexcept
    {
        CRT_matrix result(  0.0f,0.0f,0.0f,
                            0.0f,0.0f,0.0f,
                            0.0f,0.0f,0.0f);

        for (int i = 0; i < DIMENSIONS; i++)
            for (int j = 0; j < DIMENSIONS; j++)
                for (int k = 0; k < DIMENSIONS; k++)
                    result.matrix[i][j] += a.matrix[i][k] * b.matrix[k][j];
        return result;
    }
    
    friend CRT_vector operator*(const CRT_vector& v, const CRT_matrix& mat) noexcept
    {
        return CRT_vector(
            mat.matrix[0][0] * v.x + mat.matrix[1][0] * v.y + mat.matrix[2][0] * v.z,
            mat.matrix[0][1] * v.x + mat.matrix[1][1] * v.y + mat.matrix[2][1] * v.z,
            mat.matrix[0][2] * v.x + mat.matrix[1][2] * v.y + mat.matrix[2][2] * v.z
        );
    }

public:
    float matrix[DIMENSIONS][DIMENSIONS];
};



#endif