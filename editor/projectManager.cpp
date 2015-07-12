//-----------------------------------------------------------------------------
// Copyright (c) 2015 Andrew Mac
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// For compilers that don't support precompilation, include "wx/wx.h"
#include "wx/wxprec.h"
 
#ifndef WX_PRECOMP
#	include "wx/wx.h"
#endif

#include <wx/dir.h>
#include "wx/treectrl.h"
#include "wx/aui/aui.h"

#include "projectManager.h"

// Torque 6 Import
#include <Windows.h>
typedef int (*initFunc)(int argc, const char **argv, HWND windowHWND);
typedef void (*destroyFunc)();

typedef void (*mainLoopFunc)();
typedef void (*resizeFunc)(int width, int height);

mainLoopFunc T6_mainLoop = NULL;
resizeFunc T6_resize = NULL;

ProjectManager::ProjectManager(wxAuiManager* manager, wxWindow* window)
   :  mManager(manager), 
      mWindow(window),
      mProjectLoaded( false ), 
      mProjectPath("")
{

}

ProjectManager::~ProjectManager()
{

}

bool ProjectManager::openProject(wxString projectPath)
{
   // Load Torque 6 DLL
   HMODULE lib = LoadLibraryA("Torque6_DEBUG.dll");

   // Load Nessicary Functions
   initFunc T6_init = (initFunc)GetProcAddress(lib, "winInit");
   T6_mainLoop = (mainLoopFunc)GetProcAddress(lib, "winMainLoop");
   T6_resize = (resizeFunc)GetProcAddress(lib, "winResize");

   // If successful, initialize.
   if ( T6_init && T6_mainLoop && T6_resize )
   {
      const char* argv[3];
      argv[0] = "Torque6Editor.exe";
      argv[1] = "-project";
      argv[2] = projectPath;

      T6_init(3, argv, (HWND)mWindow->GetHWND());

      mProjectLoaded = true;
      mProjectPath = projectPath;
      wxDir projectDir(mProjectPath);

      onProjectLoaded(projectDir.GetName(), projectPath);
      return true;
   }

   return false;
}

void ProjectManager::tick()
{
   if ( mProjectLoaded && T6_mainLoop != NULL )
      T6_mainLoop();
}

void ProjectManager::resize(int x, int y)
{
   if ( mProjectLoaded && T6_resize != NULL )
      T6_resize(x, y);
}

// Project Tool Management
wxVector<ProjectTool*> ProjectManager::smProjectTools;

ProjectTool::ProjectTool()
{
   ProjectManager::smProjectTools.push_back(this);
}

void ProjectManager::onProjectLoaded(wxString projectName, wxString projectPath)
{
   for(int i = 0; i < smProjectTools.size(); ++i)
      smProjectTools[i]->onProjectLoaded(projectName, projectPath);
}

void ProjectManager::onProjectClosed()
{
   for(int i = 0; i < smProjectTools.size(); ++i)
      smProjectTools[i]->onProjectClosed();
}