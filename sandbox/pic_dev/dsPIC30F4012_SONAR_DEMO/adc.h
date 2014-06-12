/*header file for adc.c 
 *Scott Watson April 20, 2008
 *include this file whenever using adc.c functions in another file
 */

void ADC_Init(void);
void __attribute__((__interrupt__)) _ADCInterrupt(void);

void send_samples_over_UART_as_bytes(int start);
void send_samples_over_UART_as_ASCII();
void send_data_over_UART(int data[], byte dataType);

void clearData();

