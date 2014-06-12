/* Michael Levashov
 * Robotics@Maryland
 * A function to calculate the direction that the ping is coming from
 * given a set of data.
 * First, it runs a threshold-based ping-finding algorithm pingDetect.
 * getPingChunk returns it chunks of data containing the ping.
 * Then it runs time_diff on the data, which finds the time offset using
 * fft convolutions.
 * Finally, the times are used to calculate the direction and distance to the pinger
 * The function returns 1 if it finds the ping, 0 if not, -1 if error
 */

namespace ram {
namespace sonar {

int getDirEnvelope(sonarPing* ping, dataset* dataSet);

}
}
