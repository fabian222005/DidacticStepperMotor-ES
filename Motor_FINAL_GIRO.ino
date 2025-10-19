#include <LiquidCrystal_I2C.h>
#include <SmoothProgress.h>

// SmoothProgress supports progress bars in different visual styles
// Each style is stored in separate .h file that should be included next (you have to use at leas one)
// Let's include the style for horizontal progress bar as a square frame with 1 pixel margin
#include <BarStyle1.h>

LiquidCrystal_I2C lcd(0x27,16,2);

    // The library supports multiple progress bars (up to 4 on a single display), different styles and multiple displays
// The hardware limitation of these displays implies that all progress bars on a single display have to share the same style
// Next create the association between the lcd display object and the progress bar style
// as a bar_display (LCD) object (dispA here) to be further used when creating
// the smooth progress bar object and/or changing the style of the displayed progress bars
LCD dispA(lcd, barStyle1);

// Create the object representing the smooth progress bar to be shown on dispA (lcd)
// defining its size and position on the display
SmoothProgressBar spb(dispA, 10, 5, 0); // progress bar 10 characters wide, at 5-th column, 0-th row

#include "GUI.h"
#include "motor.h"
#include <Encoder.h>
Encoder enc(2,3);


void selVel(){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Velocidad");
    lcd.setCursor(0,1);
    lcd.print(velocity);
    lcd.setCursor(13,1);
    lcd.print("Ir");
}

void selDeg(){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Introduce Grados");
    lcd.setCursor(0,1);
    lcd.print("De: ");
    lcd.print(posActualDeg);
    deg();
    arrow();
    lcd.print(numDisp);
    deg();
    lcd.setCursor(13,1);
    if(cursorPos>=0){
        lcd.print(cursorPos+1);
    } else{
        lcd.print("Ir");
    }
}


void desplRel(){
    numDisp=0;
    selDeg();
}

void desplAbs(){
    numDisp=posActualDeg;
    selDeg();
}


void goHome(){
    numDisp=0;
    traveling();
    estado = MENU;
    drawMenu();
}

void cwTurn(){
    numDisp=0;
    traveling();
    estado = MENU;
    drawMenu();
}
void ccwTurn(){
    numDisp=0;
    traveling();
    estado = MENU;
    drawMenu();
}
long initialTurn=0;
void directTurn(){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.write(0b01111111);
    initialTurn=enc.read();
    click = 0;
    long lastPosition = map(enc.read()-initialTurn,0,80,0,360);
    while(1){
    long position = - map(enc.read()-initialTurn,0,80,0,4092); //Encoder 1 turn 80 div
    
    posActualDeg -= map(enc.read()-initialTurn,0,80,0,360)-lastPosition; //Encoder 1 turn 80 div
    diff_steps = position - motor_position;
    
    lastPosition = map(enc.read()-initialTurn,0,80,0,360);
    moveMotor();
    lcd.setCursor(0,1);
    lcd.print(posActualDeg);
    deg();
    }
    estado = MENU;
    drawMenu();

}

// Creamos el array de opciones
MenuOption menu[] = {
  {"Despl. Relativo", desplRel, MENU_Rel},
  {"Despl. Absoluto", desplAbs, MENU_Abs},
  {"Ir a casa 0deg", goHome, HOME},
  {"Giro CW", cwTurn, CW},
  {"Giro CCW", ccwTurn, CCW},
  {"Control Directo", directTurn, FOLLOWER}
};
// Número de opciones
const int menuSize = sizeof(menu) / sizeof(menu[0]);

void arrow(){
    lcd.write(0b01111110);
}
void deg(){
    lcd.write(0b11011111);
}

void drawMenu(){
    lcd.clear();
    lcd.setCursor(0, 0);
    arrow();
    lcd.print(menu[menuPos].texto);
    if(menuPos<menuSize){
        lcd.setCursor(1,1);
        lcd.print(menu[menuPos+1].texto);
    }
}


void traveling(){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Viajando");
    long newPosActualDeg=0;
    if(estado==MENU_Abs) 
        newPosActualDeg = numDisp;
    else if(estado==MENU_Rel)
        newPosActualDeg = posActualDeg+ numDisp;
    else if(estado == HOME)
        newPosActualDeg = 0;
    else if(estado ==CCW)
        newPosActualDeg = posActualDeg+360;
    else if(estado ==CW)
        newPosActualDeg = posActualDeg-360;
    diff_steps=map(newPosActualDeg - posActualDeg,0,360,0,4092);
    moveMotor();
    posActualDeg=newPosActualDeg;
}

// Activar PCINT en un PIN
void pciSetup(byte pin)
{
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // activar pin en PCMSK
    PCIFR  |= bit (digitalPinToPCICRbit(pin)); // limpiar flag de la interrupcion en PCIFR
    PCICR  |= bit (digitalPinToPCICRbit(pin)); // activar interrupcion para el grupo en PCICR
}

ISR (PCINT0_vect)
{
    // gestionar PCINT para A0 a A5
    click+=1;
    click &=0b1;
}
ISR (PCINT1_vect)
{
    // gestionar PCINT para A0 a A5
    click+=1;
    click &=0b1;
    
}
ISR (PCINT2_vect)
{
    // gestionar PCINT para A0 a A5
    click+=1;
    click &=0b1;
}
void setup()
{
    DDRB = 0b00001111;
    Serial.begin(115200);
    Serial.println("INICIANDO");
    pinMode(7,INPUT);
    pciSetup(6);
    // do the usual lcd initialization for the used liquid crystal library
    lcd.init();
    lcd.backlight();
    lcd.print("MOTOR A PASOS");

    // also initialize the bar_display object (loads the style)
    // dispA.begin();

    delay(500);

    drawMenu();
    // spb.setPosition(spb.col() - 5, 1);
    // spb.setLength(spb.getLength() + 5);
    click = 0;
    click2=0;
}
void loop(){
    loopGUI();
    loopMotor();
}

void moveMotor() {
  if(diff_steps){
   timerRunning= false;
   Serial.println(diff_steps);
   steps_left = abs(diff_steps);
   Direction = diff_steps > 0;
      while(steps_left>0){
         paso();     // Avanza un paso
         steps_left--;  // Un paso menos
      }
    motor_position+=diff_steps;
    diff_steps=0;
  }
}

void loopMotor(){
  if (!timerRunning) {
    startTime = millis();
    timerRunning = true;
    Serial.println("Timer iniciado");
  }
  // Si el timer está corriendo y pasó el tiempo apagar el motor para que el embobinado no se caliente
  if (timerRunning && (millis() - startTime >= TIMEOUT)) {
    PORTB = 0;
    timerRunning = false; // para no ejecutarla varias veces
  }
}

void paso(){  
   // digitalWrite( IN1, Matriz[Steps][ 0] );
   // digitalWrite( IN2, Matriz[Steps][ 1] );
   // digitalWrite( IN3, Matriz[Steps][ 2] );
   // digitalWrite( IN4, Matriz[Steps][ 3] );
   PORTB = RegisterMatrix[Steps];
   SetDirection();
   delayMicroseconds(650+(10/velocity)*100);
}

void SetDirection(){  
   if(Direction)
      Steps++;
   else
      Steps--;

   if (Steps>7)
      Steps=0 ;
   if (Steps<0)
      Steps=7 ;
}

void loopGUI()
{
    if(click) { click2+=1; }
    // if(click2&0b1) {Serial.println("PRESSED");}
    if(estado==MENU){
        lasttimeClick=0;
        if(click2&0b1 && digitalRead(7) == 0 ){
            click=0;
            click2=0;
            estado = menu[menuPos].estado;
            comesFromMenu=1;
            menu[menuPos].funcion();
        }
        long position = enc.read();
        if(position != oldPosition){
            long delta_position = position-oldPosition;
            oldPosition=position;
            enc4Step+=delta_position;
            if(abs(enc4Step)==4){
                enc4Step=0;
                char newMenuPos = menuPos + abs(delta_position)/delta_position;
                newMenuPos %=menuSize;
                if(newMenuPos<0) newMenuPos=0;
                if(newMenuPos!=menuPos) { menuPos=newMenuPos; drawMenu();}
            }
        }
    } else if((click2)&0b1 ){
        click = 0;
        click2= 0;
        if(comesFromMenu || digitalRead(7) == 1){
            comesFromMenu=0;
        } else{
            Serial.println("AAA");
            Serial.print("Tiempo: ");
            Serial.println(millis()-lasttimeClick);
            if(lasttimeClick>0 && millis()-lasttimeClick <=300){
                estado=MENU;
                lasttimeClick=0;
                drawMenu();
                delay(500);
                click = 0;
                click2= 0;
            } else
                lasttimeClick=millis();
            switch(estado){
                case MENU_Abs:
                case MENU_Rel:
                    cursorPos+=1;
                    if(cursorPos>2){
                        cursorPos=-1;
                    } else if (cursorPos==-1 || velMenu){
                        //TODO: Start Movement
                        velMenu=0;
                        traveling();
                        cursorPos=0;
                        selDeg();
                        cursorPos=0;
                    }
                    selDeg();
                break;
                default:
                break;
            }
        } 
    } else if(velMenu){
        Serial.println("velMenu");
        long position = enc.read();
        if(position != oldPosition){
            long delta_position = position-oldPosition;
            oldPosition=position;
            enc4Step+=delta_position;
            if(abs(enc4Step)==4){
                enc4Step=0;
                int newVelocity = velocity +( (delta_position)/abs(delta_position) );
                if(newVelocity>10) newVelocity=10;
                if(newVelocity<1) newVelocity=1;
                if(newVelocity!=velocity){
                    velocity=newVelocity; 
                    selVel();
                }
            }
        }
    } else if(estado == MENU_Abs || estado == MENU_Rel){
        long position = enc.read();
        if(position != oldPosition){
            long delta_position = position-oldPosition;
            oldPosition=position;
            enc4Step+=delta_position;
            if(abs(enc4Step)==4 && cursorPos>=0){
                enc4Step=0;
                Serial.print("CursorPos: ");
                Serial.print(cursorPos);
                Serial.print(" delta: ");
                Serial.println(delta_position);
                int newNumDisp = numDisp +( (delta_position)/abs(delta_position) )*  pow(10,cursorPos);
                Serial.println(newNumDisp);
                if(abs(newNumDisp)>999) newNumDisp=0;
                if(newNumDisp!=numDisp){numDisp=newNumDisp; selDeg();}
                
            } else if(abs(enc4Step)>=4*5 && cursorPos==-1){
                    oldPosition=position;
                    enc4Step=0;
                    velMenu=1;
                    selVel();
            }
        }
    } else if (estado == FOLLOWER) {
        long position = enc.read();
        if(position != oldPosition){
        Serial.println("FOLLOWER");
            long delta_position = position-oldPosition;
            oldPosition=position;
            enc4Step+=delta_position;
            if(abs(enc4Step)==4){
                enc4Step=0;
                long delta_steps = map( (delta_position)/abs(delta_position),0,80,0,4092); //Encoder 1 turn 80 div
                long delta_deg = map( (delta_position)/abs(delta_position),0,80,0,360); //Encoder 1 turn 80 div
                diff_steps = - delta_steps - motor_position;
                posActualDeg += delta_deg; 
                directTurn();
                moveMotor();
            }
        }
    }
}

void loop2()
{
    lcd.clear();
    // you can show the progress bar calling showProgressPct and specifying the percentage [0-100] to be filled
    for (int i = 0; i <= 100; i++)
    {
        lcd.setCursor(0, 0);                  // move cursor to top left
        lcd.print(i);                         // print the string on the display
        lcd.print('%');                       // print the string on the display
        lcd.write(0b01111110);

        spb.showProgressPct(i);
        delay(25);
    }

    delay(1000);
    lcd.clear();

    // or you can use showProgress method and specifying the number of columns to be filled
    // The length in pixel columns of the progress bar can be found by calling the size() method

    for (int i = spb.size(); i >=0; i--)
    {
        lcd.setCursor(0, 0);                  // move cursor to top left
        lcd.print(i);                         // print the number on the display
        lcd.print(' ');                       // print a character on the display (to overwrite)

        spb.showProgress(i);
        delay(100);
    }
    delay(1000);


}

