
unsigned long startTime = 0;  
bool timerRunning = false;  
const unsigned long TIMEOUT = 5000; // 5 segundos  


int value = 0;

int RegisterMatrix[8]=
{
   0b00000001,//1
   0b00000011,//2
   0b00000010,//3
   0b00000110,//4
   0b00000100,//5
   0b00001100,//6
   0b00001000,//7
   0b00001001//8
};

long steps_left=0;
long motor_position=0;
boolean Direction = true;
long Steps = 0;    

long diff_steps =0;