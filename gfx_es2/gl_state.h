#pragma once

#include <functional>
#include <string.h>
#include <string>
#include "gfx/gl_common.h"
#include "gfx_es2/gpu_features.h"

#ifdef USING_GLES2

#ifndef GL_MIN_EXT
#define GL_MIN_EXT 0x8007
#endif

#ifndef GL_MAX_EXT
#define GL_MAX_EXT 0x8008
#endif

#if defined(ANDROID) || defined(BLACKBERRY)
#include <EGL/egl.h>
// Additional extensions not included in GLES2/gl2ext.h from the NDK

typedef uint64_t EGLuint64NV;
typedef EGLuint64NV (EGLAPIENTRYP PFNEGLGETSYSTEMTIMEFREQUENCYNVPROC) (void);
typedef EGLuint64NV (EGLAPIENTRYP PFNEGLGETSYSTEMTIMENVPROC) (void);
extern PFNEGLGETSYSTEMTIMEFREQUENCYNVPROC eglGetSystemTimeFrequencyNV;
extern PFNEGLGETSYSTEMTIMENVPROC eglGetSystemTimeNV;

typedef GLvoid* (GL_APIENTRYP PFNGLMAPBUFFERPROC) (GLenum target, GLenum access);
extern PFNGLMAPBUFFERPROC glMapBuffer;

typedef void (EGLAPIENTRYP PFNGLDRAWTEXTURENVPROC) (GLuint texture, GLuint sampler, GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1, GLfloat z, GLfloat s0, GLfloat t0, GLfloat s1, GLfloat t1);
extern PFNGLDRAWTEXTURENVPROC glDrawTextureNV;
typedef void (EGLAPIENTRYP PFNGLCOPYIMAGESUBDATANVPROC) (GLuint srcName, GLenum
																											srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName,
																											GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei
																											width, GLsizei height, GLsizei depth);
extern PFNGLCOPYIMAGESUBDATANVPROC glCopyImageSubDataNV;
#ifndef ARM64
typedef void (EGLAPIENTRYP PFNGLBLITFRAMEBUFFERNVPROC) (
	GLint srcX0, GLint srcY0, GLint srcX1, GLuint srcY1,
	GLint dstX0, GLint dstY0, GLint dstX1, GLuint dstY1,
	GLint mask, GLenum filter);
#endif
extern PFNGLBLITFRAMEBUFFERNVPROC glBlitFramebufferNV;

extern PFNGLDISCARDFRAMEBUFFEREXTPROC glDiscardFramebufferEXT;
extern PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOES;
extern PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES;
extern PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES;
extern PFNGLISVERTEXARRAYOESPROC glIsVertexArrayOES;
#endif

// Rename standard functions to the OES version.
#define glGenVertexArrays glGenVertexArraysOES
#define glBindVertexArray glBindVertexArrayOES
#define glDeleteVertexArrays glDeleteVertexArraysOES
#define glIsVertexArray glIsVertexArrayOES

#endif /* EGL_NV_system_time */


// OpenGL state cache. Should convert all code to use this instead of directly calling glEnable etc,
// as GL state changes can be expensive on some hardware.
class OpenGLState {
private:
	template<GLenum cap, bool init>
	class BoolState {
	private:
		bool _value;
	public:
		BoolState() : _value(init) {
			OpenGLState::state_count++;
		}

		inline void set(bool value) {
			if(value && value != _value) {
				_value = value;
				glEnable(cap);
			}
			if(!value && value != _value) {
				_value = value;
				glDisable(cap);
			}
		}
		inline void enable() {
			set(true);
		}
		inline void disable() {
			set(false);
		}
		operator bool() const {
			return isset();
		}
		inline bool isset() {
			return _value;
		}
		void restore() {
			if(_value)
				glEnable(cap);
			else
				glDisable(cap);
		}
	};

#define STATE1(func, p1type, p1def) \
	class SavedState1_##func { \
		p1type p1; \
	public: \
		SavedState1_##func() : p1(p1def) { \
			OpenGLState::state_count++; \
		} \
		void set(p1type newp1) { \
			if(newp1 != p1) { \
				p1 = newp1; \
				func(p1); \
			} \
		} \
		void restore() { \
			func(p1); \
		} \
	}

#define STATE2(func, p1type, p2type, p1def, p2def) \
	class SavedState2_##func { \
		p1type p1; \
		p2type p2; \
	public: \
		SavedState2_##func() : p1(p1def), p2(p2def) { \
			OpenGLState::state_count++; \
		} \
		inline void set(p1type newp1, p2type newp2) { \
			if(newp1 != p1 || newp2 != p2) { \
				p1 = newp1; \
				p2 = newp2; \
				func(p1, p2); \
			} \
		} \
		inline void restore() { \
			func(p1, p2); \
		} \
	}

#define STATE3(func, p1type, p2type, p3type, p1def, p2def, p3def) \
	class SavedState3_##func { \
		p1type p1; \
		p2type p2; \
		p3type p3; \
	public: \
		SavedState3_##func() : p1(p1def), p2(p2def), p3(p3def) { \
			OpenGLState::state_count++; \
		} \
		inline void set(p1type newp1, p2type newp2, p3type newp3) { \
			if(newp1 != p1 || newp2 != p2 || newp3 != p3) { \
				p1 = newp1; \
				p2 = newp2; \
				p3 = newp3; \
				func(p1, p2, p3); \
			} \
		} \
		inline void restore() { \
			func(p1, p2, p3); \
		} \
	}

	#define STATE4(func, p1type, p2type, p3type, p4type, p1def, p2def, p3def, p4def) \
	class SavedState4_##func { \
		p1type p1; \
		p2type p2; \
		p3type p3; \
		p4type p4; \
	public: \
		SavedState4_##func() : p1(p1def), p2(p2def), p3(p3def), p4(p4def) { \
			OpenGLState::state_count++; \
		} \
		inline void set(p1type newp1, p2type newp2, p3type newp3, p4type newp4) { \
			if(newp1 != p1 || newp2 != p2 || newp3 != p3 || newp4 != p4) { \
				p1 = newp1; \
				p2 = newp2; \
				p3 = newp3; \
				p4 = newp4; \
				func(p1, p2, p3, p4); \
			} \
		} \
		inline void restore() { \
			func(p1, p2, p3, p4); \
		} \
	}

#define STATEFLOAT4(func, def) \
	class SavedState4_##func { \
		float p[4]; \
	public: \
		SavedState4_##func() { \
			for (int i = 0; i < 4; i++) {p[i] = def;} \
			OpenGLState::state_count++; \
		} \
		inline void set(const float v[4]) { \
			if(memcmp(p,v,sizeof(float)*4)) { \
				memcpy(p,v,sizeof(float)*4); \
				func(p[0], p[1], p[2], p[3]); \
			} \
		} \
		inline void restore() { \
			func(p[0], p[1], p[2], p[3]); \
		} \
	}

#define STATEBIND(func, target) \
	class SavedBind_##func_##target { \
		GLuint val_; \
	public: \
		SavedBind_##func_##target() { \
			val_ = 0; \
			OpenGLState::state_count++; \
		} \
		inline void bind(GLuint val) { \
			if (val_ != val) { \
				func(target, val); \
				val_ = val; \
			} \
		} \
		inline void unbind() { \
			bind(0); \
		} \
		inline void restore() { \
			func(target, val_); \
		} \
	}

	bool initialized;

public:
	static int state_count;
	OpenGLState() : initialized(false) {}
	void Initialize();
	void Restore();

	// When adding a state here, don't forget to add it to OpenGLState::Restore() too

	// Blending
	BoolState<GL_BLEND, false> blend;
	STATE4(glBlendFuncSeparate, GLenum, GLenum, GLenum, GLenum, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) blendFuncSeparate;

	// On OpenGL ES, using minmax blend requires glBlendEquationEXT (in theory at least but I don't think it's true in practice)
	STATE2(glBlendEquationSeparate, GLenum, GLenum, GL_FUNC_ADD, GL_FUNC_ADD) blendEquationSeparate;
	STATEFLOAT4(glBlendColor, 1.0f) blendColor;

	// Logic Ops. Not available on OpenGL ES at all.
#if !defined(USING_GLES2)
	BoolState<GL_COLOR_LOGIC_OP, false> colorLogicOp;
	STATE1(glLogicOp, GLenum, GL_COPY) logicOp;
#endif

	// Dither
	BoolState<GL_DITHER, false> dither;

	// Cull Face
	BoolState<GL_CULL_FACE, false> cullFace;
	STATE1(glCullFace, GLenum, GL_FRONT) cullFaceMode;
	STATE1(glFrontFace, GLenum, GL_CCW) frontFace;

	// Depth Test
	BoolState<GL_DEPTH_TEST, false> depthTest;
#if defined(USING_GLES2)
	STATE2(glDepthRangef, float, float, 0.f, 1.f) depthRange;
#else
	STATE2(glDepthRange, double, double, 0.0, 1.0) depthRange;
#endif
	STATE1(glDepthFunc, GLenum, GL_LESS) depthFunc;
	STATE1(glDepthMask, GLboolean, GL_TRUE) depthWrite;

	// Color Mask
	STATE4(glColorMask, bool, bool, bool, bool, true, true, true, true) colorMask;

	// Viewport
	STATE4(glViewport, GLint, GLint, GLsizei, GLsizei, 0, 0, 128, 128) viewport;

	// Scissor Test
	BoolState<GL_SCISSOR_TEST, false> scissorTest;
	STATE4(glScissor, GLint, GLint, GLsizei, GLsizei, 0, 0, 128, 128) scissorRect;

	// Stencil Test
	BoolState<GL_STENCIL_TEST, false> stencilTest;
	STATE3(glStencilOp, GLenum, GLenum, GLenum, GL_KEEP, GL_KEEP, GL_KEEP) stencilOp;
	STATE3(glStencilFunc, GLenum, GLint, GLuint, GL_ALWAYS, 0, 0xFF) stencilFunc;
	STATE1(glStencilMask, GLuint, 0xFF) stencilMask;

	STATEBIND(glBindBuffer, GL_ARRAY_BUFFER) arrayBuffer;
	STATEBIND(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER) elementArrayBuffer;

	// Only works on Win32, all other platforms are "force-vsync"
	void SetVSyncInterval(int interval);  // one of the above VSYNC, or a higher number for multi-frame waits (could be useful for 30hz games)
};

#undef STATE1
#undef STATE2

extern OpenGLState glstate;


extern std::string g_all_gl_extensions;
extern std::string g_all_egl_extensions;

void CheckGLExtensions();
