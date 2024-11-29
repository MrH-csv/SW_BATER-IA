#include "Arduino.h"
#include "sam.h" 
#include "Stepper_Motor.h"

/************************************************************************************************************************
 *                                                        Global Variables                                              *
 ************************************************************************************************************************/

// Pines para el control de los motores a pasos
const int stepPinsMotor1[4] = {31, 32, 33, 34}; // Motor 1
const int stepPinsMotor2[4] = {35, 36, 37, 38}; // Motor 2
const int stepPinsMotor3[4] = {39, 40, 41, 42}; // Motor 3

// Variables para el control de los motores
int pasoActualMotor1 = 0; // Paso actual para Motor 1
int pasoActualMotor2 = 0; // Paso actual para Motor 2
int pasoActualMotor3 = 0; // Paso actual para Motor 3

const int pasosPorRevolucion = 200; // Número de pasos por revolución

// Secuencia para control de pasos (onda completa)
const int secuencia[4][4] = {
    {1, 0, 0, 1}, // Paso 1
    {1, 0, 1, 0}, // Paso 2
    {0, 1, 1, 0}, // Paso 3
    {0, 1, 0, 1}  // Paso 4
};

/************************************************************************************************************************
 *                                                       Function deployment:                                           *
 ************************************************************************************************************************/
// Configurar los pines de un motor
void configurarPines(const int stepPins[4]) 
{
    for (int i = 0; i < 4; i++) 
    {
        pinMode(stepPins[i], OUTPUT);
        digitalWrite(stepPins[i], LOW); // Inicializar en LOW
    }
}

// Aplicar un paso a un motor específico
void aplicarPaso(const int stepPins[4], int paso) 
{
    for (int i = 0; i < 4; i++) 
    {
        digitalWrite(stepPins[i], secuencia[paso][i]);
    }
}

// Función para realizar un paso adelante en un motor específico
void pasoAdelante(const int stepPins[4], int &pasoActual) 
{
    pasoActual = (pasoActual + 1) % 4; // Incrementar el paso y asegurar rango [0,3]
    aplicarPaso(stepPins, pasoActual);
}
/*
// Función para realizar un paso atrás en un motor específico
void pasoAtras(const int stepPins[4], int &pasoActual) 
{
    pasoActual = (pasoActual - 1 + 4) % 4; // Decrementar el paso y asegurar rango [0,3]
    aplicarPaso(stepPins, pasoActual);
}
*/
void all_step_motors_init(void)
{
    // Configurar los pines de cada motor
    configurarPines(stepPinsMotor1);
    configurarPines(stepPinsMotor2);
    configurarPines(stepPinsMotor3);
}

void rotar_step_motor_1_adelante(void)
{
    for (int i = 0; i < pasosPorRevolucion; i++) 
    {
        pasoAdelante(stepPinsMotor1, pasoActualMotor1); // Motor 1
        delay(10); // Ajustar velocidad del motor (10 ms entre pasos)
    }
}

void rotar_step_motor_2_adelante(void)
{
    for (int i = 0; i < pasosPorRevolucion; i++) 
    {
        pasoAdelante(stepPinsMotor2, pasoActualMotor2); // Motor 2
        delay(10); // Ajustar velocidad del motor (10 ms entre pasos)
    }
}

void rotar_step_motor_3_adelante(void)
{
    for (int i = 0; i < pasosPorRevolucion; i++) 
    {
        pasoAdelante(stepPinsMotor3, pasoActualMotor3); // Motor 3
        delay(10); // Ajustar velocidad del motor (10 ms entre pasos)
    }
}
/*
void rotar_step_motor_1_atras(void)
{
    for (int i = 0; i < pasosPorRevolucion; i++) 
    {
        pasoAtras(stepPinsMotor1, pasoActualMotor1); // Motor 1
        delay(10); // Ajustar velocidad del motor (10 ms entre pasos)

    }
}

void rotar_step_motor_2_atras(void)
{
    for (int i = 0; i < pasosPorRevolucion; i++) 
    {
        pasoAtras(stepPinsMotor2, pasoActualMotor2); // Motor 2
        delay(10); // Ajustar velocidad del motor (10 ms entre pasos)
    }
}

void rotar_step_motor_3_atras(void)
{
    for (int i = 0; i < pasosPorRevolucion; i++) 
    {
        pasoAtras(stepPinsMotor3, pasoActualMotor3); // Motor 3
        delay(10); // Ajustar velocidad del motor (10 ms entre pasos)
    }
}
*/
