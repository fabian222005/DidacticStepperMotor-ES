
char menuPos=0;
long lasttimeClick=0;
int timefordoubleclick=1000;
bool comesFromMenu=0;

enum Status {
  MENU_Rel,
  MENU_Abs,
  MENU,
  HOME,
  CW,
  CCW,
  FOLLOWER
};

Status estado = MENU; // variable global


long oldPosition=0;
volatile char click = 0;
char click2=0;
int posActualDeg=0;
int numDisp=0;
int cursorPos=0;
int enc4Step=0;

int velocity=10;
bool velMenu=0;
// Definimos el tipo de función (sin parámetros y sin retorno)
typedef void (*MenuFunction)();
// Estructura para cada opción del menú
struct MenuOption {
  const char* texto;
  MenuFunction funcion;
  Status estado;
};