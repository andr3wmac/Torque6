$Editor::Cam::panX = "0";
$Editor::Cam::panY = "0";
$Editor::Cam::panZ = "0";
$Editor::Open = false;
$Editor::Camera = 0;

function loadEditor()
{
    echo("EDITOR LOADED BRO!");
    GlobalActionMap.bind( keyboard, "F11", ToggleEditor );

    new ActionMap(EditorControls);
    EditorControls.bind( keyboard, "w", editorMoveForward );
    EditorControls.bind( keyboard, "s", editorMoveBackward );
    EditorControls.bind( keyboard, "a", editorMoveLeft );
    EditorControls.bind( keyboard, "d", editorMoveRight );
    EditorControls.bind( keyboard, "q", editorMoveUp );
    EditorControls.bind( keyboard, "e", editorMoveDown );
    EditorControls.bind( keyboard, "r", editorRotateLeft );
    EditorControls.bind( keyboard, "f", editorRotateRight );
    EditorControls.bind( keyboard, "delete", editorDelete );
}

function ToggleEditor( %make )
{
    // Finish if being released.
    if ( !%make )
        return;

    if ( !$Editor::Open )
    {
        Editor::open();
        $Editor::Open = true;
        EditorControls.push();
        $Editor::Camera = Scene::getCamera("EditorCamera");
    } else {
        Editor::close();
        $Editor::Open = false;
        EditorControls.pop();
    }
}

function editorSetCameraPan()
{
    $Editor::Camera.setPanVelocity($Editor::Cam::panX @ " " @ $Editor::Cam::panY @ " " @ $Editor::Cam::panZ);
}

function editorDelete ( %val )
{
    if ( %val )
    {
         Editor::deleteKey();
    }
}

function editorMoveForward( %val )
{
    echo("OH SHIT!");
    if ( %val )
        $Editor::Cam::panZ = "-5";
    else
        $Editor::Cam::panZ = "0";

    editorSetCameraPan();
}

function editorMoveBackward( %val )
{
    if ( %val )
        $Editor::Cam::panZ = "5";
    else
        $Editor::Cam::panZ = "0";

    editorSetCameraPan();
}

function editorMoveLeft( %val )
{
    if ( %val )
        $Editor::Cam::panX = "5";
    else
        $Editor::Cam::panX = "0";

    editorSetCameraPan();
}

function editorMoveRight( %val )
{
    if ( %val )
        $Editor::Cam::panX = "-5";
    else
        $Editor::Cam::panX = "0";

    editorSetCameraPan();
}

function editorRotateLeft( %val )
{
    if ( %val )
        $Editor::Camera.rotate("0 0.1 0");
}

function editorRotateRight( %val )
{
    if ( %val )
        $Editor::Camera.rotate("0 -0.1 0");
}

function editorMoveUp( %val )
{
    if ( %val )
        $Editor::Camera.translate("0 1 0");
}

function editorMoveDown( %val )
{
    if ( %val )
        $Editor::Camera.translate("0 -1 0");
}