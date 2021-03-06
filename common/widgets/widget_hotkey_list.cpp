/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2016 Chris Pavlina <pavlina.chris@gmail.com>
 * Copyright (C) 2016-2019 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include <cctype>

#include <widgets/widget_hotkey_list.h>

#include <wx/statline.h>

#include <bitmaps.h>
#include <confirm.h>
#include <dialog_shim.h>
#include <draw_frame.h>

#include <panel_hotkeys_editor.h>


/**
 * Minimum width of the hotkey column
 */
static const int HOTKEY_MIN_WIDTH = 100;


/**
 * Menu IDs for the hotkey context menu
 */
enum ID_WHKL_MENU_IDS
{
    ID_EDIT = 2001,
    ID_RESET,
    ID_DEFAULT,
    ID_RESET_ALL,
    ID_DEFAULT_ALL,
};


/**
 * Class WIDGET_HOTKEY_CLIENT_DATA
 * Stores the hotkey change data associated with each row. To change a
 * hotkey, edit it via GetCurrentValue() in the row's client data, then call
 * WIDGET_HOTKEY_LIST::UpdateFromClientData().
 */
class WIDGET_HOTKEY_CLIENT_DATA : public wxClientData
{
    CHANGED_HOTKEY&  m_changed_hotkey;

public:
    WIDGET_HOTKEY_CLIENT_DATA( CHANGED_HOTKEY& aChangedHotkey )
        :   m_changed_hotkey( aChangedHotkey )
    {}

    CHANGED_HOTKEY& GetChangedHotkey() { return m_changed_hotkey; }
};


/**
 * Class HK_PROMPT_DIALOG
 * Dialog to prompt the user to enter a key.
 */
class HK_PROMPT_DIALOG : public DIALOG_SHIM
{
    wxKeyEvent m_event;

public:
    HK_PROMPT_DIALOG( wxWindow* aParent, wxWindowID aId, const wxString& aTitle,
            const wxString& aName, const wxString& aCurrentKey, const bool aValidKey,
            const wxString& aValidMessage )
            : DIALOG_SHIM( aParent, aId, aTitle, wxDefaultPosition, wxDefaultSize )
    {
        wxPanel* panel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize );
        wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );

        /* Dialog layout:
         *
         * valid_img   valid_label...........
         * ----------------------------------
         *
         * inst_label........................
         * ----------------------------------
         *
         * cmd_label_0      cmd_label_1         \
         *                                      | fgsizer
         * key_label_0      key_label_1         /
         */

        // If there is a validity error, display the error message to the user
        wxBoxSizer* valid_sizer = new wxBoxSizer( wxHORIZONTAL );
        if( !aValidKey )
        {
            wxStaticBitmap* valid_img = new wxStaticBitmap(
                    panel, wxID_ANY, KiBitmap( cancel_xpm ), wxDefaultPosition, wxDefaultSize, 0 );
            valid_sizer->Add( valid_img, 0, wxALL, 5 );

            wxStaticText* valid_label = new wxStaticText( panel, wxID_ANY, wxEmptyString,
                    wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL );

            valid_label->SetLabelText( aValidMessage );
            valid_sizer->Add( valid_label, 0, wxALL, 5 );

            // Add the validity text to the main sizer
            sizer->Add( valid_sizer, 0, wxALL, 5 );
            sizer->Add( new wxStaticLine( panel ), 0, wxALL | wxEXPAND, 2 );
        }

        wxStaticText* inst_label = new wxStaticText( panel, wxID_ANY, wxEmptyString,
                wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL );

        inst_label->SetLabelText( _( "Press a new hotkey, or press Esc to cancel..." ) );
        sizer->Add( inst_label, 0, wxALL, 5 );

        sizer->Add( new wxStaticLine( panel ), 0, wxALL | wxEXPAND, 2 );

        wxFlexGridSizer* fgsizer = new wxFlexGridSizer( 2 );

        wxStaticText* cmd_label_0 = new wxStaticText( panel, wxID_ANY, _( "Command:" ) );
        fgsizer->Add( cmd_label_0, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 5 );

        wxStaticText* cmd_label_1 = new wxStaticText( panel, wxID_ANY, wxEmptyString );
        cmd_label_1->SetFont( cmd_label_1->GetFont().Bold() );
        cmd_label_1->SetLabel( aName );
        fgsizer->Add( cmd_label_1, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 5 );

        wxStaticText* key_label_0 = new wxStaticText( panel, wxID_ANY, _( "Current key:" ) );
        fgsizer->Add( key_label_0, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 5 );

        wxStaticText* key_label_1 = new wxStaticText( panel, wxID_ANY, wxEmptyString );
        key_label_1->SetFont( key_label_1->GetFont().Bold() );
        key_label_1->SetLabel( aCurrentKey );
        fgsizer->Add( key_label_1, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 5 );

        sizer->Add( fgsizer, 1, wxEXPAND );

        // Wrap the sizer in a second to give a larger border around the whole dialog
        wxBoxSizer* outer_sizer = new wxBoxSizer( wxVERTICAL );
        outer_sizer->Add( sizer, 0, wxALL | wxEXPAND, 10 );
        panel->SetSizer( outer_sizer );

        Layout();
        outer_sizer->Fit( this );
        Center();

        SetMinClientSize( GetClientSize() );

        // Binding both EVT_CHAR and EVT_CHAR_HOOK ensures that all key events,
        // including specials like Tab and Return, are received, particularly
        // on MSW.
        panel->Bind( wxEVT_CHAR, &HK_PROMPT_DIALOG::OnChar, this );
        panel->Bind( wxEVT_CHAR_HOOK, &HK_PROMPT_DIALOG::OnCharHook, this );
    }


    void OnCharHook( wxKeyEvent& aEvent )
    {
        // On certain platforms, EVT_CHAR_HOOK is the only handler that receives
        // certain "special" keys. However, it doesn't always receive "normal"
        // keys correctly. For example, with a US keyboard, it sees ? as shift+/.
        //
        // Untangling these incorrect keys would be too much trouble, so we bind
        // both events, and simply skip the EVT_CHAR_HOOK if it receives a
        // "normal" key.

        const enum wxKeyCode skipped_keys[] =
        {
            WXK_NONE,    WXK_SHIFT,  WXK_ALT, WXK_CONTROL, WXK_CAPITAL,
            WXK_NUMLOCK, WXK_SCROLL, WXK_RAW_CONTROL
        };

        int key = aEvent.GetKeyCode();

        for( size_t i = 0; i < sizeof( skipped_keys ) / sizeof( skipped_keys[0] ); ++i )
        {
            if( key == skipped_keys[i] )
                return;
        }

        if( key <= 255 && isprint( key ) && !isspace( key ) )
        {
            // Let EVT_CHAR handle this one
            aEvent.DoAllowNextEvent();

            // On Windows, wxEvent::Skip must NOT be called.
            // On Linux and OSX, wxEvent::Skip MUST be called.
            // No, I don't know why.
#ifndef __WXMSW__
            aEvent.Skip();
#endif
        }
        else
        {
            OnChar( aEvent );
        }
    }


    void OnChar( wxKeyEvent& aEvent )
    {
        int keyCode = WIDGET_HOTKEY_LIST::MapKeypressToKeycode( aEvent );

        // Test for if the key is valid
        wxString validMsg;
        if( HOTKEY_STORE::CheckKeyValidity( keyCode, validMsg ) )
        {
            // Valid key, close the window and return
            m_event = aEvent;
            EndFlexible( wxID_OK );
        }
        else
        {
            // Invalid key, tell the user
            DisplayErrorMessage( this, validMsg, wxEmptyString );
        }
    }


    /**
     * End the dialog whether modal or quasimodal
     */
    void EndFlexible( int aRtnCode )
    {
        if( IsQuasiModal() )
            EndQuasiModal( aRtnCode );
        else
            EndModal( aRtnCode );
    }


    static wxKeyEvent PromptForKey( wxWindow* aParent, const wxString& aName,
            const wxString& aCurrentKey, const wxString& aValidMessage, const bool aValidKey )
    {
        HK_PROMPT_DIALOG dialog( aParent, wxID_ANY, _( "Set Hotkey" ), aName, aCurrentKey,
                aValidKey, aValidMessage );

        if( dialog.ShowModal() == wxID_OK )
        {
            return dialog.m_event;
        }
        else
        {
            wxKeyEvent dummy;
            return dummy;
        }
    }
};


/**
 * Class HOTKEY_FILTER
 *
 * Class to manage logic for filtering hotkeys based on user input
 */
class HOTKEY_FILTER
{
public:
    HOTKEY_FILTER( const wxString& aFilterStr )
    {
        m_normalised_filter_str = aFilterStr.Upper();
        m_valid = m_normalised_filter_str.size() > 0;
    }


    /**
     * Method FilterMatches
     *
     * Checks if the filter matches the given hotkey
     *
     * @return true on match (or if filter is disabled)
     */
    bool FilterMatches( const EDA_HOTKEY& aHotkey ) const
    {
        if( !m_valid )
            return true;

        // Match in the (translated) filter string
        const auto normedInfo = wxGetTranslation( aHotkey.m_InfoMsg ).Upper();
        if( normedInfo.Contains( m_normalised_filter_str ) )
            return true;

        const wxString keyName = KeyNameFromKeyCode( aHotkey.m_KeyCode );
        if( keyName.Upper().Contains( m_normalised_filter_str ) )
            return true;

        return false;
    }

private:

    bool m_valid;
    wxString m_normalised_filter_str;
};


WIDGET_HOTKEY_CLIENT_DATA* WIDGET_HOTKEY_LIST::GetHKClientData( wxTreeListItem aItem )
{
    if( aItem.IsOk() )
    {
        wxClientData* data = GetItemData( aItem );

        if( !data )
        {
            return NULL;
        }
        else
        {
            return static_cast<WIDGET_HOTKEY_CLIENT_DATA*>( data );
        }
    }
    else
    {
        return NULL;
    }
}


WIDGET_HOTKEY_CLIENT_DATA* WIDGET_HOTKEY_LIST::GetSelHKClientData()
{
    return GetHKClientData( GetSelection() );
}


WIDGET_HOTKEY_CLIENT_DATA* WIDGET_HOTKEY_LIST::getExpectedHkClientData( wxTreeListItem aItem )
{
    const auto hkdata = GetHKClientData( aItem );

    // This probably means a hotkey-only action is being attempted on
    // a row that is not a hotkey (like a section heading)
    wxASSERT_MSG( hkdata != nullptr, "No hotkey data found for list item" );

    return hkdata;
}


void WIDGET_HOTKEY_LIST::UpdateFromClientData()
{
    // Run a validity check on the hotkey store before updating
    m_hk_store.TestStoreValidity();

    for( wxTreeListItem i = GetFirstItem(); i.IsOk(); i = GetNextItem( i ) )
    {
        WIDGET_HOTKEY_CLIENT_DATA* hkdata = GetHKClientData( i );

        if( hkdata )
        {
            const auto& changed_hk = hkdata->GetChangedHotkey();
            const EDA_HOTKEY& hk = changed_hk.GetCurrentValue();

            wxString key_text = KeyNameFromKeyCode( hk.m_KeyCode );

            // mark unsaved changes
            if( changed_hk.HasUnsavedChange() )
                key_text += " *";

            SetItemText( i, 0, wxGetTranslation( hk.m_InfoMsg ) );
            SetItemText( i, 1, key_text);

            // Add the image to the column if the item is invalid
            if( changed_hk.IsValid() )
                SetItemImage( i, NO_IMAGE, NO_IMAGE );
            else
                SetItemImage( i, 0, NO_IMAGE );
        }
    }

    // Trigger a resize in case column widths have changed
    wxSizeEvent dummy_evt;
    TWO_COLUMN_TREE_LIST::OnSize( dummy_evt );

    // Update the panel's error message if it exists
    if( m_parentPanel )
        m_parentPanel->UpdateErrorMessage();
}


void WIDGET_HOTKEY_LIST::changeHotkey( CHANGED_HOTKEY& aHotkey, long aKey )
{
    // See if this key code is handled in hotkeys names list
    bool exists;
    KeyNameFromKeyCode( aKey, &exists );

    auto& curr_hk = aHotkey.GetCurrentValue();

    if( exists && curr_hk.m_KeyCode != aKey )
    {
        const auto& tag = aHotkey.GetSectionTag();
        bool can_update = ResolveKeyConflicts( aKey, tag );

        if( can_update )
        {
            curr_hk.m_KeyCode = aKey;
        }
    }
}


void WIDGET_HOTKEY_LIST::EditItem( wxTreeListItem aItem )
{
    WIDGET_HOTKEY_CLIENT_DATA* hkdata = getExpectedHkClientData( aItem );

    if( !hkdata )
        return;

    wxString    name = GetItemText( aItem, 0 );
    wxString    current_key = GetItemText( aItem, 1 );
    wxString    valid_msg;

    bool       valid_key = hkdata->GetChangedHotkey().IsValid( valid_msg );
    wxKeyEvent key_event =
            HK_PROMPT_DIALOG::PromptForKey( GetParent(), name, current_key, valid_msg, valid_key );
    long key = MapKeypressToKeycode( key_event );

    if( key )
    {
        changeHotkey( hkdata->GetChangedHotkey(), key );
        UpdateFromClientData();
    }
}


void WIDGET_HOTKEY_LIST::ResetItem( wxTreeListItem aItem )
{
    WIDGET_HOTKEY_CLIENT_DATA* hkdata = getExpectedHkClientData( aItem );

    if( !hkdata )
        return;

    auto& changed_hk = hkdata->GetChangedHotkey();
    const auto& orig_hk = changed_hk.GetOriginalValue();

    changeHotkey( changed_hk, orig_hk.m_KeyCode );
    UpdateFromClientData();
}


void WIDGET_HOTKEY_LIST::ResetItemToDefault( wxTreeListItem aItem )
{
    WIDGET_HOTKEY_CLIENT_DATA* hkdata = getExpectedHkClientData( aItem );

    if( !hkdata )
        return;

    auto& changed_hk = hkdata->GetChangedHotkey();

    changeHotkey( changed_hk, changed_hk.GetCurrentValue().GetDefaultKeyCode() );
    UpdateFromClientData();
}


void WIDGET_HOTKEY_LIST::OnActivated( wxTreeListEvent& aEvent )
{
    EditItem( aEvent.GetItem() );
}


void WIDGET_HOTKEY_LIST::OnContextMenu( wxTreeListEvent& aEvent )
{
    // Save the active event for use in OnMenu
    m_context_menu_item = aEvent.GetItem();

    wxMenu menu;

    WIDGET_HOTKEY_CLIENT_DATA* hkdata = GetHKClientData( m_context_menu_item );

    // Some actions only apply if the row is hotkey data
    if( hkdata )
    {
        menu.Append( ID_EDIT, _( "Edit..." ) );
        menu.Append( ID_RESET, _( "Undo Changes" ) );
        menu.Append( ID_DEFAULT, _( "Restore Default" ) );
        menu.Append( wxID_SEPARATOR );
    }

    menu.Append( ID_RESET_ALL, _( "Undo All Changes" ) );
    menu.Append( ID_DEFAULT_ALL, _( "Restore All to Default" ) );

    PopupMenu( &menu );
}


void WIDGET_HOTKEY_LIST::OnMenu( wxCommandEvent& aEvent )
{
    switch( aEvent.GetId() )
    {
    case ID_EDIT:
        EditItem( m_context_menu_item );
        break;

    case ID_RESET:
        ResetItem( m_context_menu_item );
        break;

    case ID_DEFAULT:
        ResetItemToDefault( m_context_menu_item );
        break;

    case ID_RESET_ALL:
        ResetAllHotkeys( false );
        break;

    case ID_DEFAULT_ALL:
        ResetAllHotkeys( true );
        break;

    default:
        wxFAIL_MSG( wxT( "Unknown ID in context menu event" ) );
    }
}


bool WIDGET_HOTKEY_LIST::ResolveKeyConflicts( long aKey, const wxString& aSectionTag )
{
    EDA_HOTKEY* conflicting_key = nullptr;
    EDA_HOTKEY_CONFIG* conflicting_section = nullptr;

    m_hk_store.CheckKeyConflicts( aKey, aSectionTag, &conflicting_key, &conflicting_section );

    if( conflicting_key != nullptr )
    {
        wxString    info    = wxGetTranslation( conflicting_key->m_InfoMsg );
        wxString    msg     = wxString::Format(
                _( "\"%s\" is already assigned to \"%s\" in section \"%s\". Are you sure you want "
                   "to change its assignment?" ),
                KeyNameFromKeyCode( aKey ), GetChars( info ),
                *(conflicting_section->m_Title) );

        wxMessageDialog dlg( GetParent(), msg, _( "Confirm change" ), wxYES_NO | wxNO_DEFAULT );

        if( dlg.ShowModal() == wxID_YES )
        {
            // Reset the other hotkey
            conflicting_key->m_KeyCode = 0;
            UpdateFromClientData();
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return true;
    }
}


WIDGET_HOTKEY_LIST::WIDGET_HOTKEY_LIST(
        PANEL_HOTKEYS_EDITOR* aParent, HOTKEY_STORE& aHotkeyStore, bool aReadOnly )
        : TWO_COLUMN_TREE_LIST( aParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTL_SINGLE ),
          m_hk_store( aHotkeyStore ),
          m_readOnly( aReadOnly ),
          m_parentPanel( aParent )
{
    initializeElements();
}


WIDGET_HOTKEY_LIST::WIDGET_HOTKEY_LIST(
        wxWindow* aParent, HOTKEY_STORE& aHotkeyStore, bool aReadOnly )
        : TWO_COLUMN_TREE_LIST( aParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTL_SINGLE ),
          m_hk_store( aHotkeyStore ),
          m_readOnly( aReadOnly ),
          m_parentPanel( nullptr )
{
    initializeElements();
}


void WIDGET_HOTKEY_LIST::initializeElements()
{
    wxString command_header = _( "Command" );

    if( !m_readOnly )
        command_header << " " << _( "(double-click to edit)" );

    AppendColumn( command_header );
    AppendColumn( _( "Hotkey" ) );
    SetRubberBandColumn( 0 );
    SetClampedMinWidth( HOTKEY_MIN_WIDTH );

    // Add the bitmap image for invalid hotkey warning
    // bitmap size is not know (depending on it is built). So get it:
    wxIcon icon;
    icon.CopyFromBitmap( KiBitmap( cancel_xpm ) );
    m_imgList = new wxImageList( icon.GetSize().x, icon.GetSize().y, true, 1 );
    m_imgList->Add( icon );
    AssignImageList( m_imgList );

    if( !m_readOnly )
    {
        // The event only apply if the widget is in editable mode
        Bind( wxEVT_TREELIST_ITEM_ACTIVATED, &WIDGET_HOTKEY_LIST::OnActivated, this );
        Bind( wxEVT_TREELIST_ITEM_CONTEXT_MENU, &WIDGET_HOTKEY_LIST::OnContextMenu, this );
        Bind( wxEVT_MENU, &WIDGET_HOTKEY_LIST::OnMenu, this );
    }
}

void WIDGET_HOTKEY_LIST::ApplyFilterString( const wxString& aFilterStr )
{
    updateShownItems( aFilterStr );
}


void WIDGET_HOTKEY_LIST::ResetAllHotkeys( bool aResetToDefault )
{
    Freeze();

    // Reset all the hotkeys, not just the ones shown
    // Should not need to check conflicts, as the state we're about
    // to set to a should be consistent
    if( aResetToDefault )
    {
        m_hk_store.ResetAllHotkeysToDefault();
    }
    else
    {
        m_hk_store.ResetAllHotkeysToOriginal();
    }

    UpdateFromClientData();
    Thaw();

    // Update the panel's error message if it exists
    // Call here again since the freeze/thaw seems to disrupt the update
    // inside UpdateFromClientData
    if( m_parentPanel )
        m_parentPanel->UpdateErrorMessage();
}


bool WIDGET_HOTKEY_LIST::TransferDataToControl()
{
    updateShownItems( "" );
    return true;
}


void WIDGET_HOTKEY_LIST::updateShownItems( const wxString& aFilterStr )
{
    Freeze();
    DeleteAllItems();

    HOTKEY_FILTER filter( aFilterStr );

    for( auto& section: m_hk_store.GetSections() )
    {
        // Create parent tree item
        wxTreeListItem parent = AppendItem( GetRootItem(), section.m_name );

        for( auto& hotkey: section.m_hotkeys )
        {
            if( filter.FilterMatches( hotkey.GetCurrentValue() ) )
            {
                wxTreeListItem item = AppendItem( parent, wxEmptyString );
                SetItemData( item, new WIDGET_HOTKEY_CLIENT_DATA( hotkey ) );
            }
        }

        Expand( parent );
    }

    UpdateFromClientData();
    Thaw();

    // Update the panel's error message if it exists
    // Call here again since the freeze/thaw seems to disrupt the update
    // inside UpdateFromClientData
    if( m_parentPanel )
        m_parentPanel->UpdateErrorMessage();
}


bool WIDGET_HOTKEY_LIST::TransferDataFromControl()
{
    m_hk_store.SaveAllHotkeys();
    return true;
}


long WIDGET_HOTKEY_LIST::MapKeypressToKeycode( const wxKeyEvent& aEvent )
{
    long key = aEvent.GetKeyCode();

    if( key == WXK_ESCAPE )
    {
        return 0;
    }
    else
    {
        if( key >= 'a' && key <= 'z' )    // convert to uppercase
            key = key + ('A' - 'a');

        // Remap Ctrl A (=1+GR_KB_CTRL) to Ctrl Z(=26+GR_KB_CTRL)
        // to GR_KB_CTRL+'A' .. GR_KB_CTRL+'Z'
        if( aEvent.ControlDown() && key >= WXK_CONTROL_A && key <= WXK_CONTROL_Z )
            key += 'A' - 1;

        /* Disallow shift for keys that have two keycodes on them (e.g. number and
         * punctuation keys) leaving only the "letter keys" of A-Z.
         * Then, you can have, e.g. Ctrl-5 and Ctrl-% (GB layout)
         * and Ctrl-( and Ctrl-5 (FR layout).
         * Otherwise, you'd have to have to say Ctrl-Shift-5 on a FR layout
         */
        bool keyIsLetter = key >= 'A' && key <= 'Z';

        if( aEvent.ShiftDown() && ( keyIsLetter || key > 256 ) )
            key |= GR_KB_SHIFT;

        if( aEvent.ControlDown() )
            key |= GR_KB_CTRL;

        if( aEvent.AltDown() )
            key |= GR_KB_ALT;

        return key;
    }
}
