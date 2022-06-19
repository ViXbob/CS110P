#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// #include <algorithm>
#include <sys/time.h>
#include <time.h>
#include <immintrin.h>
#include <assert.h>
//inplement dymanic

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define PI 3.14159
#define max(a, b) ((a) > (b) ? (a) : (b))

unsigned int extended = 0;

typedef struct FVec
{
    unsigned int length;
    unsigned int min_length;
    unsigned int min_deta;
    float* data;
    float* sum;
    float* ratio;
} FVec;

typedef struct Image
{
    unsigned int dimX, dimY, numChannels;
    float* data;
} Image;

void normalize_FVec(FVec v)
{
    // float sum = 0.0;
    unsigned int i,j;
    int ext = v.length / 2;
    float *ptr, tmp;
    v.sum[0] = v.data[ext];
    for (i = ext+1,j=1; i < v.length; i++,j++)
    {
        v.sum[j] = v.sum[j-1] + v.data[i]*2;
    }
    // assert(j == v.length / 2 + 1);
    #pragma omp parallel for
    for (j = 0; j < v.length / 2 + 1; j++) {
        for(i = 0; i < v.length; i++) {
            ptr = v.ratio + 3 * (j * v.length + i);
            tmp = v.data[i] / v.sum[j];
            ptr[0] = tmp;
            ptr[1] = tmp;
            ptr[2] = tmp;
        }
    }
    // for (i = 0; i <= ext; i++)
    // {
    //      v.data[i] /= v.sum[v.length - ext - 1 ] ;
    //      printf("%lf ",v.sum[i]);
    // }
}

float* get_pixel(Image img, int x, int y)
{
    if (x < 0)
    {
        x = 0;
    }
    if (x >= img.dimX)
    {
        x = img.dimX - 1;
    }
    if (y < 0)
    {
        y = 0;
    }
    if (y >= img.dimY)
    {
        y = img.dimY - 1;
    }
    return img.data + img.numChannels * (y * img.dimX + x);
}

float gd(float a, float b, float x)
{
    float c = (x-b) / a;
    return exp((-.5) * c * c) / (a * sqrt(2 * PI));
}

FVec make_gv(float a, float x0, float x1, unsigned int length, unsigned int min_length)
{
    FVec v;
    v.length = length;
    v.min_length = min_length;
    if(v.min_length > v.length){
        v.min_deta = 0;
    }else{
        v.min_deta = ((v.length - v.min_length) / 2);
    }
    v.data = malloc(length * sizeof(float));
    v.sum = malloc((length / 2 + 1)* sizeof(float));
    v.ratio = malloc(3 * length * (length / 2 + 1) * sizeof(float));
    float step = (x1 - x0) / ((float)length);
    int offset = length/2;
    for (int i = 0; i < length; i++)
    {
        v.data[i] = gd(a, 0.0f, (i-offset)*step);
    }
    normalize_FVec(v);
    return v; 
}

void print_fvec(FVec v)
{
    unsigned int i;
    printf("\n");
    for (i = 0; i < v.length; i++)
    {
        printf("%f ", v.data[i]);
    }
    printf("\n");
}

Image img_sc(Image a)
{
    Image b = a;
    b.data = malloc(b.dimX * b.dimY * b.numChannels * sizeof(float));
    return b;
}

Image gb_h(Image a, FVec gv)
{
    Image b = img_sc(a);

    int ext = gv.length / 2;
    int offset;
    unsigned int x, y, channel;
    float *pc, *pnow, *ptmp;
    float sum[24], tmp;
    int i;
    __m256 pixel_a, pixel_b, pixel_c, gauss_a, gauss_b, gauss_c, result_a, result_b, result_c;

        #pragma omp parallel for
        for (y = 0; y < a.dimY; y++)
        {    
            for (x = 0; x < a.dimX; x++)
            {
                pc = get_pixel(b, x, y);
                unsigned int deta = fmin(fmin(a.dimY-y-1, y),fmin(a.dimX-x-1, x));
                deta = fmin(deta, gv.min_deta);
                // for (channel = 0; channel < a.numChannels; channel++) sum[channel] = 0;
                // pc[0] = 0;
                // pc[1] = 0;
                // pc[2] = 0;
                ptmp = gv.ratio + 3 * ((ext - deta) * gv.length + deta);

                for(i = deta; i < gv.length-deta; i++, ptmp += 3) {
                    offset = i - ext;
                    pnow = get_pixel(a, x + offset, y);
                    // tmp = gv.data[i]/gv.sum[ext - deta];
                    pc[0] += ptmp[0] * pnow[0];
                    pc[1] += ptmp[1] * pnow[1];
                    pc[2] += ptmp[2] * pnow[2];
                }
                // for (channel = 0; channel < a.numChannels; channel++) pc[channel] = sum[channel];
                // pc[0] = sum[0];
            }
        }

    return b;
}

Image gb_h_avx(Image a, FVec gv, Image c) {
    Image b = img_sc(a);

    int ext = gv.length / 2;
    int offset;
    unsigned int x, y, channel;
    float *pc, *pnow, *ptmp, *pa, *pb;
    float sum[24], tmp[8];
    int i, j;
    __m256 pixel_a, pixel_b, pixel_c, gauss_a, gauss_b, gauss_c, result_a, result_b, result_c;
    __m128 tmp_128;
    __m256 tmp_256;

        #pragma omp parallel for private(sum, pixel_a, pixel_b, pixel_c, gauss_a, gauss_b, gauss_c, pc, pnow, ptmp, offset, x, y, i, j, tmp, tmp_128, tmp_256)
        for (y = 0; y < a.dimY; y++)
        {    
            for (x = 0; x < a.dimX; x++)
            {
                pc = get_pixel(b, x, y);
                unsigned int deta = fmin(fmin(a.dimY-y-1, y),fmin(a.dimX-x-1, x));
                deta = fmin(deta, gv.min_deta);
                // for (channel = 0; channel < a.numChannels; channel++) sum[channel] = 0;
                // pc[0] = 0;
                // pc[1] = 0;
                // pc[2] = 0;
                // tmp[0] = 0;
                // tmp[1] = 0;
                // tmp[2] = 0;
                // tmp[3] = 0;
                // tmp_128 = _mm_loadu_ps(tmp);
                ptmp = gv.ratio + 3 * ((ext - deta) * gv.length + deta);
                pnow = c.data + 3 * (y * c.dimX + x + extended + deta - ext);
                for (i = deta; i + 8 < gv.length-deta; i += 8, ptmp += 24, pnow += 24)
                {
                    // offset = i - ext;
                    // pnow = get_pixel(c, x + extended + offset, y);
                    // for (j = i; j < i + 8; j++) {
                    //     pa = get_pixel(a, x + j - ext, y);
                    //     pb = pnow + 3 * (j - i);
                    //     assert(fabs(pa[0] - pb[0]) < 1e-10);
                    //     assert(fabs(pa[1] - pb[1]) < 1e-10);
                    //     assert(fabs(pa[2] - pb[2]) < 1e-10);
                    // }
                    // tmp = gv.data[i]/gv.sum[ext - deta];
                    // pixel_a = _mm256_loadu_ps(pnow);
                    // pixel_b = _mm256_loadu_ps(pnow + 8);
                    // pixel_c = _mm256_loadu_ps(pnow + 16);

                    // gauss_a = _mm256_loadu_ps(ptmp);
                    // gauss_b = _mm256_loadu_ps(ptmp + 8);
                    // gauss_c = _mm256_loadu_ps(ptmp + 16);

                    _mm256_storeu_ps(sum, _mm256_mul_ps(_mm256_loadu_ps(pnow), _mm256_loadu_ps(ptmp)));
                    _mm256_storeu_ps(sum + 8, _mm256_mul_ps(_mm256_loadu_ps(pnow + 8), _mm256_loadu_ps(ptmp + 8)));
                    _mm256_storeu_ps(sum + 16, _mm256_mul_ps(_mm256_loadu_ps(pnow + 16), _mm256_loadu_ps(ptmp + 16)));
                    // for (j = i; j < i + 8; j++) {
                    //     pa = get_pixel(a, x + j - ext, y);
                    //     pb = pnow + 3 * (j - i);
                    //     assert(fabs(sum[(j - i) * 3 + 0] - pa[0] * ptmp[(j - i) * 3 + 0]) < 1e-5);
                    //     assert(fabs(sum[(j - i) * 3 + 1] - pa[1] * ptmp[(j - i) * 3 + 1]) < 1e-5);
                    //     assert(fabs(sum[(j - i) * 3 + 2] - pa[2] * ptmp[(j - i) * 3 + 2]) < 1e-5);
                    // }
                    // for(j = 0; j < 24; j += 3) {
                    //     pc[0] += sum[j];
                    //     pc[1] += sum[j + 1];
                    //     pc[2] += sum[j + 2];
                    // }
                    
                    // tmp_128 = _mm_add_ps(tmp_128, _mm_loadu_ps(sum));
                    // tmp_128 = _mm_add_ps(tmp_128, _mm_loadu_ps(sum + 3));
                    // tmp_128 = _mm_add_ps(tmp_128, _mm_loadu_ps(sum + 6));
                    // tmp_128 = _mm_add_ps(tmp_128, _mm_loadu_ps(sum + 9));
                    // tmp_128 = _mm_add_ps(tmp_128, _mm_loadu_ps(sum + 12));
                    // tmp_128 = _mm_add_ps(tmp_128, _mm_loadu_ps(sum + 15));
                    // tmp_128 = _mm_add_ps(tmp_128, _mm_loadu_ps(sum + 18));
                    // tmp_128 = _mm_add_ps(tmp_128, _mm_loadu_ps(sum + 21));

                    pc[0] += sum[0];
                    pc[1] += sum[1];
                    pc[2] += sum[2];

                    pc[0] += sum[3];
                    pc[1] += sum[4];
                    pc[2] += sum[5];

                    pc[0] += sum[6];
                    pc[1] += sum[7];
                    pc[2] += sum[8];

                    pc[0] += sum[9];
                    pc[1] += sum[10];
                    pc[2] += sum[11];

                    pc[0] += sum[12];
                    pc[1] += sum[13];
                    pc[2] += sum[14];

                    pc[0] += sum[15];
                    pc[1] += sum[16];
                    pc[2] += sum[17];

                    pc[0] += sum[18];
                    pc[1] += sum[19];
                    pc[2] += sum[20];

                    pc[0] += sum[21];
                    pc[1] += sum[22];
                    pc[2] += sum[23];
                }
                // _mm_storeu_ps(tmp, tmp_128);
                // pc[0] = tmp[0];
                // pc[1] = tmp[1];
                // pc[2] = tmp[2];

                for(; i < gv.length-deta; i++, ptmp += 3, pnow += 3) {
                    // offset = i - ext;
                    // pnow = get_pixel(a, x + offset, y);
                    // tmp = gv.data[i]/gv.sum[ext - deta];
                    pc[0] += ptmp[0] * pnow[0];
                    pc[1] += ptmp[1] * pnow[1];
                    pc[2] += ptmp[2] * pnow[2];
                }
                // for (channel = 0; channel < a.numChannels; channel++) pc[channel] = sum[channel];
                // pc[0] = sum[0];
            }
        }

    return b;
}

Image gb_h_fma(Image a, FVec gv, Image c) {
    Image b = img_sc(a);

    int ext = gv.length / 2;
    int offset;
    unsigned int x, y, channel;
    float *pc, *pnow, *ptmp, *pa, *pb;
    float sum[24], tmp[8];
    int i, j;
    __m256 pixel_a, pixel_b, pixel_c, gauss_a, gauss_b, gauss_c, result_a, result_b, result_c;
    __m128 tmp_128;
    __m256 tmp_256;

        #pragma omp parallel for private(sum, pixel_a, pixel_b, pixel_c, gauss_a, gauss_b, gauss_c, pc, pnow, ptmp, offset, x, y, i, j, tmp, tmp_128, tmp_256)
        for (y = 0; y < a.dimY; y++)
        {    
            for (x = 0; x < a.dimX; x++)
            {
                pc = get_pixel(b, x, y);
                unsigned int deta = fmin(fmin(a.dimY-y-1, y),fmin(a.dimX-x-1, x));
                deta = fmin(deta, gv.min_deta);
                tmp[0] = 0;
                tmp[1] = 0;
                tmp[2] = 0;
                tmp[3] = 0;
                tmp[4] = 0;
                tmp[5] = 0;
                tmp_256 = _mm256_loadu_ps(tmp);
                ptmp = gv.ratio + 3 * ((ext - deta) * gv.length + deta);
                pnow = c.data + 3 * (y * c.dimX + x + extended + deta - ext);
                for (i = deta; i + 8 < gv.length-deta; i += 8, ptmp += 24, pnow += 24)
                {
                    tmp_256 = _mm256_fmadd_ps(_mm256_loadu_ps(pnow), _mm256_loadu_ps(ptmp), tmp_256);
                    tmp_256 = _mm256_fmadd_ps(_mm256_loadu_ps(pnow + 6), _mm256_loadu_ps(ptmp + 6), tmp_256);
                    tmp_256 = _mm256_fmadd_ps(_mm256_loadu_ps(pnow + 12), _mm256_loadu_ps(ptmp + 12), tmp_256);
                    tmp_256 = _mm256_fmadd_ps(_mm256_loadu_ps(pnow + 18), _mm256_loadu_ps(ptmp + 18), tmp_256);
                }
                _mm256_storeu_ps(tmp, tmp_256);
                pc[0] += tmp[0];
                pc[1] += tmp[1];
                pc[2] += tmp[2];

                pc[0] += tmp[3];
                pc[1] += tmp[4];
                pc[2] += tmp[5];

                for(; i < gv.length-deta; i++, ptmp += 3, pnow += 3) {
                    pc[0] += ptmp[0] * pnow[0];
                    pc[1] += ptmp[1] * pnow[1];
                    pc[2] += ptmp[2] * pnow[2];
                }
            }
        }

    return b;
}

Image gb_h_fma_3_8(Image a, FVec gv, Image c) {
    Image b = img_sc(a);

    int ext = gv.length / 2;
    int offset;
    unsigned int x, y, channel;
    float *pc, *pnow, *ptmp, *pa, *pb;
    float sum[24];
    int i, j;
    __m256 sum0_256, sum1_256, sum2_256;

        #pragma omp parallel for private(sum, sum0_256, sum1_256, sum2_256, pc, pnow, ptmp, offset, x, y, i, j)
        for (y = 0; y < a.dimY; y++)
        {    
            for (x = 0; x < a.dimX; x++)
            {
                pc = get_pixel(b, x, y);
                unsigned int deta = fmin(fmin(a.dimY-y-1, y),fmin(a.dimX-x-1, x));
                deta = fmin(deta, gv.min_deta);
                sum[0] = 0; sum[1] = 0; sum[2] = 0; sum[3] = 0; sum[4] = 0; sum[5] = 0; sum[6] = 0;
                sum[7] = 0; sum[8] = 0; sum[9] = 0; sum[10] = 0; sum[11] = 0; sum[12] = 0; sum[13] = 0;
                sum[14] = 0; sum[15] = 0; sum[16] = 0; sum[17] = 0; sum[18] = 0; sum[19] = 0; sum[20] = 0;
                sum[21] = 0; sum[22] = 0; sum[23] = 0;
                sum0_256 = _mm256_loadu_ps(sum);
                sum1_256 = _mm256_loadu_ps(sum + 8);
                sum2_256 = _mm256_loadu_ps(sum + 16);
                ptmp = gv.ratio + 3 * ((ext - deta) * gv.length + deta);
                pnow = c.data + 3 * (y * c.dimX + x + extended + deta - ext);
                for (i = deta; i + 8 < gv.length-deta; i += 8, ptmp += 24, pnow += 24)
                {
                    sum0_256 = _mm256_fmadd_ps(_mm256_loadu_ps(pnow), _mm256_loadu_ps(ptmp), sum0_256);
                    sum1_256 = _mm256_fmadd_ps(_mm256_loadu_ps(pnow + 8), _mm256_loadu_ps(ptmp + 8), sum1_256);
                    sum2_256 = _mm256_fmadd_ps(_mm256_loadu_ps(pnow + 16), _mm256_loadu_ps(ptmp + 16), sum2_256);
                }
                _mm256_storeu_ps(sum, sum0_256);
                _mm256_storeu_ps(sum + 8, sum1_256);
                _mm256_storeu_ps(sum + 16, sum2_256);
                pc[0] += sum[0];
                pc[1] += sum[1];
                pc[2] += sum[2];

                pc[0] += sum[3];
                pc[1] += sum[4];
                pc[2] += sum[5];

                pc[0] += sum[6];
                pc[1] += sum[7];
                pc[2] += sum[8];

                pc[0] += sum[9];
                pc[1] += sum[10];
                pc[2] += sum[11];

                pc[0] += sum[12];
                pc[1] += sum[13];
                pc[2] += sum[14];

                pc[0] += sum[15];
                pc[1] += sum[16];
                pc[2] += sum[17];

                pc[0] += sum[18];
                pc[1] += sum[19];
                pc[2] += sum[20];

                pc[0] += sum[21];
                pc[1] += sum[22];
                pc[2] += sum[23];

                for(; i < gv.length-deta; i++, ptmp += 3, pnow += 3) {
                    pc[0] += ptmp[0] * pnow[0];
                    pc[1] += ptmp[1] * pnow[1];
                    pc[2] += ptmp[2] * pnow[2];
                }
            }
        }

    return b;
}

Image gb_h_fma_3_8_without_extend(Image a, FVec gv) {
    Image b = img_sc(a);

    int ext = gv.length / 2;
    int offset;
    unsigned int x, y, channel, upper;
    float *pc, *pnow, *ptmp, *pa, *pb, tmp, *tmp_pointer;
    float sum[24];
    int i, j;
    __m256 sum0_256, sum1_256, sum2_256;

        #pragma omp parallel for private(sum, sum0_256, sum1_256, sum2_256, pc, pnow, ptmp, offset, x, y, i, j, tmp, upper, tmp_pointer) schedule (dynamic)
        for (y = 0; y < a.dimY; y++)
        {
            pc = b.data + 3 * (y * b.dimX);
            for (x = 0; x < a.dimX; x++)
            {
                unsigned int deta = fmin(fmin(a.dimY-y-1, y),fmin(a.dimX-x-1, x));
                deta = fmin(deta, gv.min_deta);
                sum[0] = 0; sum[1] = 0; sum[2] = 0; sum[3] = 0; sum[4] = 0; sum[5] = 0; 
                sum[6] = 0; sum[7] = 0; sum[8] = 0; sum[9] = 0; sum[10] = 0; sum[11] = 0; 
                sum[12] = 0; sum[13] = 0; sum[14] = 0; sum[15] = 0; sum[16] = 0; sum[17] = 0; 
                sum[18] = 0; sum[19] = 0; sum[20] = 0; sum[21] = 0; sum[22] = 0; sum[23] = 0;
                sum0_256 = _mm256_loadu_ps(sum);
                sum1_256 = _mm256_loadu_ps(sum + 8);
                sum2_256 = _mm256_loadu_ps(sum + 16);
                assert(ext >= deta);
                ptmp = gv.ratio + 3 * ((ext - deta) * gv.length + deta);
                pnow = a.data + 3 * (y * a.dimX + x + deta) - 3 * ext;
                i = deta;
                upper = gv.length - deta;
                // adjust lower bound of i
                // x + deta - ext < 0
                if(x + deta < ext) {
                    // ext - x - deta >= 0
                    pnow += (ext - x - deta) * 3;
                    ptmp += (ext - x - deta) * 3;
                    i += (ext - x - deta);
                    // x + deta - ext < 0
                    // [x + deta - ext, 0)
                    // [deta, ext - x)
                    // tmp = gv.sum[ext - x - 1] - gv.sum[deta - 1]; 
                    // (x, ext - deta]
                    tmp = (gv.sum[ext - deta] - gv.sum[x]) * 0.5 / gv.sum[ext - deta];
                    pc[0] += tmp * pnow[0];
                    pc[1] += tmp * pnow[1];
                    pc[2] += tmp * pnow[2];
                }
                // adjust upper bound of i
                if(x + upper - ext >= a.dimX) {
                    // [a.dimX, x + upper - ext)
                    // [a.dimX - x, upper - ext)
                    tmp = (gv.sum[upper - ext - 1] - gv.sum[a.dimX - x - 1]) * 0.5 / gv.sum[ext - deta];
                    upper -= (x + upper - ext - a.dimX);
                    tmp_pointer = a.data + 3 * (y * a.dimX + a.dimX - 1);
                    pc[0] += tmp * tmp_pointer[0];
                    pc[1] += tmp * tmp_pointer[1];
                    pc[2] += tmp * tmp_pointer[2];
                }
                for (; i + 8 < upper; i += 8, ptmp += 24, pnow += 24)
                {
                    sum0_256 = _mm256_fmadd_ps(_mm256_loadu_ps(pnow), _mm256_loadu_ps(ptmp), sum0_256);
                    sum1_256 = _mm256_fmadd_ps(_mm256_loadu_ps(pnow + 8), _mm256_loadu_ps(ptmp + 8), sum1_256);
                    sum2_256 = _mm256_fmadd_ps(_mm256_loadu_ps(pnow + 16), _mm256_loadu_ps(ptmp + 16), sum2_256);
                }
                _mm256_storeu_ps(sum, sum0_256);
                _mm256_storeu_ps(sum + 8, sum1_256);
                _mm256_storeu_ps(sum + 16, sum2_256);
                pc[0] += sum[0]; pc[1] += sum[1]; pc[2] += sum[2]; pc[0] += sum[3]; pc[1] += sum[4]; pc[2] += sum[5];
                pc[0] += sum[6]; pc[1] += sum[7]; pc[2] += sum[8]; pc[0] += sum[9]; pc[1] += sum[10]; pc[2] += sum[11];
                pc[0] += sum[12]; pc[1] += sum[13]; pc[2] += sum[14]; pc[0] += sum[15]; pc[1] += sum[16]; pc[2] += sum[17];
                pc[0] += sum[18]; pc[1] += sum[19]; pc[2] += sum[20]; pc[0] += sum[21]; pc[1] += sum[22]; pc[2] += sum[23];

                for(; i < upper; i++, ptmp += 3, pnow += 3) {
                    pc[0] += ptmp[0] * pnow[0];
                    pc[1] += ptmp[1] * pnow[1];
                    pc[2] += ptmp[2] * pnow[2];
                }

                pc += 3;
            }
        }

    return b;
}

Image gb_v(Image a, FVec gv)
{
    Image b = img_sc(a);

    int ext = gv.length / 2;
    int offset;
    unsigned int x, y, channel;
    float *pc, *pnow;
    float sum, tmp;
    int i;
    // for (channel = 0; channel < a.numChannels; channel++)
    // {
        #pragma omp parallel for
        for (x = 0; x < a.dimX; x++)
        {
            for (y = 0; y < a.dimY; y++)
            {
                pc = get_pixel(b, x, y);
                unsigned int deta = fmin(fmin(a.dimY-y-1, y),fmin(a.dimX-x-1, x));
                deta = fmin(deta, gv.min_deta);
                sum = 0;
                for (i = deta; i < gv.length-deta; i++)
                {
                    offset = i - ext;
                    tmp = gv.data[i] /gv.sum[ext - deta];
                    pnow = get_pixel(a, x, y + offset);
                    pc[0] += tmp * pnow[0];
                    pc[1] += tmp * pnow[1];
                    pc[2] += tmp * pnow[2];
                }
                // pc[channel] = sum;
            }
        }
    // }
    return b;
}

// b = a^T
Image naive_transpose(Image a) {
    unsigned int x, y;
    Image b;
    b.dimX = a.dimY;
    b.dimY = a.dimX;
    b.numChannels = a.numChannels;
    b.data = malloc(b.dimX * b.dimY * b.numChannels * sizeof(float));
    float *pc_a, *pc_b;
    #pragma omp parallel for private(pc_b, pc_a)
    for(x = 0; x < a.dimX; x++) {
        for(y = 0; y < a.dimY; y++) {
            pc_a = get_pixel(a, x, y);
            pc_b = get_pixel(b, y, x);
            pc_b[0] = pc_a[0];
            pc_b[1] = pc_a[1];
            pc_b[2] = pc_a[2];
        }
    }
    return b;
}

Image block_transpose(Image a) {
    static unsigned int blocksize = 200;
    unsigned int x, y, i, j;
    Image b;
    b.dimX = a.dimY;
    b.dimY = a.dimX;
    b.numChannels = a.numChannels;
    b.data = malloc(b.dimX * b.dimY * b.numChannels * sizeof(float));
    float *pc_a, *pc_b;
    #pragma omp parallel for private(pc_b, pc_a, i, j, x, y)
    for (x = 0; x < a.dimX; x += blocksize) {
        for (y = 0; y < a.dimY; y += blocksize) {
            // dst[y + x * n] = src[x + y * n];
            for (i = x; i < x + blocksize && i < a.dimX; i++){
                for (j = y; j < y + blocksize && j < a.dimY; j++){
                    // pc_a = get_pixel(a, i, j);
                    // pc_b = get_pixel(b, j, i);
                    pc_a = a.data + 3 * (j * a.dimX + i);
                    pc_b = b.data + 3 * (i * b.dimX + j);
                    pc_b[0] = pc_a[0];
                    pc_b[1] = pc_a[1];
                    pc_b[2] = pc_a[2];
                }
            }
        }
    }
    return b;
}

Image extend_to_avx(Image a, FVec gv) {
    Image b = a;
    unsigned int x, y;
    float *pc, *pnow;
    b.dimX += extended * 2;
    b.data = malloc(b.dimX * b.dimY * b.numChannels * sizeof(float));
    
    #pragma omp parallel for private(pc, pnow, x, y)
    for(y = 0; y < b.dimY; y++) {
        pnow = a.data + 3 * (y * a.dimX);
        pc = b.data + 3 * (y * b.dimX);
        for(x = 0; x < extended; x++, pc += 3) {
            // assert(pnow == get_pixel(a, x - extended, y));
            // assert(pc == get_pixel(b, x, y));
            pc[0] = pnow[0];
            pc[1] = pnow[1];
            pc[2] = pnow[2];
        }
        for( ; x < b.dimX - extended; x++, pc += 3, pnow += 3) {
            // assert(pnow == get_pixel(a, x - extended, y));
            pc[0] = pnow[0];
            pc[1] = pnow[1];
            pc[2] = pnow[2];
        }
        pnow -= 3;
        for( ; x < b.dimX; x++, pc += 3) {
            pc[0] = pnow[0];
            pc[1] = pnow[1];
            pc[2] = pnow[2];
        }
    }

    return b;
}

Image apply_gb(Image a, FVec gv)
{
    struct timeval start_time, stop_time, elapsed_time; 
    gettimeofday(&start_time,NULL);
    Image b = gb_h(a, gv);
    gettimeofday(&stop_time,NULL);
    timersub(&stop_time, &start_time, &elapsed_time); 
    printf("time of gb_h: %f \n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);
    gettimeofday(&start_time,NULL);
    Image d = naive_transpose(b);
    Image e = gb_h(d, gv);
    Image c = naive_transpose(e);
    // Image c = gb_v(b, gv);
    free(d.data);
    free(e.data);
    gettimeofday(&stop_time,NULL);
    timersub(&stop_time, &start_time, &elapsed_time); 
    printf("time of gb_v: %f \n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);
    free(b.data);
    return c;
}

Image apply_gb_with_avx(Image a, FVec gv)
{
    struct timeval start_time, stop_time, elapsed_time; 
    gettimeofday(&start_time,NULL);
    Image b_avx = extend_to_avx(a, gv);
    Image b = gb_h_avx(a, gv, b_avx);
    gettimeofday(&stop_time,NULL);
    timersub(&stop_time, &start_time, &elapsed_time); 
    printf("time of gb_h: %f \n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);
    gettimeofday(&start_time,NULL);
    Image d = block_transpose(b);
    Image d_avx = extend_to_avx(d, gv);
    Image e = gb_h_avx(d, gv, d_avx);
    Image c = block_transpose(e);
    // Image c = gb_v(b, gv);
    gettimeofday(&stop_time,NULL);
    timersub(&stop_time, &start_time, &elapsed_time); 
    printf("time of gb_v: %f \n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);
    free(d.data);
    free(e.data);
    free(b_avx.data);
    free(d_avx.data);
    free(b.data);
    return c;
}

Image apply_gb_with_fma(Image a, FVec gv)
{
    struct timeval start_time, stop_time, elapsed_time; 
    gettimeofday(&start_time,NULL);
    Image b_avx = extend_to_avx(a, gv);
    Image b = gb_h_fma_3_8(a, gv, b_avx);
    gettimeofday(&stop_time,NULL);
    timersub(&stop_time, &start_time, &elapsed_time); 
    printf("time of gb_h: %f \n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);
    gettimeofday(&start_time,NULL);
    Image d = block_transpose(b);
    Image d_avx = extend_to_avx(d, gv);
    Image e = gb_h_fma_3_8(d, gv, d_avx);
    Image c = block_transpose(e);
    // Image c = gb_v(b, gv);
    gettimeofday(&stop_time,NULL);
    timersub(&stop_time, &start_time, &elapsed_time); 
    printf("time of gb_v: %f \n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);
    free(d.data);
    free(e.data);
    free(b_avx.data);
    free(d_avx.data);
    free(b.data);
    return c;
}

Image apply_gb_with_fma_without_extend(Image a, FVec gv)
{
    struct timeval start_time, stop_time, elapsed_time; 
    gettimeofday(&start_time,NULL);
    Image b = gb_h_fma_3_8_without_extend(a, gv);
    gettimeofday(&stop_time,NULL);
    timersub(&stop_time, &start_time, &elapsed_time); 
    printf("time of gb_h: %f \n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);
    gettimeofday(&start_time,NULL);
    Image d = block_transpose(b);
    Image e = gb_h_fma_3_8_without_extend(d, gv);
    Image c = block_transpose(e);
    gettimeofday(&stop_time,NULL);
    timersub(&stop_time, &start_time, &elapsed_time); 
    printf("time of gb_v: %f \n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);
    free(d.data);
    free(e.data);
    free(b.data);
    return c;
}

int main(int argc, char** argv)
{
    struct timeval start_time, stop_time, elapsed_time; 
    gettimeofday(&start_time,NULL);
    if (argc < 6)
    {
        printf("Usage: ./gb.exe <inputjpg> <outputname> <float: a> <float: x0> <float: x1> <unsigned int: dim>\n");
        exit(0);
    }

    float a, x0, x1;
    unsigned int dim, min_dim;

    sscanf(argv[3], "%f", &a);
    sscanf(argv[4], "%f", &x0);
    sscanf(argv[5], "%f", &x1);
    sscanf(argv[6], "%u", &dim);
    sscanf(argv[7], "%u", &min_dim);

    FVec v = make_gv(a, x0, x1, dim, min_dim);
    // print_fvec(v);
    Image img;
    img.data = stbi_loadf(argv[1], &(img.dimX), &(img.dimY), &(img.numChannels), 0);
    extended = dim / 2;

    Image imgOut = apply_gb_with_fma_without_extend(img, v);
    // puts("ok~");
    // printf("%s %u %u %u\n", argv[2], imgOut.dimX, imgOut.dimY, imgOut.numChannels);
    stbi_write_jpg(argv[2], imgOut.dimX, imgOut.dimY, imgOut.numChannels, imgOut.data, 90);
    gettimeofday(&stop_time,NULL);
    timersub(&stop_time, &start_time, &elapsed_time); 
    printf("%f \n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);
    free(imgOut.data);
    free(v.data);
    free(v.sum);
    free(v.ratio);
    return 0;
}