/****************************************************************************************************
 *                         Bater-IA SW V1 Prototype ino IDE. by: Angel Habid Navarro                *
 *                                                 20/10/2024                                       *
 ****************************************************************************************************/
/************************************************************
 *                    Headers & libraries                   *
 ************************************************************/
#include <Wire.h>
#include "INA226_V1_AHNM.h"
/************************************************************
 *                        Macro defines                     *
 ************************************************************/
//I2C Address:
#define I2C_ADDRESS 0x40
//PWM Voltage control pin:
#define GPIO_Pin_PWM2  2
//GPIO to Relay control pins:
#define K1 22
#define K2 24
#define K3 26
#define K4 28
#define K5 30
#define K6 32
#define K7 34
#define K8 36
#define K9 38
#define K10 40 
// Relation between ADC channels and Analog pins in the board:
#define pin_A0 7
#define pin_A1 6
#define pin_A2 5
//ADC resolution:
#define Res_ADC 12// #define Res_ADC 10
//ADC Vref:
#define Vmax_ADC 3.3

/************************************************************
 *                        Constructors                      *
 ************************************************************/
INA226_V1 ina226 = INA226_V1(I2C_ADDRESS);

/************************************************************
 *                       Global Variables                   *
 ************************************************************/
extern unsigned int Duty = 0;
extern bool enable1 = true;
extern int num_sample = 0;
volatile float ADC_Bat1_Meas = 0;
volatile float ADC_Bat2_Meas = 0;
volatile float ADC_Bat3_Meas = 0;
volatile int Battery_V_Status = 0; // 0 = if the Battery is not being measured.
volatile int Chargin_Status = 0; // 0 = if the Battery array is not charging
volatile float loadVoltage_V = 0.0;
volatile float busVoltage_V = 0.0;
volatile float current_mA = 0.0;
volatile float power_mW = 0.0; 
volatile float shuntVoltage_mV = 0.0;
volatile float menu = 0;
/************************************************************
 *                    Function Prototypes                   *
 ************************************************************/
void MCU_init(void);
void serial_init(void);
void GPIO_init(void);
void I2C_init(void);
void ADC_int(void);
void checkForI2cErrors(void);
void Voltage_adj(int);
void Charger(void);
int ADC_measure(void);
void discharge_Bat1(void);
void discharge_Bat2(void);
void discharge_Bat3(void);
void Battery_reinit(void);
float ADC_readig(int);
float volt_convertion(volatile float);
void Data_generator(volatile int ,volatile int);

/************************************************************
 *                    Arduino Setup function                *
 ************************************************************/
void setup() 
{
  MCU_init();
  delay(3000);
}
/************************************************************
 *           Arduino loop    (similar to main loop)         *
 ************************************************************/
void loop() 
{
  switch (menu)
  {
  case 1:
    charging_sub_routine();
    break;
  
  default:
    Serial.print("Error");
    return 55;
    break;
  }
// Describe the voltage curve
//  Voltage_adj(Duty);
//  Duty = Duty + 5;
//  if(Duty >= 255)
//  {
//    Duty = 0;
//  }
//  num_sample++;
//  delay(1000);
  //Charger();
  //delay(10000);
  while (1)
  {
   Chargin_Status = Charger();
   Battery_V_Status =  ADC_measure();
   Data_generator(Battery_V_Status,)
    delay(4000);
  }
}
/***************************************************************
 *                    Functions deploiment                     *
 ***************************************************************/
void MCU_init(void)
{
  serial_init();
  GPIO_init();
  I2C_init();
  ADC_int();
}

void serial_init(void)
{
  /*Serial Init*/
  Serial.begin(9600);
}

void GPIO_init(void)
{
  /*PWM Outputs*/
  pinMode(GPIO_Pin_PWM2, OUTPUT);// PWM2 As output.
  /*relays*/
  /*Set the GPIO as an output*/
  pinMode(K1, OUTPUT);
  pinMode(K2, OUTPUT);
  pinMode(K3, OUTPUT);
  pinMode(K4, OUTPUT);
  pinMode(K5, OUTPUT);
  pinMode(K6, OUTPUT);
  pinMode(K7, OUTPUT);
  pinMode(K8, OUTPUT);
  pinMode(K9, OUTPUT);
  pinMode(K10, OUTPUT);

  /*initialization Relay GPIOS*/
  digitalWrite(K1,HIGH);
  digitalWrite(K2,HIGH);
  digitalWrite(K3,HIGH);
  digitalWrite(K4,HIGH);
  digitalWrite(K5,HIGH);
  digitalWrite(K6,HIGH);
  digitalWrite(K7,HIGH);
  digitalWrite(K8,HIGH);
  digitalWrite(K9,HIGH);
  digitalWrite(K10,HIGH);
}

void I2C_init(void)
{
   /*I2C init*/
  Wire.begin();
  if(!ina226.init()){
  Serial.println("Failed to init INA226. Check your wiring.");
  while(1){}
  }
  ina226.setAverage(AVERAGE_4);
  ina226.setCorrectionFactor(0.898);
  ina226.waitUntilConversionCompleted(); //avoid that the first data might be zero.
}

void ADC_int(void)
{
 analogReadResolution(Res_ADC);
 ADC->ADC_MR |= 0x80; // these lines set free running mode on ADC module.
 ADC->ADC_CR = 2; // Starts the ADC module in the SAM3X.
 ADC->ADC_CHER = 0xE0; // this is (1<<7) [pin A0] | (1<<6) [pin A1] | (1<<5) [pin A2] for ADC CH5 , CH6 & CH7.
 ADC->ADC_CHDR = 0x1F; // Disable all the channels that does not gona be used. 
}

void checkForI2cErrors(void)
{
  byte errorCode = ina226.getI2cErrorCode();
  if(errorCode)
  {
    Serial.print("I2C error: ");
    Serial.println(errorCode);
    switch(errorCode)
    {
      case 1:
        Serial.println("Data too long to fit in transmit buffer");
        break;
      case 2:
        Serial.println("Received NACK on transmit of address");
        break;
      case 3: 
        Serial.println("Received NACK on transmit of data");
        break;
      case 4:
        Serial.println("Other error");
        break;
      case 5:
        Serial.println("Timeout");
        break;
      default: 
        Serial.println("Can't identify the error");
    }
    if(errorCode)
    {
      while(1){}
    }
  }
}

void Voltage_adj(int DutyC)
{
  //This function perform the adjustement of the PWM signal that controlls the input voltage
  //During the charge stage.
  analogWrite(GPIO_Pin_PWM2,DutyC);
}

int Charger(void)
{
  //charger battery sub-rutine.
  //disconect all the relays.
  digitalWrite(K4,HIGH);
  digitalWrite(K5,HIGH);
  digitalWrite(K6,HIGH);
  digitalWrite(K7,HIGH);
  digitalWrite(K8,HIGH);
  digitalWrite(K9,HIGH);
  digitalWrite(K1,LOW);//turn on K1 to charge the Batteries.
  delay(500);
  return 1;
}

int ADC_measure(void)
{
  //Input voltage measurement:
  shuntVoltage_mV = ina226.getShuntVoltage_mV();
  busVoltage_V = ina226.getBusVoltage_V();
  current_mA = ina226.getCurrent_mA();
  power_mW = ina226.getBusPower();
  checkForI2cErrors();

  //Calculate the voltage in the load
  loadVoltage_V  = busVoltage_V + (shuntVoltage_mV/1000);

  //Set the relays in measure mode:
  delay(500);
  //Battery 1 measurement sub-rutine:
  digitalWrite(K1,HIGH);//Disconnect the Batteries.
  digitalWrite(K2,LOW);//Connect the negative to MCU grownd.
  digitalWrite(K7,HIGH);//Be sure that K7 is unable.
  digitalWrite(K4,LOW);// Connect the ADC CH0 to the BT1.
  delay(500);
  //Battery 2 measurement sub-rutine:
  digitalWrite(K3,LOW);//Connect the negative to MCU grownd.
  digitalWrite(K8,HIGH);//Be sure that K8 is unable.
  digitalWrite(K5,LOW);// Connect the ADC CH1 to the BT2.
  delay(500);
  //Battery 3 measurement sub-rutine:
  digitalWrite(K9,HIGH);//Be sure that K9 is unable.
  digitalWrite(K6,LOW);// Connect the ADC CH3 to the BT3.
  delay(1000);
  //Doing the measurements:
  ADC_Bat2_Meas = ADC_readig(pin_A1);
  ADC_Bat3_Meas = ADC_readig(pin_A2);
  ADC_Bat1_Meas = ADC_readig(pin_A0);
  delay(500);
  return 1;
}

void discharge_Bat1(void)
{
  Battery_reinit();
  //Battery 1 discharge sub-rutine:
  digitalWrite(K1,HIGH);//Disconnect the Batteries.
  digitalWrite(K2,LOW);//Connect the negative to MCU grownd.
  digitalWrite(K7,LOW);//Be sure that K7 is enable.
  digitalWrite(K4,LOW);// Connect the ADC CH0 to the BT1 to measure the discharge.
  delay(100);
//  ADC_Bat_Meas = ADC_readig(pin_A0);
//  Serial.println(volt_convertion(ADC_Bat_Meas));
  ADC_Bat_Meas = analogRead(A0);
  Serial.println(volt_convertion(ADC_Bat_Meas));
}

void discharge_Bat2(void)
{
  Battery_reinit();
  //Battery 2 discharge sub-rutine:
  digitalWrite(K1,HIGH);//Disconnect the Batteries.
  digitalWrite(K2,LOW);//Disconnect the positive of bat 2.
  digitalWrite(K3,LOW);//Connect the negative to MCU grownd.
  digitalWrite(K5,LOW);//Connect the ADC CH0 to the BT1 to measure the discharge.
  digitalWrite(K8,LOW);//Connect the resistors net to the Bat2.
  delay(100);
//  ADC_Bat_Meas = ADC_readig(pin_A1);
  ADC_Bat_Meas = analogRead(A1);
  Serial.println(volt_convertion(ADC_Bat_Meas));
}

void discharge_Bat3(void)
{
  Battery_reinit();
  //Battery 3 discharge sub-rutine:
  digitalWrite(K1,HIGH);//Disconnect the Batteries.
  digitalWrite(K3,LOW);//Disconnect the positive of bat 3.
  digitalWrite(K9,LOW);//Connect the resistors net to the Bat2.
  digitalWrite(K6,LOW);//Connect the ADC CH0 to the BT1 to measure the discharge.
  delay(100);
//  ADC_Bat_Meas = ADC_readig(pin_A2);
ADC_Bat_Meas = analogRead(A2);
  Serial.println(volt_convertion(ADC_Bat_Meas));
}

void Battery_reinit(void)
{
  digitalWrite(K1,HIGH);
  digitalWrite(K2,HIGH);
  digitalWrite(K3,HIGH);
  digitalWrite(K4,HIGH);
  digitalWrite(K5,HIGH);
  digitalWrite(K6,HIGH);
  digitalWrite(K7,HIGH);
  digitalWrite(K8,HIGH);
  digitalWrite(K9,HIGH);
  digitalWrite(K10,HIGH);
}

float ADC_readig(int ADC_CH)
{
  int meas; 
  while((ADC->ADC_ISR & 0xE0)!=0xE0);// wait for two conversions (pin A0[7]  and A1[6])
  meas=ADC->ADC_CDR[ADC_CH];              // read data on ADC Chanel data register.
  return meas; 
}

float volt_convertion(volatile float digital_reading)
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

void Data_generator(volatile int ADC_status ,volatile int Charging_status)
{
  if (Charging_status = 1)
  {
    /* code */
  }

  if (ADC_status = 1)
  {
    Serial.print("");Serial.print("");Serial.print("");
  Serial.println();
  Serial.print(volt_convertion(ADC_Bat1_Meas));Serial.print(" ");
  Serial.print(volt_convertion(ADC_Bat2_Meas));Serial.print(" ");
  Serial.print(volt_convertion(ADC_Bat3_Meas));
  Serial.println();
  }
  
   while(enable1 == true)
  {
    num_sample = 1;
    delay(3000);
    Serial.print("Sample [#]:,Bus Voltage [V]:,Load Voltage [V]:,Current[mA]:,Bus Power [mW]:,Bat 1:,Bat 2:,Bat 3:");
    Serial.println();
    enable1 = false;
  }

  //Print the values in a CSV, "Comma-Separated Values".
  Serial.print(num_sample);Serial.print(",");
  Serial.print(busVoltage_V);Serial.print(",");Serial.print(loadVoltage_V);Serial.print(",");
  Serial.print(current_mA);Serial.print(",");Serial.print(power_mW);
  Serial.println();
  
}

void charging_sub_routine(void)
{
  while (/* condition */)
  {
    Charger();
    delay(30000);
    ADC_measure();
  }
  
  
}