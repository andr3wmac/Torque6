
extern "C"{
   DLL_PUBLIC GuiGridControl* GuiGridControlCreateInstance()
   {
      return new GuiGridControl();
   }

   // C-TODO implement columns and rows
}