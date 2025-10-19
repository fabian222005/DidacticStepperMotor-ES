# DidacticStepperMotor-ES
Interfaz Didáctica para el manejo de motor Paso a Paso
Objetivo
Demostrar los principios de control de un motor paso a paso mediante el control tiempo de los pulsos de cada bobina mediante un manejo unipolar del motor.
Material y Circuito
•	Placa PCB para prototipado
•	Motor paso a paso 28BYJ-48
•	Shield con driver unipolar ULN2003
•	Encoder rotativo con pulsador
•	Pantalla LCD 16x2
•	Interfaz I2C para pantalla LCD PCF8574
•	Placa de desarrollo UNO (ATmega328p)
•	Conector DC Jack
•	Resistencia de 10kΩ (acoplamiento del botón)
•	Resistencia de 1kΩ (acoplamiento del botón)
•	Capacitor C104 (acoplamiento del botón)
<img width="438" height="469" alt="imagen" src="https://github.com/user-attachments/assets/98afb802-dfcf-4c42-8f6d-9c5e8143bb2c" />

Pines de Conexión
Elemento	Pines de Arduino
Pantalla I2C. [SDA,SCL,+5V, GND]	[SDA,SCL,+5V, GND]
Encoder [A,B,BTN]	[2,3,6y7]
Shield Unipolar [A,B,C,D]	[8,9,10,11]

El código del programa se divide en 3 archivos (main.ino, GUI.h y motor.h) además de algunas librerías auxiliares. Para el control del motor se usa el registro PORTB y DDRB, para el botón se tiene una interrupción por hardware, al igual que para el encoder.
El encoder sirve para navegar la interface desplegada en la pantalla I2C.
La interfaz cuenta con las opciones:
1.	Desplazamiento Relativo: incrementa los grados indicados por el usuario.
2.	Desplazamiento Absoluto: establece la posición a los grados indicados por el usuario.
3.	Ir a casa: regresa a los 0º.
4.	Giro CW (clockwise): gira 360º en sentido de las manecillas del reloj.
5.	Giro CCW (counterclockwise): gira 360º en antisentido de las manecillas del reloj.
Las opciones 1 y 2 tienen una interfaz para seleccionar los grados y la velocidad.

