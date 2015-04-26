//#include <windows.h>
#include <dlfcn.h>
#define LoadLibraryA(path) dlopen(path, RTLD_LAZY)
#define GetProcAddress(library, fn) dlsym(library, fn)
#define FreeLibrary(library) dlclose(library)

#include <iostream>
#include <cstdio>

typedef int (*mainFunc)(int argc, const char **argv);

int main(int argc, const char **argv)
{
#ifdef TORQUE_DEBUG
   void* hGame = LoadLibraryA("/home/andrewmac/Code/Torque6/build/bin/linux/libTorque6_DEBUG.so");   
#else
   void* hGame = LoadLibraryA("/home/andrewmac/Code/Torque6/build/bin/linux/libTorque6.so");
#endif
   if(hGame == NULL)
   {
      printf("%s\n", dlerror());
      std::cout << "Failed to load libTorque6.so";
      return 0;
   }

   mainFunc enter = (mainFunc)GetProcAddress(hGame, "unixmain");
   if(enter == NULL)
   {
      printf("%s\n", dlerror());
      std::cout << "Failed to find unixmain.";
      return 0;
   } else {
      return enter(argc, argv);
   }

   return 0;
}
