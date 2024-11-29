#include "Arduino.h"
#include "PWM_CTRL.h"
/**Pending to implement the rest of functionalities for heach PWM port described in the header */

void PWM_init(void)
{
    /*PWM Outputs*/
    pinMode(BMS_PWM_1, OUTPUT);// PWM2 As output.
}
void Voltage_adj(int DutyC)
{
  //This function perform the adjustement of the PWM signal that controlls the input voltage
  //During the charge stage.
  analogWrite(BMS_PWM_1,DutyC);
}


