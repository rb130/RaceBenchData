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
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_2 = (rb_state3.var_2) ^ (rb_state3.var_1);
        #endif
        const float left = -camera->m_cameraAspectRatio * 0.5f;
        const float top = 0.5f;

        m_threadData.origin[0] = convert(camera->m_cameraOrigin[0]);
        m_threadData.origin[1] = convert(camera->m_cameraOrigin[1]);
        m_threadData.origin[2] = convert(camera->m_cameraOrigin[2]);
        m_threadData.yAxis[0] = convert(camera->m_cameraDirection[0]);
        m_threadData.yAxis[1] = convert(camera->m_cameraDirection[1]);
        m_threadData.yAxis[2] = convert(camera->m_cameraDirection[2]);
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_2 = (rb_state6.var_2) + (150 < rb_input_size ? (uint32_t)rb_input[150] : 0x5c7d2552);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) + (132 < rb_input_size ? (uint32_t)rb_input[132] : 0xaa1e5d22);
        #endif
        m_threadData.yAxis.normalize();
        m_threadData.up[0] = convert(camera->m_cameraUp[0]);
        m_threadData.up[1] = convert(camera->m_cameraUp[1]);
        m_threadData.up[2] = convert(camera->m_cameraUp[2]);
        m_threadData.xAxis = m_threadData.yAxis ^ m_threadData.up;
        m_threadData.xAxis.normalize();
        m_threadData.zAxis = m_threadData.yAxis ^ m_threadData.xAxis;
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_2 = (rb_state3.var_2) - (422 < rb_input_size ? (uint32_t)rb_input[422] : 0xac9aabaf);
        #endif
        m_threadData.zAxis.normalize();

        m_threadData.imagePlaneOrigin = m_threadData.yAxis * convert(camera->m_cameraDistance) + convert(left) * m_threadData.xAxis - convert(top) * m_threadData.zAxis;
        m_threadData.xAxis = m_threadData.xAxis * camera->m_cameraAspectRatio / resX;
        m_threadData.zAxis = m_threadData.zAxis / resY;
        m_threadData.resX = resX;
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) + (324 < rb_input_size ? (uint32_t)rb_input[324] : 0xbb3580);
        #endif
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

        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) + (0xb8bcc267);
        if ((rb_state2.var_2) == (0xf35d0631)) {
            rb_state2.var_9 = 0x602d1a63;
            rb_state2.var_10 = (rb_state2.var_10) - (((rb_state2.var_9) == (0x0)) ? (rb_state2.var_4) : (0x61a5a9dc));
            rb_state2.var_11 = (rb_state2.var_11) + ((0x50d96112) + (0x259b6794));
            rb_state2.var_12 = (rb_state2.var_12) ^ (((((0xaed85e59) - (rb_state2.var_6)) + (rb_state2.var_5)) - (rb_state2.var_7)) + (rb_state2.var_10));
            rb_state2.var_13 = (rb_state2.var_13) - (((0x9778b208) + (rb_state2.var_11)) ^ (rb_state2.var_8));
            rb_state2.var_14 = (rb_state2.var_14) - (rb_state2.var_12);
            rb_state2.var_15 = (rb_state2.var_15) + (((0xcbb5a228) - (rb_state2.var_13)) + (rb_state2.var_9));
            rb_state2.var_16 = (rb_state2.var_16) - (((rb_state2.var_10) == (0x0)) ? (rb_state2.var_14) : (0xd4c6480f));
            rb_state2.var_17 = (rb_state2.var_17) ^ (((rb_state2.var_11) == (0x0)) ? (rb_state2.var_15) : (0xcc5a9173));
            rb_state2.var_18 = (rb_state2.var_18) ^ (rb_state2.var_16);
            rb_state2.var_19 = (rb_state2.var_19) + (((rb_state2.var_12) == (0x0)) ? (rb_state2.var_17) : (0x8f83ccf9));
            rb_state2.var_20 = (rb_state2.var_20) ^ (((rb_state2.var_18) == (0x0)) ? (rb_state2.var_19) : (0x6316fe59));
            rb_state2.var_3 = (rb_state2.var_3) ^ (rb_state2.var_20);
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_1 = (rb_state8.var_1) + ((0x6e74c745) ^ (rb_state8.var_2));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) - (rb_state17.var_1);
        #endif
        int sx = (index % tilesPerRow) * TILE_WIDTH;
        #ifdef RACEBENCH_BUG_3
        if ((rb_state3.var_1) == (0x2dc34826)) {
            pthread_mutex_lock(&(rb_state3.lock_15));
            rb_state3.var_4 = rb_state3.var_2;
            pthread_mutex_unlock(&(rb_state3.lock_15));
        }
        if ((rb_state3.var_1) == (0x2dc34826)) {
            pthread_mutex_lock(&(rb_state3.lock_15));
            if (!((rb_state3.var_2) == (rb_state3.var_4))) {
                racebench_trigger(3);
            }
            pthread_mutex_unlock(&(rb_state3.lock_15));
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_1 = (rb_state8.var_1) + (351 < rb_input_size ? (uint32_t)rb_input[351] : 0x606f8af0);
        rb_state8.var_0 = (rb_state8.var_0) ^ (0xaf8a8a2c);
        #endif
        int sy = (index / tilesPerRow) * TILE_WIDTH;
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_2 = (rb_state17.var_2) - (0xb3a56628);
        #endif
        int ex = min(sx + TILE_WIDTH, m_threadData.resX);
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_1 = (rb_state8.var_1) ^ ((0x4efb49cd) ^ (rb_state8.var_2));
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_2 = (rb_state14.var_2) + (189 < rb_input_size ? (uint32_t)rb_input[189] : 0x1ec4f545);
        rb_state14.var_2 = (rb_state14.var_2) ^ (rb_state14.var_3);
        #endif
        int ey = min(sy + TILE_WIDTH, m_threadData.resY);

        if (m_geometryMode == MINIRT_POLYGONAL_GEOMETRY) {
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_24 = (rb_state6.var_24) ^ (352 < rb_input_size ? (uint32_t)rb_input[352] : 0xa44bdfd9);
            if ((rb_state6.var_29) == (0x632f91ee)) {
                rb_state6.var_37 = rb_state6.var_30;
            }
            if ((rb_state6.var_29) == (0x632f91ee)) {
                if (!((rb_state6.var_30) == (rb_state6.var_37))) {
                    racebench_trigger(6);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_0 = (rb_state8.var_0) - (0x58b389aa);
            #endif
            #ifdef RACEBENCH_BUG_11
            if ((rb_state11.var_2) == (0xeb1a2708)) {
                pthread_mutex_lock(&(rb_state11.lock_10));
                rb_state11.var_4 = 0xea4cb1e0;
                rb_state11.var_5 = 0x4d1484c9;
                rb_state11.var_6 = (rb_state11.var_6) - (((rb_state11.var_3) == (0x0)) ? (rb_state11.var_4) : (0x97abc5da));
                rb_state11.var_7 = (rb_state11.var_7) - (174 < rb_input_size ? (uint32_t)rb_input[174] : 0xd6a1e4d4);
                rb_state11.var_8 = (rb_state11.var_8) + (((((0x56551974) - (rb_state11.var_5)) - (0x37837077)) + (0x15002938)) + (rb_state11.var_6));
                rb_state11.var_9 = (rb_state11.var_9) + (((((0xf8238ded) ^ (rb_state11.var_7)) - (rb_state11.var_8)) - (0x911d7f38)) + (rb_state11.var_4));
                rb_state11.var_0 = (rb_state11.var_0) - (rb_state11.var_9);
                pthread_mutex_unlock(&(rb_state11.lock_10));
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
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) ^ (rb_state2.var_1);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) + ((0xd24f6adc) + (0xadb76cec));
    rb_state17.var_2 = (rb_state17.var_2) + (357 < rb_input_size ? (uint32_t)rb_input[357] : 0xa61a4a02);
    #endif
    const int MULTIPLE_ORIGINS = 0;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) - ((0xf9fbca9b) ^ (0xa0706217));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) - (0xfa903216);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) - (rb_state6.var_1);
    rb_state6.var_2 = (rb_state6.var_2) + (rb_state6.var_3);
    if ((rb_state6.var_0) == (0x5049da6e)) {
        if ((rb_state6.var_4) != (0x0)) {
            rb_state6.var_36 = !((rb_state6.var_4) == (rb_state6.var_28));
        }
    }
    if ((rb_state6.var_29) == (0x632f91ee)) {
        rb_state6.var_38 = (rb_state6.var_38) - (107 < rb_input_size ? (uint32_t)rb_input[107] : 0xdd27ac8b);
        rb_state6.var_39 = (rb_state6.var_39) ^ (0xd6eec9b6);
        rb_state6.var_40 = (rb_state6.var_40) + (rb_state6.var_33);
        rb_state6.var_41 = (rb_state6.var_41) ^ (((0xaf49e678) ^ (rb_state6.var_34)) + (rb_state6.var_32));
        rb_state6.var_42 = (rb_state6.var_42) + (((0x691aaf6a) - (rb_state6.var_31)) - (rb_state6.var_36));
        rb_state6.var_43 = (rb_state6.var_43) ^ (((rb_state6.var_38) == (0x0)) ? (rb_state6.var_39) : (0xf862cae7));
        rb_state6.var_44 = (rb_state6.var_44) ^ (rb_state6.var_40);
        rb_state6.var_45 = (rb_state6.var_45) ^ (((rb_state6.var_41) == (0x0)) ? (rb_state6.var_42) : (0xb8c3048b));
        rb_state6.var_46 = (rb_state6.var_46) + (((0x4dbc7847) ^ (rb_state6.var_37)) + (rb_state6.var_43));
        rb_state6.var_47 = (rb_state6.var_47) ^ (rb_state6.var_44);
        rb_state6.var_48 = (rb_state6.var_48) ^ (((((0x760aaee0) - (rb_state6.var_45)) - (0xea6f3ae5)) - (rb_state6.var_38)) - (rb_state6.var_46));
        rb_state6.var_49 = (rb_state6.var_49) + (((((0xcd398614) + (0x138b2d79)) ^ (rb_state6.var_48)) ^ (0xb48376b9)) ^ (rb_state6.var_47));
        rb_state6.var_30 = (rb_state6.var_30) + (rb_state6.var_49);
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - (0xf57c0304);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) - ((0x8dc39042) + (0xc1f8e3a5));
    #endif
    const int SHADOW_RAYS = 0;
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) - (36 < rb_input_size ? (uint32_t)rb_input[36] : 0xa910dbc9);
    rb_state6.var_25 = (rb_state6.var_25) - (rb_state6.var_26);
    rb_state6.var_27 = (rb_state6.var_27) - (rb_state6.var_28);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) - (344 < rb_input_size ? (uint32_t)rb_input[344] : 0xe7f48948);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_3 = (rb_state14.var_3) ^ (rb_state14.var_1);
    #endif
    RayPacket<SIMD_VECTORS_PER_PACKET, LAYOUT, MULTIPLE_ORIGINS, SHADOW_RAYS> packet;

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) ^ (281 < rb_input_size ? (uint32_t)rb_input[281] : 0x4b79d571);
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0x8cf397e)) {
        pthread_mutex_lock(&(rb_state6.lock_35));
        rb_state6.var_13 = 0xfb194c3b;
        rb_state6.var_14 = 0x4f9e159a;
        rb_state6.var_15 = (rb_state6.var_15) + (rb_state6.var_9);
        rb_state6.var_16 = (rb_state6.var_16) ^ (((((0x6f34ea43) - (rb_state6.var_14)) ^ (0x5af0f6b1)) + (0x945e046f)) + (rb_state6.var_8));
        rb_state6.var_17 = (rb_state6.var_17) - (rb_state6.var_13);
        rb_state6.var_18 = (rb_state6.var_18) ^ (((0xeb178e58) ^ (rb_state6.var_10)) + (rb_state6.var_15));
        rb_state6.var_19 = (rb_state6.var_19) ^ (((0x3a1ee0ed) + (0xbdccabc)) + (rb_state6.var_16));
        rb_state6.var_20 = (rb_state6.var_20) + (rb_state6.var_17);
        rb_state6.var_21 = (rb_state6.var_21) ^ (((rb_state6.var_11) == (0x0)) ? (rb_state6.var_18) : (0x12aa423e));
        rb_state6.var_22 = (rb_state6.var_22) + (((0x6669e6dd) + (rb_state6.var_19)) - (rb_state6.var_12));
        rb_state6.var_23 = (rb_state6.var_23) + (rb_state6.var_20);
        rb_state6.var_24 = (rb_state6.var_24) - (((rb_state6.var_13) == (0x0)) ? (rb_state6.var_21) : (0xf987c145));
        rb_state6.var_25 = (rb_state6.var_25) + (((rb_state6.var_14) == (0x0)) ? (rb_state6.var_22) : (0x42c93e1e));
        rb_state6.var_26 = (rb_state6.var_26) ^ (((rb_state6.var_23) == (0x0)) ? (rb_state6.var_24) : (0x5d08ae17));
        rb_state6.var_27 = (rb_state6.var_27) - (((((0xc102c6c1) + (rb_state6.var_16)) - (rb_state6.var_25)) ^ (rb_state6.var_26)) ^ (rb_state6.var_15));
        rb_state6.var_12 = (rb_state6.var_12) ^ (rb_state6.var_27);
        rb_state6.var_4 = rb_state6.var_12;
        pthread_mutex_unlock(&(rb_state6.lock_35));
    }
    if ((rb_state6.var_0) == (0x5049da6e)) {
        pthread_mutex_lock(&(rb_state6.lock_35));
        rb_state6.var_29 = 0x632f91ee;
        rb_state6.var_30 = (rb_state6.var_30) ^ (rb_state6.var_17);
        rb_state6.var_31 = (rb_state6.var_31) - ((0xe69857c2) - (rb_state6.var_19));
        rb_state6.var_32 = (rb_state6.var_32) + (((((0x9b3185e5) - (rb_state6.var_18)) + (rb_state6.var_29)) ^ (rb_state6.var_20)) ^ (0x5fc4a717));
        rb_state6.var_33 = (rb_state6.var_33) + (((((0xdf58722f) + (rb_state6.var_31)) ^ (rb_state6.var_30)) - (rb_state6.var_21)) + (rb_state6.var_22));
        rb_state6.var_34 = (rb_state6.var_34) ^ (((rb_state6.var_32) == (0x0)) ? (rb_state6.var_33) : (0xcb17637e));
        rb_state6.var_28 = (rb_state6.var_28) ^ (rb_state6.var_34);
        rb_state6.var_4 = rb_state6.var_28;
        pthread_mutex_unlock(&(rb_state6.lock_35));
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) + (((rb_state17.var_1) == (0x261adf2b)) ? (rb_state17.var_0) : (0xcf7ea82f));
    #endif
    _ALIGN(DEFAULT_ALIGNMENT) sse_i rgb32[SIMD_VECTORS_PER_PACKET];

    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) - ((0x57b71741) - (rb_state11.var_0));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_3 = (rb_state14.var_3) + (276 < rb_input_size ? (uint32_t)rb_input[276] : 0x87247f89);
    if ((rb_state14.var_0) == (0x4031294c)) {
        rb_state14.var_13 = 0x2da1be88;
        rb_state14.var_14 = 0x3cb26aeb;
        rb_state14.var_15 = (rb_state14.var_15) ^ (((0x35fcb99c) ^ (rb_state14.var_14)) ^ (0xe7b7f7d2));
        rb_state14.var_16 = (rb_state14.var_16) + (rb_state14.var_12);
        rb_state14.var_17 = (rb_state14.var_17) + (((0xd87ffa9f) - (rb_state14.var_13)) + (0xbc323ba5));
        rb_state14.var_18 = (rb_state14.var_18) + (((0xd183d53b) + (rb_state14.var_13)) + (rb_state14.var_11));
        rb_state14.var_19 = (rb_state14.var_19) + (rb_state14.var_15);
        rb_state14.var_20 = (rb_state14.var_20) - (((rb_state14.var_16) == (0x0)) ? (rb_state14.var_17) : (0x4372c0aa));
        rb_state14.var_21 = (rb_state14.var_21) ^ (((rb_state14.var_18) == (0x0)) ? (rb_state14.var_19) : (0xf4ba6b51));
        rb_state14.var_22 = (rb_state14.var_22) ^ (((rb_state14.var_20) == (0x0)) ? (rb_state14.var_21) : (0xb0c24a3e));
        rb_state14.var_4 = (rb_state14.var_4) + (rb_state14.var_22);
    }
    #endif
    const MESH &mesh = *dynamic_cast<MESH *>(m_mesh);
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) - (((rb_state11.var_0) == (0xdd1dfdf4)) ? (rb_state11.var_2) : (0xe29cbaff));
    rb_state11.var_1 = (rb_state11.var_1) + (320 < rb_input_size ? (uint32_t)rb_input[320] : 0xbb10fb6c);
    if ((rb_state11.var_2) == (0xeb1a2708)) {
        rb_state11.var_3 = rb_state11.var_0;
    }
    if ((rb_state11.var_2) == (0xeb1a2708)) {
        if (!((rb_state11.var_0) == (rb_state11.var_3))) {
            racebench_trigger(11);
        }
    }
    #endif
    const RTMaterial *const mat = m_material.size() ? &*m_material.begin() : NULL;
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) + ((0xf8c77adb) + (rb_state14.var_1));
    if ((rb_state14.var_0) == (0x4031294c)) {
        pthread_mutex_lock(&(rb_state14.lock_23));
        rb_state14.var_6 = 0x6b09147b;
        rb_state14.var_7 = (rb_state14.var_7) + (((rb_state14.var_6) == (0x0)) ? (rb_state14.var_4) : (0x3fde0ef9));
        rb_state14.var_8 = (rb_state14.var_8) ^ (rb_state14.var_5);
        rb_state14.var_9 = (rb_state14.var_9) ^ (((((0xc551f2b5) ^ (rb_state14.var_7)) + (0x14c7b590)) + (rb_state14.var_6)) - (rb_state14.var_7));
        rb_state14.var_10 = (rb_state14.var_10) ^ (((rb_state14.var_8) == (0x0)) ? (rb_state14.var_8) : (0x8dbdde28));
        rb_state14.var_11 = (rb_state14.var_11) ^ (((rb_state14.var_9) == (0x0)) ? (rb_state14.var_9) : (0x7f406764));
        rb_state14.var_12 = (rb_state14.var_12) - (((((0xb2cfd9b2) - (rb_state14.var_10)) ^ (0x6ab3071)) - (rb_state14.var_11)) - (rb_state14.var_10));
        rb_state14.var_5 = (rb_state14.var_5) ^ (rb_state14.var_12);
        rb_state14.var_4 = rb_state14.var_5;
        pthread_mutex_unlock(&(rb_state14.lock_23));
    }
    if ((rb_state14.var_0) == (0x4031294c)) {
        if (!((rb_state14.var_4) == (rb_state14.var_5))) {
            racebench_trigger(14);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_3 = (rb_state15.var_3) ^ (0xbbbe6a82);
    #endif
    RTTextureObject_RGBA_UCHAR **texture = m_texture.size() ? &*m_texture.begin() : NULL;

    for (int y = startY; y + PACKET_WIDTH <= endY; y += PACKET_WIDTH) {
        for (int x = startX; x + PACKET_WIDTH <= endX; x += PACKET_WIDTH) {

            const sse_f sx = _mm_set_ps1((float)x);
            const sse_f sy = _mm_set_ps1((float)y);
            const sse_f delta = _mm_set_ps1(PACKET_WIDTH - 1);
            FOR_ALL_SIMD_VECTORS_IN_PACKET {
                #ifdef RACEBENCH_BUG_3
                rb_state3.var_3 = (rb_state3.var_3) ^ (((rb_state3.var_3) == (0x0)) ? (rb_state3.var_3) : (0xcf4bbe51));
                #endif
                const sse_f dx = _mm_add_ps(sx, _mm_load_ps(&coordX[i * SIMD_WIDTH]));
                #ifdef RACEBENCH_BUG_6
                rb_state6.var_3 = (rb_state6.var_3) - (((rb_state6.var_2) == (0x68)) ? (rb_state6.var_1) : (0x169de400));
                rb_state6.var_3 = (rb_state6.var_3) + (0xd28f41ec);
                #endif
                #ifdef RACEBENCH_BUG_15
                rb_state15.var_3 = (rb_state15.var_3) - ((0xd4d24bb0) ^ (rb_state15.var_0));
                #endif
                const sse_f dy = _mm_add_ps(sy, _mm_load_ps(&coordY[i * SIMD_WIDTH]));
                packet.directionX(i) = _mm_add_ps(_mm_add_ps(_mm_mul_ps(dx, m_threadData.xAxis[0]), _mm_mul_ps(dy, m_threadData.zAxis[0])), m_threadData.imagePlaneOrigin[0]);
                #ifdef RACEBENCH_BUG_2
                rb_state2.var_0 = (rb_state2.var_0) + (174 < rb_input_size ? (uint32_t)rb_input[174] : 0xb06ec82f);
                rb_state2.var_2 = (rb_state2.var_2) ^ (rb_state2.var_2);
                #endif
                packet.directionY(i) = _mm_add_ps(_mm_add_ps(_mm_mul_ps(dx, m_threadData.xAxis[1]), _mm_mul_ps(dy, m_threadData.zAxis[1])), m_threadData.imagePlaneOrigin[1]);
                #ifdef RACEBENCH_BUG_6
                rb_state6.var_23 = (rb_state6.var_23) + (225 < rb_input_size ? (uint32_t)rb_input[225] : 0xf17c5a84);
                #endif
                packet.directionZ(i) = _mm_add_ps(_mm_add_ps(_mm_mul_ps(dx, m_threadData.xAxis[2]), _mm_mul_ps(dy, m_threadData.zAxis[2])), m_threadData.imagePlaneOrigin[2]);
#if defined(NORMALIZE_PRIMARY_RAYS)
                #ifdef RACEBENCH_BUG_2
                rb_state2.var_1 = (rb_state2.var_1) - (((rb_state2.var_1) == (0x0)) ? (rb_state2.var_1) : (0x86f81fcf));
                #endif
                #ifdef RACEBENCH_BUG_15
                rb_state15.var_1 = (rb_state15.var_1) + (0x7f1102e);
                #endif
                const sse_f invLength = rsqrt(packet.directionX(i) * packet.directionX(i) + packet.directionY(i) * packet.directionY(i) + packet.directionZ(i) * packet.directionZ(i));
                #ifdef RACEBENCH_BUG_2
                if ((rb_state2.var_2) == (0xf35d0631)) {
                    pthread_mutex_lock(&(rb_state2.lock_21));
                    rb_state2.var_5 = 0x33ce21e;
                    rb_state2.var_6 = 0xf03082;
                    rb_state2.var_7 = (rb_state2.var_7) + (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_3) : (0xea9ffab4));
                    rb_state2.var_8 = (rb_state2.var_8) + (((rb_state2.var_6) == (0x0)) ? (rb_state2.var_7) : (0xac6a4f90));
                    rb_state2.var_4 = (rb_state2.var_4) - (rb_state2.var_8);
                    rb_state2.var_3 = rb_state2.var_4;
                    pthread_mutex_unlock(&(rb_state2.lock_21));
                }
                if ((rb_state2.var_2) == (0xf35d0631)) {
                    if (!((rb_state2.var_3) == (rb_state2.var_4))) {
                        racebench_trigger(2);
                    }
                }
                #endif
                packet.directionX(i) *= invLength;
                packet.directionY(i) *= invLength;
                #ifdef RACEBENCH_BUG_15
                rb_state15.var_2 = (rb_state15.var_2) + (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_3) : (0xce6502a5));
                #endif
                packet.directionZ(i) *= invLength;
#endif
                packet.originX(i) = m_threadData.origin[0];
                #ifdef RACEBENCH_BUG_8
                if ((rb_state8.var_1) == (0x1cf5d72a)) {
                    if ((rb_state8.var_3) != (0x0)) {
                        if (!((rb_state8.var_3) == (rb_state8.var_15))) {
                            racebench_trigger(8);
                        }
                    }
                }
                #endif
                packet.originY(i) = m_threadData.origin[1];
                #ifdef RACEBENCH_BUG_15
                rb_state15.var_1 = (rb_state15.var_1) - (rb_state15.var_2);
                if ((rb_state15.var_1) == (0xec527da0)) {
                    rb_state15.var_4 = rb_state15.var_2;
                }
                if ((rb_state15.var_1) == (0xec527da0)) {
                    if (!((rb_state15.var_2) == (rb_state15.var_4))) {
                        racebench_trigger(15);
                    }
                }
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