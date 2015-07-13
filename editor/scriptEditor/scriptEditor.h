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

#ifndef _SCRIPTEDITOR_H_
#define _SCRIPTEDITOR_H_

#ifndef _PROJECTMANAGER_H_
#include "../project/projectManager.h"
#endif

#ifndef __TORQUE6EDITORUI_H__
#include "../Torque6EditorUI.h"
#endif

#ifndef _WX_TREECTRL_H_BASE_
#include <wx/treectrl.h>
#endif

class ScriptTreeItemData : public wxTreeItemData
{
public:
   wxString scriptPath;
   wxString scriptName;
   
   ScriptTreeItemData(wxString _scriptPath, wxString _scriptName)
      :  scriptPath(_scriptPath),
         scriptName(_scriptName)
   {
   }
};

class ScriptEditor : public wxEvtHandler, public ProjectTool
{
   protected:
      ScriptsPanel*     mScriptsPanel;
      wxImageList*      mIconList;

   public:
      ScriptEditor();
      ~ScriptEditor();

      void loadProject(wxString projectName, wxString projectPath);
      void findAllScripts(wxTreeItemId treeParent, wxString folder);


      virtual void init(ProjectManager* _projectManager, MainFrame* _frame, wxAuiManager* _manager);
      virtual void openTool();
      virtual void closeTool();

      virtual void OnTreeEvent( wxTreeEvent& evt );

      virtual void onProjectLoaded(wxString projectName, wxString projectPath);
      virtual void onProjectClosed();
};

#endif // _SCRIPTEDITOR_H_
