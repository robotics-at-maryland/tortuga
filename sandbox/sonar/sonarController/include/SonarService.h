#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <complex>
#include "Sonar.h"

#ifndef _SONAR_SONARSERVICE_H
#define _SONAR_SONARSERVICE_H


namespace ram {
namespace sonar {


typedef struct {
	adcmath_t re, im;
} complex_adcmath_t;


typedef struct {
	complex_adcmath_t channels[4];
} PingRecord_t;


#ifdef __cplusplus
class SonarService {
public:
	SonarService();
	boost::numeric::ublas::vector<double> localize(const PingRecord_t &);
private:
	//! A 4x4 matrix
	boost::numeric::ublas::matrix<double> m_calibration;
};


} // namespace sonar
} // namespace ram
#endif

#endif
