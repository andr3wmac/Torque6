function Editor::create(%this)
{   
    exec("./scripts/editor.cs");
    loadEditor();

    Plugins::load("/home/andrewmac/Code/Torque6/projects/shared-modules/Editor/1/libEditor.so");
    SysGUI::setEnabled(true);
}

function Editor::destroy( %this )
{
    //
}
