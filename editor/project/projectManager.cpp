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

#include "platform/event.h"

// UI generated from wxFormBuilder
#include "../Torque6EditorUI.h"

#include "projectManager.h"

ProjectManager::ProjectManager()
   :  mManager(NULL),
      mWindow(NULL),
      mProjectLoaded( false ), 
      mProjectName(""),
      mProjectPath(""),
      mTorque6Library(NULL),
      mTorque6Init(NULL),
      mTorque6Shutdown(NULL)
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

   // Keyboard Events
   mWindow->Connect(wxID_ANY, wxEVT_KEY_DOWN, wxKeyEventHandler(ProjectManager::OnKeyDown), NULL, this);
   mWindow->Connect(wxID_ANY, wxEVT_KEY_UP, wxKeyEventHandler(ProjectManager::OnKeyUp), NULL, this);
}

bool ProjectManager::openProject(wxString projectPath)
{
   // Load Torque 6 DLL
   if ( mTorque6Library == NULL )
   {
      mTorque6Library = openLibrary("Torque6_DEBUG");

      // Load Nessicary Functions
      mTorque6Init      = (initFunc)getLibraryFunc(mTorque6Library, "winInit");
      mTorque6Shutdown  = (shutdownFunc)getLibraryFunc(mTorque6Library, "winDestroy");
   }

   // If successful, initialize.
   if ( mTorque6Library && mTorque6Init && mTorque6Shutdown )
   {
      const char* argv[3];
      argv[0] = "Torque6Editor.exe";
      argv[1] = "-project";
      argv[2] = projectPath;

      mTorque6Init(3, argv, (HWND)mWindow->GetHWND());

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

   mTorque6Shutdown();
   //FreeLibrary(T6_lib);

   onProjectClosed();
}

void ProjectManager::OnIdle(wxIdleEvent& evt)
{
   if ( mProjectLoaded )
      Plugins::Link.Engine.mainLoop();
}

void ProjectManager::OnSize(wxSizeEvent& evt)
{
   if ( mProjectLoaded )
      Plugins::Link.Engine.resizeWindow(evt.GetSize().GetX(), evt.GetSize().GetY());
}

void ProjectManager::OnMouseMove(wxMouseEvent& evt)
{
   if ( mProjectLoaded )
      Plugins::Link.Engine.mouseMove(evt.GetPosition().x, evt.GetPosition().y);
}
void ProjectManager::OnMouseLeftDown(wxMouseEvent& evt)
{
   if ( mProjectLoaded )
      Plugins::Link.Engine.mouseButton(true, true);
}
void ProjectManager::OnMouseLeftUp(wxMouseEvent& evt)
{
   if ( mProjectLoaded )
      Plugins::Link.Engine.mouseButton(true, false);
}
void ProjectManager::OnMouseRightDown(wxMouseEvent& evt)
{
   if ( mProjectLoaded )
      Plugins::Link.Engine.mouseButton(false, true);
}
void ProjectManager::OnMouseRightUp(wxMouseEvent& evt)
{
   if ( mProjectLoaded )
      Plugins::Link.Engine.mouseButton(false, false);
}

KeyCodes getTorqueKeyCode(int key)
{
   switch(key)
   {
      case 65 : return KEY_A;
      case 66 : return KEY_B;
      case 67 : return KEY_C;
      case 68 : return KEY_D;
      case 69 : return KEY_E;
      case 70 : return KEY_F;
      case 71 : return KEY_G;
      case 72 : return KEY_H;
      case 73 : return KEY_I;
      case 74 : return KEY_J;
      case 75 : return KEY_K;
      case 76 : return KEY_L;
      case 77 : return KEY_M;
      case 78 : return KEY_N;
      case 79 : return KEY_O;
      case 80 : return KEY_P;
      case 81 : return KEY_Q;
      case 82 : return KEY_R;
      case 83 : return KEY_S;
      case 84 : return KEY_T;
      case 85 : return KEY_U;
      case 86 : return KEY_V;
      case 87 : return KEY_W;
      case 88 : return KEY_X;
      case 89 : return KEY_Y;
      case 90 : return KEY_Z;
   }

   return KEY_NULL;
}

void ProjectManager::OnKeyDown(wxKeyEvent& evt)
{
   KeyCodes torqueKey = getTorqueKeyCode(evt.GetKeyCode());

   if ( mProjectLoaded )
      Plugins::Link.Engine.keyDown(torqueKey);
}
void ProjectManager::OnKeyUp(wxKeyEvent& evt)
{
   KeyCodes torqueKey = getTorqueKeyCode(evt.GetKeyCode());

   if ( mProjectLoaded )
      Plugins::Link.Engine.keyUp(torqueKey);
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