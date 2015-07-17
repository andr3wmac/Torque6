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

#include "renderable.h"

//------------------------------------------------------------------------------

Renderable::Renderable() :
    mRender( false )
{
    // We should always start with processing of ticks off!
}

//------------------------------------------------------------------------------

Renderable::~Renderable()
{
    setRendering( false );
}

//------------------------------------------------------------------------------

Vector<Renderable *>& Renderable::getRenderList()
{
   // This helps to avoid the static initialization order fiasco
   static Vector<Renderable *> smRenderList; ///< List of tick controls
   return smRenderList;
}

//------------------------------------------------------------------------------

void Renderable::setRendering( bool render /* = true  */ )
{
    // Ignore no change.
    if ( render == mRender )
        return;

    // Update tick flag.
    mRender = render;

    // Are we processing ticks?
    if ( mRender )
    {
        // Yes, so add to process list.
        getRenderList().push_back( this );
        return;
    }

    // No, so remove from process list.
    for( RenderableListIterator i = getRenderList().begin(); i != getRenderList().end(); i++ )
    {
        if( (*i) == this )
        {
            getRenderList().erase( i );
            return;
        }
    }
}

void Renderable::preRenderAll()
{
    // Fetch a copy of the process list.
    Vector<Renderable*> renderList = getRenderList();

    // Pre-Render
    for( RenderableListIterator i = renderList.begin(); i != renderList.end(); i++ )
    {
        (*i)->preRender();
    }
}

void Renderable::renderAll()
{
    // Fetch a copy of the process list.
    Vector<Renderable*> renderList = getRenderList();

    // Render
    for( RenderableListIterator i = renderList.begin(); i != renderList.end(); i++ )
    {
        (*i)->render();
    }
}

void Renderable::postRenderAll()
{
    // Fetch a copy of the process list.
    Vector<Renderable*> renderList = getRenderList();

    // Post-Render
    for( RenderableListIterator i = renderList.begin(); i != renderList.end(); i++ )
    {
        (*i)->postRender();
    }
}