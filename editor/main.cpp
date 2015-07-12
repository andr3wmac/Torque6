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

// Torque 6 Project Manager
#include "projectManager.h"

// UI generated from wxFormBuilder
#include "Torque6EditorUI.h"

#include "consolePanel.h"
#include "main.h"
 
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

   // Main Panel Events
   mFrame->mainPanel->Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(Torque6Editor::OnIdle), NULL, this);
   mFrame->mainPanel->Connect(wxID_ANY, wxEVT_SIZE, wxSizeEventHandler(Torque6Editor::OnSize), NULL, this);

   // Main Toolbar Events
   mFrame->mainToolbar->Connect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Torque6Editor::OnToolbarEvent), NULL, this);

   // Torque 6 Project Manager
   mProjectManager = new ProjectManager(mManager, mFrame->mainPanel);

	return true;
}

Torque6Editor::~Torque6Editor()
{
   delete mProjectManager;
}

void Torque6Editor::OnMenuEvent( wxCommandEvent& evt )
{
   if ( evt.GetId() == wxID_OPEN )
   {
      wxDirDialog openFolder(mFrame, wxT("Select Project Folder"), wxT("./"));
      if ( openFolder.ShowModal() == wxID_OK )
         mProjectManager->openProject(openFolder.GetPath());

      mManager->Update();
   }
}

void Torque6Editor::OnToolbarEvent( wxCommandEvent& evt )
{
   switch(evt.GetId())
   {
      case TOOLBAR_SCRIPTS:
         mScriptEditor.open(mFrame, mManager);
         break;

      default:
         break;
   }
}

void Torque6Editor::OnIdle(wxIdleEvent& evt)
{
   mProjectManager->tick();
}

void Torque6Editor::OnSize(wxSizeEvent& evt)
{
   mProjectManager->resize(evt.GetSize().GetX(), evt.GetSize().GetY());
}