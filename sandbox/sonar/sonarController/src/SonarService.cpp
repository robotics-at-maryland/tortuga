#include "SonarService.h"

namespace ram {
namespace sonar {

SonarService::SonarService() : m_calibration(4,4) {}

ColumnVector SonarService::localize(const PingRecord_t &rec)
{
	ColumnVector tdoas(4, 1);
	for (octave_idx_type i = 0 ; i < 4 ; i ++)
	{
		Complex c(rec.channels[i].re, rec.channels[i].im);
		tdoas(i) = arg(c);
	}
	ColumnVector kvector = m_calibration * tdoas;
	kvector.resize_no_fill(3);
	return kvector;
}

} // namespace sonar
} // namespace ram
