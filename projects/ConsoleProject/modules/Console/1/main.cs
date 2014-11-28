function Console::create(%this)
{   
    // Load additional script files.
    exec("./scripts/console.cs");
    exec("./gui/guiProfiles.cs");

    // Load and configure the console.
    Console.add( TamlRead("./gui/ConsoleDialog.gui.taml") );
    GlobalActionMap.bind( keyboard, "ctrl tilde", toggleConsole );

    // Just a string to notify us the Console has been called to start.
    echo("@@@ Console::create function called");    
}

function Console::destroy( %this )
{
    // String to tell us the Console has been called to be destroyed.  
    echo("@@@ Console::destroy function called");
}
