#ifndef SW_BATER-IA_H_
#define SW_BATER-IA_H_
/************************************************************
 *                        Macro defines                     *
 ************************************************************/
/**I2C Address:*/
#define I2C_ADDRESS 0x40
/**GPIO to Relay control pins:*/
#define K1 22
#define K2 23
#define K3 24
#define K4 25
#define K5 26
#define K6 27
/**GPIO to Relays of linear actuator system control pins:*/
#define K7 28
#define K8 29
#define K9 30
/************************************************************
 *                        Constructors                      *
 ************************************************************/
INA226_V1 ina226 = INA226_V1(I2C_ADDRESS);
/************************************************************
 *                    Function Prototypes                   *
 ************************************************************/
void MCU_init(void);
void serial_init(void);
void GPIO_init(void);
void I2C_init(void);
void checkForI2cErrors(void);
int Charger(void);
int ADC_measure(void);
void discharge_Bat1(void);
void discharge_Bat2(void);
void discharge_Bat3(void);
void Battery_reinit(void);
void Data_generator(void);
int Load_balancer(void);
/************************************************************
 *                       Global Variables                   *
 ************************************************************/
extern unsigned int Duty = 5;
extern bool enable1 = true;
extern int num_sample = 0;
volatile float ADC_Bat1_Meas = 0;
volatile float ADC_Bat2_Meas = 0;
volatile float ADC_Bat3_Meas = 0;
volatile int Battery_V_Status = 0; /** 0 = if the Battery is not being measured. S*/
volatile int Chargin_Status = 0; /**0 = if the Battery array is not charging. */
volatile float loadVoltage_V = 0.0;
volatile float busVoltage_V = 0.0;
volatile float current_mA = 0.0;
volatile float power_mW = 0.0; 
volatile float shuntVoltage_mV = 0.0;
volatile float menu = 0;
/************************************************************
 ************************************************************/
#endif