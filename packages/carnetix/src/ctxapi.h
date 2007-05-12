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
    double dlyon;   /* in hours ?? */
    double bu_lo;   /* in seconds */
    double sd_lo;   /* in seconds */
    double lobatt;  /* in volts */
    unsigned char softJumpers;
};

usb_dev_handle * ctxInit();
int ctxReadValues(usb_dev_handle * hDev, struct ctxValues * val);
int ctxReadParams(usb_dev_handle * hDev, struct ctxParams * prm);
int ctxPriOn(usb_dev_handle * hDev);
int ctxPriOff(usb_dev_handle * hDev);

