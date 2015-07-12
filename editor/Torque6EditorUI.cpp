///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Torque6EditorUI.h"

///////////////////////////////////////////////////////////////////////////

MainFrame::MainFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	m_mgr.SetManagedWindow(this);
	m_mgr.SetFlags(wxAUI_MGR_DEFAULT);
	
	m_menubar2 = new wxMenuBar( 0 );
	m_menu1 = new wxMenu();
	m_menuItem1 = new wxMenuItem( m_menu1, wxID_OPEN, wxString( wxT("Open Project") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem1 );
	
	wxMenuItem* m_menuItem2;
	m_menuItem2 = new wxMenuItem( m_menu1, wxID_ANY, wxString( wxT("Save Project") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem2 );
	
	m_menu1->AppendSeparator();
	
	wxMenuItem* m_menuItem3;
	m_menuItem3 = new wxMenuItem( m_menu1, wxID_ANY, wxString( wxT("Exit") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem3 );
	
	m_menubar2->Append( m_menu1, wxT("File") ); 
	
	this->SetMenuBar( m_menubar2 );
	
	mainToolbar = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_TEXT ); 
	m_projectTool = mainToolbar->AddTool( TOOLBAR_PROJECT, wxT("Project"), wxBitmap( wxT("images/iconNew.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 
	
	mainToolbar->AddSeparator(); 
	
	m_sceneTool = mainToolbar->AddTool( TOOLBAR_SCENE, wxT("Scene"), wxBitmap( wxT("images/iconNew.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 
	
	m_scriptsTool = mainToolbar->AddTool( TOOLBAR_SCRIPTS, wxT("Scripts"), wxBitmap( wxT("images/iconNew.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 
	
	m_materialsTool = mainToolbar->AddTool( TOOLBAR_MATERIALS, wxT("Materials"), wxBitmap( wxT("images/iconNew.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 
	
	mainToolbar->Realize();
	m_mgr.AddPane( mainToolbar, wxAuiPaneInfo() .Top() .CaptionVisible( false ).CloseButton( false ).Gripper().Dock().Resizable().FloatingSize( wxSize( 37,57 ) ).Layer( 1 ) );
	
	
	mainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_mgr.AddPane( mainPanel, wxAuiPaneInfo() .Left() .CaptionVisible( false ).CloseButton( false ).PinButton( true ).Dock().Resizable().FloatingSize( wxDefaultSize ).CentrePane() );
	
	
	m_mgr.Update();
	this->Centre( wxBOTH );
}

MainFrame::~MainFrame()
{
	m_mgr.UnInit();
	
}

ProjectFrame::ProjectFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer1->Add( m_staticText5, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl3 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrl3, 0, wxALL, 5 );
	
	
	bSizer1->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	m_propertyGrid1 = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE);
	m_propertyGridItem2 = m_propertyGrid1->Append( new wxPropertyCategory( wxT("Project Settings"), wxT("Project Settings") ) ); 
	m_propertyGridItem3 = m_propertyGrid1->Append( new wxStringProperty( wxT("Project Name"), wxT("Project Name") ) ); 
	m_propertyGridItem1 = m_propertyGrid1->Append( new wxBoolProperty( wxT("Shitty Game Or What?"), wxT("Shitty Game Or What?") ) ); 
	m_propertyGridItem21 = m_propertyGrid1->Append( new wxPropertyCategory( wxT("Second Category"), wxT("Second Category") ) ); 
	m_propertyGridItem31 = m_propertyGrid1->Append( new wxStringProperty( wxT("Project Name"), wxT("Project Name") ) ); 
	bSizer1->Add( m_propertyGrid1, 0, wxALL|wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
}

ProjectFrame::~ProjectFrame()
{
}

ScriptsPanel::ScriptsPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	m_mgr.SetManagedWindow(this);
	m_mgr.SetFlags(wxAUI_MGR_DEFAULT);
	
	m_scriptsTree = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE );
	m_mgr.AddPane( m_scriptsTree, wxAuiPaneInfo() .Left() .Caption( wxT("Script Browser") ).PinButton( true ).Dock().Resizable().FloatingSize( wxDefaultSize ) );
	
	m_scriptsNotebook = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_DEFAULT_STYLE );
	m_mgr.AddPane( m_scriptsNotebook, wxAuiPaneInfo() .Left() .PinButton( true ).Dock().Resizable().FloatingSize( wxDefaultSize ).CentrePane() );
	
	
	
	m_mgr.Update();
}

ScriptsPanel::~ScriptsPanel()
{
	m_mgr.UnInit();
	
}
