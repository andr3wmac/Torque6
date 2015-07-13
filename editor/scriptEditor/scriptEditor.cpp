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

#include "wx/stc/stc.h"
#include <wx/dir.h>
#include <wx/treectrl.h>

// UI generated from wxFormBuilder
#include "../Torque6EditorUI.h"

// Syntax Highlighting
#include "defsext.h"
#include "prefs.h"
#include "edit.h"
#include "TorqueScriptLexer.h"

#include "scriptEditor.h"

ScriptEditor::ScriptEditor()
{
   Catalogue::AddLexerModule(&lmTorqueScript);

   mIconList = new wxImageList( 16, 16 );
}

ScriptEditor::~ScriptEditor()
{

}

void ScriptEditor::init(ProjectManager* _projectManager, MainFrame* _frame, wxAuiManager* _manager)
{
   ProjectTool::init(_projectManager, _frame, _manager);

   mIconList->Add(wxBitmap("images/iconFolder.png",   wxBITMAP_TYPE_PNG));
   mIconList->Add(wxBitmap("images/iconFile.png",     wxBITMAP_TYPE_PNG));
}

void ScriptEditor::openTool()
{
   mScriptsPanel = new ScriptsPanel(mFrame, wxID_ANY);

   // Icons
   mScriptsPanel->m_scriptsTree->AssignImageList(mIconList);

   // Events
   mScriptsPanel->Connect(wxID_ANY, wxEVT_TREE_ITEM_ACTIVATED, wxTreeEventHandler(ScriptEditor::OnTreeEvent), NULL, this);

   // Add Pane
   mManager->AddPane(mScriptsPanel, wxAuiPaneInfo().Caption("Scripts")
                                            .CaptionVisible( true )
                                            .CloseButton( true )
                                            .PinButton( true )
                                            .MaximizeButton(true)
                                            .Dock()
                                            .Resizable()
                                            .FloatingSize( wxDefaultSize )
                                            .Bottom());
   mManager->Update();

   if ( mProjectManager->mProjectLoaded )
      loadProject(mProjectManager->mProjectName, mProjectManager->mProjectPath);

   // For Easy Debugging:
   // loadProject("Debug Project", "C:/Users/Andrew/Documents/Projects/Torque6/projects/00-Console/");
}

void ScriptEditor::closeTool()
{

}

void ScriptEditor::loadProject(wxString projectName, wxString projectPath)
{
   wxTreeItemId root = mScriptsPanel->m_scriptsTree->AddRoot(projectName, 0);
   findAllScripts(root, projectPath);
}

void ScriptEditor::findAllScripts(wxTreeItemId treeParent, wxString folder)
{
   wxDir projectDir(folder);
   if ( projectDir.IsOpened() )
   {
      // Find All Folders
      wxString dirname;
      bool dir_cont = projectDir.GetFirst(&dirname, wxEmptyString, wxDIR_DIRS);
      while ( dir_cont )
      {
         wxTreeItemId subdir_tree = mScriptsPanel->m_scriptsTree->AppendItem(treeParent, dirname, 0);

         wxString subdir(folder);
         subdir.append("/");
         subdir.append(dirname);
         findAllScripts(subdir_tree, subdir);

         dir_cont = projectDir.GetNext(&dirname);
      }

      // Find All .cs Files
      wxString filename;
      bool file_cont = projectDir.GetFirst(&filename, "*.cs");
      while ( file_cont )
      {
         wxString fullpath(folder);
         fullpath.append("/");
         fullpath.append(filename);
         mScriptsPanel->m_scriptsTree->AppendItem(treeParent, filename, 1, -1, new ScriptTreeItemData(fullpath, filename));
         file_cont = projectDir.GetNext(&filename);
      }
   }
}

void ScriptEditor::OnTreeEvent( wxTreeEvent& evt )
{
   //evt.GetItem()
   ScriptTreeItemData* data = dynamic_cast<ScriptTreeItemData*>(mScriptsPanel->m_scriptsTree->GetItemData(evt.GetItem()));
   if ( data )
   {
      Edit* m_edit = new Edit(mFrame, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
      mScriptsPanel->m_scriptsNotebook->AddPage( m_edit, data->scriptName, false, wxNullBitmap );
      m_edit->LoadFile (data->scriptPath);
      m_edit->SetFocus();
   }
}

void ScriptEditor::onProjectLoaded(wxString projectName, wxString projectPath)
{
   if ( !mOpen ) return;

   mScriptsPanel->m_scriptsTree->DeleteAllItems();
   loadProject(projectName, projectPath);
}

void ScriptEditor::onProjectClosed()
{
   if ( !mOpen ) return;

   mScriptsPanel->m_scriptsTree->DeleteAllItems();
}