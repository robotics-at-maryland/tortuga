#include "SonarService.h"
#include <iostream>

using namespace boost::numeric;

int main()
{
	ram::sonar::SonarService ss;
	return 0;
}

namespace ram {
namespace sonar {

SonarService::SonarService() : m_calibration(4,4) {
	const static double m[4][4] = 
	{
		{0,      0.984, 0,     0},
		{0.492,  0.492, 0.696, 0},
		{-0.492, 0.492, 0.696, 0},
		{0,      0,     0,     1}
	};
	for (int i = 0 ; i < 3 ; i ++)
		for (int j = 0 ; j < 3 ; j ++)
			m_calibration(i, j) = m[i][j];
}

ublas::vector<double> SonarService::localize(const PingRecord_t &rec)
{
	ublas::vector<double> tdoas(4);
	for (int i = 0 ; i < 4 ; i ++)
	{
		std::complex<double> c(rec.channels[i].re, rec.channels[i].im);
		tdoas(i) = arg(c);
	}
	ublas::vector<double> kvector = prod(m_calibration, trans(tdoas));
	kvector.resize(3);
	return kvector;
}

} // namespace sonar
} // namespace ram
