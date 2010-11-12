/*
 * Copyright (C) 2010 Robotics at Maryland
 * All rights reserved.
 *
 * Author: Jonathan Wonders
 * File:  packages/core/include/SGolaySmoothingFilter.h
 */


#ifndef RAM_CORE_SGOLAYSMOOTHINGFILTER_H
#define RAM_CORE_SGOLAYSMOOTHINGFILTER_H

#include "math/include/MatrixN.h"
#include "math/include/VectorN.h"
#include <cassert>
#include <cmath>
#include <iostream>

namespace ram {
namespace core {

class SGolaySmoothingFilter
{
public:
    SGolaySmoothingFilter(int SIZE,int DEGREE)
        : m_windowSize(SIZE),
          m_degree(DEGREE),
          m_value(0)
    {
        m_windowData = ram::math::VectorN(SIZE);
        for(int i = 0; i < m_windowSize; i++)
            m_windowData[i] = 0.0;
        
        // Calculate the coefficients here
       
        int interval=1;
        assert( m_windowSize % 2 == 1 && "windowSize must be odd");
        ram::math::MatrixN coeff(1,m_windowSize, m_degree+1);
        //initializes matrix of coefficients
        for(int i=1; i<=m_degree; i++)
        {
            for(int j=0; j<m_windowSize; j++)
            {
            
                /*the below generates a vector of the intervals across the windowsize,
                  centered at zero, each vector is put to a power of their row*/
                double base = ((-1*(m_windowSize*interval/2))+(j*interval)+(interval/2));
                coeff[j][i]=pow(base,(double)i);
         

            }
        }
        ram::math::MatrixN JxJT=(coeff.transpose()*coeff);
        coeff=(JxJT.inverse()*coeff.transpose());
        m_coefficients=coeff;



    }

    /** Put a new value into the filter (throws the oldest off) */
    void addValue(double newValue)
    {
        //shifts all the elements over and inserts the new element
        //this will discard the oldest element
        for(int i = 0; i < m_windowSize-1; i++)
        {
            m_windowData[i] = m_windowData[i+1];
        }
        m_windowData[m_windowSize-1] = newValue;
        //next the value is smoothed
        m_value=applyPoly(applySGolay(m_windowData, m_coefficients)
                        ,(int)(m_windowSize/2));
        m_derivative=applyPoly(applySGolayDeriv(m_windowData, m_coefficients)
                             ,(int)(m_windowSize/2));

    }




    /** Gets the value of the filter */
    double getValue()
    {
        return m_value;
    }

    /** Returns the current number of data values in the filter */
    int getSize()
    {
        return m_windowSize;
    }
    
    /** Returns the window size for the filter */
    int getWindowSize()
    {
        return m_windowSize;
    }

    /** Returns the polynomial degree for the least squares fit */
    int getDegree()
    {
        return m_degree;
    }
    double  getDerivative()
    {
        return m_derivative;
    }
    /** Clears all values in the filter */
    void clear();

private:
    ram::math::VectorN applySGolay(ram::math::VectorN window,ram::math::MatrixN coeff)
    {
        assert(window.numElements()==coeff.getCols() && "Dimensions do not match");
        //applies the points to the coefficients to generate polynomial
        window = window*coeff.transpose();

        return window;
    }
    ram::math::VectorN  applySGolayDeriv(ram::math::VectorN window,ram::math::MatrixN coeff)
    {
        window = applySGolay(window, coeff);
        for(int i = 0; i < window.numElements()-1; i++)
        {
            //converts coefficients to the derivative
            window[i]=window[i+1]*(i+1);
        }
        window[window.numElements()-1]=0;
        return window;


    }
    double applyPoly(ram::math::VectorN poly, double point)
    {
        double output=0;
        for(int i=0; i<poly.numElements(); i++)
        {
            //computes each polynomial term
            output+=pow(point,i)*poly[i];
        }
        return output;

    }

    int m_windowSize;
    int m_degree;
    double m_value;
    double m_derivative;
    ram::math::VectorN m_windowData;
    ram::math::MatrixN m_coefficients;
};

} // namespace core
} // namespace ram

#endif // RAM_CORE_SGOLAYSMOOTHINGFILTER_H
