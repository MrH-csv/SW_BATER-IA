/************************************************************************************************************************
 *                                             Control software to BATER-IA Proyect.                                    *
 * **********************************************************************************************************************
 *                                                               By:                                                    *
 *                                                    Angel Habid Navarro Mendez.                                       *
 *                                                                                                                      *
 *                                                            Degree:                                                   *
 *                                      Electronic Design and Intelligent Systems Engineering.                          *
 *                                                                                                                      *
 *                                                          CETI Colomos                                                *
 *                                                                                                                      *
 *                                                           29/11/2024                                                 *
 ************************************************************************************************************************/

/************************************************************************************************************************
 *                                                            Headers:                                                  *
 ************************************************************************************************************************/
#include "Arduino.h"
#include "sam.h"
#include "Stepper_Motor.h"
#include "GPIOS_Config.h"
#include "ADC_CFG_V1.h"

/************************************************************************************************************************
 *                                                            Macros:                                                   *
 ************************************************************************************************************************/
#define Voltaje_inicial 1.4//0.9 /** hard codeado para demostracion*/
#define Voltaje_inicial_min 0.2
/************************************************************************************************************************
 *                                                        Global Variables                                              *
 ************************************************************************************************************************/
volatile float adc_value_ch0;
volatile float voltage_ch0;

volatile float adc_value_ch1;
volatile float voltage_ch1;

volatile float adc_value_ch2;
volatile float voltage_ch2;

volatile float adc_value_ch3;
volatile float voltage_ch3;

volatile float adc_value_ch4;
volatile float voltage_ch4;

volatile float adc_value_ch5;
volatile float voltage_ch5;

volatile float adc_value_ch6;
volatile float voltage_ch6;

bool Cell_full_done;
bool exit_1;


/************************************************************************************************************************
 *                                                     Functions Prototypes:                                            *
 ************************************************************************************************************************/
void pilas_incoming(void);
void medirDatos(void);
void menu (void);
void ejecutarCodigo();

/************************************************************************************************************************
 * This section, it contains the basic structure needed to use the Arduino IDE, which is necessary ...                  *
 * due to the limitations of the UDOO Quad development board.   =  int main(void)                                       *
 ************************************************************************************************************************/

void setup() 
{
  Serial.begin(9600);/**Start Serial COM */
  while (!Serial);     // Espera a que el monitor serial esté listo (opcional)
  Serial.println("Arduino listo. Esperando señal...");

  /*Variables initialization*/
  adc_value_ch0 = 0;
  voltage_ch0 = 0;
  adc_value_ch1 = 0;
  voltage_ch1 = 0;
  adc_value_ch2 = 0;
  voltage_ch2 = 0;
  adc_value_ch3 = 0;
  voltage_ch3 = 0;
  adc_value_ch4 = 0;
  voltage_ch4 = 0;
  adc_value_ch5 = 0;
  voltage_ch5 = 0;
  adc_value_ch6 = 0;
  voltage_ch6 = 0;

  

  Cell_full_done = false;
  exit_1 = false;

  /*Configuration functions*/
  Config_U1_EN();/**Buffer enables init */
  Config_U2_EN();
  Config_U3_EN();
  Config_U4_EN();
  Config_U5_EN();
  GPIOS_init();/** GPIOs initialization */
  all_step_motors_init();/** Step motors init */
  ADC_init(); /** ADCs initialization */
}

void loop() 
{ 
  if (Serial.available() > 0) 
  {
    int receivedValue = Serial.parseInt();  // Lee el número enviado
    Serial.print("Recibido: ");
    Serial.println(receivedValue);

    if (receivedValue == 99) 
    {
      Serial.println("¡Señal recibida! Iniciando ejecución...");
      ejecutarCodigo();
    }
  }    
}

/************************************************************************************************************************
 *                                                       Function deployment:                                           *
 ************************************************************************************************************************/

void ejecutarCodigo() {
  Serial.println("Ejecutando código...");
  while (!exit_1)
  {
    if (!Cell_full_done) 
    {
      pilas_incoming();
      medirDatos();
    }
    else
    {
      menu();
    }
  }
  Serial.println("Ejecución completada.");
  exit_1 = false;
}

void pilas_incoming(void)
{
    End_switch_evaluation();
    if (pila_1_en_pos == 0) /** no  se detecto pila en el interruptor de fin de carrera */
    {
        PIOD->PIO_SODR |= (1 << 3);  /** Abrir  Linear Actuator 1 -- Establecer en HIGH D28 , Linear Actuator 1 active = open battery port. */ 
        rotar_step_motor_1_adelante(); /** gira el motor */
    }
    else
    {
        PIOB->PIO_SODR = (1 << 26);  /** Encender PB26 -> PIN22  = ACTIVE K1 (to connect ADC CH0 to "pila 1"). */
        PIOD->PIO_CODR |= (1 << 3);  /** CERRAR  Linear Actuator 1 --  PD28 to LOW  = close battery port (connect "pila 1"). */   
        delay(50);
        adc_value_ch0 = ADC_reading(ADC_CH0); /** Leer el valor ADC del canal 0 */ 
        voltage_ch0 = volt_conversion(adc_value_ch0);/** convertir el valor a volts */

        if (voltage_ch0 >= Voltaje_inicial)
        {
            /* Discharge the cell to get the standart initial voltage level */
            /*ACTIVA K4*/
            PIOD->PIO_SODR = (1 << 0);   /** Encender PD0 -> PIN25 = connect the load resistor to "pila 1".  **/
        }
    }

    if (pila_2_en_pos == 0)
    {
        PIOD->PIO_SODR |= (1 << 6);  /** Abrir  Linear Actuator 2 -- Establecer en HIGH D29 , Linear Actuator 2 active = open battery port. */ 
        rotar_step_motor_2_adelante();/**Gira el motor */
    }
    else
    {
        PIOA->PIO_SODR = (1 << 14);  /** Encender PA14 -> PIN23  = ACTIVE K2 (to connect ADC CH1 to "pila 2"). */
        PIOD->PIO_CODR |= (1 << 6);  /** CERRAR  Linear Actuator 2 -- PD29 to LOW  = close battery port (connect "pila 2"). */ 
        delay(20);
        adc_value_ch1 = ADC_reading(ADC_CH1); /** Leer el valor ADC del canal 1 */ 
        voltage_ch1 = volt_conversion(adc_value_ch1);/** convertir el valor a volts */
        delay(20);

        if (voltage_ch1 >= Voltaje_inicial)
        {
            /* Discharge the cell to get the standart initial voltage level */
            PIOD->PIO_SODR = (1 << 1);   /** Encender PD1 -> PIN26  = connect the load resistor to "pila 2".  **/
        }   
    }

    if (pila_3_en_pos == 0)
    {
        PIOD->PIO_SODR |= (1 << 9);  /** Establecer en HIGH D30 , Linear Actuator 3 active = open battery port. */ 
        rotar_step_motor_3_adelante();
    }
    else
    {
        PIOA->PIO_SODR = (1 << 15);  /** Encender PA15 -> PIN24  = ACTIVE K3 (to connect ADC CH2 to "pila 3"). */
        PIOD->PIO_CODR |= (1 << 9);  /** PD30 to LOW  = close battery port (connect "pila 3"). */  
        delay(20);
        adc_value_ch2 = ADC_reading(ADC_CH2); /** Leer el valor ADC del canal 1 */ 
        voltage_ch2 = volt_conversion(adc_value_ch2);/** convertir el valor a volts */
        delay(20);

        if (voltage_ch2 >= Voltaje_inicial)
        {
            /* Discharge the cell to get the standart initial voltage level */  
            PIOD->PIO_CODR = (1 << 2);/** Apagar PD2 -> PIN27 = connect the load resistor to "pila 3".  **/
        }
    }
}


/************************************************************
 *                        Función de medición               *
 ************************************************************/
void medirDatos(void) 
{
    bool print_once = false;
    bool print_once_1 = false;

    if ((pila_3_en_pos == 1 ) && (pila_2_en_pos == 1 )&& (pila_1_en_pos == 1))
    {
        // Variables de medición
        float initial_voltage[3] = {0, 0, 0};
        float max_voltage[3] = {0, 0, 0};
        float area_curve[3] = {0, 0, 0};
        unsigned long time_to_1_4v[3] = {0, 0, 0};
        float slope_initial[3] = {0, 0, 0};
        bool initial_measurement_done = false;

        // Mide los voltajes actuales en las pilas:

        adc_value_ch0 = ADC_reading(ADC_CH0); /** Leer el valor ADC del canal 0 */ 
        voltage_ch0 = volt_conversion(adc_value_ch0);/** convertir el valor a volts */
        delay(50);
        adc_value_ch1 = ADC_reading(ADC_CH1); /** Leer el valor ADC del canal 1 */ 
        voltage_ch1 = volt_conversion(adc_value_ch1);/** convertir el valor a volts */
        delay(50);
        adc_value_ch2 = ADC_reading(ADC_CH2); /** Leer el valor ADC del canal 1 */ 
        voltage_ch2 = volt_conversion(adc_value_ch2);/** convertir el valor a volts */
        delay(50);
        /**
         * Descomentar si se desea visualizar la descarga por serial:
         */
        /*
            if (!print_once) 
            {
                Serial.println("DESCARGA:");
                Serial.print("Tiempo (ms):");
                Serial.print(", Pila 1(V):");
                Serial.print(", Pila 2:(V)");
                Serial.println(", Pila 3(V):");
                print_once = true;
            }
            
            Serial.print((millis()));
            Serial.print(", ");
            Serial.print(voltage_ch0);
            Serial.print(", ");
            Serial.print(voltage_ch1);
            Serial.print(", ");
            Serial.println(voltage_ch2);
        */
        if ((voltage_ch0 > Voltaje_inicial_min) && (voltage_ch0 < 0.9))
        {
            PIOD->PIO_CODR = (1 << 0);   /** Apagar PD0 -> PIN25 = disconnect the load resistor to "pila 1".  **/
        }

        if ((voltage_ch1 > Voltaje_inicial_min) && (voltage_ch1 < 0.9))
        {
            PIOD->PIO_CODR = (1 << 1);/** Apagar PD1 -> PIN26  = disconnect the load resistor to "pila 2".  **/
        }

        if ((voltage_ch2 > Voltaje_inicial_min) && (voltage_ch2 < 0.9))
        {
            PIOD->PIO_SODR = (1 << 2);   /** Encender PD2 -> PIN27 = disconnect the load resistor to "pila 3".  **/
        }


        if (((voltage_ch0 > Voltaje_inicial_min) && (voltage_ch0 < Voltaje_inicial))&&((voltage_ch1 > Voltaje_inicial_min) && (voltage_ch1 < Voltaje_inicial))&&((voltage_ch2 > Voltaje_inicial_min) && (voltage_ch2 < Voltaje_inicial)))
        {
            // Variables de tiempo
            const unsigned long total_duration =( millis() + 60000);// 1 minutos en milisegundos. //300000; // 5 minutos en milisegundos. 
            unsigned long start_time = millis(); // Inicio total de los 5 minutos

            // Bucle principal para 5 minutos , idealmente al menos 30 minutos
            while (millis() < total_duration) 
            {
                
                /*******************************************************************/
                /*Configuracion de los relays para iniciar la carga antes de medir*/
                /*******************************************************************/
                
                /*CONECTA AL BMS LAS PILAS*/
                PIOB->PIO_CODR = (1 << 26);  /** Apagar PB26 -> PIN22 **/
                PIOA->PIO_CODR = (1 << 14);  /** Apagar PA14 -> PIN23 **/
                PIOA->PIO_CODR = (1 << 15);  /** Apagar PA15 -> PIN24 **/

                // Leer voltajes de los tres canales
                float voltaje1 = volt_conversion(ADC_reading(ADC_CH3));
                float voltaje2 = volt_conversion(ADC_reading(ADC_CH4));
                float voltaje3 = volt_conversion(ADC_reading(ADC_CH5));


                if (!print_once_1) 
                {
                    Serial.println("CARGA:");
                    Serial.print("Tiempo (S):");
                    Serial.print(", Pila 1(V):");
                    Serial.print(", Pila 2:(V)");
                    Serial.println(", Pila 3(V):");
                    print_once_1 = true;
                }

                Serial.print((millis() - start_time)/1000);/** Tiempo en segundos */
                Serial.print(", ");
                Serial.print(voltage_ch0);
                Serial.print(", ");
                Serial.print(voltage_ch1);
                Serial.print(", ");
                Serial.println(voltage_ch2);
                delay(10000);
            }
            Cell_full_done = true;
        }   
    }   
}

void menu (void)

{
  if (Serial.available() > 0) 
  {
    int value = Serial.parseInt();  // Lee el número enviado
    Serial.print("Recibido: ");
    Serial.println(value);

    // Extrae el número de la pila y el tipo
    int pila_numero = value / 10;       // Dígito de las decenas (1, 2, 3 para Pila 1, 2, 3)
    int tipo_pila = value % 10;         // Dígito de las unidades (1, 2, 3, 4 para tipos de pila)

    // Acciones específicas según la pila y tipo
    switch (pila_numero) {
      case 1:
        Serial.print("Pila 1 - ");
        switch (tipo_pila) {
      case 1:
        Serial.println("Carbon-Zinc (Zn-MnO2)");
        break;
      case 2:
        Serial.println("Alkaline (MnO2)");
        break;
      case 3:
        Serial.println("Nickel-Cadmium (Ni-Cd)");
        break;
      case 4:
        Serial.println("Nickel-Metal Hydride (Ni-MH)");
        break;
      default:
        Serial.println("Tipo de pila desconocido.");
    }
        break;
      case 2:
      switch (tipo_pila) {
      case 1:
        Serial.println("Carbon-Zinc (Zn-MnO2)");
        break;
      case 2:
        Serial.println("Alkaline (MnO2)");
        break;
      case 3:
        Serial.println("Nickel-Cadmium (Ni-Cd)");
        break;
      case 4:
        Serial.println("Nickel-Metal Hydride (Ni-MH)");
        break;
      default:
        Serial.println("Tipo de pila desconocido.");
    }
        Serial.print("Pila 2 - ");
        break;
      case 3:
      switch (tipo_pila) {
      case 1:
        Serial.println("Carbon-Zinc (Zn-MnO2)");
        break;
      case 2:
        Serial.println("Alkaline (MnO2)");
        break;
      case 3:
        Serial.println("Nickel-Cadmium (Ni-Cd)");
        break;
      case 4:
        Serial.println("Nickel-Metal Hydride (Ni-MH)");
        break;
      default:
        Serial.println("Tipo de pila desconocido.");
    }
        Serial.print("Pila 3 - ");
        break;
      default:
        Serial.println("Pila desconocida.");
        return;
    }

    exit_1 = true;
  }
}
