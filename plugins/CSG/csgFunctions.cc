//-----------------------------------------------------------------------------
// Copyright (c) 2015 Andrew Mac
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

#include <plugins/plugins_shared.h>
#include "csgFunctions.h"

static Graphics::PosUVNormalVertex s_cubeVertices[36] =
{
   // X+
   { 1.0f,  1.0f,  1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, },
   { 1.0f, -1.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, },
   { 1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, },
   { 1.0f,  1.0f, -1.0f, 1.0f,  0.0f, 1.0f, 0.0f, 0.0f, },
   { 1.0f, -1.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, },
   { 1.0f,  1.0f,  1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, },

   // X-
   { -1.0f,  1.0f,  1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, },
   { -1.0f, -1.0f, -1.0f, 1.0f,  0.0f, -1.0f, 0.0f, 0.0f, },
   { -1.0f,  1.0f, -1.0f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, },
   { -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, },
   { -1.0f, -1.0f, -1.0f, 1.0f,  0.0f, -1.0f, 0.0f, 0.0f, },
   { -1.0f,  1.0f,  1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, },

   // Y+
   {  1.0f,  1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, },
   { -1.0f,  1.0f, -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, 0.0f, },
   {  1.0f,  1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f, },
   { -1.0f,  1.0f,  1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, },
   { -1.0f,  1.0f, -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, 0.0f, },
   {  1.0f,  1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, },

   // Y-
   {  1.0f, -1.0f,  1.0f, 1.0f, -1.0f, 0.0f, -1.0f, 0.0f, },
   { -1.0f, -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, -1.0f, 0.0f, },
   { -1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, },
   {  1.0f, -1.0f, -1.0f, 1.0f,  0.0f, 0.0f, -1.0f, 0.0f, },
   { -1.0f, -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, -1.0f, 0.0f, },
   {  1.0f, -1.0f,  1.0f, 1.0f, -1.0f, 0.0f, -1.0f, 0.0f, },

   // Z+
   {  1.0f,  1.0f,  1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, },
   { -1.0f, -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f, },
   { -1.0f,  1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, },
   {  1.0f, -1.0f,  1.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f, },
   { -1.0f, -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f, },
   {  1.0f,  1.0f,  1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, },

   // Z-
   {  1.0f,  1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, },
   { -1.0f, -1.0f, -1.0f, 1.0f,  0.0f, 0.0f, 0.0f, -1.0f, },
   {  1.0f, -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 0.0f, -1.0f, },
   { -1.0f,  1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, },
   { -1.0f, -1.0f, -1.0f, 1.0f,  0.0f, 0.0f, 0.0f, -1.0f, },
   {  1.0f,  1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, }
};

// WARNING: these verts are pre-wound for csg.js
static const uint16_t s_cubeIndices[36] =
{
   2,  1,  0,  5,  4,  3,
   8,  7,  6,  11, 10, 9,
   14, 13, 12, 17, 16, 15,
   20, 19, 18, 23, 22, 21,
   26, 25, 24, 29, 28, 27,
   32, 31, 30, 35, 34, 33,
};

csgjs_vertex createVertex(F32 pos_x, F32 pos_y, F32 pos_z, F32 normal_x, F32 normal_y, F32 normal_z, F32 u, F32 v) 
{ 
   csgjs_vertex vert;
   vert.pos.x = pos_x;
   vert.pos.y = pos_y;
   vert.pos.z = pos_z;
   vert.normal.x = normal_x;
   vert.normal.y = normal_y;
   vert.normal.z = normal_z;
   vert.uv.x = u;
   vert.uv.y = v;
   vert.uv.z = 0.0f;
   return vert;
}

csgjs_model createCube(F32 pos_x, F32 pos_y, F32 pos_z)
{
   csgjs_model model;

   // verts
   for (U32 i = 0; i < 36; ++i)
      model.vertices.push_back(createVertex(pos_x + s_cubeVertices[i].m_x, pos_y + s_cubeVertices[i].m_y, pos_z + s_cubeVertices[i].m_z, 
         s_cubeVertices[i].m_normal_x, s_cubeVertices[i].m_normal_y, s_cubeVertices[i].m_normal_z,
         s_cubeVertices[i].m_u, s_cubeVertices[i].m_v));

   // indices
   for (U32 i = 0; i < 36; ++i)
      model.indices.push_back(s_cubeIndices[i]);

   return model;
}