//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
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

#ifndef _CONSOLE_H_
#include "console/console.h"
#endif

#ifndef _FORWARDRENDERING_H_
#include "renderers/forwardRendering.h"
#endif

ConsoleFunctionWithDocs( sceneAddEntity, ConsoleVoid, 1, 1, ())
{
   Con::printf("Loading Dwarf:");
   
   AssetPtr<MeshAsset> dwarfMeshAsset;
   dwarfMeshAsset.setAssetId("Console:dwarfMesh");
   if ( dwarfMeshAsset.isNull() )
      return;

	F32 mtx[16];
	bx::mtxRotateXY(mtx, 0, 0);
	mtx[12] = 0.0f; // X Position
	mtx[13] = 0.0f; // Y Position
	mtx[14] = 0.0f; // Z Position

   // Load Temp. Dwarf Shader
   Graphics::Shader* dwarfShader = new Graphics::Shader("shaders/dwarf_vs.sc", "shaders/dwarf_fs.sc");

   Scene::ForwardRenderComponent newComp;
   newComp.indexBuffer = dwarfMeshAsset->getIndexBuffer();
   newComp.vertexBuffer = dwarfMeshAsset->getVertexBuffer();
   newComp.shader = dwarfShader->mProgram;
   TextureObject* dwarf_texture_obj = (TextureObject*)dwarfMeshAsset->getTexture();
   newComp.texture = dwarf_texture_obj->getBGFXTexture();
   dMemcpy(newComp.transformMatrix, mtx, sizeof(mtx));
   
   Scene::forwardRenderList.push_back(newComp);
}

ConsoleFunctionWithDocs( sceneCameraLookAt, ConsoleVoid, 7, 7, (F32 eye_x, F32 eye_y, F32 eye_z, F32 look_x, F32 look_y, F32 look_z))
{
   F32 eye_x = dAtof(argv[1]);
   F32 eye_y = dAtof(argv[2]);
   F32 eye_z = dAtof(argv[3]);
   F32 look_x = dAtof(argv[4]);
   F32 look_y = dAtof(argv[5]);
   F32 look_z = dAtof(argv[6]);
   Scene::cameraLookAt(Point3F(eye_x, eye_y, eye_z), Point3F(look_x, look_y, look_z));
}