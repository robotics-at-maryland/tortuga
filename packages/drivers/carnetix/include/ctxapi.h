/*****************************************************************************
 * Steve Moskovchenko  (aka evil wombat)                                     *
 * University of Maryland                                                    *
 * stevenm86 at gmail dot com                                                *
 * http://wam.umd.edu/~stevenm/carnetix.html                                 *
 *                                                                           *
 * Linux and Mac OS API for the Carnetix P2140 power supply.                 *
 * The program uses libusb, which works on Mac OS X, Linux, and Windows.     *
 *                                                                           *
 * Released under the BSD License. See License.txt for info.                 *
 *                                                                           *
 * Version 0.2. Enjoy!                                                       *
 *                                                                           *
 ****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

 /* Structure that holds the status and voltage/current readings
  * Counter and Fault Code are missing as of right now, since I do
  * not know how they are sent back.
  */
struct ctxValues
{
    double battVoltage;    /* In Volts */
    double battCurrent;    /* In Amps  */

    double priVoltage;     /* In Volts */
    double priCurrent;     /* In Amps  */

    double secVoltage;     /* In Volts */
    double secCurrent;     /* In Amps  */

    double temperature;    /* In Degrees C */

    unsigned int ledValue; /* See defines below */
    unsigned int state;    /* See array below */

};

/*
 * These LED names came out of the C# API provided by Carnetix
 * Since LED LSB and MSB are combined into one value, these are combined as well
 */
#define LED_IGNITION    0x0001
#define LED_PULSESTART  0x0002
#define LED_FANFAULT    0x0004
#define LED_DELAYON     0x0008
#define LED_ACPI        0x0010
#define LED_POWERGOOD   0x0020
#define LED_PS_ON       0x0040
#define LED_FANON       0x0080

/* Battery Voltage OK */
#define LED_VOLTAGE_OK  0x0100

/* Primary Current OK */
#define LED_PRICUR_OK   0x0200

/* Secondary Current OK */
#define LED_SECCUR_OK   0x0400



/*
 * Jumper Names
 * From the P2140 C# API Manual
 * Bit 0 = Secondary Output ON Control (0=Follow IGN, 1=Follow DLYON)
 * Bit 1 = Secondary Output OFF Control (0=Follow IGN, 1=Follow SDLO)
 * Bit 2 = not used
 * Bit 3 = Ignore Fan Fault (0=do not ingore, 1 = ignore)
 * Bit 4 = Shutdown Control Signal (0=No not use USB shutdown control, 1=Use USB shutdown control)
 * Bit 5 = not used
 * Bit 6 = Shutdown Mode msb (modes: 0x00=hibernate, 0x01=standby, 0x02=laptop)
 * Bit 7 = Shutdown Mode lsb (modes: 0x00=hibernate, 0x01=standby, 0x02=laptop)
 *
 * These are also defined below.
 */
#define JUMPER_SEC_OUT_ON_FOLLOW_DLYON     0x01
#define JUMPER_SEC_OUT_ON_FOLLOW_IGN       0x00

#define JUMPER_SEC_OUT_OFF_FOLLOW_SDLO     0x02
#define JUMPER_SEC_OUT_OFF_FOLLOW_IGN      0x00

#define JUMPER_IGNORE_FAN_FAULT            0x08

#define JUMPER_USE_USB_SHUTDOWN_CONTROL    0x10

#define JUMPER_SHUTDOWN_MODE_HIBERNATE     0x00
#define JUMPER_SHUTDOWN_MODE_STANDBY       0x40
#define JUMPER_SHUTDOWN_MODE_LAPTOP        0x80


/** Structure to store the supply parameters (configuration).
 * These are pretty much the same parameters as displayed by PSUmoni
 * This structure can be used for reading and writing parameters.
 * But, currently they are NOT range checked, so use at your own risk.
 */
struct ctxParams
{
    double sd_dly;   /* in seconds */
    double dmt;     /* in hours */
    double dlyon;   /* in seconds */
    double bu_lo;   /* in seconds */
    double sd_lo;   /* in seconds */
    double lobatt;  /* in volts */
    unsigned char softJumpers;

    double acpiDelay;   /* in seconds */
    double acpiDuration; /* in seconds */
    double lowTemp;     /* in degrees C */
};

/**
 * Opens the device and returns a device handle. If the supply was not found
 * or somehow a connection could not be made, returns NULL.
 *
 * the device type is usb_dev_handle* and is provided in usb.h from libusb
 */
usb_dev_handle * ctxInit();

/**
 * Fills the passed-in string buffer with a string representation of the
 * firmware version. Len is the length of the buffer passed in. The buffer
 * length must be at least 12 bytes, or nothing will happen and an error
 * will be returned. Returns 0 on success and -1 on error.
 *
 * hDev is the device handle, buf is a pointer to where the version is to
 * be written, and len is the length of the buffer
 */
int ctxGetFWVersion(usb_dev_handle * hDev, char * buf, int len);


/**
 * Polls the supply and fills the passed-in structure with the readings
 * from the supply. Returns 0 on success or -1 on error.
 *
 * hDev is the device handle. val is a pointer to the ctxValues structure
 * where the values are to be written.
 */
int ctxReadValues(usb_dev_handle * hDev, struct ctxValues * val);

/**
 * Polls the supply and fills the passed-in structure with the supply's
 * configuration parameters. Returns 0 on success or -1 on error.
 *
 * hDev is the device handle. prm is a pointer to the ctxParams structure
 * where the parameters are to be written.
 */
int ctxReadParams(usb_dev_handle * hDev, struct ctxParams * prm);


/**
 * Writes the given parameters to the power supply. Returns 0 on success
 * and -1 on failure. The parameters are NOT range checked yet, so the values
 * must be within the proper range (I'm not sure what that is.. maybe the
 * C# API gives the ranges).
 *
 * Note that all the parameters are written. So if you need to change just
 * one parameter, you probably should read the parameters into a ctxParams
 * structure first, change the values you want, and write the structure back.
 *
 * hDev is the device handle. prm is a pointer to the structure containing
 * the parameters that are to be sent.
 */
int ctxWriteParams(usb_dev_handle * hDev, struct ctxParams * prm);


/**
 * Given the supply's device handle, turns on the Primary supply. Returns 0
 * on success and -1 on failure.
 */
int ctxPriOn(usb_dev_handle * hDev);


/**
 * Given the supply's device handle, turns off the Primary supply. Returns 0
 * on success and -1 on failure.
 */
int ctxPriOff(usb_dev_handle * hDev);


/**
 * Given the supply's device handle, turns on the Secondary supply. Returns 0
 * on success and -1 on failure.
 */
int ctxSecOn(usb_dev_handle * hDev);


/**
 * Given the supply's device handle, turns off the Secondary supply. Returns 0
 * on success and -1 on failure.
 */
int ctxSecOff(usb_dev_handle * hDev);


/**
 * Given the supply's device handle, turns on the P5V regulator. Returns 0
 * on success and -1 on failure.
 */
int ctxP5VOn(usb_dev_handle * hDev);


/**
 * Given the supply's device handle, turns off the P5V regulator. Returns 0
 * on success and -1 on failure.
 */
int ctxP5VOff(usb_dev_handle * hDev);


/**
 * Closes the given device handle, disconnecting from the power supply.
 */
void ctxClose(usb_dev_handle * hDev);

/**
 *  Translates state name to text
 */
const char* ctxStateNameToText(int state);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
