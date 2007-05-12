#include <stdio.h>
#include <stdlib.h>
#include <usb.h>
#include <stdarg.h>
#include <string.h>
#include "ctxapi.h"

#define VENDOR              0x04D8
#define READ_ENDPOINT           0x81
#define WRITE_ENDPOINT          0x01
#define DEFAULT_CONFIGURATION       1
#define DEFAULT_INTERFACE           0
#define DEFAULT_ALT_INTERFACE       0
#define SHORT_TIMEOUT           3000

unsigned short m_product_id;
unsigned short m_vendor_id;



void debug(char *format, ...)
{
    va_list ap;
    char logmessage[2048];
    memset(logmessage,0,sizeof(logmessage));
    va_start(ap,format);
    (void)vsnprintf(logmessage,sizeof(logmessage),format,ap);
    va_end(ap);
    fprintf(stderr,"%s\n",logmessage);
}



/* This code is based on pv2fetch, the program to download pictures from a disposable CVS camera */
usb_dev_handle * ctxInit()
{
    struct usb_bus *p_bus;
    struct usb_device* pdev=NULL;

    usb_init();
    if (usb_find_busses() < 0)
    {
        debug("Error: Could not find USB bus.");
        return NULL;
    }

    if (usb_find_devices() < 0)
    {
        debug("Error: Could not find any USB devices.");
        return NULL;
    }

    p_bus = usb_get_busses();

    while (p_bus)
    {
        struct usb_device *p_device = p_bus->devices;
        while (p_device)
        {
            if (p_device->descriptor.idVendor == VENDOR)
            {
                usb_dev_handle* udev = usb_open(p_device);

                if (udev)
                {
                    pdev = p_device;
                    m_vendor_id = p_device->descriptor.idVendor;
                    m_product_id = p_device->descriptor.idProduct;
                    usb_close(udev);
                    debug("Found power supply: VID:%.4X PID:%.4X",m_vendor_id,m_product_id);
                    break;
                }
                else
                {
                    debug("Error: Found the power supply, but could not open it.");
                    break;
                }

            }
            p_device = p_device->next;
        }

        if (pdev)
            break;

        p_bus = p_bus->next;
    }

    if (pdev == NULL)
    {
        debug("Error: Could not find the power supply.");
        return NULL;
    }

    usb_dev_handle * hDev = usb_open(pdev);

    if(!hDev)
        return NULL;

    if (usb_set_configuration(hDev, DEFAULT_CONFIGURATION) >= 0)
    {
        if (usb_claim_interface(hDev, DEFAULT_INTERFACE) >= 0)
        {
            if (usb_set_altinterface(hDev, DEFAULT_ALT_INTERFACE) < 0)
            {
                usb_release_interface(hDev, DEFAULT_INTERFACE);
                usb_close(hDev);
                return NULL;
            }
        }
        else
        {
            usb_close(hDev);
            return NULL;
        }
    }
    else
    {
        usb_close(hDev);
        return NULL;
    }

    return hDev;
}

int ctxRead(usb_dev_handle * hDev, unsigned char* p_buffer, unsigned int length, int timeout)
{
    return usb_bulk_read(hDev, READ_ENDPOINT, (char*)p_buffer, length, timeout);
}

int ctxWrite(usb_dev_handle * hDev, unsigned char* p_buffer, unsigned int length, int timeout)
{
    return usb_bulk_write(hDev, WRITE_ENDPOINT, (char*)p_buffer, length, timeout);
}

int ctxReadValues(usb_dev_handle * hDev, struct ctxValues * val)
{
    unsigned char getValCmd[]={0x40, 0x17};
    unsigned char buf[23];

    if(!val)
        return -1;

    if(ctxWrite(hDev, getValCmd, 2, SHORT_TIMEOUT) != 2)
        return -1;

    if(ctxRead(hDev, buf, 23, SHORT_TIMEOUT) != 23)
        return -1;

    if(buf[0] != 0x40 || buf[1] != 0x17)
        return -1;

    val->battVoltage = ((buf[3]<<8) | buf[2]) * 0.0321;
    val->battCurrent = ((buf[5]<<8) | buf[4]) * 0.016;

    val->priVoltage = ((buf[7]<<8) | buf[6]) * 0.0321;
    val->priCurrent = ((buf[9]<<8) | buf[8]) * 0.016;

    val->secVoltage = ((buf[11]<<8) | buf[10]) * 0.0134;
    val->secCurrent = ((buf[13]<<8) | buf[12]) * 0.004;

    val->temperature = (((buf[15]<<8) | buf[14])-125) * 0.403;
    val->ledValue = ((buf[17]<<8) | buf[16]);
    val->state = buf[18];
    return 0;
}
