struct ctxValues
{
    double battVoltage;
    double battCurrent;

    double priVoltage;
    double priCurrent;

    double secVoltage;
    double secCurrent;

    double temperature;

    unsigned int ledValue;
    unsigned int state;

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
int ctxReadValues(usb_dev_handle * hDev, struct ctxValues * val);
int ctxReadParams(usb_dev_handle * hDev, struct ctxParams * prm);
int ctxPriOn(usb_dev_handle * hDev);
int ctxPriOff(usb_dev_handle * hDev);
int ctxSecOn(usb_dev_handle * hDev);
int ctxSecOff(usb_dev_handle * hDev);
int ctxP5VOn(usb_dev_handle * hDev);
int ctxP5VOff(usb_dev_handle * hDev);
void ctxClose(usb_dev_handle * hDev);
