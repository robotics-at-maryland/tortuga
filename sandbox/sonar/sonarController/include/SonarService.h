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
	complex_adcmath_t[4];
} PingRecord_t;


#ifdef __cplusplus
} // namespace sonar
} // namespace ram
#endif

#endif
