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
    unsigned int i,j;
    int ext = v.length / 2;
    float *ptr, tmp;
    v.sum[0] = v.data[ext];
    for (i = ext+1,j=1; i < v.length; i++,j++)
    {
        v.sum[j] = v.sum[j-1] + v.data[i]*2;
    }
    #pragma omp parallel for private(i, j, ptr, tmp) schedule (dynamic)
    for (j = 0; j < v.length / 2 + 1; j++) {
        ptr = v.ratio + 3 * j * v.length;
        for(i = 0; i < v.length; i++, ptr += 3) {
            tmp = v.data[i] / v.sum[j];
            ptr[0] = tmp;
            ptr[1] = tmp;
            ptr[2] = tmp;
        }
    }
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

Image block_transpose(Image a, float *data) {
    static unsigned int blocksize = 500;
    unsigned int x, y, i, j;
    Image b;
    b.dimX = a.dimY;
    b.dimY = a.dimX;
    b.numChannels = a.numChannels;
    b.data = data;
    float *pc_a, *pc_b;
    #pragma omp parallel for private(pc_b, pc_a, i, j, x, y) schedule (dynamic)
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

Image gb_h_fma_3_8_without_extend(Image a, FVec gv, float *data) {
    Image b = a;
    b.data = data;

    int ext = gv.length / 2;
    unsigned int x, y, upper;
    float *pc, *pnow, *ptmp, tmp, *tmp_pointer;
    float sum[24];
    int i, j;
    __m256 sum0_256, sum1_256, sum2_256;

    #pragma omp parallel for private(sum, sum0_256, sum1_256, sum2_256, pc, pnow, ptmp, x, y, i, j, tmp, upper, tmp_pointer) schedule (dynamic)
    for (y = 0; y < a.dimY; y++)
    {
        pc = b.data + 3 * (y * b.dimX);
        for (x = 0; x < a.dimX; x++)
        {
            unsigned int deta = fmin(fmin(a.dimY-y-1, y),fmin(a.dimX-x-1, x));
            deta = fmin(deta, gv.min_deta);
            memset(pc, 0, sizeof(float) * 3);
            sum0_256 = _mm256_setzero_ps();
            sum1_256 = _mm256_setzero_ps();
            sum2_256 = _mm256_setzero_ps();
            ptmp = gv.ratio + 3 * ((ext - deta) * gv.length + deta);
            pnow = a.data + 3 * (y * a.dimX + x + deta) - 3 * ext;
            i = deta;
            upper = gv.length - deta;
            // adjust lower bound of i
            if(x + deta < ext) {
                // ext - x - deta >= 0
                pnow += (ext - x - deta) * 3;
                ptmp += (ext - x - deta) * 3;
                i += (ext - x - deta);
                tmp = (gv.sum[ext - deta] - gv.sum[x]) * 0.5 / gv.sum[ext - deta];
                pc[0] += tmp * pnow[0];
                pc[1] += tmp * pnow[1];
                pc[2] += tmp * pnow[2];
            }
            // adjust upper bound of i
            if(x + upper - ext >= a.dimX) {
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

Image gb_h_fma_6_8_without_extend(Image a, FVec gv, float *data) {
    Image b = a;
    b.data = data;

    int ext = gv.length / 2;
    unsigned int x, y, upper;
    float *pc, *pnow, *ptmp, tmp, *tmp_pointer;
    float sum[48];
    int i, j;
    __m256 sum0_256, sum1_256, sum2_256, sum3_256, sum4_256, sum5_256;

    #pragma omp parallel for private(sum, sum0_256, sum1_256, sum2_256, sum3_256, sum4_256, sum5_256, pc, pnow, ptmp, x, y, i, j, tmp, upper, tmp_pointer) schedule (dynamic)
    for (y = 0; y < a.dimY; y++)
    {
        pc = b.data + 3 * (y * b.dimX);
        for (x = 0; x < a.dimX; x++)
        {
            unsigned int deta = fmin(fmin(a.dimY-y-1, y),fmin(a.dimX-x-1, x));
            deta = fmin(deta, gv.min_deta);
            memset(pc, 0, sizeof(float) * 3);
            sum0_256 = _mm256_setzero_ps();
            sum1_256 = _mm256_setzero_ps();
            sum2_256 = _mm256_setzero_ps();
            sum3_256 = _mm256_setzero_ps();
            sum4_256 = _mm256_setzero_ps();
            sum5_256 = _mm256_setzero_ps();
            ptmp = gv.ratio + 3 * ((ext - deta) * gv.length + deta);
            pnow = a.data + 3 * (y * a.dimX + x + deta) - 3 * ext;
            i = deta;
            upper = gv.length - deta;
            // adjust lower bound of i
            if(x + deta < ext) {
                // ext - x - deta >= 0
                pnow += (ext - x - deta) * 3;
                ptmp += (ext - x - deta) * 3;
                i += (ext - x - deta);
                tmp = (gv.sum[ext - deta] - gv.sum[x]) * 0.5 / gv.sum[ext - deta];
                pc[0] += tmp * pnow[0];
                pc[1] += tmp * pnow[1];
                pc[2] += tmp * pnow[2];
            }
            // adjust upper bound of i
            if(x + upper - ext >= a.dimX) {
                tmp = (gv.sum[upper - ext - 1] - gv.sum[a.dimX - x - 1]) * 0.5 / gv.sum[ext - deta];
                upper -= (x + upper - ext - a.dimX);
                tmp_pointer = a.data + 3 * (y * a.dimX + a.dimX - 1);
                pc[0] += tmp * tmp_pointer[0];
                pc[1] += tmp * tmp_pointer[1];
                pc[2] += tmp * tmp_pointer[2];
            }
            for (; i + 16 < upper; i += 16, ptmp += 48, pnow += 48)
            {
                sum0_256 = _mm256_fmadd_ps(_mm256_loadu_ps(pnow), _mm256_loadu_ps(ptmp), sum0_256);
                sum1_256 = _mm256_fmadd_ps(_mm256_loadu_ps(pnow + 8), _mm256_loadu_ps(ptmp + 8), sum1_256);
                sum2_256 = _mm256_fmadd_ps(_mm256_loadu_ps(pnow + 16), _mm256_loadu_ps(ptmp + 16), sum2_256);
                sum3_256 = _mm256_fmadd_ps(_mm256_loadu_ps(pnow + 24), _mm256_loadu_ps(ptmp + 24), sum3_256);
                sum4_256 = _mm256_fmadd_ps(_mm256_loadu_ps(pnow + 32), _mm256_loadu_ps(ptmp + 32), sum4_256);
                sum5_256 = _mm256_fmadd_ps(_mm256_loadu_ps(pnow + 40), _mm256_loadu_ps(ptmp + 40), sum5_256);
            }
            _mm256_storeu_ps(sum, sum0_256);
            _mm256_storeu_ps(sum + 8, sum1_256);
            _mm256_storeu_ps(sum + 16, sum2_256);
            _mm256_storeu_ps(sum + 24, sum3_256);
            _mm256_storeu_ps(sum + 32, sum4_256);
            _mm256_storeu_ps(sum + 40, sum5_256);
            // pc[0] += sum[0]; pc[1] += sum[1]; pc[2] += sum[2]; pc[0] += sum[3]; pc[1] += sum[4]; pc[2] += sum[5];
            // pc[0] += sum[6]; pc[1] += sum[7]; pc[2] += sum[8]; pc[0] += sum[9]; pc[1] += sum[10]; pc[2] += sum[11];
            // pc[0] += sum[12]; pc[1] += sum[13]; pc[2] += sum[14]; pc[0] += sum[15]; pc[1] += sum[16]; pc[2] += sum[17];
            // pc[0] += sum[18]; pc[1] += sum[19]; pc[2] += sum[20]; pc[0] += sum[21]; pc[1] += sum[22]; pc[2] += sum[23];
            // pc[0] += sum[24]; pc[1] += sum[25]; pc[2] += sum[26]; pc[0] += sum[27]; pc[1] += sum[28]; pc[2] += sum[29]; 
            // pc[0] += sum[30]; pc[1] += sum[31]; pc[2] += sum[32]; pc[0] += sum[33]; pc[1] += sum[34]; pc[2] += sum[35]; 
            // pc[0] += sum[36]; pc[1] += sum[37]; pc[2] += sum[38]; pc[0] += sum[39]; pc[1] += sum[40]; pc[2] += sum[41]; 
            // pc[0] += sum[42]; pc[1] += sum[43]; pc[2] += sum[44]; pc[0] += sum[45]; pc[1] += sum[46]; pc[2] += sum[47]; 

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

Image apply_gb_with_fma_without_extend(Image a, FVec gv)
{
    float *data;
    data = malloc(a.dimX * a.dimY * 3 * sizeof(float));

    Image b = gb_h_fma_3_8_without_extend(a, gv, data);
    Image d = block_transpose(b, a.data);
    Image e = gb_h_fma_3_8_without_extend(d, gv, data);
    Image c = block_transpose(e, a.data);
    free(data);
    return c;
}

void pre_load_m256_ps(float *data, unsigned int Xdim, unsigned int Ydim, __m256 *data_256) {
    unsigned int i,j,data_256_dimX = Xdim * 3 / 8;
    float *ptr;
    __m256 *ptr_256;
    // #pragma omp parallel for private(i, j, ptr, ptr_256) schedule (dynamic)
    for (j = 0; j < Ydim; j++) {
        ptr_256 = data_256 + j * data_256_dimX;
        ptr = data + 3 * j * Xdim;
        puts("ok");
        for(i = 0; i + 8 < Xdim; i += 8, ptr += 24, ptr_256 += 3) {
            ptr_256[0] = _mm256_setzero_ps();
            puts("G");
            ptr_256[0] = _mm256_loadu_ps(ptr);
            ptr_256[1] = _mm256_loadu_ps(ptr + 8);
            ptr_256[2] = _mm256_loadu_ps(ptr + 16);
        }
    }
}

Image gb_h_fma_3_8_without_extend_with_preload(Image a, FVec gv, float *data, __m256 *img_data) {
    unsigned int img_data_dimX = a.dimX * 3 / 8, img_data_dimY = a.dimY;
    Image b = a;
    b.data = data;
    img_data = malloc(img_data_dimX * img_data_dimY * sizeof(__m256));
    printf("%p\n", img_data);
    img_data[0] = _mm256_setzero_ps();
    puts("pre_load");
    pre_load_m256_ps(a.data, a.dimX, a.dimY, img_data);
    puts("pre_load is ok");

    int ext = gv.length / 2;
    unsigned int x, y, upper;
    float *pc, *pnow, *ptmp, tmp, *tmp_pointer;
    float sum[24];
    int i, j;
    __m256 sum0_256, sum1_256, sum2_256, *img_256;

    #pragma omp parallel for private(sum, sum0_256, sum1_256, sum2_256, pc, pnow, ptmp, x, y, i, j, tmp, upper, tmp_pointer, img_256) schedule (dynamic)
    for (y = 0; y < a.dimY; y++)
    {
        pc = b.data + 3 * (y * b.dimX);
        for (x = 0; x < a.dimX; x++)
        {
            unsigned int deta = fmin(fmin(a.dimY-y-1, y),fmin(a.dimX-x-1, x));
            deta = fmin(deta, gv.min_deta);
            memset(pc, 0, sizeof(float) * 3);
            sum0_256 = _mm256_setzero_ps();
            sum1_256 = _mm256_setzero_ps();
            sum2_256 = _mm256_setzero_ps();
            ptmp = gv.ratio + 3 * ((ext - deta) * gv.length + deta);
            pnow = a.data + 3 * (y * a.dimX + x + deta) - 3 * ext;
            i = deta;
            upper = gv.length - deta;
            // adjust lower bound of i
            if(x + deta < ext) {
                // ext - x - deta >= 0
                pnow += (ext - x - deta) * 3;
                ptmp += (ext - x - deta) * 3;
                i += (ext - x - deta);
                tmp = (gv.sum[ext - deta] - gv.sum[x]) * 0.5 / gv.sum[ext - deta];
                pc[0] += tmp * pnow[0];
                pc[1] += tmp * pnow[1];
                pc[2] += tmp * pnow[2];
            }
            // adjust upper bound of i
            if(x + upper - ext >= a.dimX) {
                tmp = (gv.sum[upper - ext - 1] - gv.sum[a.dimX - x - 1]) * 0.5 / gv.sum[ext - deta];
                upper -= (x + upper - ext - a.dimX);
                tmp_pointer = a.data + 3 * (y * a.dimX + a.dimX - 1);
                pc[0] += tmp * tmp_pointer[0];
                pc[1] += tmp * tmp_pointer[1];
                pc[2] += tmp * tmp_pointer[2];
            }
            int nxt = ceil(i / 8) * 8;
            for(; i < nxt; i++, ptmp += 3, pnow += 3) {
                pc[0] += ptmp[0] * pnow[0];
                pc[1] += ptmp[1] * pnow[1];
                pc[2] += ptmp[2] * pnow[2];
            }
            img_256 = img_data + y * img_data_dimX + i / 8;

            for(; i + 8 < upper; i += 8, ptmp += 24, pnow += 24, img_256 += 3)
            {
                sum0_256 = _mm256_fmadd_ps(img_256[0], _mm256_loadu_ps(ptmp), sum0_256);
                sum1_256 = _mm256_fmadd_ps(img_256[1], _mm256_loadu_ps(ptmp + 8), sum1_256);
                sum2_256 = _mm256_fmadd_ps(img_256[2], _mm256_loadu_ps(ptmp + 16), sum2_256);
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
    free(img_data);

    return b;
}

Image apply_gb_with_fma_without_extend_with_preload(Image a, FVec gv)
{
    float *data;
    data = malloc(a.dimX * a.dimY * 3 * sizeof(float));
    __m256 *img_data;

    Image b = gb_h_fma_3_8_without_extend_with_preload(a, gv, data, img_data);
    Image d = block_transpose(b, a.data);
    Image e = gb_h_fma_3_8_without_extend_with_preload(d, gv, data, img_data);
    Image c = block_transpose(e, a.data);
    free(data);
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
    Image img;
    img.data = stbi_loadf(argv[1], &(img.dimX), &(img.dimY), &(img.numChannels), 0);

    Image imgOut = apply_gb_with_fma_without_extend_with_preload(img, v);
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