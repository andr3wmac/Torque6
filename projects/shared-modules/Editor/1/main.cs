function Editor::create(%this)
{   
    exec("./scripts/editor.cs");
    loadEditor();

    Plugins::load("./EditorDLL.dll");
    SysGUI::setEnabled(true);
}

function Editor::destroy( %this )
{
    //
}