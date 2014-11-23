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

    exec("./scripts/camera.cs");
    GlobalActionMap.bind( keyboard, "w", moveForward );
    GlobalActionMap.bind( keyboard, "s", moveBackward );
    GlobalActionMap.bind( keyboard, "a", moveLeft );
    GlobalActionMap.bind( keyboard, "d", moveRight );
    GlobalActionMap.bind( keyboard, "q", moveUp );
    GlobalActionMap.bind( keyboard, "e", moveDown );
    GlobalActionMap.bind( keyboard, "r", rotateLeft );
    GlobalActionMap.bind( keyboard, "f", rotateRight );

    %entity1 = new SceneEntity();
    %entity1.template = "./testSceneEntity.taml";
    %entity1.position = "0 0 0";
    %entity1.rotation = "0 3.14 0";
    Scene::addEntity(%entity1);

    %entity2 = new SceneEntity();
    %entity2.template = "./testSceneEntity.taml";
    %entity2.position = "0 0 -80";
    Scene::addEntity(%entity2);

    %cam = Scene::getCamera();
    %cam.bindMouse(true);
    %cam.setPosition("0 50 -150");
}

function Console::destroy( %this )
{
    // String to tell us the Console has been called to be destroyed.  
    echo("@@@ Console::destroy function called");
}
