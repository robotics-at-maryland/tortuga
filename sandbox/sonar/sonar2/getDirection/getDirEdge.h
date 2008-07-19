//Michael Levashov
//Robotics@Maryland

namespace ram {
namespace sonar {

using namespace ram::math;

class getDirEdge {
    getPingChunk *chunk;
    adcdata_t* data[NCHANNELS];
    int locations[NCHANNELS];
    int64_t total[NCHANNELS];
    int64_t abstotal[NCHANNELS];
    adcdata_t average[NCHANNELS];
    adcdata_t absaverage[NCHANNELS];
    int pingpoints[NCHANNELS];
    int ping_found;
    MatrixN *tdoas;
    MatrixN *temp_calc; //A matrix for temporary calculations
    MatrixN *hydro_array; //A matrix for storing the hydro array

    public:
    getDirEdge();
    ~getDirEdge();
    int getEdge(sonarPing* ping, dataset *dataSet);
};
} //sonar
} //ram
