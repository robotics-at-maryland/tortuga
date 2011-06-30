/* Scenic
   Copyright (C) 2008 Société des arts technologiques (SAT)
   http://www.sat.qc.ca
   All rights reserved.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 2 of the License, or
   (at your option) any later version.

   Scenic is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Scenic .  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * Raw1394 (Firewire on GNU/Linux) bus reset.
 *
 * to compile :
 * gcc -Wall -lraw1394 -o resetbus resetbus.c
 * TODO: add bus number argument
 */

#include <stdio.h>
#include <string.h> /* for strcmp */
#include <stdlib.h> /* for getenv */
#include <libraw1394/raw1394.h>

/**
 * Callback for when raw1394_reset_bus_new() is called.
 *
 * It's a success if it has been called.
 */
static int bus_reset_handler(struct raw1394_handle *handle, unsigned int gen)
{
    printf("INFO: Bus reset occurred.\nINFO: New generation number: %d, \nINFO: %d nodes on the bus\nINFO: local ID: %d\a\n",
        gen,
        raw1394_get_nodecount(handle),
        raw1394_get_local_id(handle) & 0x3f);
    printf("SUCCESS !\n");
    /* TODO: check for errors ? */
    raw1394_update_generation(handle, gen);
    return 0;
}
/**
 * Prints an help message.
 */
void print_help()
{
    printf("Usage: firereset\n\n");
    printf("Usage: %s\n", "firereset");
    printf("Options:\n");
    printf("  -h, --help            Shows program's help message and exits.\n");
    printf("\n");
    printf("Resets the Firewire (ieee1394) bus under GNU/Linux.\n");
    printf("Use at your own risks.\n");
    printf("\n");
    printf("The default device node is /dev/raw1394, but one can override the default by \nsetting environment variable RAW1394DEV. However, if RAW1394DEV points to a \nnon-existant or invalid device node, then it also attempts to open the default \ndevice node. \n");

}

int main(int argc, char **argv)
{
    raw1394handle_t handle;
    int result;
    int type = RAW1394_LONG_RESET;
    int adapter_number = 0; /* see firecontrol/commander.c */

    if (argc >= 2)
    {
        if (strcmp(argv[1], "--help") != 0)
        {
            printf("%s\n", "Kit compiled this version 0.000001");
            return 0;
        }
        print_help();
        return 0;
    }

#if 0
    TODO: add a --device argument (-d) which would change the envrionment variable
    int setenv(const char *envname, const char *envval, int overwrite);
#endif

    handle = raw1394_new_handle();
    if (handle == 0)
    {
        printf("ERROR: Could not get a handle to the firewire bus.\n");
        printf("ERROR: Is the raw1394 module loaded? Is your user in the 'disk' group?\n");
        printf("FAILURE.\n");
        return 1;
    }
    /*
    *  If RAW1394DEV points to a non-existant or invalid device node, then it also attempts to open the default device node.
    */
    printf("INFO: $RAW1394DEV=%s\n", getenv("RAW1394DEV"));
    printf("INFO: Current generation number (driver): %d.\n", raw1394_get_generation(handle));
    raw1394_set_bus_reset_handler(handle, bus_reset_handler);
    fprintf(stdout, "INFO: using adapter %d.\n", adapter_number);

#if 0
    this was causing stack smashing, i think because of the max ports value
    nports = raw1394_get_port_info(handle, &pinf, 1000 /* maxports */);
    printf("Got %d ports\n", nports);
#endif

    if (raw1394_set_port(handle, adapter_number) < 0)
    {
        printf("ERROR: Could not set port.\n");
        printf("FAILURE.");
        return 1;
    }
    result = raw1394_reset_bus_new(handle, type);
    if (result == -1)
    {
        printf("ERROR: Could not reset bus.\n");
        printf("FAILURE.");
        return 1;
    }
#if 0
    else
    {
        printf("INFO: Trying to reset bus did not result in an error.\nResult = %d\n", result);
        printf("(0 means success)\n");
    }
#endif
    raw1394_loop_iterate(handle);
    raw1394_destroy_handle(handle);

    return 0;
}

