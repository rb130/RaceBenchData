
#include <cstring>
#include "PBOFrameBuffer.hxx"

#ifdef NEED_ARB_WRAPPERS
extern "C" {

#define ARB_NAME_WRAPPER(a) a##ARB

GLAPI GLvoid *APIENTRY glMapBuffer(GLenum a, GLenum b) {
    return ARB_NAME_WRAPPER(glMapBuffer)(a, b);
}
GLAPI GLboolean APIENTRY glUnmapBuffer(GLenum a) {
    return ARB_NAME_WRAPPER(glUnmapBuffer)(a);
}

GLAPI void APIENTRY glBindBuffer(GLenum a, GLuint b) {
    ARB_NAME_WRAPPER(glBindBuffer)(a, b);
}
GLAPI void APIENTRY glGenBuffers(GLsizei a, GLuint *b) {
    ARB_NAME_WRAPPER(glGenBuffers)(a, b);
}
GLAPI void APIENTRY glBufferData(GLenum a, GLsizeiptr b, const GLvoid *c, GLenum d) {
    ARB_NAME_WRAPPER(glBufferData)(a, b, c, d);
}
}
#endif

namespace LRT BEGIN_NAMESPACE

#define GLERROR                                                                  \
    {                                                                            \
        GLenum err = glGetError();                                               \
        if (err != GL_NO_ERROR)                                                  \
            cout << "GL Error " << __FILE__ << " " << __LINE__ << endl << flush; \
    }

    PBOFrameBuffer *
    PBOFrameBuffer::create() {
#if !USE_PBOS
    cout << "PBO: Falling back to non-PBO framebuffer" << endl;
    cout << "PBO:   The application has requested a PBO frame buffer," << endl;
    cout << "PBO:   but this code was compiled with PBO support turned off," << endl;
    cout << "PBO:   so we will revert to a non-PBO frame buffer." << endl;
    cout << "PBO:   Note that this does not mean that your hardware is unable" << endl;
    cout << "PBO:   to support PBOs -- as it is explicitly disabled in the" << endl;
    cout << "PBO:   code, we didn't even try allocating them..." << endl;

    return NULL;
#endif
    try {
        PBOFrameBuffer *fb = new PBOFrameBuffer;
        cout << "successfully allocated PBO framebuffer" << endl;
        return fb;
    } catch (const char *error) {
        cout << "error in creating PBO framebuffer : " << error << endl;
        return NULL;
    }
} // namespace PBOFrameBuffer::create()

void PBOFrameBuffer::resize(int newX, int newY) {
    OpenGLTextureRGBA8FB::resize(newX, newY);

#if USE_PBOS

    cout << "(re-)allocating PBO (" << res << ")" << endl;
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, fbPBO);
    GLERROR;

    const int size = 4 * sizeof(GL_UNSIGNED_INT) * res.x * res.y;
    glBufferData(GL_PIXEL_UNPACK_BUFFER_EXT, size, NULL, GL_STREAM_DRAW);
    GLERROR;

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, 0);
    GLERROR;

#endif
}

PBOFrameBuffer::PBOFrameBuffer() : OpenGLTextureRGBA8FB() {
    cout << "checking for PBO support ... " << flush;
    if (strstr((char *)glGetString(GL_EXTENSIONS), "_pixel_buffer_object") == NULL) {
        cout << "FAILED!" << endl;
        cout << "could not find '_pixel_buffer_object' in gl capabilities string:" << endl;
        cout << glGetString(GL_EXTENSIONS) << endl;

        throw "pixel buffer objects not in GL extension string...";
    } else {
        cout << "OK." << endl;
    }

#if USE_PBOS
    glGenBuffers(1, &fbPBO);
    GLERROR;

    if (glGetError() != GL_NO_ERROR) {
        throw "error in glGenBuffers";
    }
#endif
}

void PBOFrameBuffer::doneWithFrame() {
#if USE_PBOS
    assert(fb);

    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_EXT);
    GLERROR;

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, fbPBO);
    GLERROR;

    glBindTexture(GL_TEXTURE_2D, fbTextureID);
    GLERROR;

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, res.x, res.y, FRAME_BUFFER_MODE, GL_UNSIGNED_BYTE, 0);
    GLERROR;
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, 0);
    GLERROR;
#endif
}

void PBOFrameBuffer::startNewFrame() {
#if USE_PBOS
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, fbPBO);
    GLERROR;
    fb = (unsigned char *)glMapBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, GL_WRITE_ONLY);
    GLERROR;
    assert(fb);
#endif
}

END_NAMESPACE
