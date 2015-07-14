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
#include <wx/treectrl.h>

// UI generated from wxFormBuilder
#include "../Torque6EditorUI.h"

#include "profilerTreeModel.h"

// ----------------------------------------------------------------------------
// ProfilerTreeModel
// ----------------------------------------------------------------------------

ProfilerTreeModel::ProfilerTreeModel()
{
    m_root = new ProfilerTreeModelNode( NULL, "00-Console", 0, -1, -1 );
}

ProfilerTreeModelNode* ProfilerTreeModel::AddEntry( const wxString &name, int count, float time, float percent, ProfilerTreeModelNode* parentNode )
{
    if (!parentNode)
        parentNode = m_root;

    // 
    ProfilerTreeModelNode *child_node = new ProfilerTreeModelNode( parentNode, name, count, time, percent );
    parentNode->Append( child_node );

    // notify control
    wxDataViewItem child( (void*) child_node );
    wxDataViewItem parent( (void*) parentNode );
    ItemAdded( parent, child );

    return child_node;
}

void ProfilerTreeModel::Delete( const wxDataViewItem &item )
{
    ProfilerTreeModelNode *node = (ProfilerTreeModelNode*) item.GetID();
    if (!node)      // happens if item.IsOk()==false
        return;

    wxDataViewItem parent( node->GetParent() );
    if (!parent.IsOk())
    {
        wxASSERT(node == m_root);

        // don't make the control completely empty:
        wxLogError( "Cannot remove the root item!" );
        return;
    }

    node->GetParent()->GetChildren().Remove( node );

    // free the node
    delete node;

    // notify control
    ItemDeleted( parent, item );
}

int ProfilerTreeModel::Compare( const wxDataViewItem &item1, const wxDataViewItem &item2,
                               unsigned int column, bool ascending ) const
{
    wxASSERT(item1.IsOk() && item2.IsOk());
        // should never happen

    if (IsContainer(item1) && IsContainer(item2))
    {
        wxVariant value1, value2;
        GetValue( value1, item1, 0 );
        GetValue( value2, item2, 0 );

        wxString str1 = value1.GetString();
        wxString str2 = value2.GetString();
        int res = str1.Cmp( str2 );
        if (res) return res;

        // items must be different
        wxUIntPtr litem1 = (wxUIntPtr) item1.GetID();
        wxUIntPtr litem2 = (wxUIntPtr) item2.GetID();

        return litem1-litem2;
    }

    return wxDataViewModel::Compare( item1, item2, column, ascending );
}

void ProfilerTreeModel::GetValue( wxVariant &variant,
                                 const wxDataViewItem &item, unsigned int col ) const
{
    wxASSERT(item.IsOk());

    ProfilerTreeModelNode *node = (ProfilerTreeModelNode*) item.GetID();
    switch (col)
    {
    case 0:
        variant = node->m_name;
        break;
    case 1:
        variant = node->m_count;
        break;
    case 2:
        variant = node->m_time;
        break;
    case 3:
        variant = node->m_percent;
        break;

    default:
        wxLogError( "ProfilerTreeModel::GetValue: wrong column %d", col );
    }
}

bool ProfilerTreeModel::SetValue( const wxVariant &variant,
                                 const wxDataViewItem &item, unsigned int col )
{
    wxASSERT(item.IsOk());

    ProfilerTreeModelNode *node = (ProfilerTreeModelNode*) item.GetID();
    switch (col)
    {
        case 0:
            node->m_name = variant.GetString();
            return true;
        case 1:
            node->m_count = variant.GetInteger();
            return true;
        case 2:
            node->m_time = variant.GetDouble();
            return true;
        case 3:
            node->m_percent = variant.GetDouble();
            return true;

        default:
            wxLogError( "ProfilerTreeModel::SetValue: wrong column" );
    }
    return false;
}

bool ProfilerTreeModel::IsEnabled( const wxDataViewItem &item,
                                  unsigned int col ) const
{
    wxASSERT(item.IsOk());

    ProfilerTreeModelNode *node = (ProfilerTreeModelNode*) item.GetID();

    return true;
}

wxDataViewItem ProfilerTreeModel::GetParent( const wxDataViewItem &item ) const
{
    if (!item.IsOk())
        return wxDataViewItem(0);

    ProfilerTreeModelNode *node = (ProfilerTreeModelNode*) item.GetID();

    if (node == m_root)
        return wxDataViewItem(0);

    return wxDataViewItem( (void*) node->GetParent() );
}

bool ProfilerTreeModel::IsContainer( const wxDataViewItem &item ) const
{
    if (!item.IsOk())
        return true;

    ProfilerTreeModelNode *node = (ProfilerTreeModelNode*) item.GetID();
    return node->IsContainer();
}

unsigned int ProfilerTreeModel::GetChildren( const wxDataViewItem &parent,
                                            wxDataViewItemArray &array ) const
{
    ProfilerTreeModelNode *node = (ProfilerTreeModelNode*) parent.GetID();
    if (!node)
    {
        array.Add( wxDataViewItem( (void*) m_root ) );
        return 1;
    }

    if (node->GetChildCount() == 0)
    {
        return 0;
    }

    unsigned int count = node->GetChildren().GetCount();
    for (unsigned int pos = 0; pos < count; pos++)
    {
        ProfilerTreeModelNode *child = node->GetChildren().Item( pos );
        array.Add( wxDataViewItem( (void*) child ) );
    }

    return count;
}