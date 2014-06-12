// STD Includes
#include <cassert>
#include <cstdlib>
#include <cstdio>

// Library Includes
#include <dc1394/video.h>
#include <dc1394/camera.h>
#include <dc1394/conversions.h>

int main(int argc, char **argv) {
    initLibDC1394();
    dc1394_cleanup_iso_channels_and_bandwidth();

    return 0;
}
