/*

    Copyright (C) 2004-2012 Michael Liebscher <johnnycanuck@users.sourceforge.net>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

/**
 * \file angle.c
 * \brief Interface to OpenGL library.
 * \author Michael Liebscher
 * \date 2004-2012
 */

/*!
    \note
    This module implements the OS binding of OpenGL to our
    function pointers. The following two functions must be
    implemented when doing a port:

    OpenGL_Init() - Loads libraries and assigns function pointers.
    OpenGL_Shutdown() - Unloads libraries, NULLs function pointers.

*/

#ifdef _WIN32

#include <float.h>

#include <windows.h>

HINSTANCE hinstOpenGL;

#endif


#ifdef __unix__

#include <dlfcn.h>

void *OpenGLLib;

#endif


#include "opengl_local.h"


#include "common.h"


/**
 * \brief Shutdown interface to OpenGL.
 * \note Unloads the specified Dynamic Link Library then NULLs out all the proc pointers.
 */
PUBLIC void OpenGL_Shutdown (void)
{

#ifdef _WIN32

    if (hinstOpenGL) {
        FreeLibrary (hinstOpenGL);
        hinstOpenGL = NULL;
    }

    hinstOpenGL = NULL;

#elif __unix__

    if (OpenGLLib) {
        dlclose (OpenGLLib);
        OpenGLLib = NULL;
    }

    OpenGLLib = NULL;

#else

#error "Please define interface to OpenGL library!"

#endif

    pfglAccum                     = NULL;
    pfglAlphaFunc                 = NULL;
    pfglAreTexturesResident       = NULL;
    pfglArrayElement              = NULL;
    pfglBegin                     = NULL;
    pfglBindTexture               = NULL;
    pfglBitmap                    = NULL;
    pfglBlendFunc                 = NULL;
    pfglCallList                  = NULL;
    pfglCallLists                 = NULL;
    pfglClear                     = NULL;
    pfglClearAccum                = NULL;
    pfglClearColor                = NULL;
    pfglClearDepth                = NULL;
    pfglClearIndex                = NULL;
    pfglClearStencil              = NULL;
    pfglClipPlane                 = NULL;
    pfglColor3b                   = NULL;
    pfglColor3bv                  = NULL;
    pfglColor3d                   = NULL;
    pfglColor3dv                  = NULL;
    pfglColor3f                   = NULL;
    pfglColor3fv                  = NULL;
    pfglColor3i                   = NULL;
    pfglColor3iv                  = NULL;
    pfglColor3s                   = NULL;
    pfglColor3sv                  = NULL;
    pfglColor3ub                  = NULL;
    pfglColor3ubv                 = NULL;
    pfglColor3ui                  = NULL;
    pfglColor3uiv                 = NULL;
    pfglColor3us                  = NULL;
    pfglColor3usv                 = NULL;
    pfglColor4b                   = NULL;
    pfglColor4bv                  = NULL;
    pfglColor4d                   = NULL;
    pfglColor4dv                  = NULL;
    pfglColor4f                   = NULL;
    pfglColor4fv                  = NULL;
    pfglColor4i                   = NULL;
    pfglColor4iv                  = NULL;
    pfglColor4s                   = NULL;
    pfglColor4sv                  = NULL;
    pfglColor4ub                  = NULL;
    pfglColor4ubv                 = NULL;
    pfglColor4ui                  = NULL;
    pfglColor4uiv                 = NULL;
    pfglColor4us                  = NULL;
    pfglColor4usv                 = NULL;
    pfglColorMask                 = NULL;
    pfglColorMaterial             = NULL;
    pfglColorPointer              = NULL;
    pfglCopyPixels                = NULL;
    pfglCopyTexImage1D            = NULL;
    pfglCopyTexImage2D            = NULL;
    pfglCopyTexSubImage1D         = NULL;
    pfglCopyTexSubImage2D         = NULL;
    pfglCullFace                  = NULL;
    pfglDeleteLists               = NULL;
    pfglDeleteTextures            = NULL;
    pfglDepthFunc                 = NULL;
    pfglDepthMask                 = NULL;
    pfglDepthRange                = NULL;
    pfglDisable                   = NULL;
    pfglDisableClientState        = NULL;
    pfglDrawArrays                = NULL;
    pfglDrawBuffer                = NULL;
    pfglDrawElements              = NULL;
    pfglDrawPixels                = NULL;
    pfglEdgeFlag                  = NULL;
    pfglEdgeFlagPointer           = NULL;
    pfglEdgeFlagv                 = NULL;
    pfglEnable                    = NULL;
    pfglEnableClientState         = NULL;
    pfglEnd                       = NULL;
    pfglEndList                   = NULL;
    pfglEvalCoord1d               = NULL;
    pfglEvalCoord1dv              = NULL;
    pfglEvalCoord1f               = NULL;
    pfglEvalCoord1fv              = NULL;
    pfglEvalCoord2d               = NULL;
    pfglEvalCoord2dv              = NULL;
    pfglEvalCoord2f               = NULL;
    pfglEvalCoord2fv              = NULL;
    pfglEvalMesh1                 = NULL;
    pfglEvalMesh2                 = NULL;
    pfglEvalPoint1                = NULL;
    pfglEvalPoint2                = NULL;
    pfglFeedbackBuffer            = NULL;
    pfglFinish                    = NULL;
    pfglFlush                     = NULL;
    pfglFogf                      = NULL;
    pfglFogfv                     = NULL;
    pfglFogi                      = NULL;
    pfglFogiv                     = NULL;
    pfglFrontFace                 = NULL;
    pfglFrustum                   = NULL;
    pfglGenLists                  = NULL;
    pfglGenTextures               = NULL;
    pfglGetBooleanv               = NULL;
    pfglGetClipPlane              = NULL;
    pfglGetDoublev                = NULL;
    pfglGetError                  = NULL;
    pfglGetFloatv                 = NULL;
    pfglGetIntegerv               = NULL;
    pfglGetLightfv                = NULL;
    pfglGetLightiv                = NULL;
    pfglGetMapdv                  = NULL;
    pfglGetMapfv                  = NULL;
    pfglGetMapiv                  = NULL;
    pfglGetMaterialfv             = NULL;
    pfglGetMaterialiv             = NULL;
    pfglGetPixelMapfv             = NULL;
    pfglGetPixelMapuiv            = NULL;
    pfglGetPixelMapusv            = NULL;
    pfglGetPointerv               = NULL;
    pfglGetPolygonStipple         = NULL;
    pfglGetString                 = NULL;
    pfglGetTexEnvfv               = NULL;
    pfglGetTexEnviv               = NULL;
    pfglGetTexGendv               = NULL;
    pfglGetTexGenfv               = NULL;
    pfglGetTexGeniv               = NULL;
    pfglGetTexImage               = NULL;
    pfglGetTexParameterfv         = NULL;
    pfglGetTexParameteriv         = NULL;
    pfglHint                      = NULL;
    pfglIndexMask                 = NULL;
    pfglIndexPointer              = NULL;
    pfglIndexd                    = NULL;
    pfglIndexdv                   = NULL;
    pfglIndexf                    = NULL;
    pfglIndexfv                   = NULL;
    pfglIndexi                    = NULL;
    pfglIndexiv                   = NULL;
    pfglIndexs                    = NULL;
    pfglIndexsv                   = NULL;
    pfglIndexub                   = NULL;
    pfglIndexubv                  = NULL;
    pfglInitNames                 = NULL;
    pfglInterleavedArrays         = NULL;
    pfglIsEnabled                 = NULL;
    pfglIsList                    = NULL;
    pfglIsTexture                 = NULL;
    pfglLightModelf               = NULL;
    pfglLightModelfv              = NULL;
    pfglLightModeli               = NULL;
    pfglLightModeliv              = NULL;
    pfglLightf                    = NULL;
    pfglLightfv                   = NULL;
    pfglLighti                    = NULL;
    pfglLightiv                   = NULL;
    pfglLineStipple               = NULL;
    pfglLineWidth                 = NULL;
    pfglListBase                  = NULL;
    pfglLoadIdentity              = NULL;
    pfglLoadMatrixd               = NULL;
    pfglLoadMatrixf               = NULL;
    pfglLoadName                  = NULL;
    pfglLogicOp                   = NULL;
    pfglMap1d                     = NULL;
    pfglMap1f                     = NULL;
    pfglMap2d                     = NULL;
    pfglMap2f                     = NULL;
    pfglMapGrid1d                 = NULL;
    pfglMapGrid1f                 = NULL;
    pfglMapGrid2d                 = NULL;
    pfglMapGrid2f                 = NULL;
    pfglMaterialf                 = NULL;
    pfglMaterialfv                = NULL;
    pfglMateriali                 = NULL;
    pfglMaterialiv                = NULL;
    pfglMatrixMode                = NULL;
    pfglMultMatrixd               = NULL;
    pfglMultMatrixf               = NULL;
    pfglNewList                   = NULL;
    pfglNormal3b                  = NULL;
    pfglNormal3bv                 = NULL;
    pfglNormal3d                  = NULL;
    pfglNormal3dv                 = NULL;
    pfglNormal3f                  = NULL;
    pfglNormal3fv                 = NULL;
    pfglNormal3i                  = NULL;
    pfglNormal3iv                 = NULL;
    pfglNormal3s                  = NULL;
    pfglNormal3sv                 = NULL;
    pfglNormalPointer             = NULL;
    pfglOrtho                     = NULL;
    pfglPassThrough               = NULL;
    pfglPixelMapfv                = NULL;
    pfglPixelMapuiv               = NULL;
    pfglPixelMapusv               = NULL;
    pfglPixelStoref               = NULL;
    pfglPixelStorei               = NULL;
    pfglPixelTransferf            = NULL;
    pfglPixelTransferi            = NULL;
    pfglPixelZoom                 = NULL;
    pfglPointSize                 = NULL;
    pfglPolygonMode               = NULL;
    pfglPolygonOffset             = NULL;
    pfglPolygonStipple            = NULL;
    pfglPopAttrib                 = NULL;
    pfglPopClientAttrib           = NULL;
    pfglPopMatrix                 = NULL;
    pfglPopName                   = NULL;
    pfglPrioritizeTextures        = NULL;
    pfglPushAttrib                = NULL;
    pfglPushClientAttrib          = NULL;
    pfglPushMatrix                = NULL;
    pfglPushName                  = NULL;
    pfglRasterPos2d               = NULL;
    pfglRasterPos2dv              = NULL;
    pfglRasterPos2f               = NULL;
    pfglRasterPos2fv              = NULL;
    pfglRasterPos2i               = NULL;
    pfglRasterPos2iv              = NULL;
    pfglRasterPos2s               = NULL;
    pfglRasterPos2sv              = NULL;
    pfglRasterPos3d               = NULL;
    pfglRasterPos3dv              = NULL;
    pfglRasterPos3f               = NULL;
    pfglRasterPos3fv              = NULL;
    pfglRasterPos3i               = NULL;
    pfglRasterPos3iv              = NULL;
    pfglRasterPos3s               = NULL;
    pfglRasterPos3sv              = NULL;
    pfglRasterPos4d               = NULL;
    pfglRasterPos4dv              = NULL;
    pfglRasterPos4f               = NULL;
    pfglRasterPos4fv              = NULL;
    pfglRasterPos4i               = NULL;
    pfglRasterPos4iv              = NULL;
    pfglRasterPos4s               = NULL;
    pfglRasterPos4sv              = NULL;
    pfglReadBuffer                = NULL;
    pfglReadPixels                = NULL;
    pfglRectd                     = NULL;
    pfglRectdv                    = NULL;
    pfglRectf                     = NULL;
    pfglRectfv                    = NULL;
    pfglRecti                     = NULL;
    pfglRectiv                    = NULL;
    pfglRects                     = NULL;
    pfglRectsv                    = NULL;
    pfglRenderMode                = NULL;
    pfglRotated                   = NULL;
    pfglRotatef                   = NULL;
    pfglScaled                    = NULL;
    pfglScalef                    = NULL;
    pfglScissor                   = NULL;
    pfglSelectBuffer              = NULL;
    pfglShadeModel                = NULL;
    pfglStencilFunc               = NULL;
    pfglStencilMask               = NULL;
    pfglStencilOp                 = NULL;
    pfglTexCoord1d                = NULL;
    pfglTexCoord1dv               = NULL;
    pfglTexCoord1f                = NULL;
    pfglTexCoord1fv               = NULL;
    pfglTexCoord1i                = NULL;
    pfglTexCoord1iv               = NULL;
    pfglTexCoord1s                = NULL;
    pfglTexCoord1sv               = NULL;
    pfglTexCoord2d                = NULL;
    pfglTexCoord2dv               = NULL;
    pfglTexCoord2f                = NULL;
    pfglTexCoord2fv               = NULL;
    pfglTexCoord2i                = NULL;
    pfglTexCoord2iv               = NULL;
    pfglTexCoord2s                = NULL;
    pfglTexCoord2sv               = NULL;
    pfglTexCoord3d                = NULL;
    pfglTexCoord3dv               = NULL;
    pfglTexCoord3f                = NULL;
    pfglTexCoord3fv               = NULL;
    pfglTexCoord3i                = NULL;
    pfglTexCoord3iv               = NULL;
    pfglTexCoord3s                = NULL;
    pfglTexCoord3sv               = NULL;
    pfglTexCoord4d                = NULL;
    pfglTexCoord4dv               = NULL;
    pfglTexCoord4f                = NULL;
    pfglTexCoord4fv               = NULL;
    pfglTexCoord4i                = NULL;
    pfglTexCoord4iv               = NULL;
    pfglTexCoord4s                = NULL;
    pfglTexCoord4sv               = NULL;
    pfglTexCoordPointer           = NULL;
    pfglTexEnvf                   = NULL;
    pfglTexEnvfv                  = NULL;
    pfglTexEnvi                   = NULL;
    pfglTexEnviv                  = NULL;
    pfglTexGend                   = NULL;
    pfglTexGendv                  = NULL;
    pfglTexGenf                   = NULL;
    pfglTexGenfv                  = NULL;
    pfglTexGeni                   = NULL;
    pfglTexGeniv                  = NULL;
    pfglTexImage1D                = NULL;
    pfglTexImage2D                = NULL;
    pfglTexParameterf             = NULL;
    pfglTexParameterfv            = NULL;
    pfglTexParameteri             = NULL;
    pfglTexParameteriv            = NULL;
    pfglTexSubImage1D             = NULL;
    pfglTexSubImage2D             = NULL;
    pfglTranslated                = NULL;
    pfglTranslatef                = NULL;
    pfglVertex2d                  = NULL;
    pfglVertex2dv                 = NULL;
    pfglVertex2f                  = NULL;
    pfglVertex2fv                 = NULL;
    pfglVertex2i                  = NULL;
    pfglVertex2iv                 = NULL;
    pfglVertex2s                  = NULL;
    pfglVertex2sv                 = NULL;
    pfglVertex3d                  = NULL;
    pfglVertex3dv                 = NULL;
    pfglVertex3f                  = NULL;
    pfglVertex3fv                 = NULL;
    pfglVertex3i                  = NULL;
    pfglVertex3iv                 = NULL;
    pfglVertex3s                  = NULL;
    pfglVertex3sv                 = NULL;
    pfglVertex4d                  = NULL;
    pfglVertex4dv                 = NULL;
    pfglVertex4f                  = NULL;
    pfglVertex4fv                 = NULL;
    pfglVertex4i                  = NULL;
    pfglVertex4iv                 = NULL;
    pfglVertex4s                  = NULL;
    pfglVertex4sv                 = NULL;
    pfglVertexPointer             = NULL;
    pfglViewport                  = NULL;
    pfglXChooseVisual             = NULL;
    pfglXCreateContext            = NULL;
    pfglXDestroyContext           = NULL;
    pfglXMakeCurrent              = NULL;
    pfglXCopyContext              = NULL;
    pfglXSwapBuffers              = NULL;
}





#ifdef _WIN32

#define GPA( a ) GetProcAddress( hinstOpenGL, a )

#elif __unix__

#define GPA( a ) dlsym( OpenGLLib, a )


void *pfwglGetProcAddress (const char *symbol)
{
    if (OpenGLLib) {
        return GPA (symbol);
    }

    return NULL;
}

#else

#error "Please define interface to OpenGL library!"

#endif




/**
 * \brief Initialize OpenGL interface.
 * \param[in] dllname Pointer to a NUL-terminated string with the name of the OpenGL dynamic link library.
 * \return Non-zero on success, otherwise zero.
 * \note This is responsible for binding our gl function pointers to
 *      the appropriate OpenGL stuff. In Windows this means doing a
 *      LoadLibrary and a bunch of calls to GetProcAddress. On other
 *      operating systems we need to do the right thing, whatever that
 *      might be.
 */
PUBLIC int OpenGL_Init (const char *dllname)
{
    if ((OpenGLLib = dlopen (dllname, RTLD_LAZY | RTLD_GLOBAL)) == 0) {
        return 0;
    }

    if (! (pfglAccum                     = (GLACCUM)GPA ("glAccum"))) return 0;

    if (! (pfglAlphaFunc                 = (GLALPHAFUNC)GPA ("glAlphaFunc"))) return 0;

    if (! (pfglAreTexturesResident       = (GLARETEXTURESRESIDENT)GPA ("glAreTexturesResident"))) return 0;

    if (! (pfglArrayElement              = (GLARRAYELEMENT)GPA ("glArrayElement"))) return 0;

    if (! (pfglBegin                     = (GLBEGIN)GPA ("glBegin"))) return 0;

    if (! (pfglBindTexture               = (GLBINDTEXTURE)GPA ("glBindTexture"))) return 0;

    if (! (pfglBitmap                    = (GLBITMAP)GPA ("glBitmap"))) return 0;

    if (! (pfglBlendFunc                 = (GLBLENDFUNC)GPA ("glBlendFunc"))) return 0;

    if (! (pfglCallList                  = (GLCALLLIST)GPA ("glCallList"))) return 0;

    if (! (pfglCallLists                 = (GLCALLLISTS)GPA ("glCallLists"))) return 0;

    if (! (pfglClear                     = (GLCLEAR)GPA ("glClear"))) return 0;

    if (! (pfglClearAccum                = (GLCLEARACCUM)GPA ("glClearAccum"))) return 0;

    if (! (pfglClearColor                = (GLCLEARCOLOR)GPA ("glClearColor"))) return 0;

    if (! (pfglClearDepth                = (GLCLEARDEPTH)GPA ("glClearDepth"))) return 0;

    if (! (pfglClearIndex                = (GLCLEARINDEX)GPA ("glClearIndex"))) return 0;

    if (! (pfglClearStencil              = (GLCLEARSTENCIL)GPA ("glClearStencil"))) return 0;

    if (! (pfglClipPlane                 = (GLCLIPPLANE)GPA ("glClipPlane"))) return 0;

    if (! (pfglColor3b                   = (GLCOLOR3B)GPA ("glColor3b"))) return 0;

    if (! (pfglColor3bv                  = (GLCOLOR3BV)GPA ("glColor3bv"))) return 0;

    if (! (pfglColor3d                   = (GLCOLOR3D)GPA ("glColor3d"))) return 0;

    if (! (pfglColor3dv                  = (GLCOLOR3DV)GPA ("glColor3dv"))) return 0;

    if (! (pfglColor3f                   = (GLCOLOR3F)GPA ("glColor3f"))) return 0;

    if (! (pfglColor3fv                  = (GLCOLOR3FV)GPA ("glColor3fv"))) return 0;

    if (! (pfglColor3i                   = (GLCOLOR3I)GPA ("glColor3i"))) return 0;

    if (! (pfglColor3iv                  = (GLCOLOR3IV)GPA ("glColor3iv"))) return 0;

    if (! (pfglColor3s                   = (GLCOLOR3S)GPA ("glColor3s"))) return 0;

    if (! (pfglColor3sv                  = (GLCOLOR3SV)GPA ("glColor3sv"))) return 0;

    if (! (pfglColor3ub                  = (GLCOLOR3UB)GPA ("glColor3ub"))) return 0;

    if (! (pfglColor3ubv                 = (GLCOLOR3UBV)GPA ("glColor3ubv"))) return 0;

    if (! (pfglColor3ui                  = (GLCOLOR3UI)GPA ("glColor3ui"))) return 0;

    if (! (pfglColor3uiv                 = (GLCOLOR3UIV)GPA ("glColor3uiv"))) return 0;

    if (! (pfglColor3us                  = (GLCOLOR3US)GPA ("glColor3us"))) return 0;

    if (! (pfglColor3usv                 = (GLCOLOR3USV)GPA ("glColor3usv"))) return 0;

    if (! (pfglColor4b                   = (GLCOLOR4B)GPA ("glColor4b"))) return 0;

    if (! (pfglColor4bv                  = (GLCOLOR4BV)GPA ("glColor4bv"))) return 0;

    if (! (pfglColor4d                   = (GLCOLOR4D)GPA ("glColor4d"))) return 0;

    if (! (pfglColor4dv                  = (GLCOLOR4DV)GPA ("glColor4dv"))) return 0;

    if (! (pfglColor4f                   = (GLCOLOR4F)GPA ("glColor4f"))) return 0;

    if (! (pfglColor4fv                  = (GLCOLOR4FV)GPA ("glColor4fv"))) return 0;

    if (! (pfglColor4i                   = (GLCOLOR4I)GPA ("glColor4i"))) return 0;

    if (! (pfglColor4iv                  = (GLCOLOR4IV)GPA ("glColor4iv"))) return 0;

    if (! (pfglColor4s                   = (GLCOLOR4S)GPA ("glColor4s"))) return 0;

    if (! (pfglColor4sv                  = (GLCOLOR4SV)GPA ("glColor4sv"))) return 0;

    if (! (pfglColor4ub                  = (GLCOLOR4UB)GPA ("glColor4ub"))) return 0;

    if (! (pfglColor4ubv                 = (GLCOLOR4UBV)GPA ("glColor4ubv"))) return 0;

    if (! (pfglColor4ui                  = (GLCOLOR4UI)GPA ("glColor4ui"))) return 0;

    if (! (pfglColor4uiv                 = (GLCOLOR4UIV)GPA ("glColor4uiv"))) return 0;

    if (! (pfglColor4us                  = (GLCOLOR4US)GPA ("glColor4us"))) return 0;

    if (! (pfglColor4usv                 = (GLCOLOR4USV)GPA ("glColor4usv"))) return 0;

    if (! (pfglColorMask                 = (GLCOLORMASK)GPA ("glColorMask"))) return 0;

    if (! (pfglColorMaterial             = (GLCOLORMATERIAL)GPA ("glColorMaterial"))) return 0;

    if (! (pfglColorPointer              = (GLCOLORPOINTER)GPA ("glColorPointer"))) return 0;

    if (! (pfglCopyPixels                = (GLCOPYPIXELS)GPA ("glCopyPixels"))) return 0;

    if (! (pfglCopyTexImage1D            = (GLCOPYTEXIMAGE1D)GPA ("glCopyTexImage1D"))) return 0;

    if (! (pfglCopyTexImage2D            = (GLCOPYTEXIMAGE2D)GPA ("glCopyTexImage2D"))) return 0;

    if (! (pfglCopyTexSubImage1D         = (GLCOPYTEXSUBIMAGE1D)GPA ("glCopyTexSubImage1D"))) return 0;

    if (! (pfglCopyTexSubImage2D         = (GLCOPYTEXSUBIMAGE2D)GPA ("glCopyTexSubImage2D"))) return 0;

    if (! (pfglCullFace                  = (GLCULLFACE)GPA ("glCullFace"))) return 0;

    if (! (pfglDeleteLists               = (GLDELETELISTS)GPA ("glDeleteLists"))) return 0;

    if (! (pfglDeleteTextures            = (GLDELETETEXTURES)GPA ("glDeleteTextures"))) return 0;

    if (! (pfglDepthFunc                 = (GLDEPTHFUNC)GPA ("glDepthFunc"))) return 0;

    if (! (pfglDepthMask                 = (GLDEPTHMASK)GPA ("glDepthMask"))) return 0;

    if (! (pfglDepthRange                = (GLDEPTHRANGE)GPA ("glDepthRange"))) return 0;

    if (! (pfglDisable                   = (GLDISABLE)GPA ("glDisable"))) return 0;

    if (! (pfglDisableClientState        = (GLDISABLECLIENTSTATE)GPA ("glDisableClientState"))) return 0;

    if (! (pfglDrawArrays                = (GLDRAWARRAYS)GPA ("glDrawArrays"))) return 0;

    if (! (pfglDrawBuffer                = (GLDRAWBUFFER)GPA ("glDrawBuffer"))) return 0;

    if (! (pfglDrawElements              = (GLDRAWELEMENTS)GPA ("glDrawElements"))) return 0;

    if (! (pfglDrawPixels                = (GLDRAWPIXELS)GPA ("glDrawPixels"))) return 0;

    if (! (pfglEdgeFlag                  = (GLEDGEFLAG)GPA ("glEdgeFlag"))) return 0;

    if (! (pfglEdgeFlagPointer           = (GLEDGEFLAGPOINTER)GPA ("glEdgeFlagPointer"))) return 0;

    if (! (pfglEdgeFlagv                 = (GLEDGEFLAGV)GPA ("glEdgeFlagv"))) return 0;

    if (! (pfglEnable                    = (GLENABLE)GPA ("glEnable"))) return 0;

    if (! (pfglEnableClientState      = (GLENABLECLIENTSTATE)GPA ("glEnableClientState"))) return 0;

    if (! (pfglEnd                       = (GLEND)GPA ("glEnd"))) return 0;

    if (! (pfglEndList                   = (GLENDLIST)GPA ("glEndList"))) return 0;

    if (! (pfglEvalCoord1d                = (GLEVALCOORD1D)GPA ("glEvalCoord1d"))) return 0;

    if (! (pfglEvalCoord1dv              = (GLEVALCOORD1DV)GPA ("glEvalCoord1dv"))) return 0;

    if (! (pfglEvalCoord1f               = (GLEVALCOORD1F)GPA ("glEvalCoord1f"))) return 0;

    if (! (pfglEvalCoord1fv              = (GLEVALCOORD1FV)GPA ("glEvalCoord1fv"))) return 0;

    if (! (pfglEvalCoord2d               = (GLEVALCOORD2D)GPA ("glEvalCoord2d"))) return 0;

    if (! (pfglEvalCoord2dv              = (GLEVALCOORD2DV)GPA ("glEvalCoord2dv"))) return 0;

    if (! (pfglEvalCoord2f               = (GLEVALCOORD2F)GPA ("glEvalCoord2f"))) return 0;

    if (! (pfglEvalCoord2fv              = (GLEVALCOORD2FV)GPA ("glEvalCoord2fv"))) return 0;

    if (! (pfglEvalMesh1                 = (GLEVALMESH1)GPA ("glEvalMesh1"))) return 0;

    if (! (pfglEvalMesh2                 = (GLEVALMESH2)GPA ("glEvalMesh2"))) return 0;

    if (! (pfglEvalPoint1                = (GLEVALPOINT1)GPA ("glEvalPoint1"))) return 0;

    if (! (pfglEvalPoint2                = (GLEVALPOINT2)GPA ("glEvalPoint2"))) return 0;

    if (! (pfglFeedbackBuffer            = (GLFEEDBACKBUFFER)GPA ("glFeedbackBuffer"))) return 0;

    if (! (pfglFinish                    = (GLFINISH)GPA ("glFinish"))) return 0;

    if (! (pfglFlush                     = (GLFLUSH)GPA ("glFlush"))) return 0;

    if (! (pfglFogf                      = (GLFOGF)GPA ("glFogf"))) return 0;

    if (! (pfglFogfv                     = (GLFOGFV)GPA ("glFogfv"))) return 0;

    if (! (pfglFogi                      = (GLFOGI)GPA ("glFogi"))) return 0;

    if (! (pfglFogiv                     = (GLFOGIV)GPA ("glFogiv"))) return 0;

    if (! (pfglFrontFace                 = (GLFRONTFACE)GPA ("glFrontFace"))) return 0;

    if (! (pfglFrustum                   = (GLFRUSTUM)GPA ("glFrustum"))) return 0;

    if (! (pfglGenLists                  = (GLGENLISTS)GPA ("glGenLists"))) return 0;

    if (! (pfglGenTextures               = (GLGENTEXTURES)GPA ("glGenTextures"))) return 0;

    if (! (pfglGetBooleanv               = (GLGETBOOLEANV)GPA ("glGetBooleanv"))) return 0;

    if (! (pfglGetClipPlane              = (GLGETCLIPPLANE)GPA ("glGetClipPlane"))) return 0;

    if (! (pfglGetDoublev                = (GLGETDOUBLEV)GPA ("glGetDoublev"))) return 0;

    if (! (pfglGetError                  = (GLGETERROR)GPA ("glGetError"))) return 0;

    if (! (pfglGetFloatv                 = (GLGETFLOATV)GPA ("glGetFloatv"))) return 0;

    if (! (pfglGetIntegerv               = (GLGETINTEGERV)GPA ("glGetIntegerv"))) return 0;

    if (! (pfglGetLightfv                = (GLGETLIGHTFV)GPA ("glGetLightfv"))) return 0;

    if (! (pfglGetLightiv                = (GLGETLIGHTIV)GPA ("glGetLightiv"))) return 0;

    if (! (pfglGetMapdv                  = (GLGETMAPDV)GPA ("glGetMapdv"))) return 0;

    if (! (pfglGetMapfv                  = (GLGETMAPFV)GPA ("glGetMapfv"))) return 0;

    if (! (pfglGetMapiv                  = (GLGETMAPIV)GPA ("glGetMapiv"))) return 0;

    if (! (pfglGetMaterialfv             = (GLGETMATERIALFV)GPA ("glGetMaterialfv"))) return 0;

    if (! (pfglGetMaterialiv             = (GLGETMATERIALIV)GPA ("glGetMaterialiv"))) return 0;

    if (! (pfglGetPixelMapfv             = (GLGETPIXELMAPFV)GPA ("glGetPixelMapfv"))) return 0;

    if (! (pfglGetPixelMapuiv            = (GLGETPIXELMAPUIV)GPA ("glGetPixelMapuiv"))) return 0;

    if (! (pfglGetPixelMapusv            = (GLGETPIXELMAPUSV)GPA ("glGetPixelMapusv"))) return 0;

    if (! (pfglGetPointerv               = (GLGETPOINTERV)GPA ("glGetPointerv"))) return 0;

    if (! (pfglGetPolygonStipple         = (GLGETPOLYGONSTIPPLE)GPA ("glGetPolygonStipple"))) return 0;

    if (! (pfglGetString                 = (GLGETSTRING)GPA ("glGetString"))) return 0;

    if (! (pfglGetTexEnvfv               = (GLGETTEXENVFV)GPA ("glGetTexEnvfv"))) return 0;

    if (! (pfglGetTexEnviv               = (GLGETTEXENVIV)GPA ("glGetTexEnviv"))) return 0;

    if (! (pfglGetTexGendv               = (GLGETTEXGENDV)GPA ("glGetTexGendv"))) return 0;

    if (! (pfglGetTexGenfv               = (GLGETTEXGENFV)GPA ("glGetTexGenfv"))) return 0;

    if (! (pfglGetTexGeniv               = (GLGETTEXGENIV)GPA ("glGetTexGeniv"))) return 0;

    if (! (pfglGetTexImage               = (GLGETTEXIMAGE)GPA ("glGetTexImage"))) return 0;

    if (! (pfglGetTexParameterfv         = (GLGETTEXPARAMETERFV)GPA ("glGetTexParameterfv"))) return 0;

    if (! (pfglGetTexParameteriv         = (GLGETTEXPARAMETERIV)GPA ("glGetTexParameteriv"))) return 0;

    if (! (pfglHint                      = (GLHINT)GPA ("glHint"))) return 0;

    if (! (pfglIndexMask                 = (GLINDEXMASK)GPA ("glIndexMask"))) return 0;

    if (! (pfglIndexPointer              = (GLINDEXPOINTER)GPA ("glIndexPointer"))) return 0;

    if (! (pfglIndexd                    = (GLINDEXD)GPA ("glIndexd"))) return 0;

    if (! (pfglIndexdv                   = (GLINDEXDV)GPA ("glIndexdv"))) return 0;

    if (! (pfglIndexf                    = (GLINDEXF)GPA ("glIndexf"))) return 0;

    if (! (pfglIndexfv                   = (GLINDEXFV)GPA ("glIndexfv"))) return 0;

    if (! (pfglIndexi                    = (GLINDEXI)GPA ("glIndexi"))) return 0;

    if (! (pfglIndexiv                   = (GLINDEXIV)GPA ("glIndexiv"))) return 0;

    if (! (pfglIndexs                    = (GLINDEXS)GPA ("glIndexs"))) return 0;

    if (! (pfglIndexsv                   = (GLINDEXSV)GPA ("glIndexsv"))) return 0;

    if (! (pfglIndexub                   = (GLINDEXUB)GPA ("glIndexub"))) return 0;

    if (! (pfglIndexubv                  = (GLINDEXUBV)GPA ("glIndexubv"))) return 0;

    if (! (pfglInitNames                 = (GLINITNAMES)GPA ("glInitNames"))) return 0;

    if (! (pfglInterleavedArrays         = (GLINTERLEAVEDARRAYS)GPA ("glInterleavedArrays"))) return 0;

    if (! (pfglIsEnabled                 = (GLISENABLED)GPA ("glIsEnabled"))) return 0;

    if (! (pfglIsList                    = (GLISLIST)GPA ("glIsList"))) return 0;

    if (! (pfglIsTexture                 = (GLISTEXTURE)GPA ("glIsTexture"))) return 0;

    if (! (pfglLightModelf               = (GLLIGHTMODELF)GPA ("glLightModelf"))) return 0;

    if (! (pfglLightModelfv              = (GLLIGHTMODELFV)GPA ("glLightModelfv"))) return 0;

    if (! (pfglLightModeli               = (GLLIGHTMODELI)GPA ("glLightModeli"))) return 0;

    if (! (pfglLightModeliv              = (GLLIGHTMODELIV)GPA ("glLightModeliv"))) return 0;

    if (! (pfglLightf                    = (GLLIGHTF)GPA ("glLightf"))) return 0;

    if (! (pfglLightfv                   = (GLLIGHTFV)GPA ("glLightfv"))) return 0;

    if (! (pfglLighti                    = (GLLIGHTI)GPA ("glLighti"))) return 0;

    if (! (pfglLightiv                   = (GLLIGHTIV)GPA ("glLightiv"))) return 0;

    if (! (pfglLineStipple               = (GLLINESTIPPLE)GPA ("glLineStipple"))) return 0;

    if (! (pfglLineWidth                 = (GLLINEWIDTH)GPA ("glLineWidth"))) return 0;

    if (! (pfglListBase                  = (GLLISTBASE)GPA ("glListBase"))) return 0;

    if (! (pfglLoadIdentity              = (GLLOADIDENTITY)GPA ("glLoadIdentity"))) return 0;

    if (! (pfglLoadMatrixd               = (GLLOADMATRIXD)GPA ("glLoadMatrixd"))) return 0;

    if (! (pfglLoadMatrixf               = (GLLOADMATRIXF)GPA ("glLoadMatrixf"))) return 0;

    if (! (pfglLoadName                  = (GLLOADNAME)GPA ("glLoadName"))) return 0;

    if (! (pfglLogicOp                   = (GLLOGICOP)GPA ("glLogicOp"))) return 0;

    if (! (pfglMap1d                     = (GLMAP1D)GPA ("glMap1d"))) return 0;

    if (! (pfglMap1f                     = (GLMAP1F)GPA ("glMap1f"))) return 0;

    if (! (pfglMap2d                     = (GLMAP2D)GPA ("glMap2d"))) return 0;

    if (! (pfglMap2f                     = (GLMAP2F)GPA ("glMap2f"))) return 0;

    if (! (pfglMapGrid1d                 = (GLMAPGRID1D)GPA ("glMapGrid1d"))) return 0;

    if (! (pfglMapGrid1f                 = (GLMAPGRID1F)GPA ("glMapGrid1f"))) return 0;

    if (! (pfglMapGrid2d                 = (GLMAPGRID2D)GPA ("glMapGrid2d"))) return 0;

    if (! (pfglMapGrid2f                 = (GLMAPGRID2F)GPA ("glMapGrid2f"))) return 0;

    if (! (pfglMaterialf                 = (GLMATERIALF)GPA ("glMaterialf"))) return 0;

    if (! (pfglMaterialfv                = (GLMATERIALFV)GPA ("glMaterialfv"))) return 0;

    if (! (pfglMateriali                 = (GLMATERIALI)GPA ("glMateriali"))) return 0;

    if (! (pfglMaterialiv                = (GLMATERIALIV)GPA ("glMaterialiv"))) return 0;

    if (! (pfglMatrixMode                = (GLMATRIXMODE)GPA ("glMatrixMode"))) return 0;

    if (! (pfglMultMatrixd               = (GLMULTMATRIXD)GPA ("glMultMatrixd"))) return 0;

    if (! (pfglMultMatrixf               = (GLMULTMATRIXF)GPA ("glMultMatrixf"))) return 0;

    if (! (pfglNewList                   = (GLNEWLIST)GPA ("glNewList"))) return 0;

    if (! (pfglNormal3b                  = (GLNORMAL3B)GPA ("glNormal3b"))) return 0;

    if (! (pfglNormal3bv                 = (GLNORMAL3BV)GPA ("glNormal3bv"))) return 0;

    if (! (pfglNormal3d                  = (GLNORMAL3D)GPA ("glNormal3d"))) return 0;

    if (! (pfglNormal3dv                 = (GLNORMAL3DV)GPA ("glNormal3dv"))) return 0;

    if (! (pfglNormal3f                  = (GLNORMAL3F)GPA ("glNormal3f"))) return 0;

    if (! (pfglNormal3fv                 = (GLNORMAL3FV)GPA ("glNormal3fv"))) return 0;

    if (! (pfglNormal3i                  = (GLNORMAL3I)GPA ("glNormal3i"))) return 0;

    if (! (pfglNormal3iv                 = (GLNORMAL3IV)GPA ("glNormal3iv"))) return 0;

    if (! (pfglNormal3s                  = (GLNORMAL3S)GPA ("glNormal3s"))) return 0;

    if (! (pfglNormal3sv                 = (GLNORMAL3SV)GPA ("glNormal3sv"))) return 0;

    if (! (pfglNormalPointer             = (GLNORMALPOINTER)GPA ("glNormalPointer"))) return 0;

    if (! (pfglOrtho                     = (GLORTHO)GPA ("glOrtho"))) return 0;

    if (! (pfglPassThrough               = (GLPASSTHROUGH)GPA ("glPassThrough"))) return 0;

    if (! (pfglPixelMapfv                = (GLPIXELMAPFV)GPA ("glPixelMapfv"))) return 0;

    if (! (pfglPixelMapuiv               = (GLPIXELMAPUIV)GPA ("glPixelMapuiv"))) return 0;

    if (! (pfglPixelMapusv               = (GLPIXELMAPUSV)GPA ("glPixelMapusv"))) return 0;

    if (! (pfglPixelStoref               = (GLPIXELSTOREF)GPA ("glPixelStoref"))) return 0;

    if (! (pfglPixelStorei               = (GLPIXELSTOREI)GPA ("glPixelStorei"))) return 0;

    if (! (pfglPixelTransferf            = (GLPIXELTRANSFERF)GPA ("glPixelTransferf"))) return 0;

    if (! (pfglPixelTransferi            = (GLPIXELTRANSFERI)GPA ("glPixelTransferi"))) return 0;

    if (! (pfglPixelZoom                 = (GLPIXELZOOM)GPA ("glPixelZoom"))) return 0;

    if (! (pfglPointSize                 = (GLPOINTSIZE)GPA ("glPointSize"))) return 0;

    if (! (pfglPolygonMode               = (GLPOLYGONMODE)GPA ("glPolygonMode"))) return 0;

    if (! (pfglPolygonOffset             = (GLPOLYGONOFFSET)GPA ("glPolygonOffset"))) return 0;

    if (! (pfglPolygonStipple            = (GLPOLYGONSTIPPLE)GPA ("glPolygonStipple"))) return 0;

    if (! (pfglPopAttrib                 = (GLPOPATTRIB)GPA ("glPopAttrib"))) return 0;

    if (! (pfglPopClientAttrib           = (GLPOPCLIENTATTRIB)GPA ("glPopClientAttrib"))) return 0;

    if (! (pfglPopMatrix                 = (GLPOPMATRIX)GPA ("glPopMatrix"))) return 0;

    if (! (pfglPopName                   = (GLPOPNAME)GPA ("glPopName"))) return 0;

    if (! (pfglPrioritizeTextures        = (GLPRIORITIZETEXTURES)GPA ("glPrioritizeTextures"))) return 0;

    if (! (pfglPushAttrib                = (GLPUSHATTRIB)GPA ("glPushAttrib"))) return 0;

    if (! (pfglPushClientAttrib          = (GLPUSHCLIENTATTRIB)GPA ("glPushClientAttrib"))) return 0;

    if (! (pfglPushMatrix                = (GLPUSHMATRIX)GPA ("glPushMatrix"))) return 0;

    if (! (pfglPushName                  = (GLPUSHNAME)GPA ("glPushName"))) return 0;

    if (! (pfglRasterPos2d               = (GLRASTERPOS2D)GPA ("glRasterPos2d"))) return 0;

    if (! (pfglRasterPos2dv              = (GLRASTERPOS2DV)GPA ("glRasterPos2dv"))) return 0;

    if (! (pfglRasterPos2f               = (GLRASTERPOS2F)GPA ("glRasterPos2f"))) return 0;

    if (! (pfglRasterPos2fv              = (GLRASTERPOS2FV)GPA ("glRasterPos2fv"))) return 0;

    if (! (pfglRasterPos2i               = (GLRASTERPOS2I)GPA ("glRasterPos2i"))) return 0;

    if (! (pfglRasterPos2iv              = (GLRASTERPOS2IV)GPA ("glRasterPos2iv"))) return 0;

    if (! (pfglRasterPos2s               = (GLRASTERPOS2S)GPA ("glRasterPos2s"))) return 0;

    if (! (pfglRasterPos2sv              = (GLRASTERPOS2SV)GPA ("glRasterPos2sv"))) return 0;

    if (! (pfglRasterPos3d               = (GLRASTERPOS3D)GPA ("glRasterPos3d"))) return 0;

    if (! (pfglRasterPos3dv              = (GLRASTERPOS3DV)GPA ("glRasterPos3dv"))) return 0;

    if (! (pfglRasterPos3f               = (GLRASTERPOS3F)GPA ("glRasterPos3f"))) return 0;

    if (! (pfglRasterPos3fv              = (GLRASTERPOS3FV)GPA ("glRasterPos3fv"))) return 0;

    if (! (pfglRasterPos3i               = (GLRASTERPOS3I)GPA ("glRasterPos3i"))) return 0;

    if (! (pfglRasterPos3iv              = (GLRASTERPOS3IV)GPA ("glRasterPos3iv"))) return 0;

    if (! (pfglRasterPos3s               = (GLRASTERPOS3S)GPA ("glRasterPos3s"))) return 0;

    if (! (pfglRasterPos3sv              = (GLRASTERPOS3SV)GPA ("glRasterPos3sv"))) return 0;

    if (! (pfglRasterPos4d               = (GLRASTERPOS4D)GPA ("glRasterPos4d"))) return 0;

    if (! (pfglRasterPos4dv              = (GLRASTERPOS4DV)GPA ("glRasterPos4dv"))) return 0;

    if (! (pfglRasterPos4f               = (GLRASTERPOS4F)GPA ("glRasterPos4f"))) return 0;

    if (! (pfglRasterPos4fv              = (GLRASTERPOS4FV)GPA ("glRasterPos4fv"))) return 0;

    if (! (pfglRasterPos4i               = (GLRASTERPOS4I)GPA ("glRasterPos4i"))) return 0;

    if (! (pfglRasterPos4iv              = (GLRASTERPOS4IV)GPA ("glRasterPos4iv"))) return 0;

    if (! (pfglRasterPos4s               = (GLRASTERPOS4S)GPA ("glRasterPos4s"))) return 0;

    if (! (pfglRasterPos4sv              = (GLRASTERPOS4SV)GPA ("glRasterPos4sv"))) return 0;

    if (! (pfglReadBuffer                = (GLREADBUFFER)GPA ("glReadBuffer"))) return 0;

    if (! (pfglReadPixels                = (GLREADPIXELS)GPA ("glReadPixels"))) return 0;

    if (! (pfglRectd                     = (GLRECTD)GPA ("glRectd"))) return 0;

    if (! (pfglRectdv                    = (GLRECTDV)GPA ("glRectdv"))) return 0;

    if (! (pfglRectf                     = (GLRECTF)GPA ("glRectf"))) return 0;

    if (! (pfglRectfv                    = (GLRECTFV)GPA ("glRectfv"))) return 0;

    if (! (pfglRecti                     = (GLRECTI)GPA ("glRecti"))) return 0;

    if (! (pfglRectiv                    = (GLRECTIV)GPA ("glRectiv"))) return 0;

    if (! (pfglRects                     = (GLRECTS)GPA ("glRects"))) return 0;

    if (! (pfglRectsv                    = (GLRECTSV)GPA ("glRectsv"))) return 0;

    if (! (pfglRenderMode                = (GLRENDERMODE)GPA ("glRenderMode"))) return 0;

    if (! (pfglRotated                   = (GLROTATED)GPA ("glRotated"))) return 0;

    if (! (pfglRotatef                   = (GLROTATEF)GPA ("glRotatef"))) return 0;

    if (! (pfglScaled                    = (GLSCALED)GPA ("glScaled"))) return 0;

    if (! (pfglScalef                    = (GLSCALEF)GPA ("glScalef"))) return 0;

    if (! (pfglScissor                   = (GLSCISSOR)GPA ("glScissor"))) return 0;

    if (! (pfglSelectBuffer              = (GLSELECTBUFFER)GPA ("glSelectBuffer"))) return 0;

    if (! (pfglShadeModel                = (GLSHADEMODEL)GPA ("glShadeModel"))) return 0;

    if (! (pfglStencilFunc               = (GLSTENCILFUNC)GPA ("glStencilFunc"))) return 0;

    if (! (pfglStencilMask               = (GLSTENCILMASK)GPA ("glStencilMask"))) return 0;

    if (! (pfglStencilOp                 = (GLSTENCILOP)GPA ("glStencilOp"))) return 0;

    if (! (pfglTexCoord1d                = (GLTEXCOORD1D)GPA ("glTexCoord1d"))) return 0;

    if (! (pfglTexCoord1dv               = (GLTEXCOORD1DV)GPA ("glTexCoord1dv"))) return 0;

    if (! (pfglTexCoord1f                = (GLTEXCOORD1F)GPA ("glTexCoord1f"))) return 0;

    if (! (pfglTexCoord1fv               = (GLTEXCOORD1FV)GPA ("glTexCoord1fv"))) return 0;

    if (! (pfglTexCoord1i                = (GLTEXCOORD1I)GPA ("glTexCoord1i"))) return 0;

    if (! (pfglTexCoord1iv               = (GLTEXCOORD1IV)GPA ("glTexCoord1iv"))) return 0;

    if (! (pfglTexCoord1s                = (GLTEXCOORD1S)GPA ("glTexCoord1s"))) return 0;

    if (! (pfglTexCoord1sv               = (GLTEXCOORD1SV)GPA ("glTexCoord1sv"))) return 0;

    if (! (pfglTexCoord2d                = (GLTEXCOORD2D)GPA ("glTexCoord2d"))) return 0;

    if (! (pfglTexCoord2dv               = (GLTEXCOORD2DV)GPA ("glTexCoord2dv"))) return 0;

    if (! (pfglTexCoord2f                = (GLTEXCOORD2F)GPA ("glTexCoord2f"))) return 0;

    if (! (pfglTexCoord2fv               = (GLTEXCOORD2FV)GPA ("glTexCoord2fv"))) return 0;

    if (! (pfglTexCoord2i                = (GLTEXCOORD2I)GPA ("glTexCoord2i"))) return 0;

    if (! (pfglTexCoord2iv               = (GLTEXCOORD2IV)GPA ("glTexCoord2iv"))) return 0;

    if (! (pfglTexCoord2s                = (GLTEXCOORD2S)GPA ("glTexCoord2s"))) return 0;

    if (! (pfglTexCoord2sv               = (GLTEXCOORD2SV)GPA ("glTexCoord2sv"))) return 0;

    if (! (pfglTexCoord3d                = (GLTEXCOORD3D)GPA ("glTexCoord3d"))) return 0;

    if (! (pfglTexCoord3dv               = (GLTEXCOORD3DV)GPA ("glTexCoord3dv"))) return 0;

    if (! (pfglTexCoord3f                = (GLTEXCOORD3F)GPA ("glTexCoord3f"))) return 0;

    if (! (pfglTexCoord3fv               = (GLTEXCOORD3FV)GPA ("glTexCoord3fv"))) return 0;

    if (! (pfglTexCoord3i                = (GLTEXCOORD3I)GPA ("glTexCoord3i"))) return 0;

    if (! (pfglTexCoord3iv               = (GLTEXCOORD3IV)GPA ("glTexCoord3iv"))) return 0;

    if (! (pfglTexCoord3s                = (GLTEXCOORD3S)GPA ("glTexCoord3s"))) return 0;

    if (! (pfglTexCoord3sv               = (GLTEXCOORD3SV)GPA ("glTexCoord3sv"))) return 0;

    if (! (pfglTexCoord4d                = (GLTEXCOORD4D)GPA ("glTexCoord4d"))) return 0;

    if (! (pfglTexCoord4dv               = (GLTEXCOORD4DV)GPA ("glTexCoord4dv"))) return 0;

    if (! (pfglTexCoord4f                = (GLTEXCOORD4F)GPA ("glTexCoord4f"))) return 0;

    if (! (pfglTexCoord4fv               = (GLTEXCOORD4FV)GPA ("glTexCoord4fv"))) return 0;

    if (! (pfglTexCoord4i                = (GLTEXCOORD4I)GPA ("glTexCoord4i"))) return 0;

    if (! (pfglTexCoord4iv               = (GLTEXCOORD4IV)GPA ("glTexCoord4iv"))) return 0;

    if (! (pfglTexCoord4s                = (GLTEXCOORD4S)GPA ("glTexCoord4s"))) return 0;

    if (! (pfglTexCoord4sv               = (GLTEXCOORD4SV)GPA ("glTexCoord4sv"))) return 0;

    if (! (pfglTexCoordPointer           = (GLTEXCOORDPOINTER)GPA ("glTexCoordPointer"))) return 0;

    if (! (pfglTexEnvf                   = (GLTEXENVF)GPA ("glTexEnvf"))) return 0;

    if (! (pfglTexEnvfv                  = (GLTEXENVFV)GPA ("glTexEnvfv"))) return 0;

    if (! (pfglTexEnvi                   = (GLTEXENVI)GPA ("glTexEnvi"))) return 0;

    if (! (pfglTexEnviv                  = (GLTEXENVIV)GPA ("glTexEnviv"))) return 0;

    if (! (pfglTexGend                   = (GLTEXGEND)GPA ("glTexGend"))) return 0;

    if (! (pfglTexGendv                  = (GLTEXGENDV)GPA ("glTexGendv"))) return 0;

    if (! (pfglTexGenf                   = (GLTEXGENF)GPA ("glTexGenf"))) return 0;

    if (! (pfglTexGenfv                  = (GLTEXGENFV)GPA ("glTexGenfv"))) return 0;

    if (! (pfglTexGeni                   = (GLTEXGENI)GPA ("glTexGeni"))) return 0;

    if (! (pfglTexGeniv                  = (GLTEXGENIV)GPA ("glTexGeniv"))) return 0;

    if (! (pfglTexImage1D                = (GLTEXIMAGE1D)GPA ("glTexImage1D"))) return 0;

    if (! (pfglTexImage2D                = (GLTEXIMAGE2D)GPA ("glTexImage2D"))) return 0;

    if (! (pfglTexParameterf             = (GLTEXPARAMETERF)GPA ("glTexParameterf"))) return 0;

    if (! (pfglTexParameterfv            = (GLTEXPARAMETERFV)GPA ("glTexParameterfv"))) return 0;

    if (! (pfglTexParameteri             = (GLTEXPARAMETERI)GPA ("glTexParameteri"))) return 0;

    if (! (pfglTexParameteriv            = (GLTEXPARAMETERIV)GPA ("glTexParameteriv"))) return 0;

    if (! (pfglTexSubImage1D             = (GLTEXSUBIMAGE1D)GPA ("glTexSubImage1D"))) return 0;

    if (! (pfglTexSubImage2D             = (GLTEXSUBIMAGE2D)GPA ("glTexSubImage2D"))) return 0;

    if (! (pfglTranslated                = (GLTRANSLATED)GPA ("glTranslated"))) return 0;

    if (! (pfglTranslatef                = (GLTRANSLATEF)GPA ("glTranslatef"))) return 0;

    if (! (pfglVertex2d                  = (GLVERTEX2D)GPA ("glVertex2d"))) return 0;

    if (! (pfglVertex2dv                 = (GLVERTEX2DV)GPA ("glVertex2dv"))) return 0;

    if (! (pfglVertex2f                  = (GLVERTEX2F)GPA ("glVertex2f"))) return 0;

    if (! (pfglVertex2fv                 = (GLVERTEX2FV)GPA ("glVertex2fv"))) return 0;

    if (! (pfglVertex2i                  = (GLVERTEX2I)GPA ("glVertex2i"))) return 0;

    if (! (pfglVertex2iv                 = (GLVERTEX2IV)GPA ("glVertex2iv"))) return 0;

    if (! (pfglVertex2s                  = (GLVERTEX2S)GPA ("glVertex2s"))) return 0;

    if (! (pfglVertex2sv                 = (GLVERTEX2SV)GPA ("glVertex2sv"))) return 0;

    if (! (pfglVertex3d                  = (GLVERTEX3D)GPA ("glVertex3d"))) return 0;

    if (! (pfglVertex3dv                 = (GLVERTEX3DV)GPA ("glVertex3dv"))) return 0;

    if (! (pfglVertex3f                  = (GLVERTEX3F)GPA ("glVertex3f"))) return 0;

    if (! (pfglVertex3fv                 = (GLVERTEX3FV)GPA ("glVertex3fv"))) return 0;

    if (! (pfglVertex3i                  = (GLVERTEX3I)GPA ("glVertex3i"))) return 0;

    if (! (pfglVertex3iv                 = (GLVERTEX3IV)GPA ("glVertex3iv"))) return 0;

    if (! (pfglVertex3s                  = (GLVERTEX3S)GPA ("glVertex3s"))) return 0;

    if (! (pfglVertex3sv                 = (GLVERTEX3SV)GPA ("glVertex3sv"))) return 0;

    if (! (pfglVertex4d                  = (GLVERTEX4D)GPA ("glVertex4d"))) return 0;

    if (! (pfglVertex4dv                 = (GLVERTEX4DV)GPA ("glVertex4dv"))) return 0;

    if (! (pfglVertex4f                  = (GLVERTEX4F)GPA ("glVertex4f"))) return 0;

    if (! (pfglVertex4fv                 = (GLVERTEX4FV)GPA ("glVertex4fv"))) return 0;

    if (! (pfglVertex4i                  = (GLVERTEX4I)GPA ("glVertex4i"))) return 0;

    if (! (pfglVertex4iv                 = (GLVERTEX4IV)GPA ("glVertex4iv"))) return 0;

    if (! (pfglVertex4s                  = (GLVERTEX4S)GPA ("glVertex4s"))) return 0;

    if (! (pfglVertex4sv                 = (GLVERTEX4SV)GPA ("glVertex4sv"))) return 0;

    if (! (pfglVertexPointer             = (GLVERTEXPOINTER)GPA ("glVertexPointer"))) return 0;

    if (! (pfglViewport                  = (GLVIEWPORT)GPA ("glViewport"))) return 0;

    if (! (pfglXChooseVisual             =  GPA ("glXChooseVisual"))) return 0;

    if (! (pfglXCreateContext            =  GPA ("glXCreateContext"))) return 0;

    if (! (pfglXDestroyContext           =  GPA ("glXDestroyContext"))) return 0;

    if (! (pfglXMakeCurrent              =  GPA ("glXMakeCurrent"))) return 0;

    if (! (pfglXCopyContext              =  GPA ("glXCopyContext"))) return 0;

    if (! (pfglXSwapBuffers              =  GPA ("glXSwapBuffers"))) return 0;

    return 1;
}
