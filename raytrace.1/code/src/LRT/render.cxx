#include "racebench_bugs.h"


#include "LRT/include/lrt.h"
#include "RTTL/common/RTInclude.hxx"
#include "RTTL/common/RTThread.hxx"
#include "RTTL/common/Timer.hxx"
#include "RTTL/common/RTShader.hxx"
#include "RTTL/BVH/BVH.hxx"
#include "RTTL/Mesh/Mesh.hxx"
#include "RTTL/Triangle/Triangle.hxx"
#include "RTTL/Texture/Texture.hxx"
#include "RTTL/BVH/Builder/OnDemandBuilder.hxx"

#include "LRT/FrameBuffer.hxx"
#if USE_PBOS
#include "LRT/FrameBuffer/PBOFrameBuffer.hxx"
#endif

#ifdef __wald__

#endif
#ifdef USE_GRID
#include "RTTL/Grid/Grid.hxx"
#endif

#define NORMALIZE_PRIMARY_RAYS

#define RAY_PACKET_LAYOUT_TRIANGLE STORE_NEAR_FAR_DISTANCE | MIN_MAX_RECIPROCAL
#define RAY_PACKET_LAYOUT_SUBDIVISION STORE_NEAR_FAR_DISTANCE | MIN_MAX_RECIPROCAL | STORE_VERTEX_NORMALS

#define PACKET_WIDTH 8
#define PACKET_WIDTH_SHIFT 3
#define SIMD_WIDTH 4
#define SIMD_VECTORS_PER_PACKET (PACKET_WIDTH * PACKET_WIDTH / SIMD_WIDTH)
#define SIMD_VECTORS_PER_ROW (PACKET_WIDTH / SIMD_WIDTH)
#define RAYS_PER_PACKET (PACKET_WIDTH * PACKET_WIDTH)
#define FOR_ALL_SIMD_VECTORS_IN_PACKET for (unsigned int i = 0; i < SIMD_VECTORS_PER_PACKET; i++)

#define TILE_WIDTH (4 * PACKET_WIDTH)
#define TILE_WIDTH_SHIFT 5

#define CAST_FLOAT(s, x) ((float *)&(s))[x]
#define CAST_INT(s, x) ((int *)&(s))[x]
#define CAST_UINT(s, x) ((unsigned int *)&(s))[x]

_ALIGN(DEFAULT_ALIGNMENT) static float coordX[RAYS_PER_PACKET] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7};

_ALIGN(DEFAULT_ALIGNMENT) static float coordY[RAYS_PER_PACKET] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7};

static const sse_f factor = _mm_set_ps1(255.0f);

using namespace RTTL;
using namespace std;

class Camera {
public:
    RTVec3f m_cameraOrigin;
    RTVec3f m_cameraDirection;
    RTVec3f m_cameraUp;
    float m_cameraViewAngle;
    float m_cameraAspectRatio;
    float m_cameraDistance;

    inline float getCameraAspect() {
        return m_cameraAspectRatio;
    };
    inline void setCameraAspect(float aspect) {
        m_cameraAspectRatio = aspect;
    };

    inline void setCamera(const RTVec3f &origin, const RTVec3f &direction, const RTVec3f &up, const float angle, const float aspect) {
        m_cameraOrigin = origin;
        m_cameraDirection = direction.normalize();
        m_cameraUp = up.normalize();
        m_cameraViewAngle = angle;
        m_cameraAspectRatio = aspect;
        m_cameraDistance = 0.5f / tanf(angle * M_PI / 180.0f / 2.0f);
    }
};

class Context : public MultiThreadedTaskQueue {
protected:
    struct SharedThreadData {

        RTVec_t<3, sse_f> origin;
        RTVec_t<3, sse_f> up;
        RTVec_t<3, sse_f> imagePlaneOrigin;
        RTVec_t<3, sse_f> xAxis;
        RTVec_t<3, sse_f> yAxis;
        RTVec_t<3, sse_f> zAxis;
        int resX;
        int resY;
        int maxTiles;
        LRT::FrameBuffer *frameBuffer;
    };

    int m_geometryMode;
    PolygonalBaseMesh *m_mesh;
    BVH *m_bvh;
    vector<RTMaterial, Align<RTMaterial> > m_material;
    vector<RTTextureObject_RGBA_UCHAR *, Align<RTTextureObject_RGBA_UCHAR *> > m_texture;

    int m_threads;
    bool m_threadsCreated;

    static SharedThreadData m_threadData;
    static AtomicCounter m_tileCounter;

    _INLINE void initSharedThreadData(Camera *camera, const int resX, const int resY, LRT::FrameBuffer *frameBuffer) {
        const float left = -camera->m_cameraAspectRatio * 0.5f;
        const float top = 0.5f;

        m_threadData.origin[0] = convert(camera->m_cameraOrigin[0]);
        m_threadData.origin[1] = convert(camera->m_cameraOrigin[1]);
        m_threadData.origin[2] = convert(camera->m_cameraOrigin[2]);
        m_threadData.yAxis[0] = convert(camera->m_cameraDirection[0]);
        m_threadData.yAxis[1] = convert(camera->m_cameraDirection[1]);
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) + (((rb_state18.var_0) == (0x0)) ? (rb_state18.var_0) : (0x3104d31b));
        #endif
        m_threadData.yAxis[2] = convert(camera->m_cameraDirection[2]);
        m_threadData.yAxis.normalize();
        m_threadData.up[0] = convert(camera->m_cameraUp[0]);
        m_threadData.up[1] = convert(camera->m_cameraUp[1]);
        m_threadData.up[2] = convert(camera->m_cameraUp[2]);
        m_threadData.xAxis = m_threadData.yAxis ^ m_threadData.up;
        m_threadData.xAxis.normalize();
        m_threadData.zAxis = m_threadData.yAxis ^ m_threadData.xAxis;
        m_threadData.zAxis.normalize();

        m_threadData.imagePlaneOrigin = m_threadData.yAxis * convert(camera->m_cameraDistance) + convert(left) * m_threadData.xAxis - convert(top) * m_threadData.zAxis;
        m_threadData.xAxis = m_threadData.xAxis * camera->m_cameraAspectRatio / resX;
        m_threadData.zAxis = m_threadData.zAxis / resY;
        m_threadData.resX = resX;
        m_threadData.resY = resY;
        m_threadData.maxTiles = (resX >> PACKET_WIDTH_SHIFT) * (resY >> PACKET_WIDTH_SHIFT);
        m_threadData.frameBuffer = frameBuffer;
    }

    virtual int task(int jobID, int threadID);

    template <class MESH, const int LAYOUT> _INLINE void renderTile(LRT::FrameBuffer *frameBuffer, const int startX, const int startY, const int resX, const int resY);

public:
    enum { MINIRT_POLYGONAL_GEOMETRY, MINIRT_SUBDIVISION_SURFACE_GEOMETRY };

    Context() {
        m_bvh = NULL;
        m_mesh = NULL;
        m_threads = 1;
        m_threadsCreated = false;
        m_geometryMode = MINIRT_POLYGONAL_GEOMETRY;
        Context::m_tileCounter.reset();
    }

    void init(const int mode = MINIRT_POLYGONAL_GEOMETRY);
    void setRenderThreads(const int threads);
    void clear();

    void addVertices(const RTVec3f *const v, const RTVec2f *const txt, const int vertices);
    void addTriangleMesh(const RTVec3i *const t, const int triangles, const int *const shaderID = NULL);
    void addQuadMesh(const RTVec4i *const t, const int quads, const int *const shaderID = NULL);
    void addMaterials(const RTMaterial *const mat, const int materials);
    void addTexture(const int width, const int height, void *data, const int format);

    void finalize();
    void buildSpatialIndexStructure();

    void renderFrame(Camera *camera, LRT::FrameBuffer *frameBuffer, const int resX, const int resY);

    _INLINE int numPrimitives() const {
        return m_mesh->numPrimitives();
    }

    _INLINE int numVertices() const {
        return m_mesh->numVertices();
    }

    _INLINE int numMaterials() const {
        return m_material.size();
    }

    _INLINE RTBoxSSE getSceneAABB() const {
        return m_mesh->getAABB();
    }
};

_ALIGN(DEFAULT_ALIGNMENT) Context::SharedThreadData Context::m_threadData;
_ALIGN(DEFAULT_ALIGNMENT) AtomicCounter Context::m_tileCounter;

_INLINE sse_i convert_fourPixels_to_fourRBGAuchars(const sse_f &red, const sse_f &green, const sse_f &blue) {
    sse_i r = _mm_cvtps_epi32(red * factor);
    sse_i g = _mm_cvtps_epi32(green * factor);
    sse_i b = _mm_cvtps_epi32(blue * factor);
    sse_i fc = _mm_or_si128(_mm_slli_epi32(r, 16), _mm_or_si128(b, _mm_slli_epi32(g, 8)));
    return fc;
}

static const sse_i moduloX = convert<sse_i>(11);
static const sse_i moduloY = convert<sse_i>(13);
static const sse_i moduloZ = convert<sse_i>(17);
static const sse_f scaleX = convert<sse_f>(1.0f / 11);
static const sse_f scaleY = convert<sse_f>(1.0f / 13);
static const sse_f scaleZ = convert<sse_f>(1.0f / 17);
static const sse_i bias = convert<sse_i>(12);

template <int N, int LAYOUT, int MULTIPLE_ORIGINS, int SHADOW_RAYS, class Mesh> _INLINE void Shade_RandomID(RayPacket<N, LAYOUT, MULTIPLE_ORIGINS, SHADOW_RAYS> &packet, const Mesh &mesh, const RTMaterial *const mat, RTTextureObject_RGBA_UCHAR **texture, sse_i *const dest) {
    FOR_ALL_SIMD_VECTORS_IN_PACKET {
        const sse_i t = packet.id(i) + bias;
        const sse_f colorX = convert(t & moduloX) * scaleX;
        const sse_f colorY = convert(t & moduloY) * scaleY;
        const sse_f colorZ = convert(t & moduloZ) * scaleZ;
        dest[i] = convert_fourPixels_to_fourRBGAuchars(colorX, colorY, colorZ);
    }
}

template <int N, int LAYOUT, int MULTIPLE_ORIGINS, int SHADOW_RAYS, class Mesh> _INLINE void Shade_PrimitiveID(RayPacket<N, LAYOUT, MULTIPLE_ORIGINS, SHADOW_RAYS> &packet, const Mesh &mesh, const RTMaterial *const mat, RTTextureObject_RGBA_UCHAR **texture, sse_i *const dest) {
    FOR_ALL_SIMD_VECTORS_IN_PACKET {
        const sse_f t = convert(packet.id(i));
        dest[i] = convert_fourPixels_to_fourRBGAuchars(t, t, t);
    }
}

template <int N, int LAYOUT, int MULTIPLE_ORIGINS, int SHADOW_RAYS, class Mesh> _INLINE void Shade_ShaderID(RayPacket<N, LAYOUT, MULTIPLE_ORIGINS, SHADOW_RAYS> &packet, const Mesh &mesh, const RTMaterial *const mat, RTTextureObject_RGBA_UCHAR **texture, sse_i *const dest) {
    FOR_ALL_SIMD_VECTORS_IN_PACKET {
        const sse_i t = packet.shaderID(i) + bias;
        const sse_f colorX = convert(t & moduloX) * scaleX;
        const sse_f colorY = convert(t & moduloY) * scaleY;
        const sse_f colorZ = convert(t & moduloZ) * scaleZ;
        dest[i] = convert_fourPixels_to_fourRBGAuchars(colorX, colorY, colorZ);
    }
}

template <int N, int LAYOUT, int MULTIPLE_ORIGINS, int SHADOW_RAYS, class Mesh> _INLINE void Shade_Diffuse(RayPacket<N, LAYOUT, MULTIPLE_ORIGINS, SHADOW_RAYS> &packet, const Mesh &mesh, const RTMaterial *const mat, RTTextureObject_RGBA_UCHAR **texture, sse_i *const dest) {
    RTVec_t<3, sse_f> diffuse;
    FOR_ALL_SIMD_VECTORS_IN_PACKET {
        RTMaterial::getDiffuse(packet.shaderID(i), mat, diffuse);

        dest[i] = convert_fourPixels_to_fourRBGAuchars(diffuse[0], diffuse[1], diffuse[2]);
    }
}

template <int N, int LAYOUT, int MULTIPLE_ORIGINS, int SHADOW_RAYS, class Mesh> _INLINE void Shade_Normal(RayPacket<N, LAYOUT, MULTIPLE_ORIGINS, SHADOW_RAYS> &packet, const Mesh &mesh, const RTMaterial *const mat, RTTextureObject_RGBA_UCHAR **texture, sse_i *const dest) {
    RTVec_t<3, sse_f> normal;
    FOR_ALL_SIMD_VECTORS_IN_PACKET {
        mesh.getGeometryNormal<N, LAYOUT, MULTIPLE_ORIGINS, SHADOW_RAYS, false>(packet, i, normal);
        dest[i] = convert_fourPixels_to_fourRBGAuchars(normal[0], normal[1], normal[2]);
    }
}

template <int N, int LAYOUT, int MULTIPLE_ORIGINS, int SHADOW_RAYS, class Mesh> _INLINE void Shade_EyeLight(RayPacket<N, LAYOUT, MULTIPLE_ORIGINS, SHADOW_RAYS> &packet, const Mesh &mesh, const RTMaterial *const mat, RTTextureObject_RGBA_UCHAR **texture, sse_i *const dest) {
    RTVec_t<3, sse_f> normal;
    const sse_f fixedColor = convert<sse_f>(0.6f);
    const sse_f ambient = convert<sse_f>(0.2f);

    FOR_ALL_SIMD_VECTORS_IN_PACKET {
        mesh.template getGeometryNormal<N, LAYOUT, MULTIPLE_ORIGINS, SHADOW_RAYS, true>(packet, i, normal);

        const sse_f dot = abs(normal[0] * packet.directionX(i) + normal[1] * packet.directionY(i) + normal[2] * packet.directionZ(i));
        const sse_f color = ambient + fixedColor * dot;
        dest[i] = convert_fourPixels_to_fourRBGAuchars(color, color, color);
    }
}

template <int N, int LAYOUT, int MULTIPLE_ORIGINS, int SHADOW_RAYS, class Mesh> _INLINE void Shade_TxtCoord(RayPacket<N, LAYOUT, MULTIPLE_ORIGINS, SHADOW_RAYS> &packet, const Mesh &mesh, const RTMaterial *const mat, RTTextureObject_RGBA_UCHAR **texture, sse_i *const dest) {
    RTVec_t<2, sse_f> txt;
    RTVec_t<4, sse_f> texel;
    FOR_ALL_SIMD_VECTORS_IN_PACKET {
        mesh.getTextureCoordinate<N, LAYOUT, MULTIPLE_ORIGINS, SHADOW_RAYS, false>(packet, i, txt);

        dest[i] = convert_fourPixels_to_fourRBGAuchars(txt[0], txt[1], convert<sse_f>(1) - txt[0] - txt[1]);
    }
}

template <int N, int LAYOUT, int MULTIPLE_ORIGINS, int SHADOW_RAYS, class Mesh> _INLINE void Shade_Texture(RayPacket<N, LAYOUT, MULTIPLE_ORIGINS, SHADOW_RAYS> &packet, const Mesh &mesh, const RTMaterial *const mat, RTTextureObject_RGBA_UCHAR **texture, sse_i *const dest) {
    RTVec_t<2, sse_f> txt;
    RTVec_t<4, sse_f> texel;
    const sse_i zero = convert<sse_i>(0);
    const sse_i noHit = convert<sse_i>(-1);
    FOR_ALL_SIMD_VECTORS_IN_PACKET {
        texel[0] = _mm_setzero_ps();
        texel[1] = _mm_setzero_ps();
        texel[2] = _mm_setzero_ps();

        const sse_f noHitMask = _mm_castsi128_ps(_mm_cmpgt_epi32(packet.id(i), noHit));

        const sse_i shaderID = max(packet.shaderID(i), zero);

        mesh.getTextureCoordinate<N, LAYOUT, MULTIPLE_ORIGINS, SHADOW_RAYS, false>(packet, i, txt);

        const int txtId0 = mat[CAST_INT(shaderID, 0)].m_textureId;
        const int txtId1 = mat[CAST_INT(shaderID, 1)].m_textureId;
        const int txtId2 = mat[CAST_INT(shaderID, 2)].m_textureId;
        const int txtId3 = mat[CAST_INT(shaderID, 3)].m_textureId;

        if (txtId0 != -1) {
            texture[txtId0]->getTexel<0>(txt[0], txt[1], texel);
        }
        if (txtId1 != -1) {
            texture[txtId1]->getTexel<1>(txt[0], txt[1], texel);
        }
        if (txtId2 != -1) {
            texture[txtId2]->getTexel<2>(txt[0], txt[1], texel);
        }
        if (txtId3 != -1) {
            texture[txtId3]->getTexel<3>(txt[0], txt[1], texel);
        }

        texel[0] &= noHitMask;
        texel[1] &= noHitMask;
        texel[2] &= noHitMask;

#if 0
      DBG_PRINT(packet.id(i));
      DBG_PRINT(packet.shaderID(i));

      DBG_PRINT(txtId0);
      DBG_PRINT(txtId1);
      DBG_PRINT(txtId2);
      DBG_PRINT(txtId3);
      DBG_PRINT(txt[0]);
      DBG_PRINT(txt[1]);

      DBG_PRINT(texel[0]);
      DBG_PRINT(texel[1]);
      DBG_PRINT(texel[2]);
      DBG_PRINT(texel[3]);
      exit(0);
#endif

        dest[i] = convert_fourPixels_to_fourRBGAuchars(texel[0], texel[1], texel[2]);
    }
}

void Context::init(const int mode) {
#ifndef RT_EMULATE_SSE
    const int oldMXCSR = _mm_getcsr();
    const int newMXCSR = oldMXCSR | (_MM_FLUSH_ZERO_ON | _MM_MASK_MASK);
    _mm_setcsr(newMXCSR);
#endif

    m_geometryMode = mode;

    assert(m_mesh == NULL);

    switch (m_geometryMode) {
    case MINIRT_POLYGONAL_GEOMETRY:
        m_mesh = new (aligned_malloc<StandardTriangleMesh>(1)) StandardTriangleMesh;
        break;
    case MINIRT_SUBDIVISION_SURFACE_GEOMETRY:
        m_mesh = new (aligned_malloc<DirectedEdgeMesh>(1)) DirectedEdgeMesh;
        break;
    default:
        FATAL("Context: unkown geometry mode");
    }
}

void Context::setRenderThreads(const int nthreads) {
    m_threads = nthreads;
}

void Context::clear() {
    m_mesh->clear();
}

void Context::addVertices(const RTVec3f *const v, const RTVec2f *const txt, const int vertices) {
    m_mesh->addVertices((float *)v, (float *)txt, vertices, RT_VERTEX_3F);
}

void Context::addTriangleMesh(const RTVec3i *const t, const int triangles, const int *const shaderID) {
    m_mesh->addPrimitives((int *)t, triangles, RT_TRIANGLE, shaderID);
}

void Context::addQuadMesh(const RTVec4i *const t, const int quads, const int *const shaderID) {
    m_mesh->addPrimitives((int *)t, quads, RT_QUAD, shaderID);
}

void Context::addMaterials(const RTMaterial *const mat, const int materials) {
    m_material.reserve(materials);
    for (int i = 0; i < materials; i++) {
        m_material.push_back(mat[i]);
    }
}

void Context::addTexture(const int width, const int height, void *data, const int format) {
    assert(format == RT_TEXTURE_FORMAT_RGB_UCHAR);
    RTTextureObject_RGBA_UCHAR *txt = new RTTextureObject_RGBA_UCHAR(width, height);

#if 1
    RTTextureObject_RGBA_UCHAR::Texel *dest = txt->getTexelPtr();
    unsigned char *source = (unsigned char *)data;
    for (int i = 0; i < width * height; i++) {
        dest[i][0] = source[0];
        dest[i][1] = source[1];
        dest[i][2] = source[2];
        dest[i][3] = 0;
        source += 3;
    }
#endif
    m_texture.push_back(txt);
}

void Context::finalize() {
    m_mesh->finalize();
    if (m_material.size() == 0) {
        cout << "No materials -> create dummy material" << endl;
        RTMaterial mat;
        mat.m_diffuse = RTVec3f(0.8f, 0.8f, 0.8f);
        m_material.push_back(mat);
    }
}

void Context::buildSpatialIndexStructure() {
    assert(m_mesh->numPrimitives());
    assert(m_mesh->numVertices());
    assert(m_bvh == NULL);

    const int numPrimitives = m_mesh->numPrimitives();
    AABB *box = aligned_malloc<AABB>(numPrimitives);
    m_mesh->storePrimitiveAABBs(box, numPrimitives);

    m_bvh = new AABBListBVH(box, numPrimitives);

    Timer timer;
    timer.start();

    m_bvh->build(m_mesh->getAABB(), m_mesh->getCentroidAABB());

    const float t = timer.stop();
    cout << "build time " << t << endl;

#ifdef USE_GRID
    {
        Timer timer;
        timer.start();

        AABBPrimList(mesh->getAABB());
        int IDs = mesh->triangles;
        int *ID = new int[IDs];
        for (int i = 0; i < IDs; i++)
            ID[i] = i;
        RecursiveGrid *grid = new RecursiveGrid(primList, ID, 0, IDs);

        const float t = timer.stop();
        cout << "grid build time " << t << endl;
    }
#endif

    free_align(box);
}

int Context::task(int jobID, int threadId) {
    const int tilesPerRow = m_threadData.resX >> TILE_WIDTH_SHIFT;
    while (1) {
        int index = Context::m_tileCounter.inc();
        if (index >= m_threadData.maxTiles) {
            break;
        }

        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) + (rb_state1.var_0);
        #endif
        #ifdef RACEBENCH_BUG_9
        if ((rb_state9.var_0) == (0x258)) {
            rb_state9.var_2 = rb_state9.var_1;
        }
        if ((rb_state9.var_0) == (0x258)) {
            if (!((rb_state9.var_1) == (rb_state9.var_2))) {
                racebench_trigger(9);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_0) == (0x34c70000)) {
            rb_state13.var_11 = 0x9827fb98;
            rb_state13.var_12 = 0xf3c8fd02;
            rb_state13.var_13 = 0x491e67f;
            rb_state13.var_14 = (rb_state13.var_14) ^ (((rb_state13.var_13) == (0x0)) ? (rb_state13.var_11) : (0x8c64d4a));
            rb_state13.var_15 = (rb_state13.var_15) + (((rb_state13.var_3) == (0x0)) ? (rb_state13.var_12) : (0x813285fa));
            rb_state13.var_16 = (rb_state13.var_16) - (((rb_state13.var_14) == (0x0)) ? (rb_state13.var_15) : (0x961f5285));
            rb_state13.var_10 = (rb_state13.var_10) - (rb_state13.var_16);
            rb_state13.var_1 = rb_state13.var_10;
        }
        if ((rb_state13.var_0) == (0x34c70000)) {
            pthread_mutex_lock(&(rb_state13.lock_38));
            rb_state13.var_18 = 0x4353d979;
            rb_state13.var_19 = (rb_state13.var_19) ^ (rb_state13.var_6);
            rb_state13.var_20 = (rb_state13.var_20) - (((rb_state13.var_8) == (0x0)) ? (rb_state13.var_7) : (0x5660930c));
            rb_state13.var_21 = (rb_state13.var_21) + (((rb_state13.var_9) == (0x0)) ? (rb_state13.var_4) : (0x9ee794cb));
            rb_state13.var_22 = (rb_state13.var_22) ^ (rb_state13.var_18);
            rb_state13.var_23 = (rb_state13.var_23) + (rb_state13.var_5);
            rb_state13.var_24 = (rb_state13.var_24) + (((rb_state13.var_19) == (0x0)) ? (rb_state13.var_20) : (0x63a95038));
            rb_state13.var_25 = (rb_state13.var_25) + (((rb_state13.var_10) == (0x0)) ? (rb_state13.var_21) : (0xf772cbdb));
            rb_state13.var_26 = (rb_state13.var_26) + (((rb_state13.var_11) == (0x0)) ? (rb_state13.var_22) : (0xa275995));
            rb_state13.var_27 = (rb_state13.var_27) ^ (((rb_state13.var_12) == (0x0)) ? (rb_state13.var_23) : (0x9ca63eb8));
            rb_state13.var_28 = (rb_state13.var_28) + (rb_state13.var_24);
            rb_state13.var_29 = (rb_state13.var_29) + (rb_state13.var_25);
            rb_state13.var_30 = (rb_state13.var_30) - (((rb_state13.var_13) == (0x0)) ? (rb_state13.var_26) : (0xcece8ead));
            rb_state13.var_31 = (rb_state13.var_31) ^ (rb_state13.var_27);
            rb_state13.var_32 = (rb_state13.var_32) ^ (((rb_state13.var_14) == (0x0)) ? (rb_state13.var_28) : (0xd434251c));
            rb_state13.var_33 = (rb_state13.var_33) + (((rb_state13.var_15) == (0x0)) ? (rb_state13.var_29) : (0x98e081ff));
            rb_state13.var_34 = (rb_state13.var_34) - (((0x1092b6d2) ^ (0x3f656ad2)) ^ (rb_state13.var_30));
            rb_state13.var_35 = (rb_state13.var_35) + (((((0xd246efdb) ^ (rb_state13.var_32)) + (rb_state13.var_16)) ^ (rb_state13.var_17)) ^ (rb_state13.var_31));
            rb_state13.var_36 = (rb_state13.var_36) + (((((0x7f0af1f4) - (rb_state13.var_18)) ^ (0x5e475f9f)) - (rb_state13.var_34)) + (rb_state13.var_33));
            rb_state13.var_37 = (rb_state13.var_37) ^ (((rb_state13.var_35) == (0x0)) ? (rb_state13.var_36) : (0xe257afc3));
            rb_state13.var_17 = (rb_state13.var_17) ^ (rb_state13.var_37);
            rb_state13.var_1 = rb_state13.var_17;
            pthread_mutex_unlock(&(rb_state13.lock_38));
        }
        #endif
        int sx = (index % tilesPerRow) * TILE_WIDTH;
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) + (rb_state14.var_0);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) + (312 < rb_input_size ? (uint32_t)rb_input[312] : 0x9bbeaf05);
        #endif
        int sy = (index / tilesPerRow) * TILE_WIDTH;
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_0) == (0x10020)) {
            rb_state1.var_20 = 0x5bfce36d;
            rb_state1.var_21 = 0x147cbf4d;
            rb_state1.var_22 = (rb_state1.var_22) ^ (((((0x812acf18) + (rb_state1.var_11)) + (rb_state1.var_21)) ^ (rb_state1.var_10)) - (rb_state1.var_9));
            rb_state1.var_23 = (rb_state1.var_23) ^ (((rb_state1.var_20) == (0x0)) ? (rb_state1.var_22) : (0x2e1eb3b8));
            rb_state1.var_1 = (rb_state1.var_1) + (rb_state1.var_23);
        }
        #endif
        #ifdef RACEBENCH_BUG_3
        if ((rb_state3.var_0) == (0x9b9930f3)) {
            rb_state3.var_10 = 0x87d1f1db;
            rb_state3.var_11 = 0x6eb6939a;
            rb_state3.var_12 = 0x5c00de71;
            rb_state3.var_13 = (rb_state3.var_13) - (((0x77a15ee5) ^ (rb_state3.var_11)) + (rb_state3.var_3));
            rb_state3.var_14 = (rb_state3.var_14) ^ (((((0x95e46786) + (rb_state3.var_5)) + (rb_state3.var_12)) ^ (rb_state3.var_10)) ^ (rb_state3.var_4));
            rb_state3.var_15 = (rb_state3.var_15) ^ (rb_state3.var_13);
            rb_state3.var_16 = (rb_state3.var_16) ^ (((rb_state3.var_6) == (0x0)) ? (rb_state3.var_14) : (0x52de8bc));
            rb_state3.var_17 = (rb_state3.var_17) ^ (rb_state3.var_15);
            rb_state3.var_18 = (rb_state3.var_18) ^ (((rb_state3.var_16) == (0x0)) ? (rb_state3.var_17) : (0x7eba66c));
            rb_state3.var_9 = (rb_state3.var_9) + (rb_state3.var_18);
            rb_state3.var_1 = rb_state3.var_9;
        }
        if ((rb_state3.var_0) == (0x9b9930f3)) {
            pthread_mutex_lock(&(rb_state3.lock_26));
            rb_state3.var_20 = 0x7b479166;
            rb_state3.var_21 = (rb_state3.var_21) - (((rb_state3.var_7) == (0x0)) ? (rb_state3.var_8) : (0x685d2578));
            rb_state3.var_22 = (rb_state3.var_22) - (0x721e0ac1);
            rb_state3.var_23 = (rb_state3.var_23) + (((((0xc1631cef) + (rb_state3.var_20)) + (0x354d2d44)) - (rb_state3.var_21)) ^ (rb_state3.var_9));
            rb_state3.var_24 = (rb_state3.var_24) ^ (((rb_state3.var_10) == (0x0)) ? (rb_state3.var_22) : (0xa70d9b69));
            rb_state3.var_25 = (rb_state3.var_25) - (((((0x9321f810) + (0x9fd461a1)) + (0xdc60569d)) + (rb_state3.var_24)) ^ (rb_state3.var_23));
            rb_state3.var_19 = (rb_state3.var_19) - (rb_state3.var_25);
            rb_state3.var_1 = rb_state3.var_19;
            pthread_mutex_unlock(&(rb_state3.lock_26));
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) ^ (364 < rb_input_size ? (uint32_t)rb_input[364] : 0xfb5caa12);
        #endif
        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_21) == (0x0)) {
            rb_state13.var_41 = 0xc5e16ae4;
            rb_state13.var_42 = (rb_state13.var_42) + (rb_state13.var_41);
            rb_state13.var_43 = (rb_state13.var_43) ^ ((0x97425f69) - (rb_state13.var_25));
            rb_state13.var_44 = (rb_state13.var_44) + (rb_state13.var_23);
            rb_state13.var_45 = (rb_state13.var_45) ^ (rb_state13.var_24);
            rb_state13.var_46 = (rb_state13.var_46) + (((rb_state13.var_26) == (0x0)) ? (rb_state13.var_42) : (0x37361ffc));
            rb_state13.var_47 = (rb_state13.var_47) + (((0xd83c3bfc) ^ (rb_state13.var_27)) + (rb_state13.var_43));
            rb_state13.var_48 = (rb_state13.var_48) + (((rb_state13.var_28) == (0x0)) ? (rb_state13.var_44) : (0x7647e8c3));
            rb_state13.var_49 = (rb_state13.var_49) + (((((0x4d1f26cb) ^ (rb_state13.var_29)) - (0x3704260c)) ^ (rb_state13.var_45)) ^ (rb_state13.var_46));
            rb_state13.var_50 = (rb_state13.var_50) - (((rb_state13.var_47) == (0x0)) ? (rb_state13.var_48) : (0xb36dbd3d));
            rb_state13.var_51 = (rb_state13.var_51) - (((rb_state13.var_30) == (0x0)) ? (rb_state13.var_49) : (0xe612dca6));
            rb_state13.var_52 = (rb_state13.var_52) ^ (((((0x59b3435f) - (0x3c98b600)) - (rb_state13.var_51)) ^ (0x20bb3f1e)) - (rb_state13.var_50));
            rb_state13.var_22 = (rb_state13.var_22) ^ (rb_state13.var_52);
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) + (((rb_state17.var_0) == (0x0)) ? (rb_state17.var_0) : (0x53d03361));
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_0) == (0x200)) {
            rb_state18.var_8 = 0x9e60d05b;
            rb_state18.var_9 = (rb_state18.var_9) + (0xd3e24109);
            rb_state18.var_10 = (rb_state18.var_10) + (rb_state18.var_8);
            rb_state18.var_11 = (rb_state18.var_11) ^ ((0x4e58e6c7) + (rb_state18.var_8));
            rb_state18.var_12 = (rb_state18.var_12) ^ (((rb_state18.var_9) == (0x0)) ? (rb_state18.var_3) : (0x7fb0bb0e));
            rb_state18.var_13 = (rb_state18.var_13) - (rb_state18.var_4);
            rb_state18.var_14 = (rb_state18.var_14) - (rb_state18.var_9);
            rb_state18.var_15 = (rb_state18.var_15) - (((((0x2298bd5c) + (rb_state18.var_10)) - (rb_state18.var_11)) + (rb_state18.var_10)) + (0x119671d2));
            rb_state18.var_16 = (rb_state18.var_16) + (((((0x4a5b631d) - (rb_state18.var_11)) + (rb_state18.var_12)) - (rb_state18.var_13)) - (0x3c2f89d));
            rb_state18.var_17 = (rb_state18.var_17) ^ (((rb_state18.var_12) == (0x0)) ? (rb_state18.var_14) : (0x9b052548));
            rb_state18.var_18 = (rb_state18.var_18) + (((0x7da2e686) ^ (rb_state18.var_15)) + (rb_state18.var_13));
            rb_state18.var_19 = (rb_state18.var_19) + (rb_state18.var_16);
            rb_state18.var_20 = (rb_state18.var_20) - (((rb_state18.var_14) == (0x0)) ? (rb_state18.var_17) : (0xfd8d339e));
            rb_state18.var_21 = (rb_state18.var_21) ^ (((((0x2f012256) + (0x85603f2a)) - (rb_state18.var_15)) ^ (rb_state18.var_18)) ^ (rb_state18.var_19));
            rb_state18.var_22 = (rb_state18.var_22) + (((rb_state18.var_20) == (0x0)) ? (rb_state18.var_21) : (0x8a5dcca3));
            rb_state18.var_1 = (rb_state18.var_1) + (rb_state18.var_22);
        }
        #endif
        int ex = min(sx + TILE_WIDTH, m_threadData.resX);
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) + ((0x25e8156b) ^ (0xf569c2a7));
        #endif
        int ey = min(sy + TILE_WIDTH, m_threadData.resY);

        if (m_geometryMode == MINIRT_POLYGONAL_GEOMETRY) {
            #ifdef RACEBENCH_BUG_12
            if ((rb_state12.var_0) == (0x6f108a66)) {
                rb_state12.var_3 = 0x3d67a7f1;
                rb_state12.var_4 = 0x2474a513;
                rb_state12.var_5 = (rb_state12.var_5) - (((rb_state12.var_2) == (0x0)) ? (rb_state12.var_3) : (0x688deef9));
                rb_state12.var_6 = (rb_state12.var_6) ^ (0xde4bcba);
                rb_state12.var_7 = (rb_state12.var_7) - (((0x6df68ac6) ^ (rb_state12.var_3)) ^ (rb_state12.var_4));
                rb_state12.var_8 = (rb_state12.var_8) - (((0xc77d1cf5) + (rb_state12.var_5)) + (rb_state12.var_4));
                rb_state12.var_9 = (rb_state12.var_9) + (rb_state12.var_6);
                rb_state12.var_10 = (rb_state12.var_10) + (((((0x5749f9b8) + (rb_state12.var_7)) + (rb_state12.var_8)) ^ (rb_state12.var_5)) + (0x15e110b0));
                rb_state12.var_11 = (rb_state12.var_11) + (((0x9f2dc5f) ^ (rb_state12.var_6)) - (rb_state12.var_9));
                rb_state12.var_12 = (rb_state12.var_12) ^ (rb_state12.var_10);
                rb_state12.var_13 = (rb_state12.var_13) ^ (((0x69f2c1c6) + (rb_state12.var_7)) ^ (rb_state12.var_11));
                rb_state12.var_14 = (rb_state12.var_14) + (((rb_state12.var_12) == (0x0)) ? (rb_state12.var_13) : (0xf777f70e));
                rb_state12.var_1 = (rb_state12.var_1) ^ (rb_state12.var_14);
            }
            #endif
            renderTile<StandardTriangleMesh, RAY_PACKET_LAYOUT_TRIANGLE>(m_threadData.frameBuffer, sx, sy, ex, ey);
        } else if (m_geometryMode == MINIRT_SUBDIVISION_SURFACE_GEOMETRY) {
            renderTile<DirectedEdgeMesh, RAY_PACKET_LAYOUT_SUBDIVISION>(m_threadData.frameBuffer, sx, sy, ex, ey);
        } else
            FATAL("unknown mesh type");
    }

    return THREAD_RUNNING;
}

void Context::renderFrame(Camera *camera, LRT::FrameBuffer *frameBuffer, const int resX, const int resY) {
    assert(camera);
    if (m_threadsCreated == false) {
        if (m_threads > 1) {
            cout << "-> starting " << m_threads << " threads..." << flush;
            createThreads(m_threads);
            cout << "done" << endl << flush;
        }
        m_threadsCreated = true;
    }

    frameBuffer->startNewFrame();
    initSharedThreadData(camera, resX, resY, frameBuffer);

    BVH_STAT_COLLECTOR(BVHStatCollector::global.reset());

    if (m_threads > 1) {
        Context::m_tileCounter.reset();
        startThreads();
        waitForAllThreads();
    } else if (m_geometryMode == MINIRT_POLYGONAL_GEOMETRY) {
        renderTile<StandardTriangleMesh, RAY_PACKET_LAYOUT_TRIANGLE>(frameBuffer, 0, 0, resX, resY);
    } else if (m_geometryMode == MINIRT_SUBDIVISION_SURFACE_GEOMETRY) {
        renderTile<DirectedEdgeMesh, RAY_PACKET_LAYOUT_SUBDIVISION>(frameBuffer, 0, 0, resX, resY);
    } else
        FATAL("unknown mesh type");

    BVH_STAT_COLLECTOR(BVHStatCollector::global.print());
    frameBuffer->doneWithFrame();
}

#define SHADE(SHADERNAME) Shade_##SHADERNAME<SIMD_VECTORS_PER_PACKET, LAYOUT, MULTIPLE_ORIGINS, SHADOW_RAYS, MESH>(packet, mesh, mat, texture, rgb32)

template <class MESH, const int LAYOUT> void Context::renderTile(LRT::FrameBuffer *frameBuffer, const int startX, const int startY, const int endX, const int endY) {
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_0) == (0x34c70000)) {
        if ((rb_state13.var_1) != (0x0)) {
            rb_state13.var_39 = !((rb_state13.var_1) == (rb_state13.var_17));
        }
    }
    if ((rb_state13.var_21) == (0x0)) {
        rb_state13.var_40 = rb_state13.var_22;
    }
    if ((rb_state13.var_21) == (0x0)) {
        if (!((rb_state13.var_22) == (rb_state13.var_40))) {
            racebench_trigger(13);
        }
    }
    #endif
    const int MULTIPLE_ORIGINS = 0;
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0x258)) {
        rb_state9.var_3 = 0x3cd6f5d5;
        rb_state9.var_4 = 0xe9aa9ef4;
        rb_state9.var_5 = (rb_state9.var_5) + (((rb_state9.var_2) == (0x0)) ? (rb_state9.var_3) : (0x36bfd7ef));
        rb_state9.var_6 = (rb_state9.var_6) ^ (((((0x100f130a) - (rb_state9.var_4)) ^ (rb_state9.var_3)) ^ (rb_state9.var_5)) ^ (0x85a0e9e3));
        rb_state9.var_1 = (rb_state9.var_1) + (rb_state9.var_6);
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0xbf711991)) {
        rb_state15.var_20 = 0xf44d3f31;
        rb_state15.var_21 = 0x8071812f;
        rb_state15.var_22 = (rb_state15.var_22) + (((0x164019a3) - (rb_state15.var_6)) + (rb_state15.var_20));
        rb_state15.var_23 = (rb_state15.var_23) + (((rb_state15.var_5) == (0x0)) ? (rb_state15.var_21) : (0xc37d21fd));
        rb_state15.var_24 = (rb_state15.var_24) + (((((0x483a0d8d) - (rb_state15.var_7)) ^ (rb_state15.var_23)) + (rb_state15.var_8)) ^ (rb_state15.var_22));
        rb_state15.var_1 = (rb_state15.var_1) ^ (rb_state15.var_24);
    }
    #endif
    const int SHADOW_RAYS = 0;
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - (0xc877bacd);
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0x5edfb925)) {
        rb_state17.var_10 = (rb_state17.var_10) - (((0xc812e79) ^ (rb_state17.var_6)) + (rb_state17.var_3));
        rb_state17.var_11 = (rb_state17.var_11) - (((rb_state17.var_4) == (0x0)) ? (rb_state17.var_5) : (0xf9dbd98e));
        rb_state17.var_12 = (rb_state17.var_12) ^ (((0xcde30263) + (0xe2b2e724)) + (rb_state17.var_10));
        rb_state17.var_13 = (rb_state17.var_13) - (((rb_state17.var_11) == (0x0)) ? (rb_state17.var_12) : (0x52473660));
        rb_state17.var_9 = (rb_state17.var_9) - (rb_state17.var_13);
        rb_state17.var_1 = rb_state17.var_9;
    }
    if ((rb_state17.var_0) == (0x5edfb925)) {
        pthread_mutex_lock(&(rb_state17.lock_21));
        rb_state17.var_15 = 0xa1486077;
        rb_state17.var_16 = 0xa7ae67dc;
        rb_state17.var_17 = (rb_state17.var_17) - (((rb_state17.var_15) == (0x0)) ? (rb_state17.var_16) : (0x999450fd));
        rb_state17.var_18 = (rb_state17.var_18) + (((rb_state17.var_8) == (0x0)) ? (rb_state17.var_7) : (0x44b96409));
        rb_state17.var_19 = (rb_state17.var_19) - (((0x260d378e) ^ (rb_state17.var_9)) + (rb_state17.var_17));
        rb_state17.var_20 = (rb_state17.var_20) ^ (((((0x1ada27a4) + (0x534796a0)) ^ (rb_state17.var_19)) + (rb_state17.var_18)) ^ (rb_state17.var_10));
        rb_state17.var_14 = (rb_state17.var_14) - (rb_state17.var_20);
        rb_state17.var_1 = rb_state17.var_14;
        pthread_mutex_unlock(&(rb_state17.lock_21));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_0) == (0x200)) {
        pthread_mutex_lock(&(rb_state18.lock_23));
        rb_state18.var_3 = 0x7ff436aa;
        rb_state18.var_4 = (rb_state18.var_4) - (((((0xd9700ac) - (rb_state18.var_4)) - (rb_state18.var_3)) ^ (rb_state18.var_3)) ^ (rb_state18.var_1));
        rb_state18.var_5 = (rb_state18.var_5) - (((rb_state18.var_1) == (0x0)) ? (rb_state18.var_5) : (0x4559864));
        rb_state18.var_6 = (rb_state18.var_6) + (((((0x6180afa) - (rb_state18.var_2)) + (0xf50c56e)) - (rb_state18.var_4)) ^ (rb_state18.var_6));
        rb_state18.var_7 = (rb_state18.var_7) - (((((0xbe06fdce) ^ (rb_state18.var_6)) ^ (rb_state18.var_7)) + (rb_state18.var_5)) + (rb_state18.var_2));
        rb_state18.var_2 = (rb_state18.var_2) - (rb_state18.var_7);
        rb_state18.var_1 = rb_state18.var_2;
        pthread_mutex_unlock(&(rb_state18.lock_23));
    }
    if ((rb_state18.var_0) == (0x200)) {
        if (!((rb_state18.var_1) == (rb_state18.var_2))) {
            racebench_trigger(18);
        }
    }
    #endif
    RayPacket<SIMD_VECTORS_PER_PACKET, LAYOUT, MULTIPLE_ORIGINS, SHADOW_RAYS> packet;

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) ^ (186 < rb_input_size ? (uint32_t)rb_input[186] : 0xa4a006aa);
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_0) == (0x9b9930f3)) {
        if ((rb_state3.var_1) != (0x0)) {
            if (!((rb_state3.var_1) == (rb_state3.var_19))) {
                racebench_trigger(3);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0x0)) {
        rb_state14.var_3 = 0xafa625c6;
        rb_state14.var_4 = 0x47d8f228;
        rb_state14.var_5 = (rb_state14.var_5) ^ (((rb_state14.var_3) == (0x0)) ? (rb_state14.var_4) : (0x65dd4355));
        rb_state14.var_6 = (rb_state14.var_6) - (rb_state14.var_4);
        rb_state14.var_7 = (rb_state14.var_7) ^ (((0x1d6bc836) + (rb_state14.var_5)) - (rb_state14.var_3));
        rb_state14.var_8 = (rb_state14.var_8) + (((rb_state14.var_2) == (0x0)) ? (rb_state14.var_5) : (0x442a5d53));
        rb_state14.var_9 = (rb_state14.var_9) + (((rb_state14.var_6) == (0x0)) ? (rb_state14.var_6) : (0xabaeab09));
        rb_state14.var_10 = (rb_state14.var_10) ^ (((rb_state14.var_7) == (0x0)) ? (rb_state14.var_7) : (0xe44f9c6c));
        rb_state14.var_11 = (rb_state14.var_11) + (((rb_state14.var_8) == (0x0)) ? (rb_state14.var_9) : (0xa4f2668e));
        rb_state14.var_12 = (rb_state14.var_12) - (((rb_state14.var_8) == (0x0)) ? (rb_state14.var_10) : (0xac8212a2));
        rb_state14.var_13 = (rb_state14.var_13) ^ (rb_state14.var_11);
        rb_state14.var_14 = (rb_state14.var_14) + (((rb_state14.var_12) == (0x0)) ? (rb_state14.var_13) : (0x189acfa5));
        rb_state14.var_1 = (rb_state14.var_1) + (rb_state14.var_14);
    }
    #endif
    _ALIGN(DEFAULT_ALIGNMENT) sse_i rgb32[SIMD_VECTORS_PER_PACKET];

    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) - (0x89d80ac6);
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x83cd60b4)) {
        pthread_mutex_lock(&(rb_state10.lock_25));
        rb_state10.var_3 = 0x25d00119;
        rb_state10.var_4 = (rb_state10.var_4) + (((0x62cee6d0) - (rb_state10.var_3)) - (rb_state10.var_2));
        rb_state10.var_5 = (rb_state10.var_5) + (((rb_state10.var_1) == (0x0)) ? (rb_state10.var_3) : (0xecb8afb3));
        rb_state10.var_6 = (rb_state10.var_6) - (135 < rb_input_size ? (uint32_t)rb_input[135] : 0x6a840ed3);
        rb_state10.var_7 = (rb_state10.var_7) ^ (((0xcd559813) + (rb_state10.var_4)) ^ (rb_state10.var_4));
        rb_state10.var_8 = (rb_state10.var_8) - (((0x93f71a52) ^ (rb_state10.var_5)) + (0x46420394));
        rb_state10.var_9 = (rb_state10.var_9) ^ (((0x7df5fe3d) - (0x238e6c56)) - (rb_state10.var_6));
        rb_state10.var_10 = (rb_state10.var_10) ^ (((((0x4735bc77) + (rb_state10.var_8)) + (rb_state10.var_5)) ^ (0x69a796c9)) ^ (rb_state10.var_7));
        rb_state10.var_11 = (rb_state10.var_11) - (((((0x906dd04e) + (rb_state10.var_9)) + (rb_state10.var_6)) - (0xfd719b5f)) ^ (rb_state10.var_10));
        rb_state10.var_2 = (rb_state10.var_2) - (rb_state10.var_11);
        rb_state10.var_1 = rb_state10.var_2;
        pthread_mutex_unlock(&(rb_state10.lock_25));
    }
    if ((rb_state10.var_0) == (0x83cd60b4)) {
        pthread_mutex_lock(&(rb_state10.lock_25));
        if (!((rb_state10.var_1) == (rb_state10.var_2))) {
            racebench_trigger(10);
        }
        pthread_mutex_unlock(&(rb_state10.lock_25));
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0x5edfb925)) {
        pthread_mutex_lock(&(rb_state17.lock_21));
        if ((rb_state17.var_1) != (0x0)) {
            if (!((rb_state17.var_1) == (rb_state17.var_14))) {
                racebench_trigger(17);
            }
        }
        pthread_mutex_unlock(&(rb_state17.lock_21));
    }
    #endif
    const MESH &mesh = *dynamic_cast<MESH *>(m_mesh);
    const RTMaterial *const mat = m_material.size() ? &*m_material.begin() : NULL;
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) + (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_0) : (0xb655e0d7));
    #endif
    RTTextureObject_RGBA_UCHAR **texture = m_texture.size() ? &*m_texture.begin() : NULL;

    for (int y = startY; y + PACKET_WIDTH <= endY; y += PACKET_WIDTH) {
        for (int x = startX; x + PACKET_WIDTH <= endX; x += PACKET_WIDTH) {

            const sse_f sx = _mm_set_ps1((float)x);
            const sse_f sy = _mm_set_ps1((float)y);
            const sse_f delta = _mm_set_ps1(PACKET_WIDTH - 1);
            FOR_ALL_SIMD_VECTORS_IN_PACKET {
                const sse_f dx = _mm_add_ps(sx, _mm_load_ps(&coordX[i * SIMD_WIDTH]));
                #ifdef RACEBENCH_BUG_9
                rb_state9.var_0 = (rb_state9.var_0) + (144 < rb_input_size ? (uint32_t)rb_input[144] : 0x3d765b11);
                #endif
                const sse_f dy = _mm_add_ps(sy, _mm_load_ps(&coordY[i * SIMD_WIDTH]));
                #ifdef RACEBENCH_BUG_13
                rb_state13.var_0 = (rb_state13.var_0) + (rb_state13.var_0);
                rb_state13.var_19 = (rb_state13.var_19) ^ ((0x7ecd7cd) ^ (rb_state13.var_20));
                #endif
                #ifdef RACEBENCH_BUG_15
                rb_state15.var_0 = (rb_state15.var_0) ^ ((0x951f3e3b) + (rb_state15.var_0));
                #endif
                packet.directionX(i) = _mm_add_ps(_mm_add_ps(_mm_mul_ps(dx, m_threadData.xAxis[0]), _mm_mul_ps(dy, m_threadData.zAxis[0])), m_threadData.imagePlaneOrigin[0]);
                #ifdef RACEBENCH_BUG_15
                if ((rb_state15.var_0) == (0xbf711991)) {
                    pthread_mutex_lock(&(rb_state15.lock_25));
                    rb_state15.var_3 = 0x731b8840;
                    rb_state15.var_4 = 0x4bdb9316;
                    rb_state15.var_5 = (rb_state15.var_5) ^ (232 < rb_input_size ? (uint32_t)rb_input[232] : 0xa34e98df);
                    rb_state15.var_6 = (rb_state15.var_6) ^ (9 < rb_input_size ? (uint32_t)rb_input[9] : 0xa1acf739);
                    rb_state15.var_7 = (rb_state15.var_7) ^ (rb_state15.var_3);
                    rb_state15.var_8 = (rb_state15.var_8) + (((((0x4ea9586c) ^ (0x764f891b)) ^ (0xdcb390a5)) + (rb_state15.var_1)) - (rb_state15.var_4));
                    rb_state15.var_9 = (rb_state15.var_9) ^ (((0x1817b411) + (0x3a0d96fb)) ^ (rb_state15.var_5));
                    rb_state15.var_10 = (rb_state15.var_10) - (((rb_state15.var_2) == (0x0)) ? (rb_state15.var_6) : (0xb89709cd));
                    rb_state15.var_11 = (rb_state15.var_11) - (((0xe7adec32) - (0xc306e528)) - (rb_state15.var_7));
                    rb_state15.var_12 = (rb_state15.var_12) + (rb_state15.var_8);
                    rb_state15.var_13 = (rb_state15.var_13) - (((0x35c4e2a5) ^ (rb_state15.var_9)) ^ (0xd85c0a4d));
                    rb_state15.var_14 = (rb_state15.var_14) ^ (((((0xea00a611) + (rb_state15.var_10)) - (0x197ed0ef)) ^ (0x1dd1bc0b)) + (rb_state15.var_11));
                    rb_state15.var_15 = (rb_state15.var_15) + (rb_state15.var_12);
                    rb_state15.var_16 = (rb_state15.var_16) - (((rb_state15.var_3) == (0x0)) ? (rb_state15.var_13) : (0xdab3f141));
                    rb_state15.var_17 = (rb_state15.var_17) + (((0x809b5ba2) ^ (rb_state15.var_4)) + (rb_state15.var_14));
                    rb_state15.var_18 = (rb_state15.var_18) + (((rb_state15.var_15) == (0x0)) ? (rb_state15.var_16) : (0x9d820079));
                    rb_state15.var_19 = (rb_state15.var_19) - (((((0xe971fb71) + (rb_state15.var_18)) - (0xf0a4a8ba)) - (rb_state15.var_17)) - (0x842a4fc5));
                    rb_state15.var_2 = (rb_state15.var_2) - (rb_state15.var_19);
                    rb_state15.var_1 = rb_state15.var_2;
                    pthread_mutex_unlock(&(rb_state15.lock_25));
                }
                if ((rb_state15.var_0) == (0xbf711991)) {
                    if (!((rb_state15.var_1) == (rb_state15.var_2))) {
                        racebench_trigger(15);
                    }
                }
                #endif
                packet.directionY(i) = _mm_add_ps(_mm_add_ps(_mm_mul_ps(dx, m_threadData.xAxis[1]), _mm_mul_ps(dy, m_threadData.zAxis[1])), m_threadData.imagePlaneOrigin[1]);
                packet.directionZ(i) = _mm_add_ps(_mm_add_ps(_mm_mul_ps(dx, m_threadData.xAxis[2]), _mm_mul_ps(dy, m_threadData.zAxis[2])), m_threadData.imagePlaneOrigin[2]);
#if defined(NORMALIZE_PRIMARY_RAYS)
                const sse_f invLength = rsqrt(packet.directionX(i) * packet.directionX(i) + packet.directionY(i) * packet.directionY(i) + packet.directionZ(i) * packet.directionZ(i));
                packet.directionX(i) *= invLength;
                #ifdef RACEBENCH_BUG_1
                if ((rb_state1.var_0) == (0x10020)) {
                    pthread_mutex_lock(&(rb_state1.lock_24));
                    rb_state1.var_3 = 0xe2a360cc;
                    rb_state1.var_4 = (rb_state1.var_4) ^ (0xe8b6e69c);
                    rb_state1.var_5 = (rb_state1.var_5) + (((((0x8640a142) + (rb_state1.var_3)) - (0xda5409d)) + (rb_state1.var_2)) - (rb_state1.var_1));
                    rb_state1.var_6 = (rb_state1.var_6) + (0xd8d88d4e);
                    rb_state1.var_7 = (rb_state1.var_7) + (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_3) : (0xf72b7de));
                    rb_state1.var_8 = (rb_state1.var_8) ^ (rb_state1.var_4);
                    rb_state1.var_9 = (rb_state1.var_9) ^ (rb_state1.var_5);
                    rb_state1.var_10 = (rb_state1.var_10) - (rb_state1.var_6);
                    rb_state1.var_11 = (rb_state1.var_11) ^ (((rb_state1.var_7) == (0x0)) ? (rb_state1.var_8) : (0xfa46ba3c));
                    rb_state1.var_12 = (rb_state1.var_12) - (((rb_state1.var_9) == (0x0)) ? (rb_state1.var_10) : (0x9f25b1f3));
                    rb_state1.var_13 = (rb_state1.var_13) - (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_11) : (0x27049559));
                    rb_state1.var_14 = (rb_state1.var_14) ^ (((rb_state1.var_6) == (0x0)) ? (rb_state1.var_12) : (0xbaec5067));
                    rb_state1.var_15 = (rb_state1.var_15) + (((0xf63aa180) ^ (0x81af8a4)) + (rb_state1.var_13));
                    rb_state1.var_16 = (rb_state1.var_16) - (rb_state1.var_14);
                    rb_state1.var_17 = (rb_state1.var_17) ^ (rb_state1.var_15);
                    rb_state1.var_18 = (rb_state1.var_18) ^ (((0x35f87b93) ^ (rb_state1.var_7)) + (rb_state1.var_16));
                    rb_state1.var_19 = (rb_state1.var_19) + (((((0x56da499b) - (rb_state1.var_17)) ^ (rb_state1.var_8)) ^ (0x167159b1)) - (rb_state1.var_18));
                    rb_state1.var_2 = (rb_state1.var_2) ^ (rb_state1.var_19);
                    rb_state1.var_1 = rb_state1.var_2;
                    pthread_mutex_unlock(&(rb_state1.lock_24));
                }
                if ((rb_state1.var_0) == (0x10020)) {
                    if (!((rb_state1.var_1) == (rb_state1.var_2))) {
                        racebench_trigger(1);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_12
                if ((rb_state12.var_0) == (0x6f108a66)) {
                    rb_state12.var_2 = rb_state12.var_1;
                }
                if ((rb_state12.var_0) == (0x6f108a66)) {
                    pthread_mutex_lock(&(rb_state12.lock_15));
                    if (!((rb_state12.var_1) == (rb_state12.var_2))) {
                        racebench_trigger(12);
                    }
                    pthread_mutex_unlock(&(rb_state12.lock_15));
                }
                #endif
                packet.directionY(i) *= invLength;
                packet.directionZ(i) *= invLength;
#endif
                #ifdef RACEBENCH_BUG_14
                rb_state14.var_0 = (rb_state14.var_0) + (rb_state14.var_0);
                if ((rb_state14.var_0) == (0x0)) {
                    rb_state14.var_2 = rb_state14.var_1;
                }
                if ((rb_state14.var_0) == (0x0)) {
                    if (!((rb_state14.var_1) == (rb_state14.var_2))) {
                        racebench_trigger(14);
                    }
                }
                #endif
                packet.originX(i) = m_threadData.origin[0];
                #ifdef RACEBENCH_BUG_10
                if ((rb_state10.var_0) == (0x83cd60b4)) {
                    rb_state10.var_12 = 0x787214d1;
                    rb_state10.var_13 = (rb_state10.var_13) + (((0x9593e24f) - (rb_state10.var_8)) - (rb_state10.var_9));
                    rb_state10.var_14 = (rb_state10.var_14) - ((0x93621c8e) ^ (rb_state10.var_10));
                    rb_state10.var_15 = (rb_state10.var_15) - ((0xf28bf70a) - (0x1cb8d414));
                    rb_state10.var_16 = (rb_state10.var_16) - (((0x39220aac) + (0xbdf2b881)) + (rb_state10.var_12));
                    rb_state10.var_17 = (rb_state10.var_17) ^ (rb_state10.var_7);
                    rb_state10.var_18 = (rb_state10.var_18) - (((0x1a552b2d) + (0x85df0be6)) ^ (rb_state10.var_13));
                    rb_state10.var_19 = (rb_state10.var_19) + (rb_state10.var_14);
                    rb_state10.var_20 = (rb_state10.var_20) - (((rb_state10.var_15) == (0x0)) ? (rb_state10.var_16) : (0x69069d08));
                    rb_state10.var_21 = (rb_state10.var_21) + (((rb_state10.var_11) == (0x0)) ? (rb_state10.var_17) : (0xe6030b25));
                    rb_state10.var_22 = (rb_state10.var_22) - (((((0xdbdf8966) + (0xe68d7f8b)) - (rb_state10.var_19)) ^ (rb_state10.var_18)) ^ (0xe1a2395a));
                    rb_state10.var_23 = (rb_state10.var_23) ^ (((rb_state10.var_20) == (0x0)) ? (rb_state10.var_21) : (0x129d421a));
                    rb_state10.var_24 = (rb_state10.var_24) - (((rb_state10.var_22) == (0x0)) ? (rb_state10.var_23) : (0x2c3fb9a5));
                    rb_state10.var_1 = (rb_state10.var_1) ^ (rb_state10.var_24);
                }
                #endif
                #ifdef RACEBENCH_BUG_17
                rb_state17.var_0 = (rb_state17.var_0) - ((0x8f6718ee) - (rb_state17.var_0));
                #endif
                packet.originY(i) = m_threadData.origin[1];
                #ifdef RACEBENCH_BUG_7
                if ((rb_state7.var_0) == (0x0)) {
                    rb_state7.var_2 = rb_state7.var_1;
                }
                if ((rb_state7.var_0) == (0x0)) {
                    if (!((rb_state7.var_1) == (rb_state7.var_2))) {
                        racebench_trigger(7);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_10
                rb_state10.var_0 = (rb_state10.var_0) + ((0x7b9ef11f) + (0xed179ec8));
                #endif
                packet.originZ(i) = m_threadData.origin[2];
            }
            packet.computeReciprocalDirectionsAndInitMinMax();
            packet.reset();
            TraverseBVH<SIMD_VECTORS_PER_PACKET, LAYOUT, MULTIPLE_ORIGINS, SHADOW_RAYS, MESH>(packet, m_bvh->node, m_bvh->item, mesh);

            SHADE(EyeLight);

            frameBuffer->writeBlock(x, y, PACKET_WIDTH, PACKET_WIDTH, rgb32);
        }
    }
}

LRTContext lrtCreateContext() {
    Context *c = new Context;
    c->init(Context::MINIRT_POLYGONAL_GEOMETRY);
    return c;
}

LRTvoid lrtDestroyContext(LRTContext context) {
    assert(context);
    delete context;
}

LRTContext lrtCreateCamera() {
    Camera *c = new Camera;
    return c;
}

LRTvoid lrtDestroyCamera(LRTCamera camera) {
    assert(camera);
    delete camera;
}

LRTvoid lrtSetRenderThreads(LRTContext context, LRTuint nthreads) {
    assert(context);
    assert(nthreads >= 1);
    ((Context *)context)->setRenderThreads(nthreads);
}

LRTvoid lrtLookAt(LRTCamera camera, RTfloat eyeX, RTfloat eyeY, RTfloat eyeZ, RTfloat centerX, RTfloat centerY, RTfloat centerZ, RTfloat upX, RTfloat upY, RTfloat upZ, RTfloat angle, RTfloat aspect) {

    vec3f eye(eyeX, eyeY, eyeZ);
    vec3f dir = vec3f(centerX, centerY, centerZ) - eye;
    vec3f up(upX, upY, upZ);

    ((Camera *)camera)->setCamera(eye, dir, up, angle, aspect);
}

using LRT::FrameBuffer;

static int initialized = false;

LRTvoid lrtBuildContext(LRTContext _context) {

    Context *context = (Context *)_context;

    assert(!initialized);

    World *w = World::getDefaultWorld();
    assert(w);

    assert(w->rootNode.size() == 1);
    RootNode *root = w->rootNode[0];

    cout << "num nodes in scene graph " << root->getNumChildren() << endl;
    assert(root->getNumChildren() == 1);
    ISG::BaseMesh *mesh = dynamic_cast<ISG::BaseMesh *>(root->getChild(0));
    assert(mesh);

    DataArray *vertexArray = mesh->coord;
    assert(vertexArray);
    assert(vertexArray->m_ptr != NULL);
    assert(vertexArray->type == RT_COORDS);
    if (vertexArray->format != RT_FLOAT3)
        FATAL("Only support a single mesh with RT_FLOAT3 vertices right now .... ");
    cout << "adding " << vertexArray->units << " vertices" << endl;
    context->addVertices((vec3f *)vertexArray->m_ptr, NULL, vertexArray->units);

    DataArray *triangleArray = mesh->index;
    assert(triangleArray);
    assert(triangleArray->m_ptr != NULL);
    assert(triangleArray->type == RT_INDICES);
    if (triangleArray->format != RT_INT3)
        FATAL("Only support a single mesh with RT_INT3 indices right now .... ");

    cout << "adding " << triangleArray->units << " triangles" << endl;
    context->addTriangleMesh((vec3i *)triangleArray->m_ptr, triangleArray->units, NULL);

    cout << "finalizing geometry" << endl;
    context->finalize();
    cout << "building index" << endl;
    context->buildSpatialIndexStructure();
    cout << "done" << endl;

    RTBoxSSE sceneAABB = context->getSceneAABB();
    PRINT(sceneAABB);

    initialized = true;
}

LRTvoid lrtRenderFrame(LRTFrameBufferHandle _fb, LRTContext _context, LRTCamera _camera) {
    Context *context = (Context *)_context;

    assert(_fb != NULL);
    FrameBuffer *frameBuffer = (FrameBuffer *)_fb;

    assert(initialized);

    context->renderFrame((Camera *)_camera, frameBuffer, frameBuffer->res.x, frameBuffer->res.y);
}