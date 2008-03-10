#include <math.h>

// Project Includes
#include "math/include/MatrixN.h"

// Slight hack to allow easier folding in of changes from Ogre
#define Real double

namespace ram {
namespace math {
	
	bool MatrixN::factorLU( int *index, Real *det ) 
	{
		int i, j, k, newi, min;
		double s, t, d, w;

		// if partial pivoting should be used
		if ( index ) {
			for ( i = 0; i < rows; i++ ) {
				index[i] = i;
			}
		}

		w = 1.0f;
		min = rows < cols ? rows : cols;
		for ( i = 0; i < min; i++ ) {

			newi = i;
			s = fabsf( (*this)[i][i] );

			if ( index ) {
				// find the largest absolute pivot
				for ( j = i + 1; j < rows; j++ ) {
					t = fabsf( (*this)[j][i] );
					if ( t > s ) {
						newi = j;
						s = t;
					}
				}
			}

			if ( s == 0.0f ) {
				return false;
			}

			if ( newi != i ) {

				w = -w;

				// swap index elements
				k = index[i];
				index[i] = index[newi];
				index[newi] = k;

				// swap rows
				for ( j = 0; j < cols; j++ ) {
					t = (*this)[newi][j];
					(*this)[newi][j] = (*this)[i][j];
					(*this)[i][j] = t;
				}
			}

			if ( i < rows ) {
				d = 1.0f / (*this)[i][i];
				for ( j = i + 1; j < rows; j++ ) {
					(*this)[j][i] *= d;
				}
			}

			if ( i < min-1 ) {
				for ( j = i + 1; j < rows; j++ ) {
					d = (*this)[j][i];
					for ( k = i + 1; k < cols; k++ ) {
						(*this)[j][k] -= d * (*this)[i][k];
					}
				}
			}
		}

		if ( det ) {
			for ( i = 0; i < rows; i++ ) {
				w *= (*this)[i][i];
			}
			*det = w;
		}

		return true;
	}   

	void MatrixN::solveLU(Real *x, Real *b, const int *index) const
	{
		int i, j;
		Real sum;
 
		// solve L
		for ( i = 0; i < rows; i++ ) {
			if ( index != NULL ) {
				sum = b[index[i]];
			} else {
				sum = b[i];
			}
			for ( j = 0; j < i; j++ ) {
				sum -= (*this)[i][j] * x[j];
			}
			x[i] = sum;
		}

		// solve U
		for ( i = rows - 1; i >= 0; i-- ) {
			sum = x[i];
			for ( j = i + 1; j < rows; j++ ) {
				sum -= (*this)[i][j] * x[j];
			}
			x[i] = sum / (*this)[i][i];
		}
	}

	bool MatrixN::invert()
	{
		int i, j, *index;
		MatrixN tmp(rows, cols);
		index = new int[rows];
		tmp = *this;

		if ( !tmp.factorLU( index ) )
                {
                    delete[] index;
			return false;
                }

		Real *x = new Real[rows];
		Real *b = new Real[rows];
                
		memset(b, 0, sizeof(Real)*rows);

		for ( i = 0; i < rows; i++ ) 
		{
			b[i] = 1.0f;
			tmp.solveLU( x, b, index );
			for ( j = 0; j < rows; j++ ) 
				(*this)[j][i] = x[j];
			b[i] = 0.0f;
		}

                delete[] index;
                delete[] x;
                delete[] b;
		return true;
	}

	bool MatrixN::factorCholesky()
	{
		int i, j, k;
		double *invSqrt;
		double sum;

		assert( rows == cols );

		invSqrt = new Real[rows];
		for (i=0;i<rows;i++)
			invSqrt[i] = 0;

		for ( i = 0; i < rows; i++ ) {

			for ( j = 0; j < i; j++ ) {

				sum = (*this)[i][j];
				for ( k = 0; k < j; k++ ) {
					sum -= (*this)[i][k] * (*this)[j][k];
				}
				(*this)[i][j] = sum * invSqrt[j];
			}

			sum = (*this)[i][i];
			for ( k = 0; k < i; k++ ) {
				sum -= (*this)[i][k] * (*this)[i][k];
			}

			if ( sum <= 0.0f ) {
                            delete[] invSqrt;
				return false;
			}

			invSqrt[i] = 1.0 / sqrt(sum);
			(*this)[i][i] = invSqrt[i] * sum;
		}
                delete[] invSqrt;
		return true;
	}

} // namespace math
} // namespace ram
