function Editor::create(%this)
{   
    exec("./scripts/editor.cs");
    loadEditor();

    Plugins::load("./Editor");
    SysGUI::setEnabled(true);
}

function Editor::destroy( %this )
{
    //
}
