#include <Servo.h> // Incluir la biblioteca Servo

// Declarar los objetos Servo para cada motor
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
Servo servo6;
Servo servo7;
Servo servo8;
Servo servo9;

// Pines correspondientes a los servos
const int servoPins[9] = {5, 6, 7, 8, 9, 10, 11, 12, 13};

// Función para configurar los pines de enable de los buffers
void configurar_enable_buffers() {
    /** Configurar cada pin de enable por separado **/

    /** D49 -> PC14 **/
    PIOC->PIO_PER |= (1 << 14);  /** Habilitar GPIO **/
    PIOC->PIO_OER |= (1 << 14);  /** Configurar como salida **/
    PIOC->PIO_CODR |= (1 << 14); /** Activar en LOW **/

    /** D50 -> PC13 **/
    PIOC->PIO_PER |= (1 << 13);  /** Habilitar GPIO **/
    PIOC->PIO_OER |= (1 << 13);  /** Configurar como salida **/
    PIOC->PIO_CODR |= (1 << 13); /** Activar en LOW **/

    /** D51 -> PC12 **/
    PIOC->PIO_PER |= (1 << 12);  /** Habilitar GPIO **/
    PIOC->PIO_OER |= (1 << 12);  /** Configurar como salida **/
    PIOC->PIO_CODR |= (1 << 12); /** Activar en LOW **/

    /** D52 -> PB21 **/
    PIOB->PIO_PER |= (1 << 21);  /** Habilitar GPIO **/
    PIOB->PIO_OER |= (1 << 21);  /** Configurar como salida **/
    PIOB->PIO_CODR |= (1 << 21); /** Activar en LOW **/

    /** D53 -> PB14 **/
    PIOB->PIO_PER |= (1 << 14);  /** Habilitar GPIO **/
    PIOB->PIO_OER |= (1 << 14);  /** Configurar como salida **/
    PIOB->PIO_CODR |= (1 << 14); /** Activar en LOW **/
}

void setup() {
    // Configurar los buffers
    PMC->PMC_PCER0 |= (1 << ID_PIOB) | (1 << ID_PIOC); // Habilitar relojes para PIOB y PIOC
    configurar_enable_buffers();

    // Asignar cada servo a su pin correspondiente
    servo1.attach(servoPins[0]);
    servo2.attach(servoPins[1]);
    servo3.attach(servoPins[2]);
    servo4.attach(servoPins[3]);
    servo5.attach(servoPins[4]);
    servo6.attach(servoPins[5]);
    servo7.attach(servoPins[6]);
    servo8.attach(servoPins[7]);
    servo9.attach(servoPins[8]);

    // Llevar todos los servos a la posición inicial
    for (int i = 0; i < 9; i++) {
        Servo &servo = getServo(i);
        servo.write(90); // Posición inicial en el centro (90°)
        delay(500);      // Esperar un poco entre servos
    }
}

void loop() {
    // Mover todos los servos de 0° a 180° y de vuelta
    for (int angle = 0; angle <= 180; angle++) {
        for (int i = 0; i < 9; i++) {
            Servo &servo = getServo(i);
            servo.write(angle);
        }
        delay(20); // Controlar la velocidad del movimiento
    }

    for (int angle = 180; angle >= 0; angle--) {
        for (int i = 0; i < 9; i++) {
            Servo &servo = getServo(i);
            servo.write(angle);
        }
        delay(20); // Controlar la velocidad del movimiento
    }
}

// Función auxiliar para devolver el objeto Servo correspondiente
Servo &getServo(int index) {
    switch (index) {
        case 0: return servo1;
        case 1: return servo2;
        case 2: return servo3;
        case 3: return servo4;
        case 4: return servo5;
        case 5: return servo6;
        case 6: return servo7;
        case 7: return servo8;
        case 8: return servo9;
        default: return servo1; // Caso por defecto
    }
}
