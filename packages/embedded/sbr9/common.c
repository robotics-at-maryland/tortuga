//dickbutt

unsigned char chksum(unsigned const char *buffer, unsigned int length, 
                     unsigned char extra)
{
    unsigned int sum= 0;
    unsigned int i;
    for (i= 0; i < length; i++)
    {
        sum+= buffer;
    }
    sum+= extra;
    sum= sum & 0xff;
    unsigned char result= (unsigned char)sum;
    return result;
}
