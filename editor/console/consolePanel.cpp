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

#include <wx/socket.h>
#include "consolePanel.h"

ConsolePanel::ConsolePanel(wxWindow* parent,
                 wxWindowID id,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style)
        : wxPanel(parent, id, pos, size, style)
{
   SetMinSize(wxSize(200, 200));
   Connect( wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(ConsolePanel::OnIdle) );

   mBoxSizer = new wxBoxSizer(wxVERTICAL);
   mBoxSizer->SetSizeHints(this);
   SetSizer(mBoxSizer);

   // Output from Telnet.
   mConsoleText = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                           wxDefaultPosition, wxDefaultSize,
                           wxTE_READONLY | wxTE_MULTILINE | wxSUNKEN_BORDER);
   mConsoleText->SetFont(wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT));
   mBoxSizer->Add(mConsoleText, 1, wxEXPAND);

   // Input for Console
   mConsoleInput = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                           wxDefaultPosition, wxDefaultSize,
                           wxSUNKEN_BORDER | wxTE_PROCESS_ENTER);
   mConsoleInput->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ConsolePanel::OnEnter), NULL, this);
   mBoxSizer->Add(mConsoleInput, 0, wxEXPAND);

   // Try to connect to the debugger
   wxIPV4address addr;
   addr.Hostname(wxT("127.0.0.1"));
   addr.Service(1337);

   if( mConsoleSock.Connect(addr, true) ) 
      mConsoleText->WriteText(wxT("Connected to Torque 6.\n"));
   else
      mConsoleText->WriteText(wxT("Failed to connect to Torque 6.\n"));
}

ConsolePanel::~ConsolePanel()
{
    mConsoleSock.Close();
    Disconnect( wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(ConsolePanel::OnIdle) );
}

void ConsolePanel::OnIdle(wxIdleEvent& evt)
{
   char c = 0x00;

   // Keep reading characters until there is no data left.
   if ( mConsoleSock.IsConnected() && mConsoleSock.IsData() )
   {
      mConsoleSock.Read(&c, 1);
      mConsoleText->WriteText(wxString::Format(wxT("%c"), c));
   }
}

void ConsolePanel::OnEnter(wxCommandEvent& evt)
{
   wxString inputStr = wxString::Format(wxT("%s\n"), mConsoleInput->GetValue());
   mConsoleText->WriteText(inputStr);
   mConsoleInput->Clear();

   if ( mConsoleSock.IsConnected() )
      mConsoleSock.Write(inputStr, inputStr.Len());
}