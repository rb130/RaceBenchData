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
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_18 = (rb_state3.var_18) + ((0xe7bae209) - (rb_state3.var_16));
        #endif
        m_threadData.yAxis[0] = convert(camera->m_cameraDirection[0]);
        m_threadData.yAxis[1] = convert(camera->m_cameraDirection[1]);
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
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_2 = (rb_state14.var_2) + (rb_state14.var_1);
        #endif
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
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_1) == (0xdc)) {
        pthread_mutex_lock(&(rb_state0.lock_12));
        rb_state0.var_4 = (rb_state0.var_4) ^ (((rb_state0.var_2) == (0x0)) ? (rb_state0.var_3) : (0xafa9e0c6));
        rb_state0.var_3 = (rb_state0.var_3) + (rb_state0.var_4);
        rb_state0.var_2 = rb_state0.var_3;
        pthread_mutex_unlock(&(rb_state0.lock_12));
    }
    if ((rb_state0.var_1) == (0xdc)) {
        if (!((rb_state0.var_2) == (rb_state0.var_3))) {
            racebench_trigger(0);
        }
    }
    #endif
    const int tilesPerRow = m_threadData.resX >> TILE_WIDTH_SHIFT;
    while (1) {
        int index = Context::m_tileCounter.inc();
        if (index >= m_threadData.maxTiles) {
            break;
        }

        #ifdef RACEBENCH_BUG_8
        rb_state8.var_15 = (rb_state8.var_15) + (0xdfc8b776);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_6 = (rb_state10.var_6) ^ ((0x4da12dae) - (rb_state10.var_4));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) - (((rb_state15.var_0) == (0xe8bfcbbc)) ? (rb_state15.var_2) : (0x3d37ba51));
        #endif
        int sx = (index % tilesPerRow) * TILE_WIDTH;
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) + (0xccd43841);
        rb_state10.var_0 = (rb_state10.var_0) - (0x2fc2612c);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) ^ ((0x506a5db9) + (0xbb99f7ad));
        #endif
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_2) == (0x360fc826)) {
            rb_state15.var_10 = 0xa9348432;
            rb_state15.var_11 = 0xca41d797;
            rb_state15.var_12 = (rb_state15.var_12) ^ (((0x3f4399d6) - (rb_state15.var_6)) - (rb_state15.var_10));
            rb_state15.var_13 = (rb_state15.var_13) ^ (((((0xeed4f9db) ^ (rb_state15.var_11)) ^ (rb_state15.var_5)) ^ (0x6d7d035b)) ^ (0xf89c4df8));
            rb_state15.var_14 = (rb_state15.var_14) + (rb_state15.var_12);
            rb_state15.var_15 = (rb_state15.var_15) ^ (((0x5be8de7d) ^ (rb_state15.var_7)) + (rb_state15.var_13));
            rb_state15.var_16 = (rb_state15.var_16) - (((0xb9684a61) - (rb_state15.var_8)) ^ (rb_state15.var_14));
            rb_state15.var_17 = (rb_state15.var_17) ^ (rb_state15.var_15);
            rb_state15.var_18 = (rb_state15.var_18) - (((0x6fe5d9b7) ^ (rb_state15.var_16)) + (0xbc4162db));
            rb_state15.var_19 = (rb_state15.var_19) + (((0x634b113) + (rb_state15.var_17)) ^ (rb_state15.var_9));
            rb_state15.var_20 = (rb_state15.var_20) ^ (((rb_state15.var_10) == (0x0)) ? (rb_state15.var_18) : (0x4ee98118));
            rb_state15.var_21 = (rb_state15.var_21) + (rb_state15.var_19);
            rb_state15.var_22 = (rb_state15.var_22) + (rb_state15.var_20);
            rb_state15.var_23 = (rb_state15.var_23) - (((((0x6108d38d) - (rb_state15.var_21)) - (rb_state15.var_22)) ^ (0x11508ba0)) ^ (rb_state15.var_11));
            rb_state15.var_3 = (rb_state15.var_3) + (rb_state15.var_23);
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        if ((rb_state19.var_18) == (0x1)) {
            if ((rb_state19.var_19) != (0x0)) {
                if (!((rb_state19.var_19) == (rb_state19.var_46))) {
                    racebench_trigger(19);
                }
            }
        }
        #endif
        int sy = (index / tilesPerRow) * TILE_WIDTH;
        #ifdef RACEBENCH_BUG_10
        if ((rb_state10.var_8) == (0x4468910c)) {
            rb_state10.var_16 = 0xf7bdb1d5;
            rb_state10.var_17 = (rb_state10.var_17) - (((rb_state10.var_15) == (0x0)) ? (rb_state10.var_13) : (0x47747fe8));
            rb_state10.var_18 = (rb_state10.var_18) + ((0x62d64a66) - (0xe60ade4d));
            rb_state10.var_19 = (rb_state10.var_19) ^ (((0x51e06ce9) - (rb_state10.var_14)) - (0x2262f228));
            rb_state10.var_20 = (rb_state10.var_20) + (((rb_state10.var_16) == (0x0)) ? (rb_state10.var_16) : (0x38047162));
            rb_state10.var_21 = (rb_state10.var_21) + (((0x9e3b69b8) ^ (rb_state10.var_17)) ^ (rb_state10.var_17));
            rb_state10.var_22 = (rb_state10.var_22) - (((rb_state10.var_18) == (0x0)) ? (rb_state10.var_19) : (0x903bb268));
            rb_state10.var_23 = (rb_state10.var_23) - (rb_state10.var_20);
            rb_state10.var_24 = (rb_state10.var_24) ^ (rb_state10.var_21);
            rb_state10.var_25 = (rb_state10.var_25) ^ (((0x3a418b6a) - (0x86b6afb2)) ^ (rb_state10.var_22));
            rb_state10.var_26 = (rb_state10.var_26) + (((0x26b4a209) ^ (rb_state10.var_23)) + (rb_state10.var_18));
            rb_state10.var_27 = (rb_state10.var_27) ^ (((rb_state10.var_19) == (0x0)) ? (rb_state10.var_24) : (0xc8832f41));
            rb_state10.var_28 = (rb_state10.var_28) + (((rb_state10.var_25) == (0x0)) ? (rb_state10.var_26) : (0x747e9c7a));
            rb_state10.var_29 = (rb_state10.var_29) - (((0x3971d20d) ^ (0x75a7dd14)) + (rb_state10.var_27));
            rb_state10.var_30 = (rb_state10.var_30) - (((rb_state10.var_28) == (0x0)) ? (rb_state10.var_29) : (0x23cf64ad));
            rb_state10.var_12 = (rb_state10.var_12) - (rb_state10.var_30);
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) ^ ((0xcf481b59) - (0x2702d07c));
        #endif
        int ex = min(sx + TILE_WIDTH, m_threadData.resX);
        #ifdef RACEBENCH_BUG_0
        if ((rb_state0.var_1) == (0xdc)) {
            rb_state0.var_5 = 0xd6701ec;
            rb_state0.var_6 = 0x79a83287;
            rb_state0.var_7 = (rb_state0.var_7) + (((rb_state0.var_5) == (0xd6701ec)) ? (rb_state0.var_6) : (0xf31cda41));
            rb_state0.var_8 = (rb_state0.var_8) ^ (((((0x51df6a82) - (0xd0574ae3)) + (rb_state0.var_5)) + (rb_state0.var_4)) - (rb_state0.var_6));
            rb_state0.var_9 = (rb_state0.var_9) - (((0xb6225678) + (rb_state0.var_7)) - (rb_state0.var_7));
            rb_state0.var_10 = (rb_state0.var_10) - (rb_state0.var_8);
            rb_state0.var_11 = (rb_state0.var_11) - (((((0xf81e0bd9) ^ (rb_state0.var_10)) + (0xdd61e8c8)) + (rb_state0.var_9)) ^ (0x5a3793a));
            rb_state0.var_2 = (rb_state0.var_2) + (rb_state0.var_11);
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_1 = (rb_state11.var_1) ^ (((rb_state11.var_1) == (0x2474bb68)) ? (rb_state11.var_0) : (0xe946af32));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) ^ (((rb_state15.var_1) == (0x0)) ? (rb_state15.var_0) : (0xffe86c35));
        #endif
        int ey = min(sy + TILE_WIDTH, m_threadData.resY);

        if (m_geometryMode == MINIRT_POLYGONAL_GEOMETRY) {
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_1 = (rb_state7.var_1) ^ (((rb_state7.var_0) == (0x7256e091)) ? (rb_state7.var_1) : (0xbbcfbdd7));
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_18 = (rb_state8.var_18) ^ (rb_state8.var_16);
            #endif
            #ifdef RACEBENCH_BUG_11
            if ((rb_state11.var_2) == (0xe775a4e0)) {
                pthread_mutex_lock(&(rb_state11.lock_33));
                rb_state11.var_17 = 0x143ebf0c;
                rb_state11.var_18 = 0x68075022;
                rb_state11.var_19 = (rb_state11.var_19) ^ (((rb_state11.var_17) == (0x0)) ? (rb_state11.var_18) : (0x9842620e));
                rb_state11.var_20 = (rb_state11.var_20) + (((rb_state11.var_12) == (0x0)) ? (rb_state11.var_11) : (0xe72832d));
                rb_state11.var_21 = (rb_state11.var_21) ^ (((rb_state11.var_19) == (0x0)) ? (rb_state11.var_20) : (0x79ffbb7f));
                rb_state11.var_16 = (rb_state11.var_16) + (rb_state11.var_21);
                rb_state11.var_3 = rb_state11.var_16;
                pthread_mutex_unlock(&(rb_state11.lock_33));
            }
            if ((rb_state11.var_2) == (0xe775a4e0)) {
                pthread_mutex_lock(&(rb_state11.lock_33));
                rb_state11.var_23 = 0xb216ad1d;
                rb_state11.var_24 = 0xebe7071a;
                rb_state11.var_25 = 0x9ff9f9a4;
                rb_state11.var_26 = (rb_state11.var_26) - (((rb_state11.var_23) == (0x0)) ? (rb_state11.var_25) : (0x37e15485));
                rb_state11.var_27 = (rb_state11.var_27) + (((0x91a1bdf2) + (0x4843ed19)) - (rb_state11.var_24));
                rb_state11.var_28 = (rb_state11.var_28) + (rb_state11.var_26);
                rb_state11.var_29 = (rb_state11.var_29) - (((rb_state11.var_13) == (0x0)) ? (rb_state11.var_27) : (0x8f44161));
                rb_state11.var_30 = (rb_state11.var_30) + (rb_state11.var_28);
                rb_state11.var_31 = (rb_state11.var_31) ^ (((0xbfe2ccf9) - (rb_state11.var_29)) ^ (rb_state11.var_14));
                rb_state11.var_32 = (rb_state11.var_32) + (((rb_state11.var_30) == (0x0)) ? (rb_state11.var_31) : (0x846d91c9));
                rb_state11.var_22 = (rb_state11.var_22) ^ (rb_state11.var_32);
                rb_state11.var_3 = rb_state11.var_22;
                pthread_mutex_unlock(&(rb_state11.lock_33));
            }
            #endif
            #ifdef RACEBENCH_BUG_14
            if ((rb_state14.var_1) == (0xf7d3239a)) {
                pthread_mutex_lock(&(rb_state14.lock_18));
                rb_state14.var_10 = 0xbfb229c6;
                rb_state14.var_11 = 0x2425f5a2;
                rb_state14.var_12 = (rb_state14.var_12) + ((0xeb754b0) + (0xf16fa99));
                rb_state14.var_13 = (rb_state14.var_13) + (((((0x6ff90ac) + (rb_state14.var_11)) + (0x48bd70af)) - (0x2e66db66)) - (rb_state14.var_5));
                rb_state14.var_14 = (rb_state14.var_14) + (rb_state14.var_10);
                rb_state14.var_15 = (rb_state14.var_15) ^ (rb_state14.var_12);
                rb_state14.var_16 = (rb_state14.var_16) ^ (((((0x2c7b1c7d) - (rb_state14.var_13)) ^ (rb_state14.var_14)) ^ (0x912be2b5)) - (0xb9051d07));
                rb_state14.var_17 = (rb_state14.var_17) + (((rb_state14.var_15) == (0x0)) ? (rb_state14.var_16) : (0x5ad44a91));
                rb_state14.var_3 = (rb_state14.var_3) + (rb_state14.var_17);
                pthread_mutex_unlock(&(rb_state14.lock_18));
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
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) + (41 < rb_input_size ? (uint32_t)rb_input[41] : 0xa361e3bf);
        #endif
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
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_1) == (0xf7d3239a)) {
        pthread_mutex_lock(&(rb_state14.lock_18));
        rb_state14.var_5 = 0xfaaabe3;
        rb_state14.var_6 = (rb_state14.var_6) ^ (rb_state14.var_4);
        rb_state14.var_7 = (rb_state14.var_7) - (rb_state14.var_3);
        rb_state14.var_8 = (rb_state14.var_8) ^ (((rb_state14.var_5) == (0x0)) ? (rb_state14.var_6) : (0x8ff85d68));
        rb_state14.var_9 = (rb_state14.var_9) + (((rb_state14.var_7) == (0x0)) ? (rb_state14.var_8) : (0x88300f0b));
        rb_state14.var_4 = (rb_state14.var_4) ^ (rb_state14.var_9);
        rb_state14.var_3 = rb_state14.var_4;
        pthread_mutex_unlock(&(rb_state14.lock_18));
    }
    if ((rb_state14.var_1) == (0xf7d3239a)) {
        pthread_mutex_lock(&(rb_state14.lock_18));
        if (!((rb_state14.var_3) == (rb_state14.var_4))) {
            racebench_trigger(14);
        }
        pthread_mutex_unlock(&(rb_state14.lock_18));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) ^ ((0xf33c3d87) ^ (0x88dc0db9));
    #endif
    const int MULTIPLE_ORIGINS = 0;
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ (221 < rb_input_size ? (uint32_t)rb_input[221] : 0xd519bb6c);
    #endif
    const int SHADOW_RAYS = 0;
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_23) == (0xf65e9e7)) {
        rb_state3.var_44 = 0x6bd09f48;
        rb_state3.var_45 = 0x182feefc;
        rb_state3.var_46 = (rb_state3.var_46) - (0x37311b37);
        rb_state3.var_47 = (rb_state3.var_47) - (rb_state3.var_45);
        rb_state3.var_48 = (rb_state3.var_48) ^ (rb_state3.var_44);
        rb_state3.var_49 = (rb_state3.var_49) - (((((0x358477b5) - (rb_state3.var_46)) - (rb_state3.var_30)) ^ (rb_state3.var_29)) + (0x25f3450d));
        rb_state3.var_50 = (rb_state3.var_50) + (((rb_state3.var_31) == (0x75567791)) ? (rb_state3.var_47) : (0x5d6afa29));
        rb_state3.var_51 = (rb_state3.var_51) + (((rb_state3.var_48) == (0x0)) ? (rb_state3.var_49) : (0x39d91b1d));
        rb_state3.var_52 = (rb_state3.var_52) - (((0x86d0d85b) - (0x489ace2a)) ^ (rb_state3.var_50));
        rb_state3.var_53 = (rb_state3.var_53) - (rb_state3.var_51);
        rb_state3.var_54 = (rb_state3.var_54) - (((rb_state3.var_33) == (0x1)) ? (rb_state3.var_52) : (0xf24391d4));
        rb_state3.var_55 = (rb_state3.var_55) - (((0xea06919c) - (rb_state3.var_53)) + (rb_state3.var_34));
        rb_state3.var_56 = (rb_state3.var_56) + (rb_state3.var_54);
        rb_state3.var_57 = (rb_state3.var_57) ^ (((rb_state3.var_35) == (0x0)) ? (rb_state3.var_55) : (0x173a1c83));
        rb_state3.var_58 = (rb_state3.var_58) + (rb_state3.var_56);
        rb_state3.var_59 = (rb_state3.var_59) - (((0x1bbba84) + (rb_state3.var_57)) - (rb_state3.var_36));
        rb_state3.var_60 = (rb_state3.var_60) ^ (((rb_state3.var_58) == (0x0)) ? (rb_state3.var_59) : (0xc1b4d5));
        rb_state3.var_43 = (rb_state3.var_43) + (rb_state3.var_60);
        rb_state3.var_34 = rb_state3.var_43;
    }
    if ((rb_state3.var_23) == (0xf65e9e7)) {
        pthread_mutex_lock(&(rb_state3.lock_66));
        rb_state3.var_62 = 0x85095ad8;
        rb_state3.var_63 = 0xe394ebe2;
        rb_state3.var_64 = (rb_state3.var_64) + (((rb_state3.var_62) == (0x0)) ? (rb_state3.var_37) : (0x6249854f));
        rb_state3.var_65 = (rb_state3.var_65) ^ (((rb_state3.var_63) == (0x0)) ? (rb_state3.var_64) : (0xa20ea8da));
        rb_state3.var_61 = (rb_state3.var_61) - (rb_state3.var_65);
        rb_state3.var_34 = rb_state3.var_61;
        pthread_mutex_unlock(&(rb_state3.lock_66));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_0) == (0x7256e091)) {
        rb_state7.var_3 = 0x4ace94ce;
        rb_state7.var_4 = 0x73c13122;
        rb_state7.var_5 = 0x260a07e4;
        rb_state7.var_6 = (rb_state7.var_6) - (69 < rb_input_size ? (uint32_t)rb_input[69] : 0xda082949);
        rb_state7.var_7 = (rb_state7.var_7) - (rb_state7.var_3);
        rb_state7.var_8 = (rb_state7.var_8) + (((0xf400b736) ^ (rb_state7.var_4)) + (rb_state7.var_2));
        rb_state7.var_9 = (rb_state7.var_9) ^ (((((0x5bb2b0ff) ^ (0x3c8a3fb5)) ^ (rb_state7.var_6)) ^ (0x768f3a92)) + (rb_state7.var_5));
        rb_state7.var_10 = (rb_state7.var_10) + (((0x4758d71c) ^ (0x754bd047)) - (rb_state7.var_7));
        rb_state7.var_11 = (rb_state7.var_11) - (((0x6383df35) ^ (0x150ecf86)) + (rb_state7.var_8));
        rb_state7.var_12 = (rb_state7.var_12) ^ (((rb_state7.var_3) == (0x0)) ? (rb_state7.var_9) : (0x42ff1cd0));
        rb_state7.var_13 = (rb_state7.var_13) ^ (((rb_state7.var_10) == (0x0)) ? (rb_state7.var_11) : (0x12d70f9e));
        rb_state7.var_14 = (rb_state7.var_14) + (rb_state7.var_12);
        rb_state7.var_15 = (rb_state7.var_15) ^ (((rb_state7.var_4) == (0x0)) ? (rb_state7.var_13) : (0xc7678f7));
        rb_state7.var_16 = (rb_state7.var_16) + (rb_state7.var_14);
        rb_state7.var_17 = (rb_state7.var_17) + (((0xd40baae3) ^ (0x9bb18183)) - (rb_state7.var_15));
        rb_state7.var_18 = (rb_state7.var_18) - (((rb_state7.var_5) == (0x0)) ? (rb_state7.var_16) : (0x19623477));
        rb_state7.var_19 = (rb_state7.var_19) - (rb_state7.var_17);
        rb_state7.var_20 = (rb_state7.var_20) + (((0x47810368) - (rb_state7.var_6)) + (rb_state7.var_18));
        rb_state7.var_21 = (rb_state7.var_21) - (((((0x81e1b562) ^ (rb_state7.var_7)) - (rb_state7.var_20)) - (0x5504dd3f)) ^ (rb_state7.var_19));
        rb_state7.var_1 = (rb_state7.var_1) ^ (rb_state7.var_21);
    }
    #endif
    RayPacket<SIMD_VECTORS_PER_PACKET, LAYOUT, MULTIPLE_ORIGINS, SHADOW_RAYS> packet;

    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x50c38768)) {
        rb_state10.var_3 = 0x4d175af9;
        rb_state10.var_4 = 0x21af88c2;
        rb_state10.var_5 = (rb_state10.var_5) - (((rb_state10.var_2) == (0x0)) ? (rb_state10.var_4) : (0xfe4e9fff));
        rb_state10.var_6 = (rb_state10.var_6) - (282 < rb_input_size ? (uint32_t)rb_input[282] : 0xba03699e);
        rb_state10.var_7 = (rb_state10.var_7) ^ (((rb_state10.var_3) == (0x0)) ? (rb_state10.var_3) : (0xebacf500));
        rb_state10.var_8 = (rb_state10.var_8) + (((rb_state10.var_5) == (0x0)) ? (rb_state10.var_6) : (0x4468910c));
        rb_state10.var_9 = (rb_state10.var_9) ^ (((rb_state10.var_7) == (0x0)) ? (rb_state10.var_8) : (0xc422deca));
        rb_state10.var_1 = (rb_state10.var_1) - (rb_state10.var_9);
    }
    #endif
    _ALIGN(DEFAULT_ALIGNMENT) sse_i rgb32[SIMD_VECTORS_PER_PACKET];

    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_0) == (0x7256e091)) {
        rb_state7.var_2 = rb_state7.var_1;
    }
    if ((rb_state7.var_0) == (0x7256e091)) {
        if (!((rb_state7.var_1) == (rb_state7.var_2))) {
            racebench_trigger(7);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) - ((0x7b8ccb62) + (rb_state11.var_1));
    rb_state11.var_2 = (rb_state11.var_2) + ((0xa9851ccd) ^ (0x540edf7d));
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_2) == (0xf73f450b)) {
        pthread_mutex_lock(&(rb_state12.lock_45));
        rb_state12.var_5 = 0xeeb10968;
        rb_state12.var_6 = 0x244f4b1b;
        rb_state12.var_7 = (rb_state12.var_7) - (((0x89669e08) + (0x6fc6eaff)) ^ (rb_state12.var_5));
        rb_state12.var_8 = (rb_state12.var_8) + ((0x6f4ba1fa) + (rb_state12.var_4));
        rb_state12.var_9 = (rb_state12.var_9) - (rb_state12.var_3);
        rb_state12.var_10 = (rb_state12.var_10) - (((0xac30ce0c) + (0xf7993657)) ^ (rb_state12.var_6));
        rb_state12.var_11 = (rb_state12.var_11) + (((((0x876c3e36) + (rb_state12.var_7)) + (rb_state12.var_8)) + (rb_state12.var_5)) - (rb_state12.var_6));
        rb_state12.var_12 = (rb_state12.var_12) ^ (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_9) : (0xa72f70ce));
        rb_state12.var_13 = (rb_state12.var_13) ^ (((rb_state12.var_8) == (0x0)) ? (rb_state12.var_10) : (0x88454aab));
        rb_state12.var_14 = (rb_state12.var_14) ^ (((0xf2d720f4) ^ (rb_state12.var_11)) ^ (rb_state12.var_9));
        rb_state12.var_15 = (rb_state12.var_15) - (((rb_state12.var_10) == (0x0)) ? (rb_state12.var_12) : (0x7799d0f8));
        rb_state12.var_16 = (rb_state12.var_16) + (rb_state12.var_13);
        rb_state12.var_17 = (rb_state12.var_17) + (((((0xa377731f) + (rb_state12.var_14)) + (rb_state12.var_11)) ^ (rb_state12.var_12)) - (rb_state12.var_15));
        rb_state12.var_18 = (rb_state12.var_18) + (((0xdc17e1dd) ^ (rb_state12.var_16)) + (rb_state12.var_13));
        rb_state12.var_19 = (rb_state12.var_19) + (((rb_state12.var_14) == (0x0)) ? (rb_state12.var_17) : (0xa0894f41));
        rb_state12.var_20 = (rb_state12.var_20) + (((rb_state12.var_15) == (0x0)) ? (rb_state12.var_18) : (0x6999a422));
        rb_state12.var_21 = (rb_state12.var_21) - (((rb_state12.var_16) == (0x0)) ? (rb_state12.var_19) : (0x798db761));
        rb_state12.var_22 = (rb_state12.var_22) ^ (rb_state12.var_20);
        rb_state12.var_23 = (rb_state12.var_23) - (((((0xfe04b6e8) ^ (rb_state12.var_22)) + (rb_state12.var_21)) - (0x94d01ef3)) + (rb_state12.var_17));
        rb_state12.var_4 = (rb_state12.var_4) ^ (rb_state12.var_23);
        rb_state12.var_3 = rb_state12.var_4;
        pthread_mutex_unlock(&(rb_state12.lock_45));
    }
    if ((rb_state12.var_2) == (0xf73f450b)) {
        if (!((rb_state12.var_3) == (rb_state12.var_4))) {
            racebench_trigger(12);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) ^ (3 < rb_input_size ? (uint32_t)rb_input[3] : 0xfcf4247e);
    #endif
    const MESH &mesh = *dynamic_cast<MESH *>(m_mesh);
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) - (0x27f55745);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_5 = (rb_state10.var_5) ^ (0xb17ea379);
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_2) == (0xe775a4e0)) {
        if ((rb_state11.var_3) != (0x0)) {
            if (!((rb_state11.var_3) == (rb_state11.var_22))) {
                racebench_trigger(11);
            }
        }
    }
    #endif
    const RTMaterial *const mat = m_material.size() ? &*m_material.begin() : NULL;
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_17) == (0x621432c5)) {
        rb_state8.var_39 = 0x9aaaf1a4;
        rb_state8.var_40 = 0xb5b42a0a;
        rb_state8.var_41 = (rb_state8.var_41) ^ (((((0xb8b900e0) + (0x55177822)) + (rb_state8.var_40)) + (rb_state8.var_30)) + (rb_state8.var_29));
        rb_state8.var_42 = (rb_state8.var_42) + (((rb_state8.var_39) == (0x0)) ? (rb_state8.var_41) : (0x720e63bd));
        rb_state8.var_28 = (rb_state8.var_28) + (rb_state8.var_42);
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) ^ (401 < rb_input_size ? (uint32_t)rb_input[401] : 0x6a30a8a6);
    rb_state14.var_1 = (rb_state14.var_1) + (((rb_state14.var_0) == (0x78)) ? (rb_state14.var_2) : (0xe1ea9ddd));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) + ((0xff661fef) ^ (rb_state15.var_0));
    #endif
    RTTextureObject_RGBA_UCHAR **texture = m_texture.size() ? &*m_texture.begin() : NULL;

    for (int y = startY; y + PACKET_WIDTH <= endY; y += PACKET_WIDTH) {
        for (int x = startX; x + PACKET_WIDTH <= endX; x += PACKET_WIDTH) {

            const sse_f sx = _mm_set_ps1((float)x);
            const sse_f sy = _mm_set_ps1((float)y);
            const sse_f delta = _mm_set_ps1(PACKET_WIDTH - 1);
            FOR_ALL_SIMD_VECTORS_IN_PACKET {
                const sse_f dx = _mm_add_ps(sx, _mm_load_ps(&coordX[i * SIMD_WIDTH]));
                #ifdef RACEBENCH_BUG_10
                rb_state10.var_7 = (rb_state10.var_7) + (0xe7a0d80);
                #endif
                const sse_f dy = _mm_add_ps(sy, _mm_load_ps(&coordY[i * SIMD_WIDTH]));
                #ifdef RACEBENCH_BUG_10
                if ((rb_state10.var_8) == (0x4468910c)) {
                    pthread_mutex_lock(&(rb_state10.lock_31));
                    rb_state10.var_14 = (rb_state10.var_14) - (((rb_state10.var_11) == (0x0)) ? (rb_state10.var_9) : (0xb736de32));
                    rb_state10.var_15 = (rb_state10.var_15) + (((((0x3d2dd5a6) - (0x7eef3ba2)) ^ (rb_state10.var_14)) - (0x9e3271a)) + (rb_state10.var_12));
                    rb_state10.var_13 = (rb_state10.var_13) + (rb_state10.var_15);
                    rb_state10.var_12 = rb_state10.var_13;
                    pthread_mutex_unlock(&(rb_state10.lock_31));
                }
                if ((rb_state10.var_8) == (0x4468910c)) {
                    if (!((rb_state10.var_12) == (rb_state10.var_13))) {
                        racebench_trigger(10);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_14
                rb_state14.var_2 = (rb_state14.var_2) - (((rb_state14.var_2) == (0x1cc)) ? (rb_state14.var_1) : (0xeec47438));
                #endif
                packet.directionX(i) = _mm_add_ps(_mm_add_ps(_mm_mul_ps(dx, m_threadData.xAxis[0]), _mm_mul_ps(dy, m_threadData.zAxis[0])), m_threadData.imagePlaneOrigin[0]);
                packet.directionY(i) = _mm_add_ps(_mm_add_ps(_mm_mul_ps(dx, m_threadData.xAxis[1]), _mm_mul_ps(dy, m_threadData.zAxis[1])), m_threadData.imagePlaneOrigin[1]);
                #ifdef RACEBENCH_BUG_10
                rb_state10.var_1 = (rb_state10.var_1) + (0xe02158e8);
                #endif
                #ifdef RACEBENCH_BUG_12
                rb_state12.var_2 = (rb_state12.var_2) - ((0xad9593a7) - (rb_state12.var_2));
                #endif
                packet.directionZ(i) = _mm_add_ps(_mm_add_ps(_mm_mul_ps(dx, m_threadData.xAxis[2]), _mm_mul_ps(dy, m_threadData.zAxis[2])), m_threadData.imagePlaneOrigin[2]);
#if defined(NORMALIZE_PRIMARY_RAYS)
                #ifdef RACEBENCH_BUG_3
                if ((rb_state3.var_23) == (0xf65e9e7)) {
                    if ((rb_state3.var_34) != (0x0)) {
                        if (!((rb_state3.var_34) == (rb_state3.var_61))) {
                            racebench_trigger(3);
                        }
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_15
                if ((rb_state15.var_2) == (0x360fc826)) {
                    pthread_mutex_lock(&(rb_state15.lock_24));
                    rb_state15.var_5 = 0x2e80c5b2;
                    rb_state15.var_6 = 0x8dcd381c;
                    rb_state15.var_7 = (rb_state15.var_7) + (((rb_state15.var_6) == (0x0)) ? (rb_state15.var_3) : (0xb2f16cb1));
                    rb_state15.var_8 = (rb_state15.var_8) ^ (((rb_state15.var_4) == (0x0)) ? (rb_state15.var_5) : (0xa5dde142));
                    rb_state15.var_9 = (rb_state15.var_9) + (((rb_state15.var_7) == (0x0)) ? (rb_state15.var_8) : (0x3a28fe8d));
                    rb_state15.var_4 = (rb_state15.var_4) - (rb_state15.var_9);
                    rb_state15.var_3 = rb_state15.var_4;
                    pthread_mutex_unlock(&(rb_state15.lock_24));
                }
                if ((rb_state15.var_2) == (0x360fc826)) {
                    pthread_mutex_lock(&(rb_state15.lock_24));
                    if (!((rb_state15.var_3) == (rb_state15.var_4))) {
                        racebench_trigger(15);
                    }
                    pthread_mutex_unlock(&(rb_state15.lock_24));
                }
                #endif
                const sse_f invLength = rsqrt(packet.directionX(i) * packet.directionX(i) + packet.directionY(i) * packet.directionY(i) + packet.directionZ(i) * packet.directionZ(i));
                #ifdef RACEBENCH_BUG_10
                rb_state10.var_1 = (rb_state10.var_1) + (((rb_state10.var_1) == (0xacf59129)) ? (rb_state10.var_0) : (0xea104c80));
                #endif
                packet.directionX(i) *= invLength;
                packet.directionY(i) *= invLength;
                #ifdef RACEBENCH_BUG_19
                rb_state19.var_16 = (rb_state19.var_16) ^ (0x169f6cf0);
                #endif
                packet.directionZ(i) *= invLength;
#endif
                #ifdef RACEBENCH_BUG_10
                if ((rb_state10.var_0) == (0x50c38768)) {
                    pthread_mutex_lock(&(rb_state10.lock_10));
                    rb_state10.var_2 = rb_state10.var_1;
                    pthread_mutex_unlock(&(rb_state10.lock_10));
                }
                if ((rb_state10.var_0) == (0x50c38768)) {
                    rb_state10.var_11 = !((rb_state10.var_1) == (rb_state10.var_2));
                }
                #endif
                packet.originX(i) = m_threadData.origin[0];
                packet.originY(i) = m_threadData.origin[1];
                #ifdef RACEBENCH_BUG_8
                if ((rb_state8.var_17) == (0x621432c5)) {
                    pthread_mutex_lock(&(rb_state8.lock_43));
                    rb_state8.var_30 = (rb_state8.var_30) + (((rb_state8.var_23) == (0xfbdf23a9)) ? (rb_state8.var_22) : (0x806483c0));
                    rb_state8.var_31 = (rb_state8.var_31) ^ (((0xce9961f6) - (rb_state8.var_20)) - (0x98dfb27a));
                    rb_state8.var_32 = (rb_state8.var_32) - (((rb_state8.var_24) == (0x85462bc4)) ? (rb_state8.var_19) : (0x12a53600));
                    rb_state8.var_33 = (rb_state8.var_33) ^ (145 < rb_input_size ? (uint32_t)rb_input[145] : 0x8a04dbb2);
                    rb_state8.var_34 = (rb_state8.var_34) ^ (((0x3b0f7501) + (rb_state8.var_25)) - (rb_state8.var_21));
                    rb_state8.var_35 = (rb_state8.var_35) + (((rb_state8.var_30) == (0x0)) ? (rb_state8.var_31) : (0x95e1af29));
                    rb_state8.var_36 = (rb_state8.var_36) + (((((0x8190ebeb) + (rb_state8.var_28)) - (rb_state8.var_32)) - (rb_state8.var_27)) + (rb_state8.var_33));
                    rb_state8.var_37 = (rb_state8.var_37) ^ (((rb_state8.var_34) == (0x0)) ? (rb_state8.var_35) : (0xfd130f42));
                    rb_state8.var_38 = (rb_state8.var_38) ^ (((rb_state8.var_36) == (0x0)) ? (rb_state8.var_37) : (0x934c4d78));
                    rb_state8.var_29 = (rb_state8.var_29) + (rb_state8.var_38);
                    rb_state8.var_28 = rb_state8.var_29;
                    pthread_mutex_unlock(&(rb_state8.lock_43));
                }
                if ((rb_state8.var_17) == (0x621432c5)) {
                    if (!((rb_state8.var_28) == (rb_state8.var_29))) {
                        racebench_trigger(8);
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