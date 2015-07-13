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

// UI generated from wxFormBuilder
#include "../Torque6EditorUI.h"

#include "projectManager.h"

// Torque 6 Import
// TODO: Multiplatform support.
#include <Windows.h>

typedef int (*initFunc)(int argc, const char **argv, HWND windowHWND);
typedef void (*destroyFunc)();
typedef void (*mainLoopFunc)();
typedef void (*resizeFunc)(int width, int height);
typedef void (*mouseMoveFunc)(int x, int y);
typedef void (*mouseButtonFunc)(bool down, bool left);

HMODULE           T6_lib = NULL;
initFunc          T6_init = NULL;
destroyFunc       T6_destroy = NULL;
mainLoopFunc      T6_mainLoop = NULL;
resizeFunc        T6_resize = NULL;
mouseMoveFunc     T6_mouseMove = NULL;
mouseButtonFunc   T6_mouseButton = NULL;

ProjectManager::ProjectManager()
   :  mManager(NULL),
      mWindow(NULL),
      mProjectLoaded( false ), 
      mProjectName(""),
      mProjectPath("")
{

}

ProjectManager::~ProjectManager()
{

}

void ProjectManager::init(wxAuiManager* manager, wxWindow* window)
{
   mManager = manager;
   mWindow = window;

   // Events
   mWindow->Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(ProjectManager::OnIdle), NULL, this);
   mWindow->Connect(wxID_ANY, wxEVT_SIZE, wxSizeEventHandler(ProjectManager::OnSize), NULL, this);

   // Mouse Events
   mWindow->Connect(wxID_ANY, wxEVT_MOTION, wxMouseEventHandler(ProjectManager::OnMouseMove), NULL, this);
   mWindow->Connect(wxID_ANY, wxEVT_LEFT_DOWN, wxMouseEventHandler(ProjectManager::OnMouseLeftDown), NULL, this);
   mWindow->Connect(wxID_ANY, wxEVT_LEFT_UP, wxMouseEventHandler(ProjectManager::OnMouseLeftUp), NULL, this);
   mWindow->Connect(wxID_ANY, wxEVT_RIGHT_DOWN, wxMouseEventHandler(ProjectManager::OnMouseRightDown), NULL, this);
   mWindow->Connect(wxID_ANY, wxEVT_RIGHT_UP, wxMouseEventHandler(ProjectManager::OnMouseRightUp), NULL, this);
}

bool ProjectManager::openProject(wxString projectPath)
{
   // Load Torque 6 DLL
   if ( T6_lib == NULL )
   {
      T6_lib = LoadLibraryA("Torque6_DEBUG.dll");

      // Load Nessicary Functions
      T6_init        = (initFunc)GetProcAddress(T6_lib, "winInit");
      T6_destroy     = (destroyFunc)GetProcAddress(T6_lib, "winDestroy");
      T6_mainLoop    = (mainLoopFunc)GetProcAddress(T6_lib, "winMainLoop");
      T6_resize      = (resizeFunc)GetProcAddress(T6_lib, "winResize");
      T6_mouseMove   = (resizeFunc)GetProcAddress(T6_lib, "winMouseMove");
      T6_mouseButton = (mouseButtonFunc)GetProcAddress(T6_lib, "winMouseButton");
   }

   // If successful, initialize.
   if ( T6_lib && T6_init && T6_destroy && T6_mainLoop && T6_resize )
   {
      const char* argv[3];
      argv[0] = "Torque6Editor.exe";
      argv[1] = "-project";
      argv[2] = projectPath;

      T6_init(3, argv, (HWND)mWindow->GetHWND());

      mProjectLoaded = true;
      mProjectPath = projectPath;
      wxDir projectDir(mProjectPath);
      mProjectName = projectDir.GetName();

      Plugins::Link.Con.printf("HELLO WORLD!");

      onProjectLoaded(mProjectName, projectPath);
      return true;
   }

   return false;
}

void ProjectManager::closeProject()
{
   mProjectLoaded = false;
   mProjectPath = "";
   mProjectName = "";

   T6_destroy();
   //FreeLibrary(T6_lib);

   onProjectClosed();
}

void ProjectManager::OnIdle(wxIdleEvent& evt)
{
   if ( mProjectLoaded && T6_mainLoop != NULL )
      T6_mainLoop();
}

void ProjectManager::OnSize(wxSizeEvent& evt)
{
   if ( mProjectLoaded && T6_resize != NULL )
      T6_resize(evt.GetSize().GetX(), evt.GetSize().GetY());
}

void ProjectManager::OnMouseMove(wxMouseEvent& evt)
{
   if ( mProjectLoaded && T6_mouseMove != NULL )
      T6_mouseMove(evt.GetPosition().x, evt.GetPosition().y);
}
void ProjectManager::OnMouseLeftDown(wxMouseEvent& evt)
{
   if ( mProjectLoaded && T6_mouseButton != NULL )
      T6_mouseButton(true, true);
}
void ProjectManager::OnMouseLeftUp(wxMouseEvent& evt)
{
   if ( mProjectLoaded && T6_mouseButton != NULL )
      T6_mouseButton(true, false);
}
void ProjectManager::OnMouseRightDown(wxMouseEvent& evt)
{
   if ( mProjectLoaded && T6_mouseButton != NULL )
      T6_mouseButton(false, true);
}
void ProjectManager::OnMouseRightUp(wxMouseEvent& evt)
{
   if ( mProjectLoaded && T6_mouseButton != NULL )
      T6_mouseButton(false, false);
}

// Project Tool Management
wxVector<ProjectTool*> ProjectManager::smProjectTools;

ProjectTool::ProjectTool()
   :  mOpen(false),
      mProjectManager(NULL),
      mFrame(NULL),
      mManager(NULL)
{
   ProjectManager::smProjectTools.push_back(this);
}

void ProjectTool::init(ProjectManager* _projectManager, MainFrame* _frame, wxAuiManager* _manager)
{
   mProjectManager   = _projectManager;
   mFrame            = _frame;
   mManager          = _manager;
}

void ProjectManager::onProjectLoaded(wxString projectName, wxString projectPath)
{
   for(unsigned int i = 0; i < smProjectTools.size(); ++i)
      smProjectTools[i]->onProjectLoaded(projectName, projectPath);
}

void ProjectManager::onProjectClosed()
{
   for(unsigned int i = 0; i < smProjectTools.size(); ++i)
      smProjectTools[i]->onProjectClosed();
}