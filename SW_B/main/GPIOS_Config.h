/************************************************************************************************************************
 *                                                            Headers:                                                  *
 ************************************************************************************************************************/
#ifndef GPIOS_CONFIG_H_
#define GPIOS_CONFIG_H_

/************************************************************************************************************************
 *                                                            Macros:                                                   *
 ************************************************************************************************************************/

/************************************************************************************************************************
 *                                                        Global Variables                                              *
 ************************************************************************************************************************/

extern volatile uint32_t pila_1_en_pos;
extern volatile uint32_t pila_2_en_pos;
extern volatile uint32_t pila_3_en_pos;

/************************************************************************************************************************
 *                                                     Functions Prototypes:                                            *
 ************************************************************************************************************************/
void End_switch_evaluation(void);
void GPIOS_init (void);

void Config_U1_EN(void);    // Configura el pin D49 (PC14) en LOW
void Config_U1_DIS(void);   // Configura el pin D49 (PC14) en HIGH

void Config_U2_EN(void);    // Configura el pin D50 (PC13) en LOW
void Config_U2_DIS(void);   // Configura el pin D50 (PC13) en HIGH

void Config_U3_EN(void);    // Configura el pin D51 (PC12) en LOW
void Config_U3_DIS(void);   // Configura el pin D51 (PC12) en HIGH

void Config_U4_EN(void);    // Configura el pin D52 (PB21) en LOW
void Config_U4_DIS(void);   // Configura el pin D52 (PB21) en HIGH

void Config_U5_EN(void);    // Configura el pin D53 (PB14) en LOW
void Config_U5_DIS(void);   // Configura el pin D53 (PB14) en HIGH



#endif