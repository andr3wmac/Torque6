//-----------------------------------------------------------------------------
// Copyright (c) 2014 Andrew Mac
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

$obstacleCount = 0;

function spawnObstacle(%pos)
{
    %ob = new SceneEntity();
    %ob.template = "CollisionExample:Obstacle";
    %ob.position = %pos;
    %ob.scale = "20 20 20";
    %ob.resetColor();
    %ob.resetColorTimer = -1;
    Scene::addEntity(%ob, "Obstacle" @ $obstacleCount);
    $obstacleCount = $obstacleCount + 1;
}

function spawnNewObstacle(%val)
{
    if ( %val )
    {
        spawnObstacle("0 100 0");
    }
}

function SceneEntity::removeSelf( %this )
{
    Scene::removeEntity(%this);
}

function SceneEntity::onCollide ( %this, %hit, %type )
{
    %this.findComponent("Cube").setUniformVec4("cubeColor", "0.2 1.0 0.2 0.0");
    cancel(%this.resetColorTimer);
    %this.resetColorTimer = %this.schedule(1000, "resetColor");
}

function SceneEntity::resetColor ( %this )
{
    %this.findComponent("Cube").setUniformVec4("cubeColor", "0.2 0.2 0.2 0.0");
}
