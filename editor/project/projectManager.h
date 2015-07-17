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

#ifndef _PROJECTMANAGER_H_
#define _PROJECTMANAGER_H_
 
#ifndef _PLUGINS_SHARED_H
#include <plugins/plugins_shared.h>
#endif

#ifndef _PLATFORM_LIBRARY_H_
#include "platform/platformLibrary.h"
#endif

typedef int (*initFunc)(int argc, const char **argv, HWND windowHWND);
typedef void (*shutdownFunc)();

class ProjectManager;
class MainFrame;
class wxAuiManager;

class ProjectTool
{
   public:
      ProjectTool();

      bool              mOpen;
      ProjectManager*   mProjectManager;
      MainFrame*        mFrame;
      wxAuiManager*     mManager;

      virtual void init(ProjectManager* _projectManager, MainFrame* _frame, wxAuiManager* _manager);
      virtual void openTool() { mOpen = true; }
      virtual void closeTool() { mOpen = false; }

      virtual void onProjectLoaded(wxString projectName, wxString path) {}
      virtual void onProjectClosed() {}
};

class ProjectManager : public wxEvtHandler
{
   public:
      ProjectManager();
      ~ProjectManager();

      void init(wxAuiManager* manager, wxWindow* window);

      LIBRARY_HANDLE    mTorque6Library;
      initFunc          mTorque6Init;
      shutdownFunc      mTorque6Shutdown;

      bool              mProjectLoaded;
      wxString          mProjectName;
      wxString          mProjectPath;
      wxAuiManager*     mManager;
      wxWindow*         mWindow;

      bool openProject(wxString projectPath);
      void closeProject();

      virtual void OnIdle(wxIdleEvent& evt);
      virtual void OnSize(wxSizeEvent& evt);
      virtual void OnMouseMove(wxMouseEvent& evt);
      virtual void OnMouseLeftDown(wxMouseEvent& evt);
      virtual void OnMouseLeftUp(wxMouseEvent& evt);
      virtual void OnMouseRightDown(wxMouseEvent& evt);
      virtual void OnMouseRightUp(wxMouseEvent& evt);
      virtual void OnKeyDown(wxKeyEvent& evt);
      virtual void OnKeyUp(wxKeyEvent& evt);

      static wxVector<ProjectTool*> smProjectTools;
      static void onProjectLoaded(wxString projectName, wxString projectPath);
      static void onProjectClosed();
};
 
#endif // _PROJECTMANAGER_H_