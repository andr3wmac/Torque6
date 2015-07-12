///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __TORQUE6EDITORUI_H__
#define __TORQUE6EDITORUI_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/toolbar.h>
#include <wx/panel.h>
#include <wx/frame.h>
#include <wx/aui/aui.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/treectrl.h>
#include <wx/aui/auibook.h>

///////////////////////////////////////////////////////////////////////////

#define TOOLBAR_PROJECT 1000
#define TOOLBAR_SCENE 1001
#define TOOLBAR_SCRIPTS 1002
#define TOOLBAR_MATERIALS 1003

///////////////////////////////////////////////////////////////////////////////
/// Class MainFrame
///////////////////////////////////////////////////////////////////////////////
class MainFrame : public wxFrame 
{
	private:
	
	protected:
		wxMenuBar* m_menubar2;
		wxMenu* m_menu1;
		wxToolBarToolBase* m_projectTool; 
		wxToolBarToolBase* m_sceneTool; 
		wxToolBarToolBase* m_scriptsTool; 
		wxToolBarToolBase* m_materialsTool; 
	
	public:
		wxMenuItem* m_menuItem1;
		wxToolBar* mainToolbar;
		wxPanel* mainPanel;
		
		MainFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Torque 6 Editor"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1024,768 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		wxAuiManager m_mgr;
		
		~MainFrame();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class ProjectFrame
///////////////////////////////////////////////////////////////////////////////
class ProjectFrame : public wxFrame 
{
	private:
	
	protected:
		wxStaticText* m_staticText5;
		wxTextCtrl* m_textCtrl3;
		wxPropertyGrid* m_propertyGrid1;
		wxPGProperty* m_propertyGridItem2;
		wxPGProperty* m_propertyGridItem3;
		wxPGProperty* m_propertyGridItem1;
		wxPGProperty* m_propertyGridItem21;
		wxPGProperty* m_propertyGridItem31;
	
	public:
		
		ProjectFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Project Management"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 559,605 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		
		~ProjectFrame();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class ScriptsPanel
///////////////////////////////////////////////////////////////////////////////
class ScriptsPanel : public wxPanel 
{
	private:
	
	protected:
	
	public:
		wxTreeCtrl* m_scriptsTree;
		wxAuiNotebook* m_scriptsNotebook;
		
		ScriptsPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL ); wxAuiManager m_mgr;
		
		~ScriptsPanel();
	
};

#endif //__TORQUE6EDITORUI_H__
