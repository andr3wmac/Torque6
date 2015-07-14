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
	
	mainMenuBar = new wxMenuBar( 0 );
	m_menu1 = new wxMenu();
	m_menuItem1 = new wxMenuItem( m_menu1, wxID_OPEN, wxString( wxT("Open Project") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem1 );
	
	wxMenuItem* m_menuItem2;
	m_menuItem2 = new wxMenuItem( m_menu1, wxID_ANY, wxString( wxT("Save Project") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem2 );
	
	m_menu1->AppendSeparator();
	
	wxMenuItem* m_menuItem3;
	m_menuItem3 = new wxMenuItem( m_menu1, wxID_CLOSE, wxString( wxT("Close Project") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem3 );
	
	m_menu1->AppendSeparator();
	
	wxMenuItem* m_menuItem4;
	m_menuItem4 = new wxMenuItem( m_menu1, wxID_ANY, wxString( wxT("Exit") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem4 );
	
	mainMenuBar->Append( m_menu1, wxT("File") ); 
	
	this->SetMenuBar( mainMenuBar );
	
	mainToolbar = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_TEXT ); 
	m_tool1 = mainToolbar->AddTool( TOOLBAR_PROJECT, wxT("Project"), wxBitmap( wxT("images/iconNew.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 
	
	mainToolbar->AddSeparator(); 
	
	m_tool2 = mainToolbar->AddTool( TOOLBAR_CONSOLE, wxT("Console"), wxBitmap( wxT("images/iconNew.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 
	
	m_tool3 = mainToolbar->AddTool( TOOLBAR_SCENE, wxT("Scene"), wxBitmap( wxT("images/iconNew.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 
	
	m_tool4 = mainToolbar->AddTool( TOOLBAR_SCRIPTS, wxT("Scripts"), wxBitmap( wxT("images/iconNew.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 
	
	m_tool5 = mainToolbar->AddTool( TOOLBAR_MATERIALS, wxT("Materials"), wxBitmap( wxT("images/iconNew.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 
	
	m_tool7 = mainToolbar->AddTool( TOOLBAR_PROFILER, wxT("Profiler"), wxBitmap( wxT("images/iconFolder.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 
	
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

ScriptsPanel::ScriptsPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	m_mgr.SetManagedWindow(this);
	m_mgr.SetFlags(wxAUI_MGR_DEFAULT);
	
	m_scriptsTree = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE );
	m_mgr.AddPane( m_scriptsTree, wxAuiPaneInfo() .Left() .Caption( wxT("Script Browser") ).PinButton( true ).Dock().Resizable().FloatingSize( wxSize( 95,93 ) ).Row( 1 ).Layer( 0 ) );
	
	m_scriptsNotebook = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_DEFAULT_STYLE );
	m_mgr.AddPane( m_scriptsNotebook, wxAuiPaneInfo() .Left() .PinButton( true ).Dock().Resizable().FloatingSize( wxDefaultSize ).CentrePane() );
	
	
	
	m_mgr.Update();
}

ScriptsPanel::~ScriptsPanel()
{
	m_mgr.UnInit();
	
}

ProfilerPanel::ProfilerPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	m_mgr.SetManagedWindow(this);
	m_mgr.SetFlags(wxAUI_MGR_DEFAULT);
	
	m_profilerMenu = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_mgr.AddPane( m_profilerMenu, wxAuiPaneInfo() .Top() .CaptionVisible( false ).PinButton( true ).Movable( false ).Dock().Resizable().FloatingSize( wxSize( 316,69 ) ).BottomDockable( false ).LeftDockable( false ).RightDockable( false ).Floatable( false ).Row( 2 ).MinSize( wxSize( -1,32 ) ) );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button1 = new wxButton( m_profilerMenu, PROFILER_START, wxT("Profile"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_button1, 0, wxALL, 5 );
	
	wxString m_choice1Choices[] = { wxT("Seconds"), wxT("Milliseconds"), wxT("Microseconds"), wxT("Nanoseconds") };
	int m_choice1NChoices = sizeof( m_choice1Choices ) / sizeof( wxString );
	m_choice1 = new wxChoice( m_profilerMenu, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice1NChoices, m_choice1Choices, 0 );
	m_choice1->SetSelection( 1 );
	bSizer1->Add( m_choice1, 0, wxALL, 5 );
	
	
	m_profilerMenu->SetSizer( bSizer1 );
	m_profilerMenu->Layout();
	bSizer1->Fit( m_profilerMenu );
	m_mainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_mgr.AddPane( m_mainPanel, wxAuiPaneInfo() .Left() .CaptionVisible( false ).CloseButton( false ).PaneBorder( false ).Dock().Resizable().FloatingSize( wxDefaultSize ).CentrePane() );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	ProfilerDataView = new wxDataViewCtrl( m_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( ProfilerDataView, 1, wxALL|wxEXPAND, 5 );
	
	
	m_mainPanel->SetSizer( bSizer2 );
	m_mainPanel->Layout();
	bSizer2->Fit( m_mainPanel );
	
	m_mgr.Update();
}

ProfilerPanel::~ProfilerPanel()
{
	m_mgr.UnInit();
	
}
