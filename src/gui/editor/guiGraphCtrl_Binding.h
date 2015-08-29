//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
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

ConsoleMethod(GuiGraphCtrl, addDatum, void, 4, 4, "(int plotID, float v)"
   "Add a data point to the given plot.\n"
   "@return No return value.")
{
   S32 plotID = dAtoi(argv[2]);
   if (plotID > object->MaxPlots)
   {
      Con::errorf("Invalid plotID.");
      return;
   }
   object->addDatum(plotID, dAtof(argv[3]));
}

ConsoleMethod(GuiGraphCtrl, getDatum, F32, 4, 4, "(int plotID, int samples)"
   "Get a data point from the plot specified, samples from the start of the graph.\n"
   "@return Returns the data point from the plot.")
{
   S32 plotID = dAtoi(argv[2]);
   S32 samples = dAtoi(argv[3]);

   if (plotID > object->MaxPlots)
   {
      Con::errorf("Invalid plotID.");
      return -1;
   }
   if (samples > object->MaxDataPoints)
   {
      Con::errorf("Invalid sample.");
      return -1;
   }

   return object->getDatum(plotID, samples);
}

ConsoleMethod(GuiGraphCtrl, addAutoPlot, void, 5, 5, "(int plotID, string variable, int update)"
   "Adds a data point with value variable, every update ms.\n"
   "@return No return value.")
{
   S32 plotID = dAtoi(argv[2]);

   if (plotID > object->MaxPlots)
   {
      Con::errorf("Invalid plotID.");
      return;
   }

   object->addAutoPlot(plotID, argv[3], dAtoi(argv[4]));
}

ConsoleMethod(GuiGraphCtrl, removeAutoPlot, void, 3, 3, "(int plotID)"
   "Stops automatic pointing over set interval.\n"
   "@return No return value.")
{
   S32 plotID = dAtoi(argv[2]);

   if (plotID > object->MaxPlots)
   {
      Con::errorf("Invalid plotID.");
      return;
   }

   object->removeAutoPlot(plotID);
}

ConsoleMethod(GuiGraphCtrl, setGraphType, void, 4, 4, "(int plotID, string graphType)"
   "Change GraphType of plot plotID.\n"
   "@return No return value.")
{
   S32 plotID = dAtoi(argv[2]);

   if (plotID > object->MaxPlots)
   {
      Con::errorf("Invalid plotID.");
      return;
   }

   object->setGraphType(dAtoi(argv[2]), argv[3]);
}

ConsoleMethod(GuiGraphCtrl, matchScale, void, 3, GuiGraphCtrl::MaxPlots + 2, "(int plotID, int plotID, ...)"
   "Sets the scale of all specified plots to the maximum scale among them.\n"
   "@return No return value.")
{
   F32 Max = 0;
   for (int i = 0; i < (argc - 2); i++)
   {
      if (dAtoi(argv[2 + i]) > object->MaxPlots)
      {
         Con::errorf("Invalid plotID.");
         return;
      }
      if (object->mPlots[dAtoi(argv[2 + i])].mGraphMax > Max)
         Max = object->mPlots[dAtoi(argv[2 + i])].mGraphMax;
   }
   for (int i = 0; i < (argc - 2); i++)
      object->mPlots[dAtoi(argv[2 + i])].mGraphMax = Max;
}

extern "C"{
   DLL_PUBLIC GuiGraphCtrl* GuiGraphCtrlCreateInstance()
   {
      return new GuiGraphCtrl();
   }

   DLL_PUBLIC void GuiGraphCtrlAddDatum(GuiGraphCtrl* ctrl, S32 plotID, F32 v)
   {
      if (plotID > ctrl->MaxPlots)
      {
         Con::errorf("Invalid plotID.");
         return;
      }
      ctrl->addDatum(plotID, v);
   }

   DLL_PUBLIC F32 GuiGraphCtrlGetDatum(GuiGraphCtrl* ctrl, S32 plotID, S32 samples)
   {
      if (plotID > ctrl->MaxPlots)
      {
         Con::errorf("Invalid plotID.");
         return -1;
      }
      if (samples > ctrl->MaxDataPoints)
      {
         Con::errorf("Invalid sample.");
         return -1;
      }

      return ctrl->getDatum(plotID, samples);
   }

   DLL_PUBLIC void GuiGraphCtrlAddAutoPlot(GuiGraphCtrl* ctrl, S32 plotID, const char* variable, S32 update)
   {
      if (plotID > ctrl->MaxPlots)
      {
         Con::errorf("Invalid plotID.");
         return;
      }

      return ctrl->addAutoPlot(plotID, variable, update);
   }

   DLL_PUBLIC void GuiGraphCtrlRemoveAutoPlot(GuiGraphCtrl* ctrl, S32 plotID)
   {
      if (plotID > ctrl->MaxPlots)
      {
         Con::errorf("Invalid plotID.");
         return;
      }

      ctrl->removeAutoPlot(plotID);
   }

   DLL_PUBLIC void GuiGraphCtrlSetGraphType(GuiGraphCtrl* ctrl, S32 plotID, const char* graphType)
   {
      if (plotID > ctrl->MaxPlots)
      {
         Con::errorf("Invalid plotID.");
         return;
      }

      ctrl->setGraphType(plotID, graphType);
   }

   //CTODO - list
   DLL_PUBLIC void GuiGraphCtrlMatchScale(GuiGraphCtrl* ctrl, S32 argc, S32* argv)
   {
      F32 Max = 0;
      for (int i = 0; i < argc; i++)
      {
         if (argv[i] > ctrl->MaxPlots)
         {
            Con::errorf("Invalid plotID.");
            return;
         }
         if (ctrl->mPlots[argv[i]].mGraphMax > Max)
            Max = ctrl->mPlots[argv[i]].mGraphMax;
      }
      for (int i = 0; i < (argc - 2); i++)
         ctrl->mPlots[argv[i]].mGraphMax = Max;
   }
}