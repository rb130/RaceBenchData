

#include "incl.h"

extern int num_traced_rays_hit_volume;
extern int num_samples_trilirped;
extern int traversal_time, trilirp_time, init_time, composite_time;

#define VXEL(X, SIGN) ((SIGN) > 0 ? (int)((X) + SMALL) : (int)((X)-SMALL))

#define SBIT_ADDRESS(TA) (sbit_address + (TA))
#define SHD_ADDRESS(TA) (shd_address + (TA))
#define SBIT(TA) (*SBIT_ADDRESS(TA))
#define SHD(TA) (*SHD_ADDRESS(TA))

#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
extern pthread_t PThreadTable[];

Trace_Ray(outx, outy, foutx, fouty, pixel_address) int outx, outy;
float foutx, fouty;
PIXEL *pixel_address;
{
    float ray[2][NM];

    float rmin, rmax;

    float ray_min, ray_max;

    int segment_zmin, segment_zmax;
    int span_zmin, span_zmax;
    float sample[NM], sample2[NM];
    int outz;
    int i, samplex, sampley, samplez;
    int sample2x, sample2y, sample2z;

    float box_zmin, box_zmax;
    int starting_level, level;
    float jump[NM], min_jump;
    float voxel[NM], next_voxel[NM];
    int ivoxel[NM], next_ivoxel[NM];
    BOOLEAN bit;

    float xalpha, yalpha, zalpha;
    float one_minus_xalpha, one_minus_yalpha, one_minus_zalpha;
    float weight, wopacity, wopacitysum, wcolorsum;
    int table_addr, xnorm, ynorm, znorm;
    float normal[NM], wnormalsum[NM];
    float inv_magnitude, magnitude;
    float additional_opacity;
    float dot_product, diffuse, specular;

    float color;
    float opacity;
    float ray_color, ray_opacity;

    OPACITY *local_opc_address, *local2_opc_address;
    NORMAL *local_norm_address, *local2_norm_address;

    long pyr_offset1;
    long pyr_offset2;
    BYTE *pyr_address2;

    ray_color = (float)MIN_PIXEL;
    ray_opacity = MIN_OPACITY;

    for (i = 0; i < NM; i++) {
        ray[0][i] = out_invvertex[0][0][0][i] + invjacobian[X][i] * foutx + invjacobian[Y][i] * fouty;
        ray[1][i] = ray[0][i] + invjacobian[Z][i] * image_zlen;
    }

    ray_min = 0.0;
    ray_max = 1.0;
    for (i = 0; i < NM; i++) {
        if (ABS(ray[1][i] - ray[0][i]) < SMALL) {
            if ((ray[0][i] < 0.0 && ray[1][i] < 0.0) || (ray[0][i] > (float)(opc_len[i] - 1) && ray[1][i] > (float)(opc_len[i] - 1))) {
                return;
            } else {
                continue;
            }
        }
        rmin = (0.0 - ray[0][i]) / (ray[1][i] - ray[0][i]);
        rmax = ((float)(opc_len[i] - 1) - ray[0][i]) / (ray[1][i] - ray[0][i]);
        ray_min = MAX(MIN(rmin, rmax), ray_min);
        ray_max = MIN(MAX(rmin, rmax), ray_max);
    }

    segment_zmin = ROUNDUP(image_zlen * ray_min);
    segment_zmax = ROUNDDOWN(image_zlen * ray_max);

    if (segment_zmax < segment_zmin) {
        return;
    }

    box_zmin = (float)segment_zmin;
    voxel[X] = ray[0][X] + invjacobian[Z][X] * box_zmin;
    voxel[Y] = ray[0][Y] + invjacobian[Z][Y] * box_zmin;
    voxel[Z] = ray[0][Z] + invjacobian[Z][Z] * box_zmin;
    ivoxel[X] = VXEL(voxel[X], invjacobian[Z][X]);
    ivoxel[Y] = VXEL(voxel[Y], invjacobian[Z][Y]);
    ivoxel[Z] = VXEL(voxel[Z], invjacobian[Z][Z]);
    starting_level = MIN(pyr_highest_level, pyr_levels - 1);
    level = starting_level;

    while (1) {

        bit = PYR(level, ivoxel[Z] >> level, ivoxel[Y] >> level, ivoxel[X] >> level);
        if (bit && level > pyr_lowest_level) {

            level--;
            continue;
        }

        min_jump = BIG;
        if (invjacobian[Z][X] > SMALL) {
            jump[X] = invinvjacobian[Z][X] * (((ROUNDDOWN(voxel[X]) >> level) + 1) * pyr_voxlen[level][X] - voxel[X]);
            min_jump = MIN(min_jump, jump[X]);
        } else if (invjacobian[Z][X] < -SMALL) {
            jump[X] = invinvjacobian[Z][X] * ((STEPDOWN(voxel[X]) >> level) * pyr_voxlen[level][X] - voxel[X]);
            min_jump = MIN(min_jump, jump[X]);
        }
        if (invjacobian[Z][Y] > SMALL) {
            jump[Y] = invinvjacobian[Z][Y] * (((ROUNDDOWN(voxel[Y]) >> level) + 1) * pyr_voxlen[level][Y] - voxel[Y]);
            min_jump = MIN(min_jump, jump[Y]);
        } else if (invjacobian[Z][Y] < -SMALL) {
            jump[Y] = invinvjacobian[Z][Y] * ((STEPDOWN(voxel[Y]) >> level) * pyr_voxlen[level][Y] - voxel[Y]);
            min_jump = MIN(min_jump, jump[Y]);
        }
        if (invjacobian[Z][Z] > SMALL) {
            jump[Z] = invinvjacobian[Z][Z] * (((ROUNDDOWN(voxel[Z]) >> level) + 1) * pyr_voxlen[level][Z] - voxel[Z]);
            min_jump = MIN(min_jump, jump[Z]);
        } else if (invjacobian[Z][Z] < -SMALL) {
            jump[Z] = invinvjacobian[Z][Z] * ((STEPDOWN(voxel[Z]) >> level) * pyr_voxlen[level][Z] - voxel[Z]);
            min_jump = MIN(min_jump, jump[Z]);
        }
        box_zmax = box_zmin + min_jump;

        if (bit) {

            break;
        }

    next_box:
        if (box_zmax >= (float)segment_zmax) {
            goto end_of_segment;
        }
        box_zmin = box_zmax;

        next_voxel[X] = voxel[X] + invjacobian[Z][X] * min_jump;
        next_voxel[Y] = voxel[Y] + invjacobian[Z][Y] * min_jump;
        next_voxel[Z] = voxel[Z] + invjacobian[Z][Z] * min_jump;
        next_ivoxel[X] = VXEL(next_voxel[X], invjacobian[Z][X]);
        next_ivoxel[Y] = VXEL(next_voxel[Y], invjacobian[Z][Y]);
        next_ivoxel[Z] = VXEL(next_voxel[Z], invjacobian[Z][Z]);

        while (level < starting_level) {
            if (next_ivoxel[X] >> (level + 1) != ivoxel[X] >> (level + 1) || next_ivoxel[Y] >> (level + 1) != ivoxel[Y] >> (level + 1) || next_ivoxel[Z] >> (level + 1) != ivoxel[Z] >> (level + 1)) {
                level++;
            } else {
                break;
            }
        }

        voxel[X] = next_voxel[X];
        voxel[Y] = next_voxel[Y];
        voxel[Z] = next_voxel[Z];
        ivoxel[X] = next_ivoxel[X];
        ivoxel[Y] = next_ivoxel[Y];
        ivoxel[Z] = next_ivoxel[Z];
    }

    span_zmin = ROUNDUP(box_zmin);
    span_zmax = MIN((int)box_zmax, segment_zmax);

    if (span_zmax < span_zmin) {
        goto next_box;
    }

    sample[X] = ray[0][X] + invjacobian[Z][X] * span_zmin;
    sample[Y] = ray[0][Y] + invjacobian[Z][Y] * span_zmin;
    sample[Z] = ray[0][Z] + invjacobian[Z][Z] * span_zmin;

    for (outz = span_zmin; outz <= span_zmax; outz++) {

        samplex = (int)sample[X];
        sampley = (int)sample[Y];
        samplez = (int)sample[Z];

        bit = PYR(0, samplez, sampley, samplex);
        if (!bit) {
            goto end_of_sample;
        }

        sample2[X] = MIN(sample[X], in_max[X]);
        sample2[Y] = MIN(sample[Y], in_max[Y]);
        sample2[Z] = MIN(sample[Z], in_max[Z]);

        sample2x = (int)sample2[X];
        sample2y = (int)sample2[Y];
        sample2z = (int)sample2[Z];

        xalpha = sample2[X] - sample2x++;
        yalpha = sample2[Y] - sample2y;
        zalpha = sample2[Z] - sample2z;

        local_opc_address = OPC_ADDRESS(sample2z, sample2y, sample2x);
        local_norm_address = NORM_ADDRESS(sample2z, sample2y, sample2x, Z);

        one_minus_xalpha = 1.0 - xalpha;
        one_minus_yalpha = 1.0 - yalpha;
        one_minus_zalpha = 1.0 - zalpha;

        weight = xalpha * one_minus_yalpha * one_minus_zalpha;
        wopacity = *local_opc_address-- * weight;
        color = SHD(*local_norm_address--);
        wcolorsum = color * wopacity;
        wopacitysum = wopacity;

        weight = one_minus_xalpha * one_minus_yalpha * one_minus_zalpha;
        wopacity = *local_opc_address * weight;
        color = SHD(*local_norm_address);
        wcolorsum += color * wopacity;
        wopacitysum += wopacity;

        weight = xalpha * yalpha * one_minus_zalpha;
        local2_opc_address = local_opc_address + opc_xlen;
        local2_norm_address = local_norm_address + norm_xlen;
        wopacity = *local2_opc_address-- * weight;
        color = SHD(*local2_norm_address--);
        wcolorsum += color * wopacity;
        wopacitysum += wopacity;

        weight = one_minus_xalpha * yalpha * one_minus_zalpha;
        wopacity = *local2_opc_address * weight;
        color = SHD(*local2_norm_address);
        wcolorsum += color * wopacity;
        wopacitysum += wopacity;

        weight = xalpha * one_minus_yalpha * zalpha;
        local_opc_address = local_opc_address + opc_xylen;
        local_norm_address = local_norm_address + norm_xylen;
        wopacity = *local_opc_address-- * weight;
        color = SHD(*local_norm_address--);
        wcolorsum += color * wopacity;
        wopacitysum += wopacity;

        weight = one_minus_xalpha * one_minus_yalpha * zalpha;
        wopacity = *local_opc_address * weight;
        color = SHD(*local_norm_address);
        wcolorsum += color * wopacity;
        wopacitysum += wopacity;

        weight = xalpha * yalpha * zalpha;
        local2_opc_address = local_opc_address + opc_xlen;
        local2_norm_address = local_norm_address + norm_xlen;
        wopacity = *local2_opc_address-- * weight;
        color = SHD(*local2_norm_address--);
        wcolorsum += color * wopacity;
        wopacitysum += wopacity;

        weight = one_minus_xalpha * yalpha * zalpha;
        wopacity = *local2_opc_address * weight;
        color = SHD(*local2_norm_address);
        wcolorsum += color * wopacity;
        wopacitysum += wopacity;

        opacity = wopacitysum * INV_MAX_OPC;
        color = wcolorsum * INV_MAX_OPC;

        color *= depth_cueing[outz];

        additional_opacity = opacity * (1.0 - ray_opacity);
        ray_color += color * (1.0 - ray_opacity);
        ray_opacity += additional_opacity;

        if (ray_opacity > opacity_cutoff) {
            goto end_of_ray;
        }

    end_of_sample:;
        sample[X] += invjacobian[Z][X];
        sample[Y] += invjacobian[Z][Y];
        sample[Z] += invjacobian[Z][Z];
    }

    goto next_box;

end_of_segment:;

    if (ray_opacity <= opacity_epsilon) {
        return;
    }

end_of_ray:;

    additional_opacity = 1.0 - ray_opacity;
    ray_color += *pixel_address * additional_opacity;
    *pixel_address = NINT(ray_color);
}

Pre_Shade(int my_node) {
    int xnorm, ynorm, znorm, table_addr, norm_lshift, i;
    int shd_table_partition, zstart, zstop;
    float mag, error, inv_num_nodes;
    float normal[NM];
    float dot_product, diffuse, specular, color;
    float dpartial_product1, dpartial_product2;
    float spartial_product1, spartial_product2;
    BOOLEAN *local_sbit_address;
    int temp;

    inv_num_nodes = 1.0 / (float)num_nodes;

    norm_lshift = NORM_LSHIFT;
    error = -2.0 * NORM_RSHIFT * NORM_RSHIFT;

    shd_table_partition = ROUNDUP((float)LOOKUP_PREC * inv_num_nodes);
    zstart = -norm_lshift + shd_table_partition * my_node;
    zstop = MIN(zstart + shd_table_partition, norm_lshift + 1);

    for (znorm = zstart; znorm < zstop; znorm++) {
        for (ynorm = -norm_lshift; ynorm <= norm_lshift; ynorm++) {
            normal[Z] = (float)znorm * NORM_RSHIFT;
            normal[Y] = (float)ynorm * NORM_RSHIFT;
            mag = 1.0 - normal[Z] * normal[Z] - normal[Y] * normal[Y];
            if (mag > error) {
                mag = MAX(mag, 0.0);
                normal[X] = sqrt(mag);
                xnorm = (int)normal[X];
                dpartial_product1 = normal[Z] * obslight[Z] + normal[Y] * obslight[Y];
                dpartial_product2 = normal[X] * obslight[X];
                spartial_product1 = normal[Z] * obshighlight[Z] + normal[Y] * obshighlight[Y];
                spartial_product2 = normal[X] * obshighlight[X];
                table_addr = LOOKUP_HSIZE + (znorm * LOOKUP_PREC + ynorm) * 2;
                dot_product = dpartial_product1 + dpartial_product2;
                diffuse = MAX(dot_product, 0.0);
                dot_product = spartial_product1 + spartial_product2;
                specular = MAX(dot_product, 0.0);
                specular = pow(specular, specular_exponent);
                color = ambient_color + diffuse * diffuse_color + specular * specular_color;
                color = NINT(MIN(color, MAX_PIXEL));
                temp = (int)color;
                SHD(table_addr + 1) = (unsigned char)temp;
                if (normal[X] > 0.0) {
                    dot_product = dpartial_product1 - dpartial_product2;
                    diffuse = MAX(dot_product, 0.0);
                    dot_product = spartial_product1 - spartial_product2;
                    specular = MAX(dot_product, 0.0);
                    specular = pow(specular, specular_exponent);
                    color = ambient_color + diffuse * diffuse_color + specular * specular_color;
                    color = NINT(MIN(color, MAX_PIXEL));
                }
                temp = (int)color;
                SHD(table_addr) = (unsigned char)temp;
            }
        }
    }
    table_addr = LOOKUP_HSIZE + (norm_lshift * LOOKUP_PREC + 2) * 2 + 1;
    temp = (int)ambient_color;
    SHD(table_addr) = (unsigned char)temp;
}
