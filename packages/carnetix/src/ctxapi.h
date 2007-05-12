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

usb_dev_handle * ctxInit();
int ctxReadValues(usb_dev_handle * hDev, struct ctxValues * val);

