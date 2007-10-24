/////////////////////////////////////////////////////////////////////////////
// Name:        dialog_display_options.cpp
// Purpose:     
// Author:      jean-pierre Charras
// Modified by: 
// Created:     17/02/2006 17:47:55
// RCS-ID:      
// Copyright:   License GNU
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// Generated by DialogBlocks (unregistered), 17/02/2006 17:47:55

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "dialog_display_options.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes
#include "fctsys.h"

#include "wxstruct.h"
#include "common.h"
#include "cvpcb.h"
#include "protos.h"

#include "dialog_display_options.h"

////@begin XPM images
////@end XPM images


/*********************************************************************/
void WinEDA_DisplayFrame::InstallOptionsDisplay(wxCommandEvent& event)
/*********************************************************************/
/* Creation de la fenetre d'options de la fenetre de visu */
{
	KiDisplayOptionsFrame * OptionWindow = new KiDisplayOptionsFrame(this);
	OptionWindow->ShowModal();
	OptionWindow->Destroy();
}



/*!
 * KiDisplayOptionsFrame type definition
 */

IMPLEMENT_DYNAMIC_CLASS( KiDisplayOptionsFrame, wxDialog )

/*!
 * KiDisplayOptionsFrame event table definition
 */

BEGIN_EVENT_TABLE( KiDisplayOptionsFrame, wxDialog )

////@begin KiDisplayOptionsFrame event table entries
    EVT_RADIOBOX( EDGE_SELECT, KiDisplayOptionsFrame::OnEdgeSelectSelected )

    EVT_RADIOBOX( TEXT_SELECT, KiDisplayOptionsFrame::OnTextSelectSelected )

    EVT_CHECKBOX( PADFILL_OPT, KiDisplayOptionsFrame::OnPadfillOptClick )

    EVT_CHECKBOX( PADNUM_OPT, KiDisplayOptionsFrame::OnPadnumOptClick )

    EVT_BUTTON( ID_SAVE_CONFIG, KiDisplayOptionsFrame::OnSaveConfigClick )

    EVT_BUTTON( wxID_OK, KiDisplayOptionsFrame::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, KiDisplayOptionsFrame::OnCancelClick )

    EVT_BUTTON( wxID_APPLY, KiDisplayOptionsFrame::OnApplyClick )

////@end KiDisplayOptionsFrame event table entries

END_EVENT_TABLE()

/*!
 * KiDisplayOptionsFrame constructors
 */

KiDisplayOptionsFrame::KiDisplayOptionsFrame( )
{
}

KiDisplayOptionsFrame::KiDisplayOptionsFrame( WinEDA_BasePcbFrame* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	m_Parent = parent;

    Create(parent, id, caption, pos, size, style);
}

/*!
 * KiDisplayOptionsFrame creator
 */

bool KiDisplayOptionsFrame::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin KiDisplayOptionsFrame member initialisation
    m_EdgesDisplayOption = NULL;
    m_TextDisplayOption = NULL;
    m_IsShowPadFill = NULL;
    m_IsShowPadNum = NULL;
////@end KiDisplayOptionsFrame member initialisation

////@begin KiDisplayOptionsFrame creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end KiDisplayOptionsFrame creation
    return true;
}

/*!
 * Control creation for KiDisplayOptionsFrame
 */

void KiDisplayOptionsFrame::CreateControls()
{    
	SetFont(*g_DialogFont);

////@begin KiDisplayOptionsFrame content construction
    // Generated by DialogBlocks, 24/10/2007 22:11:46 (unregistered)

    KiDisplayOptionsFrame* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxGROW|wxALL, 5);

    wxArrayString m_EdgesDisplayOptionStrings;
    m_EdgesDisplayOptionStrings.Add(_("&Line"));
    m_EdgesDisplayOptionStrings.Add(_("&Filled"));
    m_EdgesDisplayOptionStrings.Add(_("&Sketch"));
    m_EdgesDisplayOption = new wxRadioBox( itemDialog1, EDGE_SELECT, _("Edges:"), wxDefaultPosition, wxDefaultSize, m_EdgesDisplayOptionStrings, 1, wxRA_SPECIFY_COLS );
    m_EdgesDisplayOption->SetSelection(0);
    itemBoxSizer3->Add(m_EdgesDisplayOption, 0, wxGROW|wxALL, 5);

    wxArrayString m_TextDisplayOptionStrings;
    m_TextDisplayOptionStrings.Add(_("&Line"));
    m_TextDisplayOptionStrings.Add(_("&Filled"));
    m_TextDisplayOptionStrings.Add(_("&Sketch"));
    m_TextDisplayOption = new wxRadioBox( itemDialog1, TEXT_SELECT, _("Texts:"), wxDefaultPosition, wxDefaultSize, m_TextDisplayOptionStrings, 1, wxRA_SPECIFY_COLS );
    m_TextDisplayOption->SetSelection(0);
    itemBoxSizer3->Add(m_TextDisplayOption, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer3->Add(itemBoxSizer6, 0, wxGROW|wxALL, 5);

    m_IsShowPadFill = new wxCheckBox( itemDialog1, PADFILL_OPT, _("&Pad Filled"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_IsShowPadFill->SetValue(false);
    itemBoxSizer6->Add(m_IsShowPadFill, 0, wxGROW|wxALL, 5);

    m_IsShowPadNum = new wxCheckBox( itemDialog1, PADNUM_OPT, _("Display Pad &Num"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_IsShowPadNum->SetValue(false);
    if (ShowToolTips())
        m_IsShowPadNum->SetToolTip(_("Display pad number"));
    itemBoxSizer6->Add(m_IsShowPadNum, 0, wxGROW|wxALL, 5);

    itemBoxSizer6->Add(5, 5, 0, wxGROW|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxButton* itemButton10 = new wxButton( itemDialog1, ID_SAVE_CONFIG, _("Save Cfg"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemButton10, 0, wxGROW|wxALL, 5);

    wxStaticLine* itemStaticLine11 = new wxStaticLine( itemDialog1, ID_STATICLINE1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer2->Add(itemStaticLine11, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer12 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer12, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* itemButton13 = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton13->SetDefault();
    itemButton13->SetForegroundColour(wxColour(255, 0, 0));
    itemBoxSizer12->Add(itemButton13, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton14 = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton14->SetForegroundColour(wxColour(0, 0, 255));
    itemBoxSizer12->Add(itemButton14, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton15 = new wxButton( itemDialog1, wxID_APPLY, _("&Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer12->Add(itemButton15, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Set validators
    m_EdgesDisplayOption->SetValidator( wxGenericValidator(& DisplayOpt.DisplayModEdge) );
    m_TextDisplayOption->SetValidator( wxGenericValidator(& DisplayOpt.DisplayModText) );
    m_IsShowPadFill->SetValidator( wxGenericValidator(& DisplayOpt.DisplayPadFill) );
    m_IsShowPadNum->SetValidator( wxGenericValidator(& DisplayOpt.DisplayPadNum) );
////@end KiDisplayOptionsFrame content construction
}

/*!
 * Should we show tooltips?
 */

bool KiDisplayOptionsFrame::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap KiDisplayOptionsFrame::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin KiDisplayOptionsFrame bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end KiDisplayOptionsFrame bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon KiDisplayOptionsFrame::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin KiDisplayOptionsFrame icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end KiDisplayOptionsFrame icon retrieval
}

/*!
 * Update settings related to edges, text strings, and pads
 */

void KiDisplayOptionsFrame::UpdateObjectSettings(void)
{
    // Update settings
////@begin KiDisplayOptionsFrame update settings
	DisplayOpt.DisplayModEdge = m_Parent->m_DisplayModEdge =
		m_EdgesDisplayOption->GetSelection();

	DisplayOpt.DisplayModText = m_Parent->m_DisplayModText = 
		m_TextDisplayOption->GetSelection();

    DisplayOpt.DisplayPadNum = m_Parent->m_DisplayPadNum =
        m_IsShowPadNum->GetValue();

	DisplayOpt.DisplayPadFill = m_Parent->m_DisplayPadFill =
		m_IsShowPadFill->GetValue();

	m_Parent->ReDrawPanel();
////@end KiDisplayOptionsFrame update settings
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SAVE_CONFIG
 */

void KiDisplayOptionsFrame::OnSaveConfigClick( wxCommandEvent& event )
{
	Save_Config(this);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void KiDisplayOptionsFrame::OnOkClick( wxCommandEvent& event )
{
	UpdateObjectSettings();
    EndModal( 1 );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void KiDisplayOptionsFrame::OnCancelClick( wxCommandEvent& event )
{
    EndModal( -1 );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
 */

void KiDisplayOptionsFrame::OnApplyClick( wxCommandEvent& event )
{
	UpdateObjectSettings();
}



/*!
 * wxEVT_COMMAND_RADIOBOX_SELECTED event handler for EDGE_SELECT
 */

void KiDisplayOptionsFrame::OnEdgeSelectSelected( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_RADIOBOX_SELECTED event handler for EDGE_SELECT in KiDisplayOptionsFrame.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_RADIOBOX_SELECTED event handler for EDGE_SELECT in KiDisplayOptionsFrame. 
}


/*!
 * wxEVT_COMMAND_RADIOBOX_SELECTED event handler for TEXT_SELECT
 */

void KiDisplayOptionsFrame::OnTextSelectSelected( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_RADIOBOX_SELECTED event handler for TEXT_SELECT in KiDisplayOptionsFrame.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_RADIOBOX_SELECTED event handler for TEXT_SELECT in KiDisplayOptionsFrame. 
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for PADFILL_OPT
 */

void KiDisplayOptionsFrame::OnPadfillOptClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_CHECKBOX_CLICKED event handler for PADFILL_OPT in KiDisplayOptionsFrame.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_CHECKBOX_CLICKED event handler for PADFILL_OPT in KiDisplayOptionsFrame. 
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for PADNUM_OPT
 */

void KiDisplayOptionsFrame::OnPadnumOptClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_CHECKBOX_CLICKED event handler for PADNUM_OPT in KiDisplayOptionsFrame.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_CHECKBOX_CLICKED event handler for PADNUM_OPT in KiDisplayOptionsFrame. 
}

