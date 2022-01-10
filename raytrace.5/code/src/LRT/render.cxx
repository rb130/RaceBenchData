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
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_2 = (rb_state0.var_2) + (0xec7cd1a0);
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_2 = (rb_state1.var_2) + (285 < rb_input_size ? (uint32_t)rb_input[285] : 0x44109d33);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) - (114 < rb_input_size ? (uint32_t)rb_input[114] : 0x5cc75217);
        #endif
        const float top = 0.5f;

        m_threadData.origin[0] = convert(camera->m_cameraOrigin[0]);
        m_threadData.origin[1] = convert(camera->m_cameraOrigin[1]);
        m_threadData.origin[2] = convert(camera->m_cameraOrigin[2]);
        m_threadData.yAxis[0] = convert(camera->m_cameraDirection[0]);
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) ^ ((0x7f84ab33) ^ (rb_state15.var_0));
        #endif
        m_threadData.yAxis[1] = convert(camera->m_cameraDirection[1]);
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_1 = (rb_state12.var_1) - (120 < rb_input_size ? (uint32_t)rb_input[120] : 0xce48b5a9);
        #endif
        m_threadData.yAxis[2] = convert(camera->m_cameraDirection[2]);
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) + (rb_state6.var_0);
        #endif
        m_threadData.yAxis.normalize();
        m_threadData.up[0] = convert(camera->m_cameraUp[0]);
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) - (328 < rb_input_size ? (uint32_t)rb_input[328] : 0x19e939b0);
        #endif
        m_threadData.up[1] = convert(camera->m_cameraUp[1]);
        m_threadData.up[2] = convert(camera->m_cameraUp[2]);
        m_threadData.xAxis = m_threadData.yAxis ^ m_threadData.up;
        m_threadData.xAxis.normalize();
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_2 = (rb_state16.var_2) + (0xa7ae06e4);
        #endif
        m_threadData.zAxis = m_threadData.yAxis ^ m_threadData.xAxis;
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) ^ (((rb_state19.var_0) == (0x0)) ? (rb_state19.var_0) : (0x9cf473ec));
        #endif
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

        #ifdef RACEBENCH_BUG_2
        rb_state2.var_3 = (rb_state2.var_3) - (6 < rb_input_size ? (uint32_t)rb_input[6] : 0x3d2e8c48);
        rb_state2.var_3 = (rb_state2.var_3) + ((0xc8d804a3) + (rb_state2.var_2));
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_2 = (rb_state4.var_2) + (0x73626bc0);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_18 = (rb_state6.var_18) - (rb_state6.var_19);
        if ((rb_state6.var_25) == (0x1)) {
            rb_state6.var_26 = rb_state6.var_9;
        }
        if ((rb_state6.var_25) == (0x1)) {
            if (!((rb_state6.var_9) == (rb_state6.var_26))) {
                racebench_trigger(6);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_0) == (0xf2084ba)) {
            pthread_mutex_lock(&(rb_state7.lock_22));
            rb_state7.var_6 = 0x858cafc0;
            rb_state7.var_7 = 0xce5daa1d;
            rb_state7.var_8 = (rb_state7.var_8) ^ (((rb_state7.var_6) == (0x0)) ? (rb_state7.var_5) : (0x5adeb8e8));
            rb_state7.var_9 = (rb_state7.var_9) + (((rb_state7.var_7) == (0x0)) ? (rb_state7.var_7) : (0xb6547f3d));
            rb_state7.var_10 = (rb_state7.var_10) + (((rb_state7.var_6) == (0x0)) ? (rb_state7.var_4) : (0x7565343f));
            rb_state7.var_11 = (rb_state7.var_11) - (((0xd5319e43) - (0xdd9083d1)) ^ (rb_state7.var_8));
            rb_state7.var_12 = (rb_state7.var_12) - (((rb_state7.var_9) == (0x0)) ? (rb_state7.var_10) : (0x95e1893f));
            rb_state7.var_13 = (rb_state7.var_13) + (rb_state7.var_11);
            rb_state7.var_14 = (rb_state7.var_14) ^ (((0xc1a0e869) + (0x37c25d82)) + (rb_state7.var_12));
            rb_state7.var_15 = (rb_state7.var_15) ^ (rb_state7.var_13);
            rb_state7.var_16 = (rb_state7.var_16) - (((((0x4eb193fc) - (rb_state7.var_8)) ^ (rb_state7.var_9)) + (rb_state7.var_15)) ^ (rb_state7.var_14));
            rb_state7.var_5 = (rb_state7.var_5) - (rb_state7.var_16);
            rb_state7.var_4 = rb_state7.var_5;
            pthread_mutex_unlock(&(rb_state7.lock_22));
        }
        if ((rb_state7.var_0) == (0xf2084ba)) {
            if (!((rb_state7.var_4) == (rb_state7.var_5))) {
                racebench_trigger(7);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_24 = (rb_state10.var_24) ^ (411 < rb_input_size ? (uint32_t)rb_input[411] : 0xbed197c8);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_3 = (rb_state15.var_3) ^ (rb_state15.var_2);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_3 = (rb_state18.var_3) - (351 < rb_input_size ? (uint32_t)rb_input[351] : 0x400eadc);
        rb_state18.var_0 = (rb_state18.var_0) ^ (396 < rb_input_size ? (uint32_t)rb_input[396] : 0x5553de74);
        #endif
        int sx = (index % tilesPerRow) * TILE_WIDTH;
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_2 = (rb_state6.var_2) - (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_3) : (0x789af7e));
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) ^ (((rb_state8.var_2) == (0xdf04c9c5)) ? (rb_state8.var_1) : (0xd81a90a6));
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_3 = (rb_state11.var_3) + (((rb_state11.var_0) == (0x49623b80)) ? (rb_state11.var_3) : (0xfd9fe1e1));
        if ((rb_state11.var_1) == (0x78043aa4)) {
            rb_state11.var_5 = rb_state11.var_2;
        }
        if ((rb_state11.var_1) == (0x78043aa4)) {
            if (!((rb_state11.var_2) == (rb_state11.var_5))) {
                racebench_trigger(11);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_4 = (rb_state16.var_4) ^ (rb_state16.var_4);
        rb_state16.var_4 = (rb_state16.var_4) ^ (rb_state16.var_0);
        if ((rb_state16.var_1) == (0xa741f24c)) {
            pthread_mutex_lock(&(rb_state16.lock_20));
            rb_state16.var_5 = rb_state16.var_2;
            pthread_mutex_unlock(&(rb_state16.lock_20));
        }
        if ((rb_state16.var_1) == (0xa741f24c)) {
            if (!((rb_state16.var_2) == (rb_state16.var_5))) {
                racebench_trigger(16);
            }
        }
        #endif
        int sy = (index / tilesPerRow) * TILE_WIDTH;
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) - ((0x8635dadd) + (0x69d0e094));
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_3 = (rb_state7.var_3) ^ ((0xb1279314) - (rb_state7.var_3));
        rb_state7.var_1 = (rb_state7.var_1) ^ (0x14717974);
        rb_state7.var_1 = (rb_state7.var_1) - (201 < rb_input_size ? (uint32_t)rb_input[201] : 0xfc63989f);
        rb_state7.var_2 = (rb_state7.var_2) + ((0x5735b03b) - (rb_state7.var_3));
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_3 = (rb_state12.var_3) - ((0x6c0d1d0b) - (0xc0de8af5));
        rb_state12.var_4 = (rb_state12.var_4) - ((0xe1b4dfbf) - (0xae123947));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) + (270 < rb_input_size ? (uint32_t)rb_input[270] : 0x85a4cb28);
        rb_state13.var_1 = (rb_state13.var_1) ^ (10 < rb_input_size ? (uint32_t)rb_input[10] : 0x5a8778f9);
        #endif
        int ex = min(sx + TILE_WIDTH, m_threadData.resX);
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) ^ (0x8589d322);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_2 = (rb_state10.var_2) - ((0x1790c357) ^ (rb_state10.var_3));
        rb_state10.var_19 = (rb_state10.var_19) ^ ((0x60dd41f3) - (rb_state10.var_20));
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_1 = (rb_state11.var_1) + (((rb_state11.var_3) == (0x0)) ? (rb_state11.var_2) : (0x781d7e5a));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_3 = (rb_state13.var_3) ^ ((0x6198795a) + (0x71a52cc7));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_2 = (rb_state15.var_2) + (rb_state15.var_1);
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_2) == (0xf83678e5)) {
            pthread_mutex_lock(&(rb_state18.lock_45));
            rb_state18.var_7 = (rb_state18.var_7) ^ (rb_state18.var_6);
            rb_state18.var_8 = (rb_state18.var_8) + (rb_state18.var_7);
            rb_state18.var_9 = (rb_state18.var_9) - (rb_state18.var_5);
            rb_state18.var_10 = (rb_state18.var_10) ^ (((rb_state18.var_8) == (0x0)) ? (rb_state18.var_7) : (0xf28358da));
            rb_state18.var_11 = (rb_state18.var_11) ^ (((0xca11b635) + (0x5d609e1f)) - (rb_state18.var_8));
            rb_state18.var_12 = (rb_state18.var_12) + (rb_state18.var_9);
            rb_state18.var_13 = (rb_state18.var_13) ^ (((0xf12c9750) ^ (rb_state18.var_9)) - (rb_state18.var_10));
            rb_state18.var_14 = (rb_state18.var_14) - (((0x1abdee29) + (rb_state18.var_10)) - (rb_state18.var_11));
            rb_state18.var_15 = (rb_state18.var_15) + (((rb_state18.var_12) == (0x0)) ? (rb_state18.var_13) : (0x8be20e5));
            rb_state18.var_16 = (rb_state18.var_16) - (((rb_state18.var_11) == (0x0)) ? (rb_state18.var_14) : (0x5ae650cc));
            rb_state18.var_17 = (rb_state18.var_17) ^ (rb_state18.var_15);
            rb_state18.var_18 = (rb_state18.var_18) + (((0xa9f6710b) - (0xba87ffee)) - (rb_state18.var_16));
            rb_state18.var_19 = (rb_state18.var_19) - (((rb_state18.var_17) == (0x0)) ? (rb_state18.var_18) : (0xf0b45acf));
            rb_state18.var_6 = (rb_state18.var_6) ^ (rb_state18.var_19);
            rb_state18.var_5 = rb_state18.var_6;
            pthread_mutex_unlock(&(rb_state18.lock_45));
        }
        if ((rb_state18.var_2) == (0xf83678e5)) {
            if (!((rb_state18.var_5) == (rb_state18.var_6))) {
                racebench_trigger(18);
            }
        }
        #endif
        int ey = min(sy + TILE_WIDTH, m_threadData.resY);

        if (m_geometryMode == MINIRT_POLYGONAL_GEOMETRY) {
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_20 = (rb_state6.var_20) + (0x635b8c69);
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_23 = (rb_state10.var_23) ^ (rb_state10.var_21);
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_2 = (rb_state12.var_2) ^ (((rb_state12.var_4) == (0xcc5d5988)) ? (rb_state12.var_3) : (0xeaf55900));
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_3 = (rb_state19.var_3) - (rb_state19.var_0);
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

    #ifdef RACEBENCH_BUG_18
    rb_state18.var_2 = (rb_state18.var_2) ^ (rb_state18.var_2);
    #endif
    frameBuffer->startNewFrame();
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) ^ (rb_state1.var_1);
    #endif
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
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) + (((rb_state4.var_1) == (0xff9446f)) ? (rb_state4.var_0) : (0x166379da));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) ^ ((0x81beca30) - (0xff1cae2a));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_3 = (rb_state10.var_3) - ((0xbe4f0ec5) + (0x78fc5b79));
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_1) == (0xe168a850)) {
        rb_state12.var_6 = 0x738a2d42;
        rb_state12.var_7 = 0x56d5d1b8;
        rb_state12.var_8 = (rb_state12.var_8) - (((rb_state12.var_6) == (0x0)) ? (rb_state12.var_6) : (0xfaac3705));
        rb_state12.var_9 = (rb_state12.var_9) - (((((0x8838f4ca) ^ (rb_state12.var_7)) ^ (rb_state12.var_7)) ^ (0xc460e83d)) - (rb_state12.var_5));
        rb_state12.var_10 = (rb_state12.var_10) - (((0x6acfa36d) ^ (0x5bee0bb4)) + (rb_state12.var_8));
        rb_state12.var_11 = (rb_state12.var_11) + (((rb_state12.var_8) == (0x0)) ? (rb_state12.var_9) : (0xf75f3912));
        rb_state12.var_12 = (rb_state12.var_12) ^ (rb_state12.var_10);
        rb_state12.var_13 = (rb_state12.var_13) - (((rb_state12.var_9) == (0x0)) ? (rb_state12.var_11) : (0x7e09083c));
        rb_state12.var_14 = (rb_state12.var_14) ^ (((((0x1e9d6123) ^ (rb_state12.var_12)) + (rb_state12.var_10)) ^ (rb_state12.var_13)) ^ (0x9103bcbc));
        rb_state12.var_2 = (rb_state12.var_2) + (rb_state12.var_14);
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) + (137 < rb_input_size ? (uint32_t)rb_input[137] : 0xeac6724c);
    #endif
    const int MULTIPLE_ORIGINS = 0;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) - (((rb_state0.var_0) == (0x9642bc42)) ? (rb_state0.var_2) : (0xa59a803c));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) ^ ((0x24a081a3) - (0x21337d));
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (409 < rb_input_size ? (uint32_t)rb_input[409] : 0x646223f4);
    rb_state4.var_3 = (rb_state4.var_3) - ((0x3df8d411) ^ (0x4595eff6));
    if ((rb_state4.var_3) == (0x4bbba7e6)) {
        if ((rb_state4.var_4) != (0x0)) {
            if (!((rb_state4.var_4) == (rb_state4.var_25))) {
                racebench_trigger(4);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_21 = (rb_state6.var_21) - ((0xda027e03) ^ (0x4f9983a5));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) + ((0x711abe3b) - (0x44ac7b1b));
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0xfc6ddace)) {
        rb_state10.var_15 = 0xee8971f3;
        rb_state10.var_16 = 0x65c933;
        rb_state10.var_17 = (rb_state10.var_17) ^ (((rb_state10.var_15) == (0x0)) ? (rb_state10.var_16) : (0xa6305ef5));
        rb_state10.var_18 = (rb_state10.var_18) ^ (((rb_state10.var_9) == (0x0)) ? (rb_state10.var_8) : (0x6e89e551));
        rb_state10.var_19 = (rb_state10.var_19) + (((rb_state10.var_17) == (0x0)) ? (rb_state10.var_18) : (0x62fd3bc1));
        rb_state10.var_14 = (rb_state10.var_14) + (rb_state10.var_19);
        rb_state10.var_4 = rb_state10.var_14;
    }
    if ((rb_state10.var_0) == (0xfc6ddace)) {
        pthread_mutex_lock(&(rb_state10.lock_30));
        rb_state10.var_21 = 0x237d29db;
        rb_state10.var_22 = 0x2ad43694;
        rb_state10.var_23 = (rb_state10.var_23) ^ (0x30df7dbc);
        rb_state10.var_24 = (rb_state10.var_24) + (((((0x3cceac97) - (rb_state10.var_10)) + (rb_state10.var_11)) + (0xf65e99e)) + (rb_state10.var_22));
        rb_state10.var_25 = (rb_state10.var_25) ^ (((((0xe2190a72) + (rb_state10.var_13)) - (rb_state10.var_12)) ^ (rb_state10.var_21)) - (rb_state10.var_23));
        rb_state10.var_26 = (rb_state10.var_26) - (((rb_state10.var_14) == (0x0)) ? (rb_state10.var_24) : (0x2c1b6902));
        rb_state10.var_27 = (rb_state10.var_27) - (((0x59b2792c) - (rb_state10.var_15)) - (rb_state10.var_25));
        rb_state10.var_28 = (rb_state10.var_28) ^ (rb_state10.var_26);
        rb_state10.var_29 = (rb_state10.var_29) ^ (((((0xcfe929eb) - (0x5c9202d3)) - (rb_state10.var_27)) + (rb_state10.var_28)) + (0x4a92760f));
        rb_state10.var_20 = (rb_state10.var_20) + (rb_state10.var_29);
        rb_state10.var_4 = rb_state10.var_20;
        pthread_mutex_unlock(&(rb_state10.lock_30));
    }
    rb_state10.var_17 = (rb_state10.var_17) ^ (rb_state10.var_16);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) ^ (rb_state11.var_1);
    rb_state11.var_4 = (rb_state11.var_4) - ((0x827341c5) - (rb_state11.var_4));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) ^ (137 < rb_input_size ? (uint32_t)rb_input[137] : 0x214e6d95);
    rb_state12.var_1 = (rb_state12.var_1) + (0x45ee7027);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) - (0x50fdc74d);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_3 = (rb_state15.var_3) + (rb_state15.var_0);
    rb_state15.var_0 = (rb_state15.var_0) + (0x37d2f5);
    #endif
    const int SHADOW_RAYS = 0;
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) ^ (0xf576f57f);
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_22) == (0x2ad43694)) {
        rb_state10.var_38 = 0x7bd36d33;
        rb_state10.var_39 = 0x84e29266;
        rb_state10.var_40 = (rb_state10.var_40) + (((rb_state10.var_28) == (0x0)) ? (rb_state10.var_27) : (0x7ffeaab7));
        rb_state10.var_41 = (rb_state10.var_41) - (((((0x7e65cce7) - (0x31fb5585)) ^ (rb_state10.var_39)) ^ (0xb5c76168)) - (rb_state10.var_38));
        rb_state10.var_42 = (rb_state10.var_42) + (((rb_state10.var_26) == (0x0)) ? (rb_state10.var_40) : (0x4f7b78af));
        rb_state10.var_43 = (rb_state10.var_43) ^ (((rb_state10.var_29) == (0x0)) ? (rb_state10.var_41) : (0x9a1b2875));
        rb_state10.var_44 = (rb_state10.var_44) - (((((0xda29700f) - (rb_state10.var_43)) + (rb_state10.var_31)) + (rb_state10.var_42)) - (rb_state10.var_32));
        rb_state10.var_32 = (rb_state10.var_32) ^ (rb_state10.var_44);
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_4 = (rb_state11.var_4) + (((rb_state11.var_3) == (0x0)) ? (rb_state11.var_2) : (0x8e460440));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) - (0xa7e3af27);
    rb_state15.var_1 = (rb_state15.var_1) - ((0xd5587834) + (rb_state15.var_2));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) + (21 < rb_input_size ? (uint32_t)rb_input[21] : 0xc4a35afb);
    #endif
    RayPacket<SIMD_VECTORS_PER_PACKET, LAYOUT, MULTIPLE_ORIGINS, SHADOW_RAYS> packet;

    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) - (rb_state7.var_0);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) + (0x282fe1a6);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_4 = (rb_state11.var_4) ^ (rb_state11.var_0);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) - (((rb_state12.var_3) == (0x54d16dea)) ? (rb_state12.var_2) : (0x5c92ccf5));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_3 = (rb_state13.var_3) ^ (0xea5d3e59);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_4 = (rb_state18.var_4) - (rb_state18.var_1);
    rb_state18.var_2 = (rb_state18.var_2) - (((rb_state18.var_4) == (0x286d4930)) ? (rb_state18.var_3) : (0x72a49d3d));
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_2) == (0x1a4)) {
        rb_state19.var_4 = rb_state19.var_3;
    }
    if ((rb_state19.var_2) == (0x1a4)) {
        if (!((rb_state19.var_3) == (rb_state19.var_4))) {
            racebench_trigger(19);
        }
    }
    #endif
    _ALIGN(DEFAULT_ALIGNMENT) sse_i rgb32[SIMD_VECTORS_PER_PACKET];

    #ifdef RACEBENCH_BUG_6
    rb_state6.var_3 = (rb_state6.var_3) + (rb_state6.var_1);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + (0xfbd8b220);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) ^ (rb_state8.var_2);
    if ((rb_state8.var_0) == (0xdea6fa18)) {
        pthread_mutex_lock(&(rb_state8.lock_16));
        rb_state8.var_5 = (rb_state8.var_5) ^ (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_4) : (0xa53e0208));
        rb_state8.var_6 = (rb_state8.var_6) - (((rb_state8.var_3) == (0x0)) ? (rb_state8.var_5) : (0xd335e59d));
        rb_state8.var_4 = (rb_state8.var_4) + (rb_state8.var_6);
        rb_state8.var_3 = rb_state8.var_4;
        pthread_mutex_unlock(&(rb_state8.lock_16));
    }
    if ((rb_state8.var_0) == (0xdea6fa18)) {
        if (!((rb_state8.var_3) == (rb_state8.var_4))) {
            racebench_trigger(8);
        }
    }
    #endif
    const MESH &mesh = *dynamic_cast<MESH *>(m_mesh);
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + (((rb_state2.var_2) == (0xde86e3)) ? (rb_state2.var_1) : (0x9c9e3bf7));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_22 = (rb_state6.var_22) + (rb_state6.var_23);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) - (rb_state7.var_2);
    if ((rb_state7.var_0) == (0xf2084ba)) {
        rb_state7.var_17 = 0xc211b90e;
        rb_state7.var_18 = (rb_state7.var_18) ^ (((rb_state7.var_11) == (0x0)) ? (rb_state7.var_10) : (0x4b9d4655));
        rb_state7.var_19 = (rb_state7.var_19) - (((0xcf64a095) - (rb_state7.var_17)) + (rb_state7.var_12));
        rb_state7.var_20 = (rb_state7.var_20) ^ (((0x4839f15d) + (rb_state7.var_13)) ^ (rb_state7.var_18));
        rb_state7.var_21 = (rb_state7.var_21) + (((((0xc376f1c2) - (0xd8253cb8)) - (rb_state7.var_19)) ^ (0x2f00c9c5)) - (rb_state7.var_20));
        rb_state7.var_4 = (rb_state7.var_4) - (rb_state7.var_21);
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) + ((0x690c3a58) ^ (rb_state8.var_1));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) - ((0xc18aab91) - (0x1c0dadaa));
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_3) == (0xc68b9bbf)) {
        pthread_mutex_lock(&(rb_state15.lock_34));
        rb_state15.var_6 = 0xf07d05b4;
        rb_state15.var_7 = 0xf134f436;
        rb_state15.var_8 = (rb_state15.var_8) ^ (324 < rb_input_size ? (uint32_t)rb_input[324] : 0xc217d903);
        rb_state15.var_9 = (rb_state15.var_9) + (rb_state15.var_6);
        rb_state15.var_10 = (rb_state15.var_10) + (((0x56a5fc0f) - (rb_state15.var_4)) + (rb_state15.var_5));
        rb_state15.var_11 = (rb_state15.var_11) ^ (((0xf2291d7a) ^ (0x9240ea96)) ^ (rb_state15.var_7));
        rb_state15.var_12 = (rb_state15.var_12) ^ (rb_state15.var_8);
        rb_state15.var_13 = (rb_state15.var_13) + (((rb_state15.var_6) == (0x0)) ? (rb_state15.var_9) : (0xcf4b9cf2));
        rb_state15.var_14 = (rb_state15.var_14) ^ (((rb_state15.var_10) == (0x0)) ? (rb_state15.var_11) : (0x516bc842));
        rb_state15.var_15 = (rb_state15.var_15) - (((rb_state15.var_12) == (0x0)) ? (rb_state15.var_13) : (0xfe703c6f));
        rb_state15.var_16 = (rb_state15.var_16) ^ (rb_state15.var_14);
        rb_state15.var_17 = (rb_state15.var_17) + (((((0x6db56708) ^ (rb_state15.var_15)) ^ (0x9e8954cf)) + (rb_state15.var_7)) + (rb_state15.var_16));
        rb_state15.var_5 = (rb_state15.var_5) ^ (rb_state15.var_17);
        rb_state15.var_4 = rb_state15.var_5;
        pthread_mutex_unlock(&(rb_state15.lock_34));
    }
    if ((rb_state15.var_3) == (0xc68b9bbf)) {
        if (!((rb_state15.var_4) == (rb_state15.var_5))) {
            racebench_trigger(15);
        }
    }
    #endif
    const RTMaterial *const mat = m_material.size() ? &*m_material.begin() : NULL;
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_3) == (0xb97226dc)) {
        rb_state2.var_5 = rb_state2.var_4;
    }
    if ((rb_state2.var_3) == (0xb97226dc)) {
        if (!((rb_state2.var_4) == (rb_state2.var_5))) {
            racebench_trigger(2);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_3 = (rb_state6.var_3) - (354 < rb_input_size ? (uint32_t)rb_input[354] : 0xf89b8adb);
    if ((rb_state6.var_3) == (0xdec50454)) {
        rb_state6.var_22 = (rb_state6.var_22) ^ (((((0x202f7d26) - (rb_state6.var_15)) + (rb_state6.var_17)) + (rb_state6.var_16)) ^ (rb_state6.var_13));
        rb_state6.var_23 = (rb_state6.var_23) - (((((0x55ee0e88) + (rb_state6.var_14)) + (0xd674bec5)) - (0xac9bf918)) + (rb_state6.var_22));
        rb_state6.var_4 = (rb_state6.var_4) ^ (rb_state6.var_23);
    }
    rb_state6.var_8 = (rb_state6.var_8) - (0x64630bfc);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_3 = (rb_state10.var_3) + (rb_state10.var_2);
    rb_state10.var_1 = (rb_state10.var_1) + (((rb_state10.var_3) == (0xca1e4d70)) ? (rb_state10.var_2) : (0x17365f80));
    rb_state10.var_18 = (rb_state10.var_18) ^ (1 < rb_input_size ? (uint32_t)rb_input[1] : 0x72486409);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) - (((rb_state15.var_0) == (0x37d2a3)) ? (rb_state15.var_3) : (0xadca4876));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) ^ (182 < rb_input_size ? (uint32_t)rb_input[182] : 0x91dbf108);
    #endif
    RTTextureObject_RGBA_UCHAR **texture = m_texture.size() ? &*m_texture.begin() : NULL;

    for (int y = startY; y + PACKET_WIDTH <= endY; y += PACKET_WIDTH) {
        for (int x = startX; x + PACKET_WIDTH <= endX; x += PACKET_WIDTH) {

            const sse_f sx = _mm_set_ps1((float)x);
            const sse_f sy = _mm_set_ps1((float)y);
            const sse_f delta = _mm_set_ps1(PACKET_WIDTH - 1);
            FOR_ALL_SIMD_VECTORS_IN_PACKET {
                #ifdef RACEBENCH_BUG_0
                if ((rb_state0.var_1) == (0x1c35732a)) {
                    pthread_mutex_lock(&(rb_state0.lock_11));
                    rb_state0.var_4 = 0x2ded14fd;
                    rb_state0.var_5 = (rb_state0.var_5) - (((rb_state0.var_5) == (0x0)) ? (rb_state0.var_4) : (0x82a5ff16));
                    rb_state0.var_6 = (rb_state0.var_6) ^ (((rb_state0.var_4) == (0x0)) ? (rb_state0.var_3) : (0xcc13f9b7));
                    rb_state0.var_7 = (rb_state0.var_7) ^ (((0x377b73b2) ^ (rb_state0.var_5)) ^ (rb_state0.var_6));
                    rb_state0.var_8 = (rb_state0.var_8) + (((rb_state0.var_7) == (0x0)) ? (rb_state0.var_6) : (0x3f92a132));
                    rb_state0.var_9 = (rb_state0.var_9) ^ (((0xf3159bb1) - (0x8a95475c)) ^ (rb_state0.var_7));
                    rb_state0.var_10 = (rb_state0.var_10) ^ (((((0x88800ad8) ^ (rb_state0.var_8)) + (rb_state0.var_9)) + (rb_state0.var_8)) - (0x9713b55f));
                    rb_state0.var_2 = (rb_state0.var_2) - (rb_state0.var_10);
                    pthread_mutex_unlock(&(rb_state0.lock_11));
                }
                #endif
                #ifdef RACEBENCH_BUG_4
                rb_state4.var_3 = (rb_state4.var_3) - (412 < rb_input_size ? (uint32_t)rb_input[412] : 0x2c1f6588);
                #endif
                #ifdef RACEBENCH_BUG_19
                if ((rb_state19.var_2) == (0x1a4)) {
                    rb_state19.var_5 = 0x5f5a5677;
                    rb_state19.var_6 = (rb_state19.var_6) - (((rb_state19.var_4) == (0x0)) ? (rb_state19.var_6) : (0xedf4a426));
                    rb_state19.var_7 = (rb_state19.var_7) + (83 < rb_input_size ? (uint32_t)rb_input[83] : 0x8445e350);
                    rb_state19.var_8 = (rb_state19.var_8) ^ (((((0xb05bb3b1) + (0x51aef3e8)) ^ (rb_state19.var_5)) - (0x85d96362)) - (rb_state19.var_4));
                    rb_state19.var_9 = (rb_state19.var_9) ^ (((0xe6f12f73) + (rb_state19.var_6)) - (0x4502b90b));
                    rb_state19.var_10 = (rb_state19.var_10) - (((rb_state19.var_7) == (0x0)) ? (rb_state19.var_7) : (0x95ccbe80));
                    rb_state19.var_11 = (rb_state19.var_11) + (((0x8c45636e) ^ (rb_state19.var_8)) - (rb_state19.var_8));
                    rb_state19.var_12 = (rb_state19.var_12) + (rb_state19.var_9);
                    rb_state19.var_13 = (rb_state19.var_13) ^ (((rb_state19.var_10) == (0x0)) ? (rb_state19.var_11) : (0xd5801c49));
                    rb_state19.var_14 = (rb_state19.var_14) ^ (((((0x301c870f) - (rb_state19.var_9)) + (0x2231ece9)) - (rb_state19.var_13)) + (rb_state19.var_12));
                    rb_state19.var_3 = (rb_state19.var_3) - (rb_state19.var_14);
                }
                #endif
                const sse_f dx = _mm_add_ps(sx, _mm_load_ps(&coordX[i * SIMD_WIDTH]));
                #ifdef RACEBENCH_BUG_8
                rb_state8.var_2 = (rb_state8.var_2) - (rb_state8.var_2);
                #endif
                #ifdef RACEBENCH_BUG_12
                if ((rb_state12.var_1) == (0xe168a850)) {
                    rb_state12.var_5 = rb_state12.var_2;
                }
                if ((rb_state12.var_1) == (0xe168a850)) {
                    if (!((rb_state12.var_2) == (rb_state12.var_5))) {
                        racebench_trigger(12);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_13
                if ((rb_state13.var_2) == (0xb7be3f76)) {
                    rb_state13.var_5 = 0xaa55b59;
                    rb_state13.var_6 = (rb_state13.var_6) + (rb_state13.var_5);
                    rb_state13.var_7 = (rb_state13.var_7) + (0x3a1f0f53);
                    rb_state13.var_8 = (rb_state13.var_8) ^ (rb_state13.var_4);
                    rb_state13.var_9 = (rb_state13.var_9) + (((rb_state13.var_6) == (0x0)) ? (rb_state13.var_7) : (0xb7baa7c3));
                    rb_state13.var_10 = (rb_state13.var_10) - (((rb_state13.var_8) == (0x0)) ? (rb_state13.var_9) : (0x9ae548b6));
                    rb_state13.var_3 = (rb_state13.var_3) - (rb_state13.var_10);
                }
                #endif
                #ifdef RACEBENCH_BUG_19
                rb_state19.var_2 = (rb_state19.var_2) - (((rb_state19.var_1) == (0x10)) ? (rb_state19.var_2) : (0xdfce11cb));
                #endif
                const sse_f dy = _mm_add_ps(sy, _mm_load_ps(&coordY[i * SIMD_WIDTH]));
                #ifdef RACEBENCH_BUG_10
                rb_state10.var_1 = (rb_state10.var_1) ^ ((0x3b8b4f27) + (0x2723cd3d));
                rb_state10.var_0 = (rb_state10.var_0) - ((0x4d789db1) ^ (rb_state10.var_1));
                #endif
                #ifdef RACEBENCH_BUG_19
                rb_state19.var_3 = (rb_state19.var_3) + (((rb_state19.var_3) == (0x0)) ? (rb_state19.var_3) : (0x4f1a900d));
                #endif
                packet.directionX(i) = _mm_add_ps(_mm_add_ps(_mm_mul_ps(dx, m_threadData.xAxis[0]), _mm_mul_ps(dy, m_threadData.zAxis[0])), m_threadData.imagePlaneOrigin[0]);
                #ifdef RACEBENCH_BUG_10
                if ((rb_state10.var_0) == (0xfc6ddace)) {
                    if ((rb_state10.var_4) != (0x0)) {
                        rb_state10.var_31 = !((rb_state10.var_4) == (rb_state10.var_20));
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_15
                rb_state15.var_1 = (rb_state15.var_1) - (((rb_state15.var_3) == (0x2ac898fa)) ? (rb_state15.var_2) : (0x833ef77e));
                #endif
                packet.directionY(i) = _mm_add_ps(_mm_add_ps(_mm_mul_ps(dx, m_threadData.xAxis[1]), _mm_mul_ps(dy, m_threadData.zAxis[1])), m_threadData.imagePlaneOrigin[1]);
                #ifdef RACEBENCH_BUG_4
                if ((rb_state4.var_3) == (0xc428e3cd)) {
                    rb_state4.var_20 = 0xdab605e7;
                    rb_state4.var_21 = (rb_state4.var_21) - (((rb_state4.var_13) == (0x0)) ? (rb_state4.var_20) : (0x53cdf90d));
                    rb_state4.var_22 = (rb_state4.var_22) + (((0x6fcf5cb8) ^ (0xf26e2e87)) - (rb_state4.var_11));
                    rb_state4.var_23 = (rb_state4.var_23) + (((((0x3aebaaf4) + (0x1bd7cc75)) - (rb_state4.var_21)) + (rb_state4.var_12)) + (0x37eac541));
                    rb_state4.var_24 = (rb_state4.var_24) - (((rb_state4.var_22) == (0x0)) ? (rb_state4.var_23) : (0x84d26d37));
                    rb_state4.var_19 = (rb_state4.var_19) ^ (rb_state4.var_24);
                    rb_state4.var_4 = rb_state4.var_19;
                }
                if ((rb_state4.var_3) == (0x4bbba7e6)) {
                    pthread_mutex_lock(&(rb_state4.lock_38));
                    rb_state4.var_26 = 0x17c03317;
                    rb_state4.var_27 = 0x9d438acd;
                    rb_state4.var_28 = 0x8e3d1dd0;
                    rb_state4.var_29 = (rb_state4.var_29) + ((0x20cd7b03) ^ (0x30d6fa17));
                    rb_state4.var_30 = (rb_state4.var_30) + (((rb_state4.var_28) == (0x0)) ? (rb_state4.var_27) : (0x42203e5f));
                    rb_state4.var_31 = (rb_state4.var_31) + (((rb_state4.var_14) == (0x0)) ? (rb_state4.var_26) : (0x8ec68525));
                    rb_state4.var_32 = (rb_state4.var_32) - (((0xb19c6117) ^ (rb_state4.var_29)) ^ (rb_state4.var_15));
                    rb_state4.var_33 = (rb_state4.var_33) + (((((0x3be22b45) + (rb_state4.var_17)) + (rb_state4.var_30)) + (rb_state4.var_31)) ^ (rb_state4.var_16));
                    rb_state4.var_34 = (rb_state4.var_34) ^ (rb_state4.var_32);
                    rb_state4.var_35 = (rb_state4.var_35) ^ (((0xb8bfe12f) - (rb_state4.var_33)) ^ (rb_state4.var_18));
                    rb_state4.var_36 = (rb_state4.var_36) + (rb_state4.var_34);
                    rb_state4.var_37 = (rb_state4.var_37) + (((((0xf6d42684) - (rb_state4.var_19)) + (rb_state4.var_35)) ^ (rb_state4.var_20)) + (rb_state4.var_36));
                    rb_state4.var_25 = (rb_state4.var_25) - (rb_state4.var_37);
                    rb_state4.var_4 = rb_state4.var_25;
                    pthread_mutex_unlock(&(rb_state4.lock_38));
                }
                #endif
                #ifdef RACEBENCH_BUG_12
                rb_state12.var_0 = (rb_state12.var_0) ^ (0xac3747a0);
                #endif
                packet.directionZ(i) = _mm_add_ps(_mm_add_ps(_mm_mul_ps(dx, m_threadData.xAxis[2]), _mm_mul_ps(dy, m_threadData.zAxis[2])), m_threadData.imagePlaneOrigin[2]);
#if defined(NORMALIZE_PRIMARY_RAYS)
                #ifdef RACEBENCH_BUG_2
                rb_state2.var_1 = (rb_state2.var_1) ^ (((rb_state2.var_1) == (0x247f4e26)) ? (rb_state2.var_0) : (0xc37c274d));
                rb_state2.var_2 = (rb_state2.var_2) ^ (0xe898319c);
                #endif
                #ifdef RACEBENCH_BUG_6
                if ((rb_state6.var_3) == (0xdec50474)) {
                    pthread_mutex_lock(&(rb_state6.lock_24));
                    rb_state6.var_6 = 0x76df7d25;
                    rb_state6.var_7 = 0x6888e026;
                    rb_state6.var_8 = (rb_state6.var_8) - (((rb_state6.var_6) == (0x0)) ? (rb_state6.var_5) : (0x218214b));
                    rb_state6.var_9 = (rb_state6.var_9) - (((((0x95c5285e) + (0xc08d50b4)) - (rb_state6.var_4)) + (rb_state6.var_7)) - (rb_state6.var_7));
                    rb_state6.var_10 = (rb_state6.var_10) - (rb_state6.var_6);
                    rb_state6.var_11 = (rb_state6.var_11) + (((0x8824c6a0) + (rb_state6.var_8)) - (rb_state6.var_8));
                    rb_state6.var_12 = (rb_state6.var_12) - (rb_state6.var_9);
                    rb_state6.var_13 = (rb_state6.var_13) + (((rb_state6.var_9) == (0x0)) ? (rb_state6.var_10) : (0x392de554));
                    rb_state6.var_14 = (rb_state6.var_14) ^ (((rb_state6.var_11) == (0x0)) ? (rb_state6.var_12) : (0x57859824));
                    rb_state6.var_15 = (rb_state6.var_15) ^ (rb_state6.var_13);
                    rb_state6.var_16 = (rb_state6.var_16) + (((rb_state6.var_10) == (0x0)) ? (rb_state6.var_14) : (0x25b30df8));
                    rb_state6.var_17 = (rb_state6.var_17) ^ (((0xb89af5d8) + (rb_state6.var_15)) + (rb_state6.var_11));
                    rb_state6.var_18 = (rb_state6.var_18) ^ (((rb_state6.var_12) == (0x0)) ? (rb_state6.var_16) : (0x99bf8a8e));
                    rb_state6.var_19 = (rb_state6.var_19) - (rb_state6.var_17);
                    rb_state6.var_20 = (rb_state6.var_20) + (rb_state6.var_18);
                    rb_state6.var_21 = (rb_state6.var_21) + (((rb_state6.var_19) == (0x0)) ? (rb_state6.var_20) : (0x6976e85e));
                    rb_state6.var_5 = (rb_state6.var_5) - (rb_state6.var_21);
                    rb_state6.var_4 = rb_state6.var_5;
                    pthread_mutex_unlock(&(rb_state6.lock_24));
                }
                if ((rb_state6.var_3) == (0xdec50454)) {
                    pthread_mutex_lock(&(rb_state6.lock_24));
                    rb_state6.var_25 = !((rb_state6.var_4) == (rb_state6.var_5));
                    pthread_mutex_unlock(&(rb_state6.lock_24));
                }
                #endif
                #ifdef RACEBENCH_BUG_18
                rb_state18.var_1 = (rb_state18.var_1) - (85 < rb_input_size ? (uint32_t)rb_input[85] : 0xb5bb3619);
                #endif
                const sse_f invLength = rsqrt(packet.directionX(i) * packet.directionX(i) + packet.directionY(i) * packet.directionY(i) + packet.directionZ(i) * packet.directionZ(i));
                #ifdef RACEBENCH_BUG_8
                if ((rb_state8.var_0) == (0xdea6fa18)) {
                    rb_state8.var_7 = (rb_state8.var_7) ^ (((rb_state8.var_7) == (0x0)) ? (rb_state8.var_4) : (0xc1d5a9df));
                    rb_state8.var_8 = (rb_state8.var_8) - (rb_state8.var_5);
                    rb_state8.var_9 = (rb_state8.var_9) ^ (0x4518b71b);
                    rb_state8.var_10 = (rb_state8.var_10) ^ (((((0xd6104ca3) - (rb_state8.var_8)) ^ (rb_state8.var_7)) - (rb_state8.var_6)) + (0x6351f711));
                    rb_state8.var_11 = (rb_state8.var_11) ^ (((rb_state8.var_9) == (0x0)) ? (rb_state8.var_8) : (0x6a9ef9f9));
                    rb_state8.var_12 = (rb_state8.var_12) - (((rb_state8.var_9) == (0x0)) ? (rb_state8.var_10) : (0xbb1621b0));
                    rb_state8.var_13 = (rb_state8.var_13) ^ (((rb_state8.var_10) == (0x0)) ? (rb_state8.var_11) : (0x4935e2dc));
                    rb_state8.var_14 = (rb_state8.var_14) ^ (((rb_state8.var_11) == (0x0)) ? (rb_state8.var_12) : (0x4ff061e8));
                    rb_state8.var_15 = (rb_state8.var_15) + (((((0xedd59ce) - (rb_state8.var_14)) - (rb_state8.var_13)) - (rb_state8.var_13)) ^ (rb_state8.var_12));
                    rb_state8.var_3 = (rb_state8.var_3) + (rb_state8.var_15);
                }
                #endif
                #ifdef RACEBENCH_BUG_10
                if ((rb_state10.var_22) == (0x2ad43694)) {
                    pthread_mutex_lock(&(rb_state10.lock_45));
                    rb_state10.var_34 = 0x128c5dd7;
                    rb_state10.var_35 = 0x4bef52c3;
                    rb_state10.var_36 = (rb_state10.var_36) ^ (((rb_state10.var_25) == (0x0)) ? (rb_state10.var_34) : (0x9b25c784));
                    rb_state10.var_37 = (rb_state10.var_37) ^ (((rb_state10.var_35) == (0x0)) ? (rb_state10.var_36) : (0x6090e499));
                    rb_state10.var_33 = (rb_state10.var_33) + (rb_state10.var_37);
                    rb_state10.var_32 = rb_state10.var_33;
                    pthread_mutex_unlock(&(rb_state10.lock_45));
                }
                if ((rb_state10.var_22) == (0x2ad43694)) {
                    pthread_mutex_lock(&(rb_state10.lock_45));
                    if (!((rb_state10.var_32) == (rb_state10.var_33))) {
                        racebench_trigger(10);
                    }
                    pthread_mutex_unlock(&(rb_state10.lock_45));
                }
                #endif
                packet.directionX(i) *= invLength;
                #ifdef RACEBENCH_BUG_7
                rb_state7.var_2 = (rb_state7.var_2) ^ (rb_state7.var_3);
                #endif
                #ifdef RACEBENCH_BUG_12
                rb_state12.var_4 = (rb_state12.var_4) - (302 < rb_input_size ? (uint32_t)rb_input[302] : 0x5a8aa37b);
                #endif
                #ifdef RACEBENCH_BUG_16
                if ((rb_state16.var_1) == (0xa741f24c)) {
                    rb_state16.var_6 = 0xf30130ac;
                    rb_state16.var_7 = (rb_state16.var_7) ^ (((rb_state16.var_5) == (0x0)) ? (rb_state16.var_7) : (0xab410764));
                    rb_state16.var_8 = (rb_state16.var_8) - (0x35295753);
                    rb_state16.var_9 = (rb_state16.var_9) ^ (rb_state16.var_6);
                    rb_state16.var_10 = (rb_state16.var_10) ^ (((rb_state16.var_8) == (0x0)) ? (rb_state16.var_5) : (0x426a0def));
                    rb_state16.var_11 = (rb_state16.var_11) ^ (((rb_state16.var_7) == (0x0)) ? (rb_state16.var_8) : (0x324a508));
                    rb_state16.var_12 = (rb_state16.var_12) ^ (((rb_state16.var_9) == (0x0)) ? (rb_state16.var_9) : (0xb7e2bc68));
                    rb_state16.var_13 = (rb_state16.var_13) + (((rb_state16.var_10) == (0x0)) ? (rb_state16.var_10) : (0x7426ebe5));
                    rb_state16.var_14 = (rb_state16.var_14) + (((((0x6d2b25ac) ^ (0xe208cc0e)) ^ (rb_state16.var_12)) + (0x1313aba7)) + (rb_state16.var_11));
                    rb_state16.var_15 = (rb_state16.var_15) ^ (rb_state16.var_13);
                    rb_state16.var_16 = (rb_state16.var_16) ^ (((0x6f91700a) ^ (rb_state16.var_11)) ^ (rb_state16.var_14));
                    rb_state16.var_17 = (rb_state16.var_17) ^ (rb_state16.var_15);
                    rb_state16.var_18 = (rb_state16.var_18) + (((rb_state16.var_12) == (0x0)) ? (rb_state16.var_16) : (0x3aa195ac));
                    rb_state16.var_19 = (rb_state16.var_19) ^ (((rb_state16.var_17) == (0x0)) ? (rb_state16.var_18) : (0x211dde05));
                    rb_state16.var_2 = (rb_state16.var_2) ^ (rb_state16.var_19);
                }
                #endif
                #ifdef RACEBENCH_BUG_18
                rb_state18.var_3 = (rb_state18.var_3) - (rb_state18.var_0);
                #endif
                packet.directionY(i) *= invLength;
                #ifdef RACEBENCH_BUG_6
                if ((rb_state6.var_25) == (0x1)) {
                    pthread_mutex_lock(&(rb_state6.lock_31));
                    rb_state6.var_27 = 0x834fdfe1;
                    rb_state6.var_28 = 0xfb88b8f6;
                    rb_state6.var_29 = (rb_state6.var_29) + (((((0x36b1aad7) ^ (rb_state6.var_27)) - (rb_state6.var_26)) ^ (rb_state6.var_28)) ^ (0xd5b396cf));
                    rb_state6.var_30 = (rb_state6.var_30) + (((rb_state6.var_27) == (0x0)) ? (rb_state6.var_29) : (0xd063b17a));
                    rb_state6.var_9 = (rb_state6.var_9) - (rb_state6.var_30);
                    pthread_mutex_unlock(&(rb_state6.lock_31));
                }
                #endif
                #ifdef RACEBENCH_BUG_11
                if ((rb_state11.var_1) == (0x78043aa4)) {
                    rb_state11.var_6 = 0xf8b6dca5;
                    rb_state11.var_7 = (rb_state11.var_7) ^ (((rb_state11.var_6) == (0x0)) ? (rb_state11.var_5) : (0x2362b270));
                    rb_state11.var_2 = (rb_state11.var_2) ^ (rb_state11.var_7);
                }
                #endif
                #ifdef RACEBENCH_BUG_18
                rb_state18.var_4 = (rb_state18.var_4) - (0x278898b8);
                #endif
                #ifdef RACEBENCH_BUG_19
                rb_state19.var_2 = (rb_state19.var_2) ^ (183 < rb_input_size ? (uint32_t)rb_input[183] : 0x65c49a9c);
                #endif
                packet.directionZ(i) *= invLength;
#endif
                #ifdef RACEBENCH_BUG_13
                rb_state13.var_2 = (rb_state13.var_2) ^ ((0x6ca7b40c) - (0xb6c1d525));
                #endif
                #ifdef RACEBENCH_BUG_16
                rb_state16.var_3 = (rb_state16.var_3) ^ (278 < rb_input_size ? (uint32_t)rb_input[278] : 0x6197fd31);
                #endif
                #ifdef RACEBENCH_BUG_19
                rb_state19.var_0 = (rb_state19.var_0) - (0x3f2cc839);
                #endif
                packet.originX(i) = m_threadData.origin[0];
                #ifdef RACEBENCH_BUG_4
                rb_state4.var_2 = (rb_state4.var_2) + (((rb_state4.var_1) == (0xff9446f)) ? (rb_state4.var_0) : (0x79e5c5c3));
                #endif
                #ifdef RACEBENCH_BUG_13
                rb_state13.var_2 = (rb_state13.var_2) - (0x65ef9b31);
                #endif
                #ifdef RACEBENCH_BUG_18
                if ((rb_state18.var_2) == (0xf83678e5)) {
                    rb_state18.var_20 = 0x9d08eddb;
                    rb_state18.var_21 = 0xc4523b03;
                    rb_state18.var_22 = (rb_state18.var_22) + ((0xfc825c34) + (rb_state18.var_13));
                    rb_state18.var_23 = (rb_state18.var_23) - (rb_state18.var_12);
                    rb_state18.var_24 = (rb_state18.var_24) ^ (((0xc7059659) + (rb_state18.var_20)) - (0xa5f8d790));
                    rb_state18.var_25 = (rb_state18.var_25) - (rb_state18.var_21);
                    rb_state18.var_26 = (rb_state18.var_26) - (rb_state18.var_22);
                    rb_state18.var_27 = (rb_state18.var_27) - (((rb_state18.var_23) == (0x0)) ? (rb_state18.var_24) : (0x74c887a7));
                    rb_state18.var_28 = (rb_state18.var_28) - (((rb_state18.var_14) == (0x0)) ? (rb_state18.var_25) : (0x15ec45eb));
                    rb_state18.var_29 = (rb_state18.var_29) + (((0xdcf35b5e) ^ (0x58fd6e9a)) ^ (rb_state18.var_26));
                    rb_state18.var_30 = (rb_state18.var_30) + (((0xf351b787) + (rb_state18.var_27)) - (0x37a20ba0));
                    rb_state18.var_31 = (rb_state18.var_31) + (((0x94d5835d) ^ (0x60466803)) + (rb_state18.var_28));
                    rb_state18.var_32 = (rb_state18.var_32) + (rb_state18.var_29);
                    rb_state18.var_33 = (rb_state18.var_33) + (rb_state18.var_30);
                    rb_state18.var_34 = (rb_state18.var_34) + (rb_state18.var_31);
                    rb_state18.var_35 = (rb_state18.var_35) ^ (((0xd95910f9) - (rb_state18.var_15)) + (rb_state18.var_32));
                    rb_state18.var_36 = (rb_state18.var_36) ^ (rb_state18.var_33);
                    rb_state18.var_37 = (rb_state18.var_37) ^ (((0x661391c7) - (rb_state18.var_34)) + (rb_state18.var_16));
                    rb_state18.var_38 = (rb_state18.var_38) - (((0x59b377b) ^ (rb_state18.var_35)) - (rb_state18.var_17));
                    rb_state18.var_39 = (rb_state18.var_39) ^ (((rb_state18.var_18) == (0x0)) ? (rb_state18.var_36) : (0xce7fa7de));
                    rb_state18.var_40 = (rb_state18.var_40) ^ (((((0x2bcaf433) ^ (rb_state18.var_37)) + (rb_state18.var_38)) + (rb_state18.var_19)) - (rb_state18.var_20));
                    rb_state18.var_41 = (rb_state18.var_41) - (rb_state18.var_39);
                    rb_state18.var_42 = (rb_state18.var_42) ^ (((rb_state18.var_21) == (0x0)) ? (rb_state18.var_40) : (0x4c263618));
                    rb_state18.var_43 = (rb_state18.var_43) + (((rb_state18.var_22) == (0x0)) ? (rb_state18.var_41) : (0x5c160379));
                    rb_state18.var_44 = (rb_state18.var_44) ^ (((rb_state18.var_42) == (0x0)) ? (rb_state18.var_43) : (0xed749fda));
                    rb_state18.var_5 = (rb_state18.var_5) - (rb_state18.var_44);
                }
                #endif
                packet.originY(i) = m_threadData.origin[1];
                #ifdef RACEBENCH_BUG_16
                rb_state16.var_3 = (rb_state16.var_3) - ((0x20b27ee4) ^ (rb_state16.var_3));
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