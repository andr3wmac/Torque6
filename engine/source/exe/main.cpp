#include <windows.h>

typedef int (*enter_mainFunc)(HINSTANCE hInstance, int argc, const char **argv);

/*int main(int argc, const char **argv)
{
   HMODULE hGame = LoadLibrary(TEXT("Torque6_DEBUG.dll"));
   enter_mainFunc enter = (enter_mainFunc)GetProcAddress(hGame, TEXT("enter_main"));
   if ( enter != 0 )
      return enter(argc, argv);
   return 0;
}*/

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCommandShow)
{
   HMODULE hGame = LoadLibrary(TEXT("Torque6_DEBUG.dll"));
   enter_mainFunc enter = (enter_mainFunc)GetProcAddress(hGame, TEXT("enter_main"));
   if ( enter != 0 )
      return enter(hInstance, 0, NULL);
   return 0;
}