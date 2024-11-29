#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

/************************************************************************************************************************
 *                                                        Global Variables                                              *
 ************************************************************************************************************************/

// Pines para el control de los motores a pasos
extern const int stepPinsMotor1[4];
extern const int stepPinsMotor2[4];
extern const int stepPinsMotor3[4];

// Variables para el control de los motores
extern int pasoActualMotor1; // Paso actual para Motor 1
extern int pasoActualMotor2; // Paso actual para Motor 2
extern int pasoActualMotor3; // Paso actual para Motor 3

extern const int pasosPorRevolucion; // Número de pasos por revolución (depende del motor)

// Secuencia para control de pasos (onda completa)
extern const int secuencia[4][4];

/************************************************************************************************************************
 *                                                     Functions Prototypes:                                            *
 ************************************************************************************************************************/

// Configurar los pines de un motor
void configurarPines(const int stepPins[4]);

// Aplicar un paso a un motor específico
void aplicarPaso(const int stepPins[4], int paso);

// Función para realizar un paso adelante en un motor específico
void pasoAdelante(const int stepPins[4], int &pasoActual);
/*
// Función para realizar un paso atrás en un motor específico
void pasoAtras(const int stepPins[4], int &pasoActual);
*/
// Inicializar todos los motores paso a paso
void all_step_motors_init(void);

// Rotar motor paso a paso 1 hacia adelante
void rotar_step_motor_1_adelante(void);

// Rotar motor paso a paso 2 hacia adelante
void rotar_step_motor_2_adelante(void);

// Rotar motor paso a paso 3 hacia adelante
void rotar_step_motor_3_adelante(void);

/*
// Rotar motor paso a paso 1 hacia atrás
void rotar_step_motor_1_atras(void);

// Rotar motor paso a paso 2 hacia atrás
void rotar_step_motor_2_atras(void);

// Rotar motor paso a paso 3 hacia atrás
void rotar_step_motor_3_atras(void);
*/
#endif
