

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "ImageProjection.h"
#include "CameraModel.h"
#include <vector>

inline void ProjectPoints(Vector3f &pt_3D, Point &pt_2D, Camera &camera) {
    Vector3f X2 = camera.mc_ext * pt_3D;
    float inv_Z = 1.00f / X2.z;
    Point pt(inv_Z * X2.x, inv_Z * X2.y);

    float r2 = (pt.x * pt.x) + (pt.y * pt.y);
    float r4 = r2 * r2;
    float r6 = r4 * r2;
    float cdist = 1.00f + camera.kc[0] * r2 + camera.kc[1] * r4 + camera.kc[4] * r6;
    Point pt2(cdist * pt.x, cdist * pt.y);

    float a1 = 2 * pt.x * pt.y;
    float a2 = r2 + 2 * (pt.x * pt.x);
    float a3 = r2 + 2 * (pt.y * pt.y);

    Point pt3(pt2.x + camera.kc[2] * a1 + camera.kc[2] * a2, pt2.y + camera.kc[2] * a3 + camera.kc[2] * a1);
    Point pt4(pt3.x + camera.alpha_c * pt3.y, pt3.y);

    pt_2D.Set(camera.fc.x * pt4.x + camera.cc.x, camera.fc.y * pt4.y + camera.cc.y);
}

void ProjectedCylinder::ImageProjection(const KTCylinder &cyl, Camera &camera) {
    Vector3f point(0.00f, 0.00f, cyl.length / 2);
    Vector3f r = point - Inverse(cyl.pose) * camera.eye;
    double rnorm2 = r.x * r.x + r.y * r.y;
    float norm_r = (float)sqrt(rnorm2);
    Vector3f r2(-r.y / norm_r, r.x / norm_r, 0.00f);

    Vector3f pts[4];
    pts[0].Set(-cyl.bottom * r2.x, -cyl.bottom * r2.y, -cyl.bottom * r2.z);
    pts[1].Set(-cyl.top * r2.x, -cyl.top * r2.y, cyl.length - (cyl.top * r2.z));
    pts[2].Set(cyl.top * r2.x, cyl.top * r2.y, cyl.length + (cyl.top * r2.z));
    pts[3].Set(cyl.bottom * r2.x, cyl.bottom * r2.y, cyl.bottom * r2.z);
    pts[0] = cyl.pose * pts[0];
    pts[1] = cyl.pose * pts[1];
    pts[2] = cyl.pose * pts[2];
    pts[3] = cyl.pose * pts[3];

    for (int i = 0; i < 4; i++) {
        ProjectPoints(pts[i], mPts[i], camera);
    }
}

void ProjectedBody::ImageProjection(const BodyGeometry &body, Camera &camera) {
    int n_parts = body.GetBodyPartCount();
    mProjCyls.resize(n_parts);

    for (int i = 0; i < n_parts; i++) {
        mProjCyls[i].ImageProjection(body(i), camera);
    }
}

void MultiCameraProjectedBody::ImageProjection(const BodyGeometry &body, MultiCamera &cameras) {
    int n_cameras = cameras.GetCameraCount();
    mProjBodies.resize(n_cameras);
    for (int i = 0; i < n_cameras; i++) {
        mProjBodies[i].ImageProjection(body, cameras(i));
    }
}
