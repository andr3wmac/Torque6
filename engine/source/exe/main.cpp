#include <windows.h>

typedef int (*mainFunc)(int argc, const char **argv, HINSTANCE hInstance);

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCommandShow)
{

#ifdef TORQUE_DEBUG
   HMODULE hGame = LoadLibrary(TEXT("Torque6_DEBUG.dll"));
#else
   HMODULE hGame = LoadLibrary(TEXT("Torque6.dll"));
#endif

   mainFunc enter = (mainFunc)GetProcAddress(hGame, TEXT("main"));
   if ( enter != 0 )
      return enter(__argc, (const char **)__argv, hInstance);
   return 0;
}