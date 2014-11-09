#include "test3D.h"
#include "console/consoleInternal.h"
#include "graphics/shaders.h"
#include "graphics/TextureManager.h"

// bgfx/bx
#include <bgfx.h>
#include <bx/fpumath.h>

// assimp
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

// Vertex Layout
struct PosTexcoordVertex
{
	float m_x;
	float m_y;
	float m_z;
	float m_u;
	float m_v;

	static void init()
	{
		ms_decl
			.begin()
			.add(bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.end();
	}

	static bgfx::VertexDecl ms_decl;
};
bgfx::VertexDecl PosTexcoordVertex::ms_decl;

// Just Dwarf Things
Vector<PosTexcoordVertex> dwarf_verts;
Vector<uint16_t> dwarf_indicies;
bgfx::VertexBufferHandle dwarf_vbh;
bgfx::IndexBufferHandle dwarf_ibh;
Shader* dwarf_shader = NULL;
TextureHandle dwarf_texture = NULL;

void test3DRender(U32 width, U32 height)
{
   if ( dwarf_shader == NULL )
   {
      // Use Assimp To Load Dwarf.x
      const struct aiScene* mScene = aiImportFile("models/dwarf.x", aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
      if ( mScene )
      {
         //Con::printf("[ASSIMP] Mesh Count: %d", mScene->mNumMeshes);
         //Con::printf("[ASSIMP] Material Count: %d", mScene->mNumMaterials);

         // Dwarf Mesh
         aiMesh* mMeshData = mScene->mMeshes[1];
         
         // Verts/Normals/Tangent/UVs
         for ( U32 n = 0; n < mMeshData->mNumVertices; ++n)
         {
            PosTexcoordVertex vert;

            // Verts
            aiVector3D pt = mMeshData->mVertices[n];
            vert.m_x = pt.x;
            vert.m_y = pt.y;
            vert.m_z = pt.z;

            // UVs
            if ( mMeshData->HasTextureCoords(0) )
            {
               vert.m_u = mMeshData->mTextureCoords[0][n].x;
               vert.m_v = mMeshData->mTextureCoords[0][n].y;
            }

            dwarf_verts.push_back(vert);
         }
         
         // Faces
         for ( U32 n = 0; n < mMeshData->mNumFaces; ++n)
         {
            const struct aiFace* face = &mMeshData->mFaces[n];
            if ( face->mNumIndices == 3 )
            {
               // Con::printf("[ASSIMP] Face: %d %d %d", face->mIndices[0], face->mIndices[1], face->mIndices[2]);
               dwarf_indicies.push_back(face->mIndices[0]);
               dwarf_indicies.push_back(face->mIndices[1]);
               dwarf_indicies.push_back(face->mIndices[2]);
            } else {
               Con::printf("[ASSIMP] Non-Triangle Face Found.");
            }
         }

         // Clean up.
         aiReleaseImport(mScene);
      }

      // Create vertex stream declaration.
	   PosTexcoordVertex::init();

	   // Create static vertex buffer.
	   dwarf_vbh = bgfx::createVertexBuffer(
		     bgfx::makeRef(&dwarf_verts[0], dwarf_verts.size() * sizeof(PosTexcoordVertex) )
		   , PosTexcoordVertex::ms_decl
		   );

	   // Create static index buffer.
	   dwarf_ibh = bgfx::createIndexBuffer(
		   bgfx::makeRef(&dwarf_indicies[0], dwarf_indicies.size() * sizeof(uint16_t) )
		   );

      // Dwarf Texture
      dwarf_texture = TextureHandle("models/dwarf.png", TextureHandle::TextureHandleType::BitmapTexture);

	   // Create program from shaders.
	   dwarf_shader = new Shader("shaders/dwarf_vs.sc", "shaders/dwarf_fs.sc");
   }

   // ---------------------
   // Clear Frame
   // ---------------------
   bgfx::setViewClear(0
		, BGFX_CLEAR_COLOR_BIT|BGFX_CLEAR_DEPTH_BIT
		, 0
		, 1.0f
		, 0
	);

   // Dummy submit to ensure viewport is cleared.
   bgfx::submit(0);

   // ---------------------
   // Camera Controls.
   // ---------------------
   F32 look_at[3]  =       { 0.0f, 35.0f,   0.0f };
   F32 eye_position[3] =   { 0.0f, 35.0f,   75.0f };

   float view[16];
	bx::mtxLookAt(view, eye_position, look_at);

	float proj[16];
	bx::mtxProj(proj, 60.0f, float(width)/float(height), 0.1f, 100.0f);
	bgfx::setViewTransform(0, view, proj);

	// Set view 0 default viewport.
	bgfx::setViewRect(0, 0, 0, width, height);

   // ---------------------
   // Draw Dwarf.
   // ---------------------
	float mtx[16];
	bx::mtxRotateXY(mtx, 0, 0);
	mtx[12] = 0.0f; // X Position
	mtx[13] = 0.0f; // Y Position
	mtx[14] = 0.0f; // Z Position

	// Set model matrix for rendering.
	bgfx::setTransform(mtx);

   // Set dwarf texture
   TextureObject* dwarf_texture_obj = (TextureObject*)dwarf_texture;
   bgfx::setTexture(0, Shader::u_texColor, dwarf_texture_obj->getBGFXTexture());

	// Set shaders and buffers.
   bgfx::setProgram(dwarf_shader->mProgram);
	bgfx::setVertexBuffer(dwarf_vbh);
	bgfx::setIndexBuffer(dwarf_ibh);

	// Set render states.
	bgfx::setState(BGFX_STATE_DEFAULT);

	// Submit primitive for rendering to view 0.
	bgfx::submit(0);
}