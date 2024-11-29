#include "Arduino.h"
#include "ADC_CFG_V1.h"
#include "Sam.h"


void ADC_init(void)
{
 analogReadResolution(Res_ADC);
 ADC->ADC_MR |= 0x80; // these lines set free running mode on ADC module.
 ADC->ADC_CR = 2; // Starts the ADC module in the SAM3X.
 ADC->ADC_CHER = 0xE0; // this is (1<<7) [pin A0] | (1<<6) [pin A1] | (1<<5) [pin A2] for ADC CH5 , CH6 & CH7.
 ADC->ADC_CHDR = 0x1F; // Disable all the channels that does not gona be used. 
}

float ADC_reading(int ADC_CH)
{
  int meas; 
  while((ADC->ADC_ISR & 0xE0)!=0xE0);// wait for two conversions (pin A0[7]  and A1[6])
  meas=ADC->ADC_CDR[ADC_CH];              // read data on ADC Chanel data register.
  return meas; 
}

float volt_conversion(volatile float digital_reading)
{
  float voltage = 0;
  float resolution = 0;
  int num_base = 2;

  for (int i = 1; i < Res_ADC; i++)// Reach the resolution number in samples.
  {
    num_base *= 2;
  }
  //Serial.println(num_base);// only to debug (to now if the resolution is correct).
  resolution = (Vmax_ADC)/((num_base)-1);// getting the voltage resolution using (Vref)/((2^n)-1).
  voltage = (resolution)*(digital_reading);// getting the voltage measured.
  return voltage;
}