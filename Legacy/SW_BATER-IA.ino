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
#include "SW_BATER-IA.h"
#include "PWM_CTRL.h"
#include "ADC_CFG.h"

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
  Load_balancer(); // integrar a la subrrutina de carga para que solo se ejecute 1 vez.
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
  PWM_init();
}

void serial_init(void)
{
  /*Serial Init*/
  Serial.begin(9600);
}

void GPIO_init(void)
{
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
  delay(15000);
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
  delay(15000);
}

void discharge_Bat3(void)
{
  // Battery_reinit();
  //Battery 3 discharge sub-rutine:
  digitalWrite(K1,HIGH);//Disconnect the Batteries.
  digitalWrite(K3,LOW);//Disconnect the positive of bat 3.
  digitalWrite(K9,LOW);//Connect the resistors net to the Bat2.
  digitalWrite(K6,LOW);//Connect the ADC CH0 to the BT1 to measure the discharge.
  delay(15000);
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
    Serial.print("Balancing the loads...\n");
    ADC_measure();
    while (ADC_Bat1_Meas > ADC_Bat2_Meas)
    {
      Serial.print("Balancing the load B1\n");
      discharge_Bat1();
      ADC_measure();
      Serial.print(volt_convertion(ADC_Bat1_Meas));Serial.print(",");
      Serial.print(volt_convertion(ADC_Bat2_Meas));Serial.print(",");
      Serial.print(volt_convertion(ADC_Bat3_Meas));Serial.print("\n");
    }

    while (ADC_Bat1_Meas > ADC_Bat3_Meas)
    {
      Serial.print("Balancing the load B1\n");
      discharge_Bat1();
      ADC_measure();
      Serial.print(volt_convertion(ADC_Bat1_Meas));Serial.print(",");
      Serial.print(volt_convertion(ADC_Bat2_Meas));Serial.print(",");
      Serial.print(volt_convertion(ADC_Bat3_Meas));Serial.print("\n");
    }

    while (ADC_Bat2_Meas > ADC_Bat1_Meas)
    {
      Serial.print("Balancing the load B2\n");
      discharge_Bat2();
      ADC_measure();
      Serial.print(volt_convertion(ADC_Bat1_Meas));Serial.print(",");
      Serial.print(volt_convertion(ADC_Bat2_Meas));Serial.print(",");
      Serial.print(volt_convertion(ADC_Bat3_Meas));Serial.print("\n");
    }

    while (ADC_Bat2_Meas > ADC_Bat3_Meas)
    {
      Serial.print("Balancing the load B2\n");
      discharge_Bat2();
      ADC_measure();
      Serial.print(volt_convertion(ADC_Bat1_Meas));Serial.print(",");
      Serial.print(volt_convertion(ADC_Bat2_Meas));Serial.print(",");
      Serial.print(volt_convertion(ADC_Bat3_Meas));Serial.print("\n");
    }

    while (ADC_Bat3_Meas > ADC_Bat1_Meas)
    {
      Serial.print("Balancing the load B3\n");
      discharge_Bat3();
      ADC_measure();
      Serial.print(volt_convertion(ADC_Bat1_Meas));Serial.print(",");
      Serial.print(volt_convertion(ADC_Bat2_Meas));Serial.print(",");
      Serial.print(volt_convertion(ADC_Bat3_Meas));Serial.print("\n");
    }

    while (ADC_Bat3_Meas > ADC_Bat2_Meas)
    {
      Serial.print("Balancing the load B3\n");
      discharge_Bat3();
      ADC_measure();
      Serial.print(volt_convertion(ADC_Bat1_Meas));Serial.print(",");
      Serial.print(volt_convertion(ADC_Bat2_Meas));Serial.print(",");
      Serial.print(volt_convertion(ADC_Bat3_Meas));Serial.print("\n");
    }
  }
}
