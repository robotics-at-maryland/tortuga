/* chksum
   adds the first 'length' bytes contained in the byte array 'buffer',
   adds 'extra' to that result, then returns the total.
   Written by Keith McCready; last edited 6/26/2011
*/

unsigned char chksum(unsigned const char *buffer, unsigned int length, 
                     unsigned char extra)
{
    unsigned char sum= 0;
    unsigned int i;
    for (i= 0; i < length; i++)
        sum+= buffer[i];
    sum+= extra;
    sum= sum & 0xFF;

    return sum;
}
