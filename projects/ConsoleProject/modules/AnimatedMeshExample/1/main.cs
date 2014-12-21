function AnimatedMeshExample::create(%this)
{     
    SysGUI::beginScrollArea("Test Window!", 2, 2, 200, 200);
    SysGUI::separator();
    SysGUI::textInput("test val");
    SysGUI::label("Test Label!");
    SysGUI::label("Test Label2!");
    SysGUI::separator();
    SysGUI::checkBox("Scripted CheckBox", false);
    SysGUI::checkBox("Scripted CheckBox 2", true);
    SysGUI::separator();
    SysGUI::slider("SLIDAH!", 50, 0, 100);
    SysGUI::slider("ANOTHER SLIDAH!", 7, 0, 10);
    SysGUI::separator();
    SysGUI::endScrollArea();

    SysGUI::beginScrollArea("Test Window 2!", 2, 300, 200, 200);
    SysGUI::separator();
    SysGUI::label("Test Label!");
    SysGUI::label("Test Label2!");
    SysGUI::separator();
    %testID = SysGUI::checkBox("Scripted CheckBox", false);
    SysGUI::checkBox("Scripted CheckBox 2", true);
    SysGUI::separator();
    SysGUI::slider("SLIDAH!", 50, 0, 100);
    SysGUI::slider("ANOTHER SLIDAH!", 7, 0, 10);
    SysGUI::separator();
    SysGUI::endScrollArea();

    SysGUI::setLabelValue(%testID, "WOO IT WORKED!");

    /*%light1 = new SceneEntity();
    %light1.template = "./entities/lightTest2.taml";
    %light1.position = "0 0 0";
    Scene::addEntity(%light1);

    %light2 = new SceneEntity();
    %light2.template = "./entities/lightTest.taml";
    %light2.position = "0 0 -80";
    Scene::addEntity(%light2);

    %entity1 = new SceneEntity();
    %entity1.template = "./entities/bigDwarfRedDwarf.taml";
    %entity1.position = "0 0 0";
    %entity1.rotation = "0 3.14 0";
    Scene::addEntity(%entity1);

    %entity2 = new SceneEntity();
    %entity2.template = "./entities/bigDwarfRedDwarf.taml";
    %entity2.position = "0 0 -80";
    Scene::addEntity(%entity2);

    %light3 = new SceneEntity();
    %light3.template = "./entities/lightTest2.taml";
    %light3.position = "0 0 -160";
    Scene::addEntity(%light3);

    %light4 = new SceneEntity();
    %light4.template = "./entities/lightTest.taml";
    %light4.position = "0 0 -240";
    Scene::addEntity(%light4);

    %entity3 = new SceneEntity();
    %entity3.template = "./entities/bigDwarfRedDwarf.taml";
    %entity3.position = "0 0 -160";
    %entity3.rotation = "0 3.14 0";
    Scene::addEntity(%entity3);

    %light5 = new SceneEntity();
    %light5.template = "./entities/lightTest2.taml";
    %light5.position = "0 80 0";
    Scene::addEntity(%light5);

    %light6 = new SceneEntity();
    %light6.template = "./entities/lightTest.taml";
    %light6.position = "0 80 -80";
    Scene::addEntity(%light6);

    %entity4 = new SceneEntity();
    %entity4.template = "./entities/bigDwarfRedDwarf.taml";
    %entity4.position = "0 0 -240";
    Scene::addEntity(%entity4);

    %light7 = new SceneEntity();
    %light7.template = "./entities/lightTest2.taml";
    %light7.position = "0 80 -160";
    Scene::addEntity(%light7);

    %light8 = new SceneEntity();
    %light8.template = "./entities/lightTest.taml";
    %light8.position = "0 80 -240";
    Scene::addEntity(%light8);
    */

    %entity1 = new SceneEntity();
    %entity1.template = "./entities/bigDwarfRedDwarf.taml";
    %entity1.position = "0 0 0";
    %entity1.rotation = "0 0 0";
    Scene::addEntity(%entity1);

    %light1 = new SceneEntity();
    %light1.template = "./entities/lightTest2.taml";
    %light1.position = "0 0 0";
    Scene::addEntity(%light1);

    %example_room = new SceneEntity();
    %example_room.template = "./entities/ExampleRoom.taml";
    %example_room.position = "0 -100 0";
    Scene::addEntity(%example_room);

    %cam = Scene::getCamera();
    %cam.bindMouse(true);
    %cam.setPosition("0 50 -150");
}

function AnimatedMeshExample::destroy( %this )
{
    // String to tell us the Console has been called to be destroyed.  
    echo("AnimatedMeshExample::destroy function called");
}
