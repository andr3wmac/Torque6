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

#ifndef _PROFILER_TOOL_H_
#define _PROFILER_TOOL_H_

#ifndef _PROJECTMANAGER_H_
#include "../project/projectManager.h"
#endif

#ifndef __TORQUE6EDITORUI_H__
#include "../Torque6EditorUI.h"
#endif

#ifndef _WX_TREECTRL_H_BASE_
#include <wx/treectrl.h>
#endif

#ifndef _PROFILER_TREE_MODEL_H_
#include "profilerTreeModel.h"
#endif

#ifndef _PLUGINS_SHARED_H
#include <plugins/plugins_shared.h>
#endif

class ProfilerTool : public wxEvtHandler, public ProjectTool
{
   protected:
      ProfilerPanel*       mProfilerPanel;
      ProfilerTreeModel    mProfilerData;
      int                  mFrameCount;

   public:
      ProfilerTool();
      ~ProfilerTool();

      void processProfilerCachedData(ProfilerCachedData* data, ProfilerTreeModelNode* node);

      virtual void openTool();
      virtual void closeTool();

      virtual void OnButtonEvent( wxCommandEvent& evt );

      virtual void onProjectLoaded(wxString projectName, wxString projectPath);
      virtual void onProjectClosed();
};

#endif // _CONSOLE_TOOL_H_
