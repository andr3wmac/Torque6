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
 
class ProjectTool
{
   public:
      ProjectTool();

      virtual void onProjectLoaded(wxString projectName, wxString path) {}
      virtual void onProjectClosed() {}
};

class ProjectManager
{
   public:
      ProjectManager(wxAuiManager* manager, wxWindow* window);
      ~ProjectManager();

      bool          mProjectLoaded;
      wxString      mProjectName;
      wxString      mProjectPath;
      wxAuiManager* mManager;
      wxWindow*     mWindow;

      bool openProject(wxString projectPath);

      virtual void tick();
      virtual void resize(int x, int y);

      static wxVector<ProjectTool*> smProjectTools;
      static void onProjectLoaded(wxString projectName, wxString projectPath);
      static void onProjectClosed();
};
 
#endif // _PROJECTMANAGER_H_