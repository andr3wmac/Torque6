// For compilers that don't support precompilation, include "wx/wx.h"
#include "wx/wxprec.h"
 
#ifndef WX_PRECOMP
#	include "wx/wx.h"
#endif

#include "wx/aui/aui.h"
#include "mainPanel.h"

MainPanel::MainPanel(wxWindow* parent,
                 wxWindowID id,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style)
        : wxPanel(parent, id, pos, size, style)
{
   SetMinSize(wxSize(400, 300));
}

MainPanel::~MainPanel()
{

}

void MainPanel::OnIdle(wxIdleEvent& evt)
{

}

void MainPanel::OnSize(wxSizeEvent& evt)
{

}