
#include "GLTextureFB.hxx"
#include "../include/lrt.h"

#define DBG(a)

namespace LRT BEGIN_NAMESPACE

    void
    OpenGLTextureRGBA8FB::resize(int newX, int newY) {

    RGBAucharFrameBuffer::resize(newX, newY);

    if (fbTextureRes.x < res.x || fbTextureRes.y < res.y) {
        fbTextureRes = vec2i(16, 16);
        while (fbTextureRes.x < res.x) {
            fbTextureRes.x *= 2;
        }
        while (fbTextureRes.y < res.y) {
            fbTextureRes.y *= 2;
        }

        cout << "resizing to " << fbTextureRes << endl;
        glEnable(GL_TEXTURE_2D);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fbTextureRes.x, fbTextureRes.y, 0, FRAME_BUFFER_MODE, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }
    fbTexCoords.x = res.x / float(fbTextureRes.x);
    fbTexCoords.y = res.y / float(fbTextureRes.y);
    DBG(PING; cout << res << " " << fbTextureRes << " " << fbTexCoords << endl);
} // namespace OpenGLTextureRGBA8FB::resize(intnewX,intnewY)

void OpenGLTextureRGBA8FB::display() {
    DBG(PING; cout << res << endl);
    glBindTexture(GL_TEXTURE_2D, fbTextureID);
    glEnable(GL_TEXTURE_2D);

    glShadeModel(GL_FLAT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT, GL_FILL);

    glColor4f(1, 1, 1, 1);

    glMatrixMode(GL_PROJECTION);
    glViewport(0, 0, (GLsizei)res.x, (GLsizei)res.y);
    glLoadIdentity();

    glOrtho(0, 1, 0, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_QUADS);
    {
#if 0
    glTexCoord2f(0, 0);
    glVertex2f(0,0);

    glTexCoord2f(fbTexCoords.x, 0);
    glVertex2f(1,0);

    glTexCoord2f(fbTexCoords.x, fbTexCoords.y);
    glVertex2f(1,1);

    glTexCoord2f(0, fbTexCoords.y);
    glVertex2f(0,1);
#else
        glTexCoord2f(0, 0);
        glVertex2f(0, 1);

        glTexCoord2f(fbTexCoords.x, 0);
        glVertex2f(1, 1);

        glTexCoord2f(fbTexCoords.x, fbTexCoords.y);
        glVertex2f(1, 0);

        glTexCoord2f(0, fbTexCoords.y);
        glVertex2f(0, 0);
#endif
    }
    glEnd();
}

void BufferedOpenGLTextureRGBA8FB::resize(int newX, int newY) {
    OpenGLTextureRGBA8FB::resize(newX, newY);

    long size = nextMultipleOf<16 * 16 * 4>(res.x * res.y * 4);
    if (size >= allocedSize) {
        if (fb) {
            cout << "freeing" << endl;
            aligned_free(fb);
        }
        cout << "resizing fb buffer to " << res << endl;
        cout << "alloc'ing " << size << " (" << res.x * res.y * 4 << ")" << endl;
        fb = aligned_malloc<unsigned char>(size);

        allocedSize = size;
        assert(is_aligned<16>(fb));
    }
}

void BufferedOpenGLTextureRGBA8FB::doneWithFrame() {
    glBindTexture(GL_TEXTURE_2D, fbTextureID);
    glEnable(GL_TEXTURE_2D);

    DBG(PING);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, res.x, res.y, FRAME_BUFFER_MODE, GL_UNSIGNED_BYTE, (unsigned char *)fb);
}

END_NAMESPACE
