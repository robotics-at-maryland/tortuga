/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Chris Giles <cgiles@umd.edu>
 * All rights reserved.
 *
 * Author: Chris Giles <cgiles@umd.edu>
 * File:  packages/math/include/MatrixN.h
 */

#ifndef RAM_MATH_MATRIX4_N_28_02_2008
#define RAM_MATH_MATRIX4_N_28_02_2008

// Project Includes
#include "math/include/Vector2.h"
#include "math/include/Matrix2.h"
#include "math/include/Vector3.h"
#include "math/include/Matrix3.h"
#include "math/include/Vector4.h"
#include "math/include/Matrix4.h"

// Slight hack to allow easier folding in of changes from Ogre
#define Real double
#define OGRE_FORCE_ANGLE_TYPES
#define RAM_MATRIXN_RC(r,c) (r) * (cols) + (c)

// Must Be Included last
#include "math/include/Export.h"

namespace ram {
namespace math {

class RAM_EXPORT MatrixN
{
protected:
    
	Real *data;
	int rows, cols;

public:

    inline MatrixN()
    {
		rows = 1;
		cols = 1;
		data = new Real[rows*cols];
		data[0] = 0;
    }

    inline MatrixN(const MatrixN& o)
    {
        rows = 0;
        cols = 0;
        data = 0;
           
        *this = o;
    }
    
	inline MatrixN(int rows_, int cols_)
	{
		rows = rows_;
		cols = cols_;
		int rc = rows*cols;
		data = new Real[rc];
		memset(data, 0, sizeof(Real)*rows*cols);
	}

    inline MatrixN(const Real *data_, int rows_, int cols_)
    {
        rows = rows_;
		cols = cols_;
		data = new Real[rows*cols];
		memcpy(data, data_, sizeof(Real)*rows*cols);
    }

	inline MatrixN(const Vector2& v, int rows_ = -1, int cols_ = -1)
    {
		if (rows_ <= 0)
			rows_ = 2;
		if (cols_ <= 0)
			cols_ = 1;
        rows = rows_;
		cols = cols_;
		int rc = rows*cols;
		data = new Real[rc];
		for (int i=0;i<rc;i++)
			data[i] = 0;
		data[RAM_MATRIXN_RC(0,0)] = v.x;
		data[RAM_MATRIXN_RC(1,0)] = v.y;
    }

	inline MatrixN(const Vector3& v, int rows_ = -1, int cols_ = -1)
    {
		if (rows_ <= 0)
			rows_ = 3;
		if (cols_ <= 0)
			cols_ = 1;
        rows = rows_;
		cols = cols_;
		int rc = rows*cols;
		data = new Real[rc];
		for (int i=0;i<rc;i++)
			data[i] = 0;
		data[RAM_MATRIXN_RC(0,0)] = v.x;
		data[RAM_MATRIXN_RC(1,0)] = v.y;
		data[RAM_MATRIXN_RC(2,0)] = v.z;
    }

	inline MatrixN(const Vector4& v, int rows_ = -1, int cols_ = -1)
    {
		if (rows_ <= 0)
			rows_ = 4;
		if (cols_ <= 0)
			cols_ = 1;
        rows = rows_;
		cols = cols_;
		int rc = rows*cols;
		data = new Real[rc];
		for (int i=0;i<rc;i++)
			data[i] = 0;
		data[RAM_MATRIXN_RC(0,0)] = v.x;
		data[RAM_MATRIXN_RC(1,0)] = v.y;
		data[RAM_MATRIXN_RC(2,0)] = v.z;
		data[RAM_MATRIXN_RC(3,0)] = v.w;
    }

	inline MatrixN(const Matrix2& v, int rows_ = -1, int cols_ = -1)
    {
		if (rows_ <= 0)
			rows_ = 2;
		if (cols_ <= 0)
			cols_ = 2;
        rows = rows_;
		cols = cols_;
		int rc = rows*cols;
		data = new Real[rc];
		for (int i=0;i<rc;i++)
			data[i] = 0;
		for(int i=0;i<2;i++)
			for (int j=0;j<2;j++)
				data[RAM_MATRIXN_RC(i,j)] = v[i][j];
    }

	inline MatrixN(const Matrix3& v, int rows_ = -1, int cols_ = -1)
    {
		if (rows_ <= 0)
			rows_ = 3;
		if (cols_ <= 0)
			cols_ = 3;
        rows = rows_;
		cols = cols_;
		int rc = rows*cols;
		data = new Real[rc];
		for (int i=0;i<rc;i++)
			data[i] = 0;
		for(int i=0;i<3;i++)
			for (int j=0;j<3;j++)
				data[RAM_MATRIXN_RC(i,j)] = v[i][j];
    }

	inline MatrixN(const Matrix4& v, int rows_ = -1, int cols_ = -1)
    {
		if (rows_ <= 0)
			rows_ = 4;
		if (cols_ <= 0)
			cols_ = 4;
        rows = rows_;
		cols = cols_;
		int rc = rows*cols;
		data = new Real[rc];
		for (int i=0;i<rc;i++)
			data[i] = 0;
		for(int i=0;i<4;i++)
			for (int j=0;j<4;j++)
				data[RAM_MATRIXN_RC(i,j)] = v[i][j];
    }

        inline MatrixN& operator= (const MatrixN& o)
	{
		// Handle assign to self
		if (this == &o)
			return *this;
		if ((cols != o.cols) || (rows != o.rows))
		{
			delete[] data;
			data = new Real[o.cols * o.rows];
		}
                
		cols = o.cols;
		rows = o.rows;
		memcpy(data, o.data, sizeof(Real) * o.rows * o.cols);
		return *this;
	}
        
	~MatrixN()
	{
		delete[] data;
		data = NULL;
	}

    inline Real* operator [] ( int row_ )
    {
        assert( row_ >= 0 && row_ < rows );
        return data + row_ * cols;
    }

    inline const Real *const operator [] ( int row_ ) const
    {
        assert( row_ >= 0 && row_ < rows );
        return data + row_ * cols;
    }

    inline MatrixN operator * ( const MatrixN &m2 ) const
    {
		assert( cols == m2.rows );

		MatrixN out(rows, m2.cols);

		for ( int i = 0; i < rows; i++ ) 
		{
			for ( int j = 0; j < m2.cols; j++ ) 
			{
				for ( int k = 0; k < cols; k++ ) 
				{
					out[i][j] += (*this)[i][k] * m2[k][j];
				}
			}
		}

		return out;
    }

    inline MatrixN operator + ( const MatrixN &m2 ) const
    {
        assert(rows == m2.rows && cols == m2.cols);
		MatrixN out = *this;
		for (int i=0;i<rows;i++)
			for (int j=0;j<cols;j++)
				out[i][j] += m2[i][j];
		return out;
    }

    inline MatrixN operator - ( const MatrixN &m2 ) const
    {
        assert(rows == m2.rows && cols == m2.cols);
		MatrixN out = *this;
		for (int i=0;i<rows;i++)
			for (int j=0;j<cols;j++)
				out[i][j] -= m2[i][j];
		return out;
    }

    inline bool operator == ( const MatrixN& m2 ) const
    {
        if (rows != m2.rows || cols != m2.cols)
			return false;

		for (int i=0;i<rows;i++)
			for (int j=0;j<cols;j++)
				if (data[RAM_MATRIXN_RC(i,j)] != m2[i][j])
					return false;
		return true;
    }

    inline bool operator != ( const MatrixN& m2 ) const
    {
        if (rows != m2.rows || cols != m2.cols)
			return true;

		for (int i=0;i<rows;i++)
			for (int j=0;j<cols;j++)
				if (data[RAM_MATRIXN_RC(i,j)] != m2[i][j])
					return true;
		return false;
    }

    inline MatrixN transpose(void) const
    {
        MatrixN out(cols, rows);
		for (int i=0;i<cols;i++)
			for (int j=0;j<rows;j++)
				out[i][j] = data[RAM_MATRIXN_RC(j,i)];
		return out;
    }

    inline MatrixN operator*(Real scalar) const
    {
        MatrixN out = *this;
		for (int i=0;i<rows;i++)
			for (int j=0;j<cols;j++)
				out[i][j] *= scalar;
		return out;
    }

    inline  friend std::ostream& operator <<
        ( std::ostream& o, const MatrixN& m )
    {
		o << "MatrixN(" << m.rows << ", " << m.cols << ") = {\n";
		for (int i=0;i<m.rows;i++)
		{
			for (int j=0;j<m.cols;j++)
				o << m.data[i*m.cols+j] << " ";
			o << "\n";
		}
		o << "}";
        return o;
    }

	inline void resize(int rows_, int cols_)
	{
		if (data)
		{
			Real *newData = new Real[rows_ * cols_];
			for (int i=0;i<rows_;i++)
			{
			  for (int j=0;j<cols_;j++)
				{
				  if (i < rows && j < cols)
					newData[i * cols_ + j] = data[i * cols + j];
				  else
					newData[i * cols_ + j] = 0;
				}
			}
			delete[] data;
			data = newData;
			newData = NULL;
		}
		else
		{
			data = new Real[rows_ * cols_];
			for (int i=0;i<rows_;i++)
				for (int j=0;j<cols_;j++)
					data[RAM_MATRIXN_RC(i,j)] = 0;
		}
		rows = rows_;
		cols = cols_;
	}

	inline int getRows() const
	{
		return rows;
	}

	inline int getCols() const
	{
		return cols;
	}

	inline void identity()
	{
		for (int i=0;i<rows;i++)
			for (int j=0;j<cols;j++)
			{
				if (i == j)
					data[RAM_MATRIXN_RC(i,j)] = 1;
				else
					data[RAM_MATRIXN_RC(i,j)] = 0;
			}
	}

	inline void zero()
	{
		for (int i=0;i<rows;i++)
			for (int j=0;j<cols;j++)
				data[RAM_MATRIXN_RC(i,j)] = 0;
	}

	inline void removeRow(int r)
	{
		assert(r >= 0 && r < rows);

		for (int i=r;i<rows-1;i++)
			for (int j=0;j<cols;j++)
				data[RAM_MATRIXN_RC(i,j)] = data[RAM_MATRIXN_RC(i+1,j)];

		resize(rows-1, cols);
	}

	inline void removeCols(int r)
	{
		assert(r >= 0 && r < cols);

		for (int i=0;i<rows;i++)
			for (int j=r;j<cols-1;j++)
				data[RAM_MATRIXN_RC(i,j)] = data[RAM_MATRIXN_RC(i,j+1)];

		resize(rows, cols-1);
	}

	// Pointer accessor for direct copying
	inline Real* ptr(){
	  return data;
	}

	/** In place LU factorization
         *  @todo Document my parameter
         */
	bool factorLU(int *index, Real *det = NULL);

	/** In place LU Solve
         *  @todo Document my parameters
         */
	void solveLU(Real *x, Real *b, const int *index) const;

	/** In place matrix inversion */
	bool invert();
    
    inline MatrixN inverse() const {
        MatrixN inv(*this);
        inv.invert();
        return inv;
    }

        /** Preform an inplace Cholesky Factorization
         *
         *  This function uses only the diagonal and upper triangle of the
         *  current matrix.  The initial matrix has to be symmetric positive
         *  definite.  That means the lower triangular is assumed to be the
         *  (complex conjugate) transpose of the upper.
         *
         *  The result is placed in the bottom diagonal of the matrix.  The
         *  resulting matrix (with the top diagonal zeroed) results gives you
         *  the following: R'*R = X.  Where R is the result and X is initail
         *  matrix.
         *
         *  @return  true on success, false if the function fails
         */
	bool factorCholesky();
	
};

inline MatrixN operator * (Real scalar, const MatrixN& mat)
{
	MatrixN out = mat;
	for (int i=0;i<mat.getRows();i++)
		for (int j=0;j<mat.getCols();j++)
			out[i][j] *= scalar;
	return out;
}

} // namespace math
} // namespace ram

// Removal of "Real" hack
#undef Real

#endif
