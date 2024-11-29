/************************************************************************************************************************
 *                                                            Headers:                                                  *
 ************************************************************************************************************************/
#include "Arduino.h"
#include "sam.h" 
#include "GPIOS_Config.h"

/************************************************************************************************************************
 *                                                            Macros:                                                   *
 ************************************************************************************************************************/

/************************************************************************************************************************
 *                                                        Global Variables                                              *
 ************************************************************************************************************************/
volatile uint32_t pila_1_en_pos = 0;
volatile uint32_t pila_2_en_pos = 0;
volatile uint32_t pila_3_en_pos = 0;

/************************************************************************************************************************
 *                                                     Functions Prototypes:                                            *
 ************************************************************************************************************************/
void End_switch_evaluation(void)
{
     // D43
    if (digitalRead(43) == HIGH) {
        // Serial.println("Pila 1 activado");
        // Serial.println(digitalRead(43));
        pila_1_en_pos = 1;
    } else {
        // Serial.println("Pila 1 desactivado");
        // Serial.println(digitalRead(43));
        pila_1_en_pos = 0;
    }
    // D45
    if (digitalRead(45) == HIGH) {
        // Serial.println("Pila 2 activado");
        // Serial.println(digitalRead(45));
        pila_2_en_pos = 1;
    } else {
        // Serial.println("Pila 2 desactivado");
        // Serial.println(digitalRead(45));
        pila_2_en_pos = 0;
    }
    // D47
    if (digitalRead(47) == HIGH) {
        // Serial.println("Interruptor Pila 3 activado");
        pila_3_en_pos = 1;
    } else {
        // Serial.println("Interruptor Pila 3 desactivado");
        pila_3_en_pos = 0;
    }
}

void GPIOS_init (void)
{
    /*RELAYS PINS CONFIGURATION:*/

    /*RELAY K1:*/
    /** D22 -> PB26 **/
    PIOB->PIO_PER |= (1 << 26);  /** Habilitar GPIO **/
    PIOB->PIO_OER |= (1 << 26);  /** Configurar como salida **/
    PIOB->PIO_CODR |= (1 << 26); /** Inicializar en LOW **/

    /*RELAY K2:*/
    /** D23 -> PA14 **/
    PIOA->PIO_PER |= (1 << 14);  /** Habilitar GPIO **/
    PIOA->PIO_OER |= (1 << 14);  /** Configurar como salida **/
    PIOA->PIO_CODR |= (1 << 14); /** Inicializar en LOW **/

    /*RELAY K3:*/
    /** D24 -> PA15 **/
    PIOA->PIO_PER |= (1 << 15);  /** Habilitar GPIO **/
    PIOA->PIO_OER |= (1 << 15);  /** Configurar como salida **/
    PIOA->PIO_CODR |= (1 << 15); /** Inicializar en LOW **/

    /*RELAY K4:*/
    /** D25 -> PD0 **/
    PIOD->PIO_PER |= (1 << 0);   /** Habilitar GPIO **/
    PIOD->PIO_OER |= (1 << 0);   /** Configurar como salida **/
    PIOD->PIO_CODR |= (1 << 0);  /** Inicializar en LOW **/

    /*RELAY K5:*/
    /** D26 -> PD1 **/
    PIOD->PIO_PER |= (1 << 1);   /** Habilitar GPIO **/
    PIOD->PIO_OER |= (1 << 1);   /** Configurar como salida **/
    PIOD->PIO_CODR |= (1 << 1);  /** Inicializar en LOW **/

    /*RELAY K6:*/
    /** D27 -> PD2 **/
    PIOD->PIO_PER |= (1 << 2);   /** Habilitar GPIO **/
    PIOD->PIO_OER |= (1 << 2);   /** Configurar como salida **/
    PIOD->PIO_CODR |= (1 << 2);  /** Inicializar en LOW **/


    /*LINEAR ACTUATORS CONFIG:*/

    /** D28 -> PD3 **/
    PIOD->PIO_PER |= (1 << 3);   /** Habilitar GPIO **/
    PIOD->PIO_OER |= (1 << 3);   /** Configurar como salida **/
    PIOD->PIO_CODR |= (1 << 3);  /** Inicializar en LOW **/

    /** D29 -> PD6 **/
    PIOD->PIO_PER |= (1 << 6);   /** Habilitar GPIO **/
    PIOD->PIO_OER |= (1 << 6);   /** Configurar como salida **/
    PIOD->PIO_CODR |= (1 << 6);  /** Inicializar en LOW **/

    /** D30 -> PD9 **/
    PIOD->PIO_PER |= (1 << 9);   /** Habilitar GPIO **/
    PIOD->PIO_OER |= (1 << 9);   /** Configurar como salida **/
    PIOD->PIO_CODR |= (1 << 9);  /** Inicializar en LOW **/

    /*LIMIT SWITCH CONFIGURATION:*/

    /*INPUTS:*/

    pinMode(43, INPUT);
    pinMode(45, INPUT);
    pinMode(47, INPUT);

/*
    // Configurar D43 (PA20) como entrada con pull-up
    D43_PORT->PIO_PER |= (1 << D43_PIN);  // Habilitar control GPIO
    D43_PORT->PIO_ODR |= (1 << D43_PIN);  // Configurar como entrada
    D43_PORT->PIO_PUER |= (1 << D43_PIN); // Habilitar pull-up

    // Configurar D45 (PC18) como entrada con pull-up
    D45_PORT->PIO_PER |= (1 << D45_PIN);  // Habilitar control GPIO
    D45_PORT->PIO_ODR |= (1 << D45_PIN);  // Configurar como entrada
    D45_PORT->PIO_PUER |= (1 << D45_PIN); // Habilitar pull-up

    // Configurar D47 (PC16) como entrada con pull-up
    D47_PORT->PIO_PER |= (1 << D47_PIN);  // Habilitar control GPIO
    D47_PORT->PIO_ODR |= (1 << D47_PIN);  // Configurar como entrada
    D47_PORT->PIO_PUER |= (1 << D47_PIN); // Habilitar pull-up
*/

/*
    /*OUTPUTS (LOW ALWAYS)
    // Configurar D44 (PC19) como GPIO y en LOW
    PIOC->PIO_PER |= (1 << 19);  // Habilitar control GPIO
    PIOC->PIO_OER |= (1 << 19);  // Configurar como salida
    PIOC->PIO_CODR |= (1 << 19); // Establecer en LOW

    // Configurar D46 (PC17) como GPIO y en LOW
    PIOC->PIO_PER |= (1 << 17);  // Habilitar control GPIO
    PIOC->PIO_OER |= (1 << 17);  // Configurar como salida
    PIOC->PIO_CODR |= (1 << 17); // Establecer en LOW

    // Configurar D48 (PC15) como GPIO y en LOW
    PIOC->PIO_PER |= (1 << 15);  // Habilitar control GPIO
    PIOC->PIO_OER |= (1 << 15);  // Configurar como salida
    PIOC->PIO_CODR |= (1 << 15); // Establecer en LOW

    */

}

/*BUFFERS ENABLES & DISABLES: */
void Config_U1_EN(void)
{
    /** D49 -> PC14 **/
    PIOC->PIO_PER |= (1 << 14);  /** Habilitar GPIO **/
    PIOC->PIO_OER |= (1 << 14);  /** Configurar como salida **/
    PIOC->PIO_CODR |= (1 << 14); /** Activar en LOW **/
}

void Config_U1_DIS(void)
{
    /** D49 -> PC14 **/
    PIOC->PIO_PER |= (1 << 14);  /** Habilitar GPIO **/
    PIOC->PIO_OER |= (1 << 14);  /** Configurar como salida **/
    PIOC->PIO_SODR |= (1 << 14); /** Desactivar en HIGH **/
}

void Config_U2_EN(void)
{
    /** D50 -> PC13 **/
    PIOC->PIO_PER |= (1 << 13);  /** Habilitar GPIO **/
    PIOC->PIO_OER |= (1 << 13);  /** Configurar como salida **/
    PIOC->PIO_CODR |= (1 << 13); /** Activar en LOW **/
}

void Config_U2_DIS(void)
{
    /** D50 -> PC13 **/
    PIOC->PIO_PER |= (1 << 13);  /** Habilitar GPIO **/
    PIOC->PIO_OER |= (1 << 13);  /** Configurar como salida **/
    PIOC->PIO_SODR |= (1 << 13); /** Desactivar en HIGH **/
}

void Config_U3_EN(void)
{
    /** D51 -> PC12 **/
    PIOC->PIO_PER |= (1 << 12);  /** Habilitar GPIO **/
    PIOC->PIO_OER |= (1 << 12);  /** Configurar como salida **/
    PIOC->PIO_CODR |= (1 << 12); /** Activar en LOW **/
}

void Config_U3_DIS(void)
{
    /** D50 -> PC13 **/
    PIOC->PIO_PER |= (1 << 13);  /** Habilitar GPIO **/
    PIOC->PIO_OER |= (1 << 13);  /** Configurar como salida **/
    PIOC->PIO_SODR |= (1 << 12); /** Desactivar en HIGH **/
}

void Config_U4_EN(void)
{
    /** D52 -> PB21 **/
    PIOB->PIO_PER |= (1 << 21);  /** Habilitar GPIO **/
    PIOB->PIO_OER |= (1 << 21);  /** Configurar como salida **/
    PIOB->PIO_CODR |= (1 << 21); /** Activar en LOW **/
}

void Config_U4_DIS(void)
{
    /** D52 -> PB21 **/
    PIOB->PIO_PER |= (1 << 21);  /** Habilitar GPIO **/
    PIOB->PIO_OER |= (1 << 21);  /** Configurar como salida **/
    PIOB->PIO_SODR |= (1 << 21); /** Desactivar en HIGH **/
}

void Config_U5_EN(void)
{
    /** D53 -> PB14 **/
    PIOB->PIO_PER |= (1 << 14);  /** Habilitar GPIO **/
    PIOB->PIO_OER |= (1 << 14);  /** Configurar como salida **/
    PIOB->PIO_CODR |= (1 << 14); /** Activar en LOW **/
}

void Config_U5_DIS(void)
{
    /** D52 -> PB21 **/
    PIOB->PIO_PER |= (1 << 14);  /** Habilitar GPIO **/
    PIOB->PIO_OER |= (1 << 14);  /** Configurar como salida **/
    PIOB->PIO_SODR |= (1 << 14); /** Desactivar en HIGH **/
}
