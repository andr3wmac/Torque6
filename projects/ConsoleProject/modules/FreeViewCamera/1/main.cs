function FreeViewCamera::create(%this)
{     
    exec("./scripts/camera.cs");
    GlobalActionMap.bind( keyboard, "w", moveForward );
    GlobalActionMap.bind( keyboard, "s", moveBackward );
    GlobalActionMap.bind( keyboard, "a", moveLeft );
    GlobalActionMap.bind( keyboard, "d", moveRight );
    GlobalActionMap.bind( keyboard, "q", moveUp );
    GlobalActionMap.bind( keyboard, "e", moveDown );
    GlobalActionMap.bind( keyboard, "r", rotateLeft );
    GlobalActionMap.bind( keyboard, "f", rotateRight );

    %cam = Scene::getCamera();
    %cam.bindMouse(true);
    %cam.setPosition("0 50 -150");
}

function FreeViewCamera::destroy( %this )
{
    // String to tell us the Console has been called to be destroyed.  
    echo("AnimatedMeshExample::destroy function called");
}
