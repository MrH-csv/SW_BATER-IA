/****************************************************************************************************
 *                         Bater-IA SW V1 Prototype ino IDE. by: Angel Habid Navarro                *
 *                                                 20/10/2024                                       *
 ****************************************************************************************************/


/*
LIST OF PENDING FUNCTIONALITIES:
- Develop the functionality that allows the measurement of the cells during the discharge process.
- Develop the process to determine if a cell is fully charged/discharged.
- Develop the environment in which the GUI interacts with the basic software (serial menu with numerical options).

*/


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
extern unsigned int Duty = 5;
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
int Charger(void);
int ADC_measure(void);
void discharge_Bat1(void);
void discharge_Bat2(void);
void discharge_Bat3(void);
void Battery_reinit(void);
float ADC_readig(int);
float volt_convertion(volatile float);
void Data_generator(void);
int Load_balancer(void);

/************************************************************
 *                    Arduino Setup function                *
 ************************************************************/
void setup() 
{
  MCU_init();
  delay(5000);// add time (5 sec) to be able of configure the csv maker.
}
/************************************************************
 *           Arduino loop    (similar to main loop)         *
 ************************************************************/
void loop() 
{
  charging_sub_routine();// apply a tipical charge rutine.
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

void Battery_reinit(void)
{
  delay(100);
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
  delay(100);
}

int Charger(void)
{
  /*
  Function to setup the relays to charge the 3 cells in the system, open all 
  relays and keep connected the relay K1 (to principal source of voltage) , 
  the relay K2 (the series connection between Bat1 and Bat2) and the
  relay K3 (this relay is the series connection between the BAT2 and BAT3), 
  at the end, BAT3 is connected directely to grownd .
  */
  //disconect all the relays.
  digitalWrite(K4,HIGH);
  digitalWrite(K5,HIGH);
  digitalWrite(K6,HIGH);
  digitalWrite(K7,HIGH);
  digitalWrite(K8,HIGH);
  digitalWrite(K9,HIGH);
  digitalWrite(K1,LOW);//turn on K1 to charge the Batteries.
  delay(500);
  return 1;//Return 1 to inform that the relays array was alrready set.
}

int ADC_measure(void)
{
  /*
  This function made a reading of the input voltage , current , power , and load voltage...
  After that, setup the relays to disconnect the batteries of the main source of voltage and 
  perform a measurement of the voltage level (this going to be called sample), this process
  must be done every 30 seconds.
  */ 

  //Input voltage measurement and save it in a global variable:
  delay(500);//wait until the relays are set.
  shuntVoltage_mV = ina226.getShuntVoltage_mV();
  busVoltage_V = ina226.getBusVoltage_V();
  current_mA = ina226.getCurrent_mA();
  power_mW = ina226.getBusPower();
  checkForI2cErrors();// validate the I2C comunication.

  //Calculate the voltage in the load
  loadVoltage_V  = busVoltage_V + (shuntVoltage_mV/1000);

  //Set the relays in measure mode:
  delay(100);
  //Battery 1 measurement sub-rutine:
  digitalWrite(K1,HIGH);//Disconnect the Batteries.
  digitalWrite(K2,LOW);//Connect the negative to MCU grownd.
  digitalWrite(K7,HIGH);//Be sure that K7 is disable.
  digitalWrite(K4,LOW);// Connect the ADC CH0 to the BT1.
  delay(100);
  //Battery 2 measurement sub-rutine:
  digitalWrite(K3,LOW);//Connect the negative to MCU grownd.
  digitalWrite(K8,HIGH);//Be sure that K8 is disable.
  digitalWrite(K5,LOW);// Connect the ADC CH1 to the BT2.
  delay(100);
  //Battery 3 measurement sub-rutine:
  digitalWrite(K9,HIGH);//Be sure that K9 is disable.
  digitalWrite(K6,LOW);// Connect the ADC CH3 to the BT3.
  delay(500);
  //Doing the measurements of each battery:
  ADC_Bat2_Meas = ADC_readig(pin_A1);
  ADC_Bat3_Meas = ADC_readig(pin_A2);
  ADC_Bat1_Meas = ADC_readig(pin_A0);
  delay(500);
  return 1;//return 1 to inform that the measurement was done.
}

void discharge_Bat1(void)
{
  // Battery_reinit();
  //Battery 1 discharge sub-rutine:
  digitalWrite(K1,HIGH);//Disconnect the Batteries.
  digitalWrite(K2,LOW);//Connect the negative to MCU grownd.
  digitalWrite(K7,LOW);//Be sure that K7 is enable.
  digitalWrite(K4,LOW);// Connect the ADC CH0 to the BT1 to measure the discharge.
  delay(5000);
}

void discharge_Bat2(void)
{
  // Battery_reinit();
  //Battery 2 discharge sub-rutine:
  digitalWrite(K1,HIGH);//Disconnect the Batteries.
  digitalWrite(K2,LOW);//Disconnect the positive of bat 2.
  digitalWrite(K3,LOW);//Connect the negative to MCU grownd.
  digitalWrite(K5,LOW);//Connect the ADC CH0 to the BT1 to measure the discharge.
  digitalWrite(K8,LOW);//Connect the resistors net to the Bat2.
  delay(5000);
}

void discharge_Bat3(void)
{
  // Battery_reinit();
  //Battery 3 discharge sub-rutine:
  digitalWrite(K1,HIGH);//Disconnect the Batteries.
  digitalWrite(K3,LOW);//Disconnect the positive of bat 3.
  digitalWrite(K9,LOW);//Connect the resistors net to the Bat2.
  digitalWrite(K6,LOW);//Connect the ADC CH0 to the BT1 to measure the discharge.
  delay(5000);
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

void Data_generator(void)
{
  /*
  The data generator order all the information to be printed,
  the finality of this function is print by the serial port the information in the
  better way to create a CSV from the serial bus.
  */

  //Print the headers of the CSV file.
  while(enable1 == true)//Be sure that the code inside of the while  only will be executed once.
  {
    num_sample = 1;
    delay(3000);
    Serial.print("Sample [#]:,Bus Voltage [V]:,Load Voltage [V]:,Current[mA]:,Bus Power [mW]:,Voltage Bat 1:,Voltage Bat 2:,Voltage Bat 3:");
    Serial.println();
    enable1 = false;
  }

  //Print the values in a CSV, "Comma-Separated Values".
  Serial.print(num_sample);Serial.print(",");
  Serial.print(busVoltage_V);Serial.print(",");
  Serial.print(loadVoltage_V);Serial.print(",");
  Serial.print(current_mA);Serial.print(",");
  Serial.print(power_mW);Serial.print(",");
  Serial.print(volt_convertion(ADC_Bat1_Meas));Serial.print(",");
  Serial.print(volt_convertion(ADC_Bat2_Meas));Serial.print(",");
  Serial.print(volt_convertion(ADC_Bat3_Meas));Serial.print(",");
  Serial.println();
  num_sample++;
}

void charging_sub_routine(void)
{
  //Pendiente mejorar la subrutina de carga , en la que se timea la carga y medicion de las celdas y se aplcia una curva de carga adecuada.
  Load_balancer();
  // Describe the voltage curve
  Voltage_adj(Duty);
  //  Duty = Duty + 5;
  //  if(Duty >= 255)
  //  {
  //    Duty = 0;
  //  }
  Chargin_Status = Charger();
  delay(30000);// 30 sec of charging.
  Battery_V_Status =  ADC_measure();
  Data_generator();//print the csv file.
  delay(100);
}

int Load_balancer(void)
{
  ADC_measure();// take measurements of the loads.
  Battery_reinit();//reset the relays to open
  while (ADC_Bat1_Meas != ADC_Bat2_Meas && ADC_Bat2_Meas!= ADC_Bat3_Meas)
  {
    while (ADC_Bat1_Meas > ADC_Bat2_Meas)
    {
      discharge_Bat1();
      ADC_measure();
    }

    while (ADC_Bat1_Meas > ADC_Bat3_Meas)
    {
      discharge_Bat1();
      ADC_measure();
    }

    while (ADC_Bat2_Meas > ADC_Bat1_Meas)
    {
      discharge_Bat2();
      ADC_measure();
    }

    while (ADC_Bat2_Meas > ADC_Bat3_Meas)
    {
      discharge_Bat2();
      ADC_measure();
    }

    while (ADC_Bat3_Meas > ADC_Bat1_Meas)
    {
      discharge_Bat3();
      ADC_measure();
    }

    while (ADC_Bat3_Meas > ADC_Bat2Meas)
    {
      discharge_Bat3();
      ADC_measure();
    }
    Serial.print("Balancing the loads...\n");
  }
}
