#ifndef ADC_CFG_V1_H_
#define ADC_CFG_V1_H_
/************************************************************
 *                        Macro defines                     *
 ************************************************************/
/**Relation between ADC channels and Analog pins in the board:*/
#define ADC_CH0 7
#define ADC_CH1 6
#define ADC_CH2 5
#define ADC_CH3 4
#define ADC_CH4 3
#define ADC_CH5 2
#define ADC_CH6 1
#define ADC_CH7 0

/**ADC resolution:*/
#define Res_ADC 12
/**ADC Vref:*/
#define Vmax_ADC 3.3
/************************************************************
 *                    Function Prototypes                   *
 ************************************************************/
void ADC_init(void);
float ADC_reading(int);
float volt_conversion(volatile float);
/************************************************************
 ************************************************************/
#endif