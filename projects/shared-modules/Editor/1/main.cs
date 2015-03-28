function Editor::create(%this)
{   
    exec("./scripts/editor.cs");
    loadEditor();

    Plugins::load("./Editor.dll");
    SysGUI::setEnabled(true);
}

function Editor::destroy( %this )
{
    //
}