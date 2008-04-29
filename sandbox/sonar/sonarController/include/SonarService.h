#include <octave/oct.h>
#include "Sonar.h"

#ifndef _SONAR_SONARSERVICE_H
#define _SONAR_SONARSERVICE_H

#ifdef __cplusplus
namespace ram {
namespace sonar {
#endif


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
	ColumnVector localize(const PingRecord_t &);
private:
	//! A 4x4 matrix
	Matrix m_calibration;
};


} // namespace sonar
} // namespace ram
#endif

#endif
