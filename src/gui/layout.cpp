///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "layout.h"

///////////////////////////////////////////////////////////////////////////

MainWindowGen::MainWindowGen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 450,500 ), wxDefaultSize );

	bSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	bSizer1->AddGrowableCol( 1 );
	bSizer1->AddGrowableRow( 0 );
	bSizer1->SetFlexibleDirection( wxHORIZONTAL );
	bSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer4->AddGrowableRow( 0 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	visualiser = new VisualisationControl( this, wxID_ANY, wxDefaultPosition, wxSize( 200,480 ), wxBORDER_THEME );
	visualiser->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNTEXT ) );
	visualiser->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_APPWORKSPACE ) );
	visualiser->SetMinSize( wxSize( 200,480 ) );

	fgSizer4->Add( visualiser, 1, wxALL|wxEXPAND, 5 );

	stopButton = new wxButton( this, wxID_ANY, wxT("Stop"), wxDefaultPosition, wxDefaultSize, 0 );
	stopButton->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );

	fgSizer4->Add( stopButton, 0, wxALIGN_CENTER|wxALL, 5 );


	bSizer1->Add( fgSizer4, 1, wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer2->AddGrowableCol( 0 );
	fgSizer2->AddGrowableRow( 1 );
	fgSizer2->SetFlexibleDirection( wxVERTICAL );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );

	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Device:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer3->Add( m_staticText4, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );

	deviceCombo = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_SORT );
	fgSizer3->Add( deviceCombo, 0, wxALL|wxEXPAND, 5 );

	m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("Flux source/sink:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer3->Add( m_staticText5, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );

	fluxSourceSinkCombo = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	fluxSourceSinkCombo->Append( wxT("drive:0") );
	fluxSourceSinkCombo->Append( wxT("drive:1") );
	fgSizer3->Add( fluxSourceSinkCombo, 0, wxALL|wxEXPAND, 5 );

	m_staticText51 = new wxStaticText( this, wxID_ANY, wxT("Format:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText51->Wrap( -1 );
	fgSizer3->Add( m_staticText51, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );

	wxArrayString formatChoiceChoices;
	formatChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, formatChoiceChoices, wxCB_SORT );
	formatChoice->SetSelection( 0 );
	fgSizer3->Add( formatChoice, 0, wxALL|wxEXPAND, 5 );


	fgSizer3->Add( 0, 0, 1, wxEXPAND, 5 );

	highDensityToggle = new wxCheckBox( this, wxID_ANY, wxT("High density disk"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( highDensityToggle, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );

	m_staticText7 = new wxStaticText( this, wxID_ANY, wxT("Cylinders:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer3->Add( m_staticText7, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );

	cylindersText = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( cylindersText, 0, wxALL|wxEXPAND, 5 );

	m_staticText9 = new wxStaticText( this, wxID_ANY, wxT("Heads:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	fgSizer3->Add( m_staticText9, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );

	headsText = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( headsText, 0, wxALL|wxEXPAND, 5 );


	fgSizer2->Add( fgSizer3, 1, wxEXPAND, 5 );


	fgSizer2->Add( 0, 0, 1, wxEXPAND, 5 );

	wxGridSizer* m_sizer;
	m_sizer = new wxGridSizer( 0, 2, 0, 0 );

	readFluxButton = new wxButton( this, wxID_ANY, wxT("Read flux"), wxDefaultPosition, wxDefaultSize, 0 );
	readFluxButton->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );

	m_sizer->Add( readFluxButton, 0, wxALL|wxEXPAND, 5 );

	readImageButton = new wxButton( this, wxID_ANY, wxT("Read image"), wxDefaultPosition, wxDefaultSize, 0 );
	m_sizer->Add( readImageButton, 0, wxALL|wxEXPAND, 5 );

	writeFluxButton = new wxButton( this, wxID_ANY, wxT("Write flux"), wxDefaultPosition, wxDefaultSize, 0 );
	m_sizer->Add( writeFluxButton, 0, wxALL|wxEXPAND, 5 );

	writeImageButton = new wxButton( this, wxID_ANY, wxT("Write image"), wxDefaultPosition, wxDefaultSize, 0 );
	m_sizer->Add( writeImageButton, 0, wxALL|wxEXPAND, 5 );


	fgSizer2->Add( m_sizer, 1, wxEXPAND|wxFIXED_MINSIZE, 5 );


	bSizer1->Add( fgSizer2, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();
	m_menubar1 = new wxMenuBar( 0 );
	m_menu1 = new wxMenu();
	wxMenuItem* m_menuItem2;
	m_menuItem2 = new wxMenuItem( m_menu1, wxID_ABOUT, wxString( wxT("About") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem2 );

	wxMenuItem* m_menuItem1;
	m_menuItem1 = new wxMenuItem( m_menu1, wxID_EXIT, wxString( wxT("E&xit") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem1 );

	m_menubar1->Append( m_menu1, wxT("&File") );

	this->SetMenuBar( m_menubar1 );


	// Connect Events
	m_menu1->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainWindowGen::OnAbout ), this, m_menuItem2->GetId());
	m_menu1->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainWindowGen::OnExit ), this, m_menuItem1->GetId());
}

MainWindowGen::~MainWindowGen()
{
	// Disconnect Events

}
