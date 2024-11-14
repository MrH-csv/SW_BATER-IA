#ifndef PWM_CTRL_H_
#define PWM_CTRL_H_
/************************************************************
 *                        Macro defines                     *
 ************************************************************/
/**PWM Voltage control pins:*/
#define BMS_PWM_1  2
#define BMS_PWM_2  3
#define BMS_PWM_3  4
/**PWM Voltage control pins:*/
#define SERVO_1 5
#define SERVO_2 6
#define SERVO_3 7
#define SERVO_4 8
#define SERVO_5 9
#define SERVO_6 10
/**GENERAL PURPOSE PWM PIN */
#define GPPP_1 11
#define GPPP_2 12
#define GPPP_3 13
/************************************************************
 *                    Function Prototypes                   *
 ************************************************************/
void PWM_init(void);
void Voltage_adj(int);
/************************************************************
 ************************************************************/
#endif