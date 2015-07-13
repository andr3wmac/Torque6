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

#include "main.h"

#include <wx/dir.h>
#include "wx/treectrl.h"
#include "wx/aui/aui.h"
 
IMPLEMENT_APP(Torque6Editor)

// This is executed upon startup, like 'main()' in non-wxWidgets programs.
bool Torque6Editor::OnInit()
{
   wxInitAllImageHandlers();

	mFrame = new MainFrame((wxFrame*) NULL, -1, _T("Torque 6 Editor"));
   mFrame->Show(true);
	SetTopWindow(mFrame);

   // Setup Advanced UI Manager
   mManager = &mFrame->m_mgr;

   // Frame Events
   mFrame->Connect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Torque6Editor::OnMenuEvent), NULL, this);

   // Main Toolbar Events
   mFrame->mainToolbar->Connect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Torque6Editor::OnToolbarEvent), NULL, this);

   // Torque 6 Project Manager
   mProjectManager.init(mManager, mFrame->mainPanel);

   // Tools
   mConsoleTool.init(&mProjectManager, mFrame, mManager);
   mScriptEditor.init(&mProjectManager, mFrame, mManager);

	return true;
}

Torque6Editor::~Torque6Editor()
{

}

void Torque6Editor::OnMenuEvent( wxCommandEvent& evt )
{
   if ( evt.GetId() == wxID_OPEN )
   {
      wxDirDialog openFolder(mFrame, wxT("Select Project Folder"), wxGetCwd());
      if ( openFolder.ShowModal() == wxID_OK )
         mProjectManager.openProject(openFolder.GetPath());

      mManager->Update();
   }

   if ( evt.GetId() == wxID_CLOSE )
   {
      mProjectManager.closeProject();
      mManager->Update();
   }
}

void Torque6Editor::OnToolbarEvent( wxCommandEvent& evt )
{
   switch(evt.GetId())
   {
      case TOOLBAR_CONSOLE:
         mConsoleTool.openTool();
         break;

      case TOOLBAR_SCRIPTS:
         mScriptEditor.openTool();
         break;

      default:
         break;
   }
}