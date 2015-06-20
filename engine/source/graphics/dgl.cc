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

#include "math/mPoint.h"
#include "graphics/TextureManager.h"
#include "graphics/dgl.h"
#include "graphics/color.h"
#include "graphics/shaders.h"
#include "graphics/core.h"
#include "3d/rendering/common.h"
#include "math/mPoint.h"
#include "math/mRect.h"
#include "graphics/gFont.h"
#include "console/console.h"
#include "math/mMatrix.h"
#include "memory/frameAllocator.h"
#include "debug/profiler.h"
#include "string/unicode.h"

#include <bgfx.h>
#include <bx/bx.h>
#include <bx/fpumath.h>
#include <imgui/imgui.h>

#include "dglMac_ScriptBinding.h"
#include "dgl_ScriptBinding.h"

namespace {

ColorI sg_bitmapModulation(255, 255, 255, 255);
ColorI sg_textAnchorColor(255, 255, 255, 255);
ColorI sg_stackColor(255, 255, 255, 255);
RectI sgCurrentClipRect;

} // namespace {}

NVGcontext*                nvgContext = NULL;
Graphics::Shader*          dglGUIShader = NULL;
Graphics::Shader*          dglGUIColorShader = NULL;
Graphics::ViewTableEntry*  v_TorqueGUITop = NULL;

void dglInit()
{
   v_TorqueGUITop = Graphics::getView("TorqueGUITop", "SysGUI", true);
}

void dglDestroy()
{
   if ( nvgContext != NULL )
      nvgDelete(nvgContext);
}

//--------------------------------------------------------------------------
void dglSetBitmapModulation(const ColorF& in_rColor)
{
   ColorF c = in_rColor;
   c.clamp();
   sg_bitmapModulation = c;
   sg_textAnchorColor = sg_bitmapModulation;
}

void dglGetBitmapModulation(ColorF* color)
{
   *color = sg_bitmapModulation;
}

void dglGetBitmapModulation(ColorI* color)
{
   *color = sg_bitmapModulation;
}

void dglClearBitmapModulation()
{
   sg_bitmapModulation.set(255, 255, 255, 255);
}

void dglSetTextAnchorColor(const ColorF& in_rColor)
{
   ColorF c = in_rColor;
   c.clamp();
   sg_textAnchorColor = c;
}


//--------------------------------------------------------------------------
void dglDrawBitmapStretchSR(TextureObject* texture,
                       const RectI&   dstRect,
                       const RectI&   srcRect,
                       const U32   in_flip,
                       F32          fSpin,
                       bool            bSilhouette)
{   
   // TODO: I hate loading things this way, will clean up later.
   if ( dglGUIShader == NULL )
      dglGUIShader = Graphics::getShader("gui/gui_vs.sc", "gui/gui_fs.sc");

   AssertFatal(texture != NULL, "GSurface::drawBitmapStretchSR: NULL Handle");
   if(!dstRect.isValidRect())
      return;
   AssertFatal(srcRect.isValidRect() == true,
               "GSurface::drawBitmapStretchSR: routiin nes assume normal rects");
 
   dglScreenQuadSrc(dstRect.point.x, dstRect.point.y, dstRect.extent.x, dstRect.extent.y,
      srcRect.point.x, srcRect.point.y, srcRect.extent.x, srcRect.extent.y, texture->getTextureWidth(), texture->getTextureHeight());
   bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), texture->getBGFXTexture());
   bgfx::setState(BGFX_STATE_RGB_WRITE|BGFX_STATE_ALPHA_WRITE);
   bgfx::setProgram(dglGUIShader->mProgram);
   bgfx::submit(v_TorqueGUITop->id);
}

void dglDrawBitmap(TextureObject* texture, const Point2I& in_rAt, const U32 in_flip)
{
   AssertFatal(texture != NULL, "GSurface::drawBitmap: NULL Handle");

   // All non-StretchSR bitmaps are transformed into StretchSR calls...
   //
   RectI subRegion(0, 0,
                   texture->getBitmapWidth(),
                   texture->getBitmapHeight());
   RectI stretch(in_rAt.x, in_rAt.y,
                   texture->getBitmapWidth(),
                   texture->getBitmapHeight());
   dglDrawBitmapStretchSR(texture,
                          stretch,
                          subRegion,
                          in_flip);
}

void dglDrawBitmapTile(TextureObject* texture, const RectI& dstRect, const U32 in_flip, F32 fSpin, bool bSilhouette)
{
   AssertFatal(texture != NULL, "GSurface::drawBitmapTile: NULL Handle");
   // since the texture coords are calculated from the texture sub-rect we pass in,
   // but no actual indexing of that sub-rect happens, we can pass any texture coords
   // that we like to dglDrawBitmapSR(). We use this to force tiling.
   RectI subregion(0,0, dstRect.extent.x, dstRect.extent.y);
   dglDrawBitmapStretchSR(texture, dstRect, subregion, in_flip, fSpin, bSilhouette);
}

void dglDrawBitmapStretch(TextureObject* texture, const RectI& dstRect, const U32 in_flip, F32 fSpin, bool bSilhouette)
{
   AssertFatal(texture != NULL, "GSurface::drawBitmapStretch: NULL Handle");
   AssertFatal(dstRect.isValidRect() == true,
               "GSurface::drawBitmapStretch: routines assume normal rects");

   RectI subRegion(0, 0,
                   texture->getBitmapWidth(),
                   texture->getBitmapHeight());
   dglDrawBitmapStretchSR(texture,
                          dstRect,
                          subRegion,
                          in_flip,
                          fSpin,
                          bSilhouette);
}

void dglDrawBitmapSR(TextureObject *texture, const Point2I& in_rAt, const RectI& srcRect, const U32 in_flip)
{
   AssertFatal(texture != NULL, "GSurface::drawBitmapSR: NULL Handle");
   AssertFatal(srcRect.isValidRect() == true,
               "GSurface::drawBitmapSR: routines assume normal rects");

   RectI stretch(in_rAt.x, in_rAt.y,
                 srcRect.len_x(),
                 srcRect.len_y());
   dglDrawBitmapStretchSR(texture,
                          stretch,
                          srcRect,
                          in_flip);
}

U32 dglDrawText(GFont*   font,
          const Point2I& ptDraw,
          const UTF16*    in_string,
          const ColorI*  colorTable,
          const U32      maxColorIndex,
          F32            rot)
{
   return dglDrawTextN(font, ptDraw, in_string, dStrlen(in_string), colorTable, maxColorIndex, rot);
}

U32 dglDrawText(GFont*   font,
          const Point2I& ptDraw,
          const UTF8*    in_string,
          const ColorI*  colorTable,
          const U32      maxColorIndex,
          F32            rot)
{
   // Just a note - dStrlen(utf8) isn't strictly correct but it's guaranteed to be
   // as long or longer than the real length. dglDrawTextN fails gracefully
   // if you specify overlong, so this is ok.
   return dglDrawTextN(font, ptDraw, in_string, dStrlen((const UTF8 *) in_string), colorTable, maxColorIndex, rot);
}

struct TextVertex
{
   Point2F p;
   Point2F t;
   ColorI c;
    TextVertex() { set( 0.0f, 0.0f, 0.0f, 0.0f, ColorI(0, 0, 0) ); }
   void set(F32 x, F32 y, F32 tx, F32 ty, ColorI color)
   {
      p.x = x;
      p.y = y;
      t.x = tx;
      t.y = ty;
      c = color;
   }
};

//------------------------------------------------------------------------------

U32 dglDrawTextN(GFont*          font,
                 const Point2I&  ptDraw,
                 const UTF8*     in_string,
                 U32             n,
                 const ColorI*   colorTable,
                 const U32       maxColorIndex,
                 F32             rot)
{
   PROFILE_START(DrawText_UTF8);
   
   U32 len = dStrlen(in_string) + 1;
   FrameTemp<UTF16> ubuf(len);
   convertUTF8toUTF16(in_string, ubuf, len);
   U32 tmp = dglDrawTextN(font, ptDraw, ubuf, n, colorTable, maxColorIndex, rot);

   PROFILE_END();

   return tmp;
}

//-----------------------------------------------------------------------------

U32 dglDrawTextN(GFont*          font,
                 const Point2I&  ptDraw,
                 const UTF16*    in_string,
                 U32             n,
                 const ColorI*   colorTable,
                 const U32       maxColorIndex,
                 F32             rot)
{
   // return on zero length strings
   if( n < 1 )
      return ptDraw.x;
   PROFILE_START(DrawText);

   NVGcontext* nvgContext = dglGetNVGContext();
   if ( !nvgContext ) return ptDraw.x;

   // Need to pad this because the conversion doesn't
   // seem to be clean, or at least the character count
   // doesn't match. 4 was enough, but I chose 12 for 
   // now in case there's strings that will need more.
   UTF8* text = new UTF8[n + 12];
   convertUTF16toUTF8(in_string, text, n + 12);

   nvgFontSize(nvgContext, font->getHeight());

   const char* faceName = font->getFontFaceName();
   U32 fontID = nvgFindFont(nvgContext, faceName);
   if ( fontID < 0 )
      fontID = nvgFindFont(nvgContext, "sans");

   nvgFontFaceId(nvgContext, fontID);
   if ( !colorTable )
      nvgFillColor(nvgContext, nvgRGBA(0, 0, 0, 255));
   else
      nvgFillColor(nvgContext, nvgRGBA(colorTable[0].red, colorTable[0].green, colorTable[0].blue, colorTable[0].alpha));
      
   nvgTextAlign(nvgContext, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
   nvgText(nvgContext, ptDraw.x, ptDraw.y, text, NULL);

   SAFE_DELETE(text);

   PROFILE_END();
   return ptDraw.x;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //
// Drawing primitives

void dglDrawLine(S32 x1, S32 y1, S32 x2, S32 y2, const ColorI &color, F32 lineWidth)
{
	NVGcontext* vg = dglGetNVGContext();
	if (vg)
	{
		nvgBeginPath(vg);
      
		nvgMoveTo(vg, x1, y1);
      nvgLineTo(vg, x2, y2);
		nvgStrokeColor(vg, nvgRGBA(color.red, color.green, color.blue, color.alpha));
		nvgStrokeWidth(vg, lineWidth);
		nvgStroke(vg);
	}
}

void dglDrawLine(const Point2I &startPt, const Point2I &endPt, const ColorI &color, F32 lineWidth)
{
    dglDrawLine(startPt.x, startPt.y, endPt.x, endPt.y, color, lineWidth);
}

void dglDrawRect(const Point2I &upperL, const Point2I &lowerR, const ColorI &color, const float &lineWidth)
{
   /*glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_TEXTURE_2D);

   glLineWidth(lineWidth);

   glColor4ub(color.red, color.green, color.blue, color.alpha);
#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
    GLfloat verts[] = {
        (GLfloat)(upperL.x), (GLfloat)(upperL.y),
        (GLfloat)(lowerR.x), (GLfloat)(upperL.y),
        (GLfloat)(lowerR.x), (GLfloat)(lowerR.y),
        (GLfloat)(upperL.x), (GLfloat)(lowerR.y),
    };
    
    glVertexPointer(2, GL_FLOAT, 0, verts );
    glDrawArrays(GL_LINE_LOOP, 0, 4 );//draw last two
#else
   glBegin(GL_LINE_LOOP);
      glVertex2f((F32)upperL.x + 0.5f, (F32)upperL.y + 0.5f);
      glVertex2f((F32)lowerR.x + 0.5f, (F32)upperL.y + 0.5f);
      glVertex2f((F32)lowerR.x + 0.5f, (F32)lowerR.y + 0.5f);
      glVertex2f((F32)upperL.x + 0.5f, (F32)lowerR.y + 0.5f);
   glEnd();
#endif*/
}

// the fill convention for lined rects is that they outline the rectangle border of the
// filled region specified.

void dglDrawRect(const RectI &rect, const ColorI &color, const float &lineWidth)
{
   Point2I lowerR(rect.point.x + rect.extent.x - 1, rect.point.y + rect.extent.y - 1);
   dglDrawRect(rect.point, lowerR, color, lineWidth);
}

// the fill convention says that pixel at upperL will be filled and
// that pixel at lowerR will NOT be filled.

void dglDrawRectFill(const Point2I &upperL, const Point2I &lowerR, const ColorI &color)
{
   NVGcontext* nvg = dglGetNVGContext();
   if ( !nvg ) return;
  
   nvgBeginPath(nvg);
   nvgRect(nvg, upperL.x, upperL.y, lowerR.x - upperL.x, lowerR.y - upperL.y);
   nvgFillColor(nvg, nvgRGBA(color.red, color.green, color.blue, color.alpha));
   nvgFill(nvg);
}
void dglDrawRectFill(const RectI &rect, const ColorI &color)
{
   Point2I lowerR(rect.point.x + rect.extent.x, rect.point.y + rect.extent.y);
   dglDrawRectFill(rect.point, lowerR, color);
}

void dglDraw2DSquare( const Point2F &screenPoint, F32 width, F32 spinAngle )
{
   /*width *= 0.5;

   MatrixF rotMatrix( EulerF( 0.0, 0.0, spinAngle ) );

   Point3F offset( screenPoint.x, screenPoint.y, 0.0 );
   Point3F points[4];

   points[0] = Point3F(-width, -width, 0.0);
   points[1] = Point3F(-width,  width, 0.0);
   points[2] = Point3F( width,  width, 0.0);
   points[3] = Point3F( width, -width, 0.0);

   for( int i=0; i<4; i++ )
   {
      rotMatrix.mulP( points[i] );
      points[i] += offset;
   }

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
    GLfloat verts[] = {
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,//may need to switch last two
        1.0, 1.0,
    };

    GLfloat texVerts[] = {
        (GLfloat)points[0].x, (GLfloat)points[0].y,
        (GLfloat)points[1].x, (GLfloat)points[1].y,
        (GLfloat)points[3].x, (GLfloat)points[3].y,
        (GLfloat)points[2].x, (GLfloat)points[2].y,
    };
    
    glDisableClientState(GL_COLOR_ARRAY);
    //glDisableClientState(GL_POINT_SIZE_ARRAY_OES);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glVertexPointer(2, GL_FLOAT, 0, verts);
    glTexCoordPointer(2, GL_FLOAT, 0, texVerts);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
#else
   glBegin(GL_TRIANGLE_FAN);
      glTexCoord2f(0.0, 0.0);
      glVertex2fv(points[0]);

      glTexCoord2f(0.0, 1.0);
      glVertex2fv(points[1]);

      glTexCoord2f(1.0, 1.0);
      glVertex2fv(points[2]);

      glTexCoord2f(1.0, 0.0);
      glVertex2fv(points[3]);
   glEnd();
#endif*/
}

void dglDrawBillboard( const Point3F &position, F32 width, F32 spinAngle )
{
/*  MatrixF modelview;
   dglGetModelview( &modelview );
   modelview.transpose();


   width *= 0.5;
   Point3F points[4];
   points[0] = Point3F(-width, 0.0, -width);
   points[1] = Point3F(-width, 0.0,  width);
   points[2] = Point3F( width, 0.0,  width);
   points[3] = Point3F( width, 0.0, -width);


   MatrixF rotMatrix( EulerF( 0.0, spinAngle, 0.0 ) );

   for( int i=0; i<4; i++ )
   {
      rotMatrix.mulP( points[i] );
      modelview.mulP( points[i] );
      points[i] += position;
   }


#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
    GLfloat verts[] = {
        0.0, 1.0,
        0.0, 0.0,
        1.0, 1.0,//may need to switch last two
        1.0, 0.0,
    };
    
    GLfloat texVerts[] = {
        (GLfloat)points[0].x, (GLfloat)points[0].y,
        (GLfloat)points[1].x, (GLfloat)points[1].y,
        (GLfloat)points[3].x, (GLfloat)points[3].y,
        (GLfloat)points[2].x, (GLfloat)points[2].y,
    };
    
    glDisableClientState(GL_COLOR_ARRAY);
    //glDisableClientState(GL_POINT_SIZE_ARRAY_OES);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glVertexPointer(2, GL_FLOAT, 0, verts);
    glTexCoordPointer(2, GL_FLOAT, 0, texVerts);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
#else
   glBegin(GL_TRIANGLE_FAN);
      glTexCoord2f(0.0, 1.0);
      glVertex3fv(points[0]);

      glTexCoord2f(0.0, 0.0);
      glVertex3fv(points[1]);

      glTexCoord2f(1.0, 0.0);
      glVertex3fv(points[2]);

      glTexCoord2f(1.0, 1.0);
      glVertex3fv(points[3]);
   glEnd();
#endif*/
}

void dglWireCube(const Point3F & extent, const Point3F & center)
{
/*   static Point3F cubePoints[8] =
   {
      Point3F(-1, -1, -1), Point3F(-1, -1,  1), Point3F(-1,  1, -1), Point3F(-1,  1,  1),
         Point3F( 1, -1, -1), Point3F( 1, -1,  1), Point3F( 1,  1, -1), Point3F( 1,  1,  1)
   };

   static U32 cubeFaces[6][4] =
   {
      { 0, 2, 6, 4 }, { 0, 2, 3, 1 }, { 0, 1, 5, 4 },
      { 3, 2, 6, 7 }, { 7, 6, 4, 5 }, { 3, 7, 5, 1 }
   };

   glDisable(GL_CULL_FACE);

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
//PUAP -Mat untested
   for (S32 i = 0; i < 6; i++)
   {
       GLfloat verts[] = {
           0, 0, 0,
           0, 0, 0,
           0, 0, 0,
           0, 0, 0,
       };
       int j = 0;
      for(int vert = 0; vert < 4; vert++)
      {
         int idx = cubeFaces[i][vert];
         //glVertex3f(cubePoints[idx].x * extent.x + center.x,
            //cubePoints[idx].y * extent.y + center.y,
            //cubePoints[idx].z * extent.z + center.z);
          verts[j] =    cubePoints[idx].x * extent.x + center.x;
          verts[++j] = cubePoints[idx].y * extent.y + center.y;
          verts[++j] = cubePoints[idx].z * extent.z + center.z;
      }
       glVertexPointer(3, GL_FLOAT, 0, verts);
       glDrawArrays(GL_LINE_LOOP, 0, 4);
   }
#else
   for (S32 i = 0; i < 6; i++)
   {
      glBegin(GL_LINE_LOOP);
      for(int vert = 0; vert < 4; vert++)
      {
         int idx = cubeFaces[i][vert];
         glVertex3f(cubePoints[idx].x * extent.x + center.x,
            cubePoints[idx].y * extent.y + center.y,
            cubePoints[idx].z * extent.z + center.z);
      }
      glEnd();
   }
#endif*/
}


void dglSolidCube(const Point3F & extent, const Point3F & center)
{
/*   static Point3F cubePoints[8] =
   {
      Point3F(-1, -1, -1), Point3F(-1, -1,  1), Point3F(-1,  1, -1), Point3F(-1,  1,  1),
         Point3F( 1, -1, -1), Point3F( 1, -1,  1), Point3F( 1,  1, -1), Point3F( 1,  1,  1)
   };

   static U32 cubeFaces[6][4] =
   {
      { 0, 2, 6, 4 }, { 0, 2, 3, 1 }, { 0, 1, 5, 4 },
      { 3, 2, 6, 7 }, { 7, 6, 4, 5 }, { 3, 7, 5, 1 }
   };

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
//PUAP -Mat untested
   for (S32 i = 0; i < 6; i++)
   {
       GLfloat verts[] = {
           0, 0, 0,
           0, 0, 0,
           0, 0, 0,
           0, 0, 0,
       };
       int j = 0;
      for(int vert = 0; vert < 4; vert++)
      {
         int idx = cubeFaces[i][vert];
         //glVertex3f(cubePoints[idx].x * extent.x + center.x,
            //cubePoints[idx].y * extent.y + center.y,
            //cubePoints[idx].z * extent.z + center.z);
          verts[j] = cubePoints[idx].x * extent.x + center.x;
          verts[++j] = cubePoints[idx].y * extent.y + center.y;
          verts[++j] = cubePoints[idx].z * extent.z + center.z;
      }
       glVertexPointer(3, GL_FLOAT, 0, verts);
       glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
   }
#else
   for (S32 i = 0; i < 6; i++)
   {
      glBegin(GL_TRIANGLE_FAN);
      for(int vert = 0; vert < 4; vert++)
      {
         int idx = cubeFaces[i][vert];
         glVertex3f(cubePoints[idx].x * extent.x + center.x,
             cubePoints[idx].y * extent.y + center.y,
            cubePoints[idx].z * extent.z + center.z);
      }
      glEnd();
   }
#endif
   */
}

void dglSetClipRect(const RectI &clipRect)
{
/*   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   U32 screenHeight = Platform::getWindowSize().y;

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID)
   glOrthof(clipRect.point.x, clipRect.point.x + clipRect.extent.x,
           clipRect.extent.y, 0,
           0, 1);
#else
   glOrtho(clipRect.point.x, clipRect.point.x + clipRect.extent.x,
           clipRect.extent.y, 0,
           0, 1);
#endif

   glTranslatef(0.0f, (F32)-clipRect.point.y, 0.0f);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glViewport(clipRect.point.x, screenHeight - (clipRect.point.y + clipRect.extent.y),
              clipRect.extent.x, clipRect.extent.y);

   sgCurrentClipRect = clipRect;
   */
}

const RectI& dglGetClipRect()
{
   return sgCurrentClipRect;
}

bool dglPointToScreen( Point3F &point3D, Point3F &screenPoint )
{
/*#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
   GLfloat       glMV[16];
   GLfloat       glPR[16];
   GLint          glVP[4];

   glGetFloatv(GL_PROJECTION_MATRIX, glPR);
   glGetFloatv(GL_MODELVIEW_MATRIX, glMV);

   RectI viewport;
   dglGetViewport(&viewport);

   glVP[0] = viewport.point.x;
   glVP[1] = viewport.point.y + viewport.extent.y;
   glVP[2] = viewport.extent.x;
   glVP[3] = -viewport.extent.y;

   MatrixF mv;
   dglGetModelview(&mv);
   MatrixF pr;
   dglGetProjection(&pr);

   F64 x, y, z;
// PUAP -Mat untested
   int result = gluProject( point3D.x, point3D.y, point3D.z, (const F64 *)&glMV, (const F64 *)&glPR, (const GLint *)&glVP, &x, &y, &z );

   screenPoint.x = x;
   screenPoint.y = y;
   screenPoint.z = z;


   return (result == GL_TRUE);
#else
   GLdouble       glMV[16];
   GLdouble       glPR[16];
   GLint          glVP[4];


   glGetDoublev(GL_PROJECTION_MATRIX, glPR);
   glGetDoublev(GL_MODELVIEW_MATRIX, glMV);

   RectI viewport;
   dglGetViewport(&viewport);

   glVP[0] = viewport.point.x;
   glVP[1] = viewport.point.y + viewport.extent.y;
   glVP[2] = viewport.extent.x;
   glVP[3] = -viewport.extent.y;

   MatrixF mv;
   dglGetModelview(&mv);
   MatrixF pr;
   dglGetProjection(&pr);

   F64 x, y, z;
   int result = gluProject( (GLdouble)point3D.x, (GLdouble)point3D.y, (GLdouble)point3D.z, (const F64 *)&glMV, (const F64 *)&glPR, (const GLint *)&glVP, &x, &y, &z );
   screenPoint.x = (F32)x;
   screenPoint.y = (F32)y;
   screenPoint.z = (F32)z;


   return (result == GL_TRUE);
#endif
    */
   return false;
}



bool dglIsInCanonicalState()
{
 /*  bool ret = true;

   // Canonical state:
   //  BLEND disabled
   //  TEXTURE_2D disabled on both texture units.
   //  ActiveTexture set to 0
   //  LIGHTING off
   //  winding : clockwise ?
   //  cullface : disabled

   ret &= glIsEnabled(GL_BLEND) == GL_FALSE;
   ret &= glIsEnabled(GL_CULL_FACE) == GL_FALSE;
   GLint temp;
#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
// PUAP -Mat removed unsupported textureARB and Fog stuff
   if (dglDoesSupportARBMultitexture() == true) {
      //glActiveTextureARB(GL_TEXTURE1_ARB);
      ret &= glIsEnabled(GL_TEXTURE_2D) == GL_FALSE;
      glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &temp);
      ret &= temp == GL_REPLACE;

      //glActiveTextureARB(GL_TEXTURE0_ARB);
      ret &= glIsEnabled(GL_TEXTURE_2D) == GL_FALSE;
      glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &temp);
      ret &= temp == GL_REPLACE;

      //glClientActiveTextureARB(GL_TEXTURE1_ARB);
      ret &= glIsEnabled(GL_TEXTURE_COORD_ARRAY) == GL_FALSE;
      //glClientActiveTextureARB(GL_TEXTURE0_ARB);
      ret &= glIsEnabled(GL_TEXTURE_COORD_ARRAY) == GL_FALSE;
   } else {
      ret &= glIsEnabled(GL_TEXTURE_2D) == GL_FALSE;
      glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &temp);
      ret &= temp == GL_REPLACE;

      ret &= glIsEnabled(GL_TEXTURE_COORD_ARRAY) == GL_FALSE;
   }

   ret &= glIsEnabled(GL_LIGHTING) == GL_FALSE;

   ret &= glIsEnabled(GL_COLOR_ARRAY)         == GL_FALSE;
   ret &= glIsEnabled(GL_VERTEX_ARRAY)        == GL_FALSE;
   ret &= glIsEnabled(GL_NORMAL_ARRAY)        == GL_FALSE;
   //if (dglDoesSupportFogCoord())
      //ret &= glIsEnabled(GL_FOG_COORDINATE_ARRAY_EXT) == GL_FALSE;

#else

   if (dglDoesSupportARBMultitexture() == true) {
      glActiveTextureARB(GL_TEXTURE1_ARB);
      ret &= glIsEnabled(GL_TEXTURE_2D) == GL_FALSE;
      glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &temp);
      ret &= temp == GL_REPLACE;

      glActiveTextureARB(GL_TEXTURE0_ARB);
      ret &= glIsEnabled(GL_TEXTURE_2D) == GL_FALSE;
      glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &temp);
      ret &= temp == GL_REPLACE;

      glClientActiveTextureARB(GL_TEXTURE1_ARB);
      ret &= glIsEnabled(GL_TEXTURE_COORD_ARRAY) == GL_FALSE;
      glClientActiveTextureARB(GL_TEXTURE0_ARB);
      ret &= glIsEnabled(GL_TEXTURE_COORD_ARRAY) == GL_FALSE;
   } else {
      ret &= glIsEnabled(GL_TEXTURE_2D) == GL_FALSE;
      glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &temp);
      ret &= temp == GL_REPLACE;

      ret &= glIsEnabled(GL_TEXTURE_COORD_ARRAY) == GL_FALSE;
   }

   ret &= glIsEnabled(GL_LIGHTING) == GL_FALSE;

   ret &= glIsEnabled(GL_COLOR_ARRAY)         == GL_FALSE;
   ret &= glIsEnabled(GL_VERTEX_ARRAY)        == GL_FALSE;
   ret &= glIsEnabled(GL_NORMAL_ARRAY)        == GL_FALSE;
   if (dglDoesSupportFogCoord())
      ret &= glIsEnabled(GL_FOG_COORDINATE_ARRAY_EXT) == GL_FALSE;
#endif
   return ret;
   */
   return false;
}


void dglSetCanonicalState()
{
   /*
#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
// PUAP -Mat removed unsupported textureARB and Fog stuff
   glDisable(GL_BLEND);
   glDisable(GL_CULL_FACE);
   glBlendFunc(GL_ONE, GL_ZERO);
   glDisable(GL_LIGHTING);
   if (dglDoesSupportARBMultitexture() == true) {
      //glActiveTextureARB(GL_TEXTURE1_ARB);
      glDisable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      //glActiveTextureARB(GL_TEXTURE0_ARB);
      glDisable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   } else {
      glDisable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   }

   glDisableClientState(GL_COLOR_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   //if (dglDoesSupportFogCoord())
      //glDisableClientState(GL_FOG_COORDINATE_ARRAY_EXT);
#else
   glDisable(GL_BLEND);
   glDisable(GL_CULL_FACE);
   glBlendFunc(GL_ONE, GL_ZERO);
   glDisable(GL_LIGHTING);
   if (dglDoesSupportARBMultitexture() == true) {
      glActiveTextureARB(GL_TEXTURE1_ARB);
      glDisable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glDisable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   } else {
      glDisable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   }

   glDisableClientState(GL_COLOR_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   if (dglDoesSupportFogCoord())
      glDisableClientState(GL_FOG_COORDINATE_ARRAY_EXT);
#endif
      */
}

void dglGetTransformState(S32* mvDepth,
                          S32* pDepth,
                          S32* t0Depth,
                          F32* t0Matrix,
                          S32* t1Depth,
                          F32* t1Matrix,
                          S32* vp)
{
   /*
   glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, (GLint*)mvDepth);
   glGetIntegerv(GL_PROJECTION_STACK_DEPTH, (GLint*)pDepth);

   glGetIntegerv(GL_TEXTURE_STACK_DEPTH, (GLint*)t0Depth);
   glGetFloatv(GL_TEXTURE_MATRIX, t0Matrix);
   if (dglDoesSupportARBMultitexture())
   {
#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
// PUAP -Mat removed unsupported textureARB stuff
      //glActiveTextureARB(GL_TEXTURE1_ARB);
      glGetIntegerv(GL_TEXTURE_STACK_DEPTH, (GLint*)t1Depth);
      glGetFloatv(GL_TEXTURE_MATRIX, t1Matrix);
      //glActiveTextureARB(GL_TEXTURE0_ARB);
#else
      glActiveTextureARB(GL_TEXTURE1_ARB);
      glGetIntegerv(GL_TEXTURE_STACK_DEPTH, (GLint*)t1Depth);
      glGetFloatv(GL_TEXTURE_MATRIX, t1Matrix);
      glActiveTextureARB(GL_TEXTURE0_ARB);
#endif
   }
   else
   {
      *t1Depth = 0;
      for (U32 i = 0; i < 16; i++)
         t1Matrix[i] = 0;
   }

   RectI v;
   dglGetViewport(&v);
   vp[0] = v.point.x;
   vp[1] = v.point.y;
   vp[2] = v.extent.x;
   vp[3] = v.extent.y;
   */
}


bool dglCheckState(const S32 mvDepth, const S32 pDepth,
                   const S32 t0Depth, const F32* t0Matrix,
                   const S32 t1Depth, const F32* t1Matrix,
                   const S32* vp)
{
   /*
   GLint md, pd;
   RectI v;

   glGetIntegerv(GL_MODELVIEW_STACK_DEPTH,  &md);
   glGetIntegerv(GL_PROJECTION_STACK_DEPTH, &pd);

   GLint t0d, t1d;
   GLfloat t0m[16], t1m[16];
   glGetIntegerv(GL_TEXTURE_STACK_DEPTH, &t0d);
   glGetFloatv(GL_TEXTURE_MATRIX, t0m);
   if (dglDoesSupportARBMultitexture())
   {
#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
// PUAP -Mat removed unsupported textureARB and Fog stuff
      //glActiveTextureARB(GL_TEXTURE1_ARB);
      glGetIntegerv(GL_TEXTURE_STACK_DEPTH, &t1d);
      glGetFloatv(GL_TEXTURE_MATRIX, t1m);
      //glActiveTextureARB(GL_TEXTURE0_ARB);
#else
      glActiveTextureARB(GL_TEXTURE1_ARB);
      glGetIntegerv(GL_TEXTURE_STACK_DEPTH, &t1d);
      glGetFloatv(GL_TEXTURE_MATRIX, t1m);
      glActiveTextureARB(GL_TEXTURE0_ARB);
#endif
   }
   else
   {
      t1d = 0;
      for (U32 i = 0; i < 16; i++)
         t1m[i] = 0;
   }

   dglGetViewport(&v);

   return ((md == mvDepth) &&
           (pd == pDepth) &&
           (t0d == t0Depth) &&
           (dMemcmp(t0m, t0Matrix, sizeof(F32) * 16) == 0) &&
           (t1d == t1Depth) &&
           (dMemcmp(t1m, t1Matrix, sizeof(F32) * 16) == 0) &&
           ((v.point.x  == vp[0]) &&
            (v.point.y  == vp[1]) &&
            (v.extent.x == vp[2]) &&
            (v.extent.y == vp[3])));
            */

   return false;
}

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
GLfloat gVertexFloats[8];
GLfloat gTextureVerts[8];
#endif

NVGcontext* dglGetNVGContext()
{
   if ( nvgContext != NULL )
      return nvgContext;

   bgfx::setViewSeq(v_TorqueGUITop->id, true);

   Point2I size = Platform::getWindowSize();
   nvgContext = nvgCreate(1, v_TorqueGUITop->id);
   return nvgContext;
}

void dglBeginFrame()
{
   if ( !dglGetNVGContext() ) return;
   Point2I size = Platform::getWindowSize();

   nvgViewId(nvgContext, v_TorqueGUITop->id); 
   nvgBeginFrame(nvgContext, size.x, size.y, 1.0f);

   // GUI Orthographic Projection
   float ortho[16];
   bx::mtxOrtho(ortho, 0.0f, (float)size.x, (float)size.y, 0.0f, 0.0f, 1000.0f);
   bgfx::setViewTransform(v_TorqueGUITop->id, NULL, ortho);
   bgfx::setViewRect(v_TorqueGUITop->id, 0, 0, size.x, size.y);
}

void dglEndFrame()
{
   if ( !dglGetNVGContext() ) return;
   nvgEndFrame(nvgContext);
}

void dglScreenQuadSrc(U32 _x, U32 _y, U32 _width, U32 _height, 
                      F32 _srcx, F32 _srcy, F32 _srcwidth, F32 _srcheight, F32 _srcimgwidth, F32 _srcimgheight)
{
   const bgfx::RendererType::Enum renderer = bgfx::getRendererType();
   bool _originBottomLeft = bgfx::RendererType::OpenGL == renderer || bgfx::RendererType::OpenGLES == renderer;

   // TODO: Shouldn't be creating a vertex definition every goddamn frame.
   bgfx::VertexDecl decl;
   decl.begin();
   decl.add(bgfx::Attrib::Position,  2, bgfx::AttribType::Float);
   decl.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float);
   decl.end();

   //_originBottomLeft = true;

   if (bgfx::checkAvailTransientVertexBuffer(6, decl) )
   {
      bgfx::TransientVertexBuffer vb;
      bgfx::allocTransientVertexBuffer(&vb, 6, decl);
      PosUvVertex* vertex = (PosUvVertex*)vb.data;

      const float widthf  = float(_width);
      const float heightf = float(_height);

      const float minx = float(_x);
      const float miny = float(_y);
      const float maxx = minx+widthf;
      const float maxy = miny+heightf;

      F32 m_halfTexel = (bgfx::getRendererType() == bgfx::RendererType::Direct3D9) ? 0.5f : 0.0f;
      const float texelHalfW = m_halfTexel/_srcimgwidth;
      const float texelHalfH = m_halfTexel/_srcimgheight;
      const float minu = texelHalfW + (_srcx / _srcimgwidth);
      const float maxu = ((_srcx + _srcwidth) / _srcimgwidth) - texelHalfW;
      const float minv = _originBottomLeft ? texelHalfH + ((_srcy + _srcheight) / _srcimgheight) : texelHalfH + (_srcy / _srcimgheight);
      const float maxv = _originBottomLeft ? texelHalfH + (_srcy / _srcimgheight) : texelHalfH + ((_srcy + _srcheight) / _srcimgheight);

      vertex[0].m_x = minx;
      vertex[0].m_y = miny;
      vertex[0].m_u = minu;
      vertex[0].m_v = minv;

      vertex[1].m_x = maxx;
      vertex[1].m_y = miny;
      vertex[1].m_u = maxu;
      vertex[1].m_v = minv;

      vertex[2].m_x = maxx;
      vertex[2].m_y = maxy;
      vertex[2].m_u = maxu;
      vertex[2].m_v = maxv;

      vertex[3].m_x = maxx;
      vertex[3].m_y = maxy;
      vertex[3].m_u = maxu;
      vertex[3].m_v = maxv;

      vertex[4].m_x = minx;
      vertex[4].m_y = maxy;
      vertex[4].m_u = minu;
      vertex[4].m_v = maxv;

      vertex[5].m_x = minx;
      vertex[5].m_y = miny;
      vertex[5].m_u = minu;
      vertex[5].m_v = minv;

      bgfx::setVertexBuffer(&vb);
   }
}

void dglScreenQuad(U32 _x, U32 _y, U32 _width, U32 _height)
{
   dglScreenQuadSrc(_x, _y, _width, _height, 0, 0, _width, _height, _width, _height);
}

void fullScreenQuad(F32 _textureWidth, F32 _textureHeight, F32 _z)
{
   const bgfx::RendererType::Enum renderer = bgfx::getRendererType();
   F32 _texelHalf = bgfx::RendererType::Direct3D9 == renderer ? 0.5f : 0.0f;
   bool _originBottomLeft = bgfx::RendererType::OpenGL == renderer || bgfx::RendererType::OpenGLES == renderer;
   F32 _width = 1.0f;
   F32 _height = 1.0f;

   if (bgfx::checkAvailTransientVertexBuffer(3, Graphics::PosUVVertex::ms_decl) )
   {
      bgfx::TransientVertexBuffer vb;
      bgfx::allocTransientVertexBuffer(&vb, 3, Graphics::PosUVVertex::ms_decl);
      Graphics::PosUVVertex* vertex = (Graphics::PosUVVertex*)vb.data;

      const F32 minx = -_width;
      const F32 maxx =  _width;
      const F32 miny = 0.0f;
      const F32 maxy = _height*2.0f;

      const F32 texelHalfW = _texelHalf/_textureWidth;
      const F32 texelHalfH = _texelHalf/_textureHeight;
      const F32 minu = -1.0f + texelHalfW;
      const F32 maxu =  1.0f + texelHalfH;

      float minv = texelHalfH;
      float maxv = 2.0f + texelHalfH;

      if (_originBottomLeft)
      {
         float temp = minv;
         minv = maxv;
         maxv = temp;

         minv -= 1.0f;
         maxv -= 1.0f;
      }

      vertex[0].m_x = minx;
      vertex[0].m_y = miny;
      vertex[0].m_z = _z;
      vertex[0].m_u = minu;
      vertex[0].m_v = minv;

      vertex[1].m_x = maxx;
      vertex[1].m_y = miny;
      vertex[1].m_z = _z;
      vertex[1].m_u = maxu;
      vertex[1].m_v = minv;

      vertex[2].m_x = maxx;
      vertex[2].m_y = maxy;
      vertex[2].m_z = _z;
      vertex[2].m_u = maxu;
      vertex[2].m_v = maxv;

      bgfx::setVertexBuffer(&vb);
   }
}

void drawLine3D(Point3F start, Point3F end, ColorI color, F32 lineWidth)
{
   Point2I startPos = Rendering::worldToScreen(start);
   Point2I endPos = Rendering::worldToScreen(end);

   dglDrawLine(startPos, endPos, color, lineWidth);
}

void drawBox3D(Box3F box, ColorI color, F32 lineWidth)
{
   Point3F bottom0 = box.minExtents;
   Point3F bottom1(box.minExtents.x, box.minExtents.y, box.maxExtents.z);
   Point3F bottom2(box.maxExtents.x, box.minExtents.y, box.maxExtents.z);
   Point3F bottom3(box.maxExtents.x, box.minExtents.y, box.minExtents.z);

   Point3F top0(box.minExtents.x, box.maxExtents.y, box.minExtents.z);
   Point3F top1(box.minExtents.x, box.maxExtents.y, box.maxExtents.z);
   Point3F top2(box.maxExtents.x, box.maxExtents.y, box.maxExtents.z);
   Point3F top3(box.maxExtents.x, box.maxExtents.y, box.minExtents.z);

   drawLine3D(bottom0, bottom1, color, lineWidth);
   drawLine3D(bottom1, bottom2, color, lineWidth);
   drawLine3D(bottom2, bottom3, color, lineWidth);
   drawLine3D(bottom3, bottom0, color, lineWidth);

   drawLine3D(top0, top1, color, lineWidth);
   drawLine3D(top1, top2, color, lineWidth);
   drawLine3D(top2, top3, color, lineWidth);
   drawLine3D(top3, top0, color, lineWidth);

   drawLine3D(top0, bottom0, color, lineWidth);
   drawLine3D(top1, bottom1, color, lineWidth);
   drawLine3D(top2, bottom2, color, lineWidth);
   drawLine3D(top3, bottom3, color, lineWidth);
}

void screenSpaceQuad(F32 _x, F32 _y, F32 _width, F32 _height, F32 _targetWidth, F32 _targetHeight)
{
   const bgfx::RendererType::Enum renderer = bgfx::getRendererType();
   float _texelHalf = bgfx::RendererType::Direct3D9 == renderer ? 0.5f : 0.0f;
   bool _originBottomLeft = bgfx::RendererType::OpenGL == renderer || bgfx::RendererType::OpenGLES == renderer;
   float width = _width / _targetWidth;
   float height = _height / _targetHeight;

   if (bgfx::checkAvailTransientVertexBuffer(6, Graphics::PosUVVertex::ms_decl) )
   {
      bgfx::TransientVertexBuffer vb;
      bgfx::allocTransientVertexBuffer(&vb, 6, Graphics::PosUVVertex::ms_decl);
      Graphics::PosUVVertex* vertex = (Graphics::PosUVVertex*)vb.data;

      const float minx = (_x / _targetWidth);
      const float maxx = (_x / _targetWidth) + width;
      const float miny = (_y / _targetHeight);
      const float maxy = (_y / _targetHeight) + height;

      const float texelHalfW = _texelHalf/_targetWidth;
      const float texelHalfH = _texelHalf/_targetHeight;
      const float minu = 0.0f + texelHalfW;
      const float maxu = 1.0f + texelHalfH;

      const float zz = 0.0f;

      float minv = texelHalfH;
      float maxv = 1.0f + texelHalfH;

      if (_originBottomLeft)
      {
         float temp = minv;
         minv = maxv;
         maxv = temp;

         minv -= 1.0f;
         maxv -= 1.0f;
      }

      vertex[0].m_x = minx;
      vertex[0].m_y = miny;
      vertex[0].m_z = zz;
      vertex[0].m_u = minu;
      vertex[0].m_v = minv;

      vertex[1].m_x = maxx;
      vertex[1].m_y = miny;
      vertex[1].m_z = zz;
      vertex[1].m_u = maxu;
      vertex[1].m_v = minv;

      vertex[2].m_x = maxx;
      vertex[2].m_y = maxy;
      vertex[2].m_z = zz;
      vertex[2].m_u = maxu;
      vertex[2].m_v = maxv;

      vertex[3].m_x = maxx;
      vertex[3].m_y = maxy;
      vertex[3].m_z = zz;
      vertex[3].m_u = maxu;
      vertex[3].m_v = maxv;

      vertex[4].m_x = minx;
      vertex[4].m_y = maxy;
      vertex[4].m_z = zz;
      vertex[4].m_u = minu;
      vertex[4].m_v = maxv;

      vertex[5].m_x = minx;
      vertex[5].m_y = miny;
      vertex[5].m_z = zz;
      vertex[5].m_u = minu;
      vertex[5].m_v = minv;

      bgfx::setVertexBuffer(&vb);
   }
}
