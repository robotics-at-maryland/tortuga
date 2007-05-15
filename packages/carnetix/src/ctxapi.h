struct ctxValues
{
    double battVoltage;
    double battCurrent;

    double priVoltage;
    double priCurrent;

    double secVoltage;
    double secCurrent;

    double temperature;

    unsigned int ledValue; /* See Below */
    unsigned int state;

};

/* Since LED LSB and MSB are combined into one value, these are combined as well */
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



/* Jumper Names */
/* From the P2140 C# API Manual:
 Bit 0 = Secondary Output ON Control (0=Follow IGN, 1=Follow DLYON)
 Bit 1 = Secondary Output OFF Control (0=Follow IGN, 1=Follow SDLO)
 Bit 2 = not used
 Bit 3 = Ignore Fan Fault (0=do not ingore, 1 = ignore)
 Bit 4 = Shutdown Control Signal (0=No not use USB shutdown control, 1=Use USB shutdown control)
 Bit 5 = not used
 Bit 6 = Shutdown Mode msb (modes: 0x00=hibernate, 0x01=standby, 0x02=laptop)
 Bit 7 = Shutdown Mode lsb (modes: 0x00=hibernate, 0x01=standby, 0x02=laptop)
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


static const unsigned char * ctxStateNames[] =
{
    "Idle", "Power PSU", "Power PC", "Bootup Lockout", "Run PC", "Shutdown Delay",
    "ACPI Pulse", "Shutdown Lockout", "Standby/Sleep", "Forced Shutdown", "Initialization"
};

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

usb_dev_handle * ctxInit();
int ctxGetFWVersion(usb_dev_handle * hDev, char * buf, int len);
int ctxReadValues(usb_dev_handle * hDev, struct ctxValues * val);
int ctxReadParams(usb_dev_handle * hDev, struct ctxParams * prm);
int ctxWriteParams(usb_dev_handle * hDev, struct ctxParams * prm);
int ctxPriOn(usb_dev_handle * hDev);
int ctxPriOff(usb_dev_handle * hDev);
int ctxSecOn(usb_dev_handle * hDev);
int ctxSecOff(usb_dev_handle * hDev);
int ctxP5VOn(usb_dev_handle * hDev);
int ctxP5VOff(usb_dev_handle * hDev);
void ctxClose(usb_dev_handle * hDev);
