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

#ifndef _PROFILER_TREE_MODEL_H_
#define _PROFILER_TREE_MODEL_H_

#ifndef _PROJECTMANAGER_H_
#include "../project/projectManager.h"
#endif

#ifndef __TORQUE6EDITORUI_H__
#include "../Torque6EditorUI.h"
#endif

#ifndef _WX_TREECTRL_H_BASE_
#include <wx/treectrl.h>
#endif

// ----------------------------------------------------------------------------
// ProfilerTreeModelNode
// ----------------------------------------------------------------------------

class ProfilerTreeModelNode;
WX_DEFINE_ARRAY_PTR( ProfilerTreeModelNode*, ProfilerTreeModelNodePtrArray );

class ProfilerTreeModelNode
{
public:
    ProfilerTreeModelNode( ProfilerTreeModelNode* parent,
                          const wxString &title, int count, float time, float percent )
    {
        m_parent = parent;

        m_name = title;
        m_count = count;
        m_time = time;
        m_percent = percent;

        m_container = false;
    }

    ~ProfilerTreeModelNode()
    {
        // free all our children nodes
        size_t count = m_children.GetCount();
        for (size_t i = 0; i < count; i++)
        {
            ProfilerTreeModelNode *child = m_children[i];
            delete child;
        }
    }

    bool IsContainer() const
        { return m_container; }

    ProfilerTreeModelNode* GetParent()
        { return m_parent; }
    ProfilerTreeModelNodePtrArray& GetChildren()
        { return m_children; }
    ProfilerTreeModelNode* GetNthChild( unsigned int n )
        { return m_children.Item( n ); }
    void Insert( ProfilerTreeModelNode* child, unsigned int n)
        { m_children.Insert( child, n); }
    void Append( ProfilerTreeModelNode* child )
        { m_children.Add( child ); }
    unsigned int GetChildCount() const
        { return m_children.GetCount(); }

public:
    wxString               m_name;
    long                   m_count;
    float                  m_time;
    float                  m_percent;
    bool                   m_container;

private:
    ProfilerTreeModelNode          *m_parent;
    ProfilerTreeModelNodePtrArray   m_children;
};


// ----------------------------------------------------------------------------
// ProfilerTreeModel
// ----------------------------------------------------------------------------

class ProfilerTreeModel: public wxDataViewModel
{
public:
    ProfilerTreeModelNode*   m_root;

    ProfilerTreeModel();
    ~ProfilerTreeModel()
    {
        delete m_root;
    }

    // helper methods to change the model

    ProfilerTreeModelNode* AddEntry( const wxString &name, int count, float time, float percent, ProfilerTreeModelNode* parent = NULL );
    void Delete( const wxDataViewItem &item );

    // override sorting to always sort branches ascendingly

    int Compare( const wxDataViewItem &item1, const wxDataViewItem &item2,
                 unsigned int column, bool ascending ) const;

    // implementation of base class virtuals to define model

    virtual unsigned int GetColumnCount() const
    {
        return 6;
    }

    virtual wxString GetColumnType( unsigned int col ) const
    {
        if (col == 1)
            return wxT("long");

        if (col > 1)
            return wxT("double");

        return wxT("string");
    }

    virtual void GetValue( wxVariant &variant,
                           const wxDataViewItem &item, unsigned int col ) const;
    virtual bool SetValue( const wxVariant &variant,
                           const wxDataViewItem &item, unsigned int col );

    virtual bool IsEnabled( const wxDataViewItem &item,
                            unsigned int col ) const;

    virtual wxDataViewItem GetParent( const wxDataViewItem &item ) const;
    virtual bool IsContainer( const wxDataViewItem &item ) const;
    virtual unsigned int GetChildren( const wxDataViewItem &parent,
                                      wxDataViewItemArray &array ) const;
};

#endif // _PROFILER_TREE_MODEL_H_
