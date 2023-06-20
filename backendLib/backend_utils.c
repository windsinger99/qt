/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    backend_util.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */
#include "backend_main.h"
#include "backend_utils.h"
#include "backend_debug.h"
#include "IRTP_layout.h"
#if (MODEL_MCU == RT1052) || (MODEL_MCU == RT1064)
//#include <cr_section_macros.h>
#endif

ATTR_BACKEND_RAM2 static poly_e_t BS_p1datae, BS_p2datae;
#ifdef USE_CUST_MALLOC //nsmoon@201012
uint8_t BS_max_p1_p2_ve;
ATTR_BACKEND_RAM2 static vec_e_t  *BS_p1_ve, *BS_p2_ve;
#else
ATTR_BACKEND_RAM2 static vec_e_t  BS_p1_ve[MAX_P1_P2_VE];
ATTR_BACKEND_RAM2 static vec_e_t  BS_p2_ve[MAX_P1_P2_VE];
#endif

#define INITIAL_POL_TOL		0 //nsmoon@200109, 4=>0
ATTR_BACKEND_RAM2 clipping_range_info_t BS_clipping_range_info[ENUM_AXIS_END]; //nsmoon@200115

#ifdef USE_CUST_MALLOC //nsmoon@201012
//#if (MODEL_MCU != RT1052) && (MODEL_MCU != RT1064)
//ATTR_BACKEND_RAM2 unsigned int __end_of_heap; //nsmoon@201013
//#endif

//#define USE_STD_MALLOC //nsmoon@201021, NXP-BUG!! do not use

#ifndef USE_STD_MALLOC //nsmoon@201021
//#define MAX_BS_HEAP_SIZE    0x30000
ATTR_BACKEND_RAM3 uint8_t BS_heap[MAX_BS_HEAP_SIZE];
#endif

ATTR_BACKEND_RAM2 uint32_t BS_start_of_heap;
ATTR_BACKEND_RAM2 uint32_t BS_end_of_heap;

void BS_malloc_init()
{
#ifdef USE_STD_MALLOC //nsmoon@201021
  #if (MODEL_MCU == RT1052) || (MODEL_MCU == RT1064)
    BS_start_of_heap = BS_end_of_heap = __end_of_heap;
  #else
    BS_start_of_heap = BS_end_of_heap = 0;
  #endif
#else
    BS_start_of_heap = (uint32_t)&BS_heap[0];
    BS_end_of_heap = 0;
    TRACE_MALLOC("BS_start_of_heap= %x", BS_start_of_heap);
#endif
}

void *BS_malloc(int size)
{
#ifdef USE_STD_MALLOC //nsmoon@201021
    void *arry = malloc(size);
#if (MODEL_MCU == RT1052) || (MODEL_MCU == RT1064)
    BS_end_of_heap = __end_of_heap;
#else
    BS_end_of_heap += size;
#endif
    TRACE_MALLOC("..arry= %x %x (%x)", arry, size, BS_end_of_heap);
    return (void *)arry;
#else
    void *arry = 0;
    if (size > 0) {
        uint32_t tmpAddr = (uint32_t)((BS_end_of_heap + 3) / 4);
        BS_end_of_heap = tmpAddr * 4;
        if ((BS_end_of_heap + size) < MAX_BS_HEAP_SIZE) {
            arry = (void *)&BS_heap[BS_end_of_heap];
            BS_end_of_heap += size;
            //TRACE_MALLOC("..arry=,%x,%x,%x,", arry, size, BS_end_of_heap);
        }
        else {
        	TRACE_ERROR("ERROR! BS_malloc..invalid size=%d/%d/%d", size, BS_end_of_heap, MAX_BS_HEAP_SIZE);
        }
    }
    return (void *)arry;
#endif
}

void *BS_malloc_uint8_t2(int arryRow, int arryCol)
{
    uint8_t **arry, *ptr;
    int i, size;

    size = (sizeof(uint8_t *) * arryRow) + (sizeof(uint8_t) * arryRow * arryCol); //column
    arry = (uint8_t **)BS_malloc(size);
    if (arry == 0) {
        TRACE_ERROR("ERROR! BS_malloc_uint8_t2..");
        return 0; //error
    }
    //ptr is pointing to the first element in of 2D array
    ptr = (uint8_t *)(arry + arryRow);
    for(i = 0; i < arryRow; i++) { //column
        arry[i] = (ptr + (arryCol * i));
    }

    return (void *)arry;
}

int BS_utils_malloc()
{
    if (BS_offsetTblLenX < 10 || BS_offsetTblLenY < 10) {
        TRACE_ERROR("ERROR! BS_utils_malloc invalid BS_offsetTblLenX,BS_offsetTblLenY %d %d", BS_offsetTblLenX, BS_offsetTblLenY);
        return 1; //error
    }

    ///////////////////////////////////////////////////////////////////////////////////
    /// ep_buf_t *BS_p1_ve
    ///////////////////////////////////////////////////////////////////////////////////
    BS_max_p1_p2_ve = GET_MAX(BS_offsetTblLenX, BS_offsetTblLenY) * 2;
    BS_p1_ve = (vec_e_t *)BS_malloc(sizeof(vec_e_t) * BS_max_p1_p2_ve);
    if (BS_p1_ve == 0) {
        TRACE_ERROR("ERROR! BS_utils_malloc..BS_p1_ve");
        return 1; //error
    }
    TRACE_MALLOC("BS_p1_ve=,%x,%x,%x,", &BS_p1_ve[0], sizeof(vec_e_t) * BS_max_p1_p2_ve, BS_end_of_heap);

    ///////////////////////////////////////////////////////////////////////////////////
    /// ep_buf_t *BS_p2_ve
    ///////////////////////////////////////////////////////////////////////////////////
    BS_max_p1_p2_ve = GET_MAX(BS_offsetTblLenX, BS_offsetTblLenY) * 2;
    BS_p2_ve = (vec_e_t *)BS_malloc(sizeof(vec_e_t) * BS_max_p1_p2_ve);
    if (BS_p2_ve == 0) {
        TRACE_ERROR("ERROR! BS_utils_malloc..BS_p2_ve");
        return 1; //error
    }
    TRACE_MALLOC("BS_p2_ve=,%x,%x,%x,", &BS_p2_ve[0], sizeof(vec_e_t) * BS_max_p1_p2_ve, BS_end_of_heap);

    return 0; //no-error
}
#endif

#if 0 //for test
uint8_t bit_mask_byte_arry[BIT_MASK_BYTE_ARRY_LEN];

void backend_utils_init(void)
{
	int i;
	for (i = 0; i < BIT_MASK_BYTE_ARRY_LEN; i++) {
		bit_mask_byte_arry[i] = 0x1 << ((i) % 8);
	}
}
#endif

////////////////////////////////////////////
/* tells if vec c lies on the left side of directed edge a->b
* //1 if left, -1 if right, 0 if colinear (incorrect)
* //-1 if left, 1 if right, 0 if colinear
*/
ATTR_BACKEND_RAMFUNC
static int left_of(vec a, vec b, vec c)
{
    vec_t tmp1, tmp2;
    float x;
    //vsub(b, a, &tmp1);
    tmp1.x = b->x - a->x;
    tmp1.y = b->y - a->y;

    //vsub(c, b, &tmp2);
    tmp2.x = c->x - b->x;
    tmp2.y = c->y - b->y;

    //x = cross(&tmp1, &tmp2);
    x = (tmp1.x * tmp2.y) - (tmp1.y * tmp2.x);

    //return x < 0 ? -1 : x > 0;
    if (x < 0) { //nsmoon@171123
        if ((-x) > EPSILON) {
            //-
            return -1;
        }
        else
        {
            //zero
            return 0;
        }
    }
    else {
        if (x > EPSILON) {
            //+
            return 1;
        }
        else
        {
            //zero
            return 0;
        }
    }
}

int BS_left_of(vec p0, vec p1, vec dt)
{
    return left_of(p0, p1, dt);
}

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define TRACE_LI(...)	//TRACE(__VA_ARGS__)
#else
#define TRACE_LI(...)
#endif
ATTR_BACKEND_RAMFUNC
int BS_line_intersection(vec p0, vec p1, vec p2, vec p3, vec res)
{
    float s02_x, s02_y, s10_x, s10_y, s32_x, s32_y, s_numer, t_numer, denom, t;
    s10_x = p1->x - p0->x;
    s10_y = p1->y - p0->y;
    s32_x = p3->x - p2->x;
    s32_y = p3->y - p2->y;

    denom = s10_x * s32_y - s32_x * s10_y;
    if (GET_ABS(denom) < EPSILON) {
        return 0; // Collinear
    }
    int denomPositive = (denom > 0);

    s02_x = p0->x - p2->x;
    s02_y = p0->y - p2->y;
    s_numer = s10_x * s02_y - s10_y * s02_x;
    if ((s_numer < 0) == denomPositive) {
        IS_DEBUG_FLAG{ TRACE_LI("s_numer: %f", s_numer);}
        return 0; // No collision
    }
    //if ((s_numer > denom) == denomPositive)
    if (((s_numer - denom) > -EPSILON) == denomPositive)
    {
        IS_DEBUG_FLAG{ TRACE_LI("s_numer,denom: %f %f", s_numer, denom);}
        return 0; // No collision
    }

    t_numer = s32_x * s02_y - s32_y * s02_x;
    if ((t_numer < 0) == denomPositive) {
        IS_DEBUG_FLAG{ TRACE_LI("t_numer: %f", t_numer);}
        return 0; // No collision
    }
    //if ((t_numer > denom) == denomPositive)
    if (((t_numer - denom) > -EPSILON) == denomPositive)
    {
        IS_DEBUG_FLAG{ TRACE_LI("t_numer,denom: %f %f", t_numer, denom);}
        return 0; // No collision
    }

    // Collision detected
    t = t_numer / denom;
    if (res != 0)
        res->x = p0->x + (t * s10_x);
    if (res != 0)
        res->y = p0->y + (t * s10_y);

    return 1;
}

ATTR_BACKEND_RAMFUNC
int BS_line_intersection_test(float *posX, int idX, float *posY, int idY)
{
    vec_t p0, p1, p2, p3, res;
    p0.x = posX[idX]; p0.y = 0;
    p1.x = posX[idX]; p1.y = 2000;
    p2.y = posY[idY]; p2.x = 0;
    p3.y = posY[idY]; p3.x = 2000;
    return BS_line_intersection(&p0, &p1, &p2, &p3, &res);
}

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_line_sect		0
#endif
ATTR_BACKEND_RAMFUNC
int line_sect(vec x0, vec x1, vec y0, vec y1, vec res)
{
    vec_t dx, dy, d;
    float dyx, dyx1, dyx2;
    //vsub(x1, x0, &dx);
    dx.x = x1->x - x0->x;
    dx.y = x1->y - x0->y;

    //vsub(y1, y0, &dy);
    dy.x = y1->x - y0->x;
    dy.y = y1->y - y0->y;

    //vsub(x0, y0, &d);
    d.x = x0->x - y0->x;
    d.y = x0->y - y0->y;

    /* x0 + a dx = y0 + b dy ->
    x0 X dx = y0 X dx + b dy X dx ->
    b = (x0 - y0) X dx / (dy X dx) */
    //float dyx1 = cross(&dy, &dx);
    dyx1 = (dy.x * dx.y) - (dy.y * dx.x);
    if (GET_ABS(dyx1) < EPSILON) {
#if (DEBUG_line_sect == 1)
        TRACE("x0,x1,y0,y1,dyx1:(,%f,%f,)(,%f,%f,)(,%f,%f,)(,%f,%f,),%f,",
            x0->x, x0->y, x1->x, x1->y, y0->x, y0->y, y1->x, y1->y, dyx1);
#endif
        return 0; //not crossed
    }

    //float dyx2 = cross(&d, &dx);
    dyx2 = (d.x * dx.y) - (d.y * dx.x);
    dyx = dyx2 / dyx1;
#if (DEBUG_line_sect == 1)
    TRACE("dyx1,dyx2,dyx: %f %f %f", dyx1, dyx2, dyx);
#endif
    //if (dyx <= 0 || dyx >= 1) return 0;
    if (dyx < 0) //nsmoon@171024a
    {
        if ((-dyx) > EPSILON) {
            //not include 0
#if (DEBUG_line_sect == 1)
            TRACE("x0,x1,y0,y1,dyx:(,%f,%f,)(,%f,%f,)(,%f,%f,)(,%f,%f,),%f,",
                x0->x, x0->y, x1->x, x1->y, y0->x, y0->y, y1->x, y1->y, dyx);
#endif
            return 0; //not crossed
        }
    }
    else if ((dyx - 1) > EPSILON) //nsmoon@171024a
    {
        //not include 1
#if (DEBUG_line_sect == 1)
        TRACE("x0,x1,y0,y1,dyx:(,%f,%f,)(,%f,%f,)(,%f,%f,)(,%f,%f,),%f,",
            x0->x, x0->y, x1->x, x1->y, y0->x, y0->y, y1->x, y1->y, dyx);
#endif
        return 0; //not crossed
    }
#if 0 //for testing
    else if (GET_ABS(dyx) < EPSILON || GET_ABS(dyx - 1) < EPSILON) {
        //include 0 or 1;
        return 0;
    }
#endif

    res->x = y0->x + dyx * dy.x;
    res->y = y0->y + dyx * dy.y;
#if (DEBUG_line_sect == 1)
    TRACE("x0,x1:(,%f,%f,)(,%f,%f,)", x0->x, x0->y, x1->x, x1->y);
    TRACE("y0,y1:(,%f,%f,)(,%f,%f,)", y0->x, y0->y, y1->x, y1->y);
    TRACE("res->x,res->y:,%f,%f,", res->x, res->y);
#endif
    return 1; //crossed
}

ATTR_BACKEND_RAMFUNC
void BS_3point_angle(vec a, vec b, vec c, float *angle)
{
    vec_t p1, p2;
    p1.x = a->x - c->x;
    p1.y = a->y - c->y;
    p2.x = b->x - c->x;
    p2.y = b->y - c->y;
    //(*angle) = GET_ABS(p1.x * p2.x) + GET_ABS(p1.y * p2.y);
    (*angle) = GET_ABS((p1.x * p2.x) + (p1.y * p2.y));
    IS_DEBUG_FLAG {
      //TRACE("(*angle)= %0.1f (%0.1f,%0.1f)(%0.1f,%0.1f)", (*angle), p1.x, p1.y, p2.x, p2.y);
    };
}

////////////////////////////////////////////
#if 0 //for test
ATTR_BACKEND_RAMFUNC
static int line_e_edge_clip(vec subject, vec x0e, vec x1e, vec result)
{
    int i, side0, side1, len = 0;
    vec_t tmpe;
    vec v0e = &subject[1], v1e;

    side0 = left_of(x0e, x1e, v0e);
    if (side0 != 1) {
        result[len++] = *v0e;
    }

    for (i = 0; i < RESULT_LINE_VERTEX_NO; i++) {
        v1e = &subject[i];
        side1 = left_of(x0e, x1e, v1e);
        if (side0 + side1 == 0 && side0) {
            /* last point and current straddle the edge */
            if (BS_line_intersection(x0e, x1e, v0e, v1e, &tmpe))
            {
                if (len < RESULT_LINE_VERTEX_NO) {
                    result[len++] = tmpe;
                }
                else {
                    TRACE_ERROR("ERROR! 1.invalid len %d [%d]", BG_frame_no);
                }
            }
        }
        if (len > 1) break;

        if (side1 != 1) {
            if (len < RESULT_LINE_VERTEX_NO) {
                result[len++] = *v1e;
            }
            else {
                TRACE_ERROR("ERROR! 2.invalid len %d [%d]", BG_frame_no);
            }
        }
        v0e = v1e;
        side0 = side1;
    }

    return len;
}

ATTR_BACKEND_RAMFUNC
static int line_e_clip(vec subject, vec clipper, vec result)
{
    int ret = 0;
    vec_t tmpLine[RESULT_LINE_VERTEX_NO];

    ret = line_e_edge_clip(subject, &clipper[0], &clipper[1], &tmpLine[0]);
    if (ret == 2) {
        ret = line_e_edge_clip(&tmpLine[0], &clipper[2], &clipper[3], result);
    }
#if 0
    else {
        ret = line_e_edge_clip(subject, &clipper[2], &clipper[3], result);
    }
#endif
    return ret;
}
#endif
////////////////////////////////////////////

////////////////////////////////////////////
ATTR_BACKEND_RAMFUNC
static void poly_e_edge_clip(poly_e sube, vec_e x0e, vec_e x1e, poly_e rese)
{
    int 	i, side0, side1;
    vec_e_t tmpe;
    vec_e 	v0e = sube->ve + sube->len - 1, v1e;
    rese->len = 0;
#ifdef USE_CUST_MALLOC //nsmoon@201012
    int max_p1_p2_ve = BS_max_p1_p2_ve;
#endif

    side0 = left_of(&(x0e->vert), &(x1e->vert), &(v0e->vert));
    if (side0 != 1) {
#ifdef USE_CUST_MALLOC //nsmoon@201012
        if (rese->len < max_p1_p2_ve) { //nsmoon@201012
#else
        if (rese->len < MAX_P1_P2_VE) {
#endif
        rese->ve[rese->len++] = *v0e;
    }
        else {
            TRACE_ERROR("ERROR! invalid rese->len-3-");
        }
    }

    for (i = 0; i < sube->len; i++) {
        v1e = sube->ve + i;
        side1 = left_of(&(x0e->vert), &(x1e->vert), &(v1e->vert));
        if (side0 + side1 == 0 && side0) {
            /* last point and current straddle the edge */
            //if (line_sect(&(x0e->vert), &(x1e->vert), &(v0e->vert), &(v1e->vert), &tmpe.vert)) )
            if (BS_line_intersection(&(x0e->vert), &(x1e->vert), &(v0e->vert), &(v1e->vert), &tmpe.vert)) {
            if (side0 == -1) { 	// inner to outer
                tmpe.edgeId = x0e->edgeId;
            }
            else { // side0 == 1, outer to inner
                tmpe.edgeId = v0e->edgeId;
            }
#ifdef USE_CUST_MALLOC //nsmoon@201012
                if (rese->len < max_p1_p2_ve) { //nsmoon@201012
#else
                if (rese->len < MAX_P1_P2_VE) {
#endif
            rese->ve[rese->len++] = tmpe;
            }
                else {
                    TRACE_ERROR("ERROR! invalid rese->len-4-");
                }
            }
        }
        if (i == sube->len - 1) {
            break;
        }
        if (side1 != 1) {
#ifdef USE_CUST_MALLOC //nsmoon@201012
            if (rese->len < max_p1_p2_ve) { //nsmoon@201012
#else
            if (rese->len < MAX_P1_P2_VE) {
#endif
            rese->ve[rese->len++] = *v1e;
        }
            else {
                TRACE_ERROR("ERROR! invalid rese->len-5-");
            }
        }
        v0e = v1e;
        side0 = side1;
    }
}

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_poly_reduce_len   0
#endif
#if (DEBUG_poly_reduce_len == 1)
#define TRACE_PRL(...)  TRACE(__VA_ARGS__)
#else
#define TRACE_PRL(...)
#endif
static int poly_len_limited(poly_e poly)
{
    int i, j;
    int len = poly->len;
    float dX, dY, mDist;
    float mDistMin = MIN_INITIAL_VAL;
    int mDistIdx = -1;

    //search min dist
    for (i = 0; i < len; i++) {
        j = (i == 0) ? (len - 1) : (i - 1);
        dX = GET_ABS(poly->ve[i].vert.x - poly->ve[j].vert.x);
        dY = GET_ABS(poly->ve[i].vert.y - poly->ve[j].vert.y);
        mDist = dX + dY;
        //IS_DEBUG_FLAG{ TRACE_PRL("mDist: (%d) %0.2f (%0.2f %0.2f)", i, mDist, dX, dY);};
        if (mDist < mDistMin) {
            mDistMin = mDist;
            mDistIdx = i;
        }
    }

    //remove min dist vert
    if (mDistIdx >= 0) {
        IS_DEBUG_FLAG{ TRACE_PRL("=>mDistIdx: %d %0.2f", mDistIdx, mDistMin);};
        for (i = mDistIdx; i < (len - 1); i++) {
            poly->ve[i].edgeId = poly->ve[i + 1].edgeId;
            poly->ve[i].vert = poly->ve[i + 1].vert;
        }
        poly->len--;
        return 0; //no-error
    }

    return 1; //error
}

ATTR_BACKEND_RAMFUNC
poly_e BS_poly_e_clip(poly_e sube, poly_e clipe)
{
    //poly_e_t p1datae = { 0, 0, BS_p1_ve };
    //poly_e_t p2datae = { 0, 0, BS_p2_ve };
    BS_p1datae.ve = &BS_p1_ve[0];
    BS_p2datae.ve = &BS_p2_ve[0];
    BS_p1datae.len = 0;
    BS_p2datae.len = 0;
    poly_e p1e = &BS_p1datae, p2e = &BS_p2datae;
    poly_e tmpe;

    if (clipe->ve->edgeId != NO_CLIPPING_EDGEID) {
        //#0 polygon
        IS_DEBUG_FLAG{ TRACE_POLYGON_POLYCLIP(sube, clipe); } //nsmoon@170119
        poly_e_edge_clip(sube, clipe->ve, clipe->ve + 1, p2e);		// clip polygon's vertex [0]->[1] vector check
        IS_DEBUG_FLAG{ TRACE_POLYGON_POLYCLIP(0, p2e); } //nsmoon@170119

            if ((clipe->ve + 2)->edgeId != NO_CLIPPING_EDGEID) {
                //#3 polygon
                if (p2e->len > 0) {
                    tmpe = p2e; p2e = p1e; p1e = tmpe; 							// swap p1e and p2e
                    IS_DEBUG_FLAG{ TRACE_POLYGON_POLYCLIP(p1e, clipe); } //nsmoon@170119
                    poly_e_edge_clip(p1e, clipe->ve + 2, clipe->ve + 3, p2e);	// clip polygon's vertex [2]->[3] vector check
                    IS_DEBUG_FLAG{ TRACE_POLYGON_POLYCLIP(0, p2e); } //nsmoon@170119
                }
            }
    }
    else {
        if ((clipe->ve + 2)->edgeId != NO_CLIPPING_EDGEID) {
            if (sube->len > 0) {
                IS_DEBUG_FLAG{ TRACE_POLYGON_POLYCLIP(sube, clipe); } //nsmoon@170119
                poly_e_edge_clip(sube, clipe->ve + 2, clipe->ve + 3, p2e);	// clip polygon's vertex [2]->[3] vector check
                IS_DEBUG_FLAG{ TRACE_POLYGON_POLYCLIP(0, p2e); } //nsmoon@170119
            }
        }
    }

    if (p2e->len < 3) {
        p2e->len = 0;
    }

#ifdef POLY_VERTEX_LIMIT_ENABLE //nsmoon@200303
    if (p2e->len > POLY_MAX_VERTEX_LEN) {
        IS_DEBUG_FLAG{
            TRACE_NOP;
        };
        IS_DEBUG_FLAG{ TRACE_PRL("p2e->len: %d", p2e->len);};
        poly_len_limited(p2e);
    }
#endif
    return p2e;
}

#if 0 //for testing
ATTR_BACKEND_RAMFUNC
static void poly_edge_clip_all(poly_e sube, vec_e x0e, vec_e x1e, poly_e rese)
{
    int 	i, side0, side1;
    vec_e_t tmpe;
    vec_e 	v0e = sube->ve + sube->len - 1, v1e;
    rese->len = 0;

    side0 = left_of(&(x0e->vert), &(x1e->vert), &(v0e->vert));
    if (side0 != 1) {
        rese->ve[rese->len++] = *v0e;
    }

    for (i = 0; i < sube->len; i++) {
        v1e = sube->ve + i;
        side1 = left_of(&(x0e->vert), &(x1e->vert), &(v1e->vert));
        if (side0 + side1 == 0 && side0) {
            /* last point and current straddle the edge */
#if 0
            IS_DEBUG_FLAG{
                if (v1e->vert.x == 1211 && v1e->vert.y == 654) {
                    TRACE_NOP;
                }
            }
#endif

                if (BS_line_intersection(&(x0e->vert), &(x1e->vert), &(v0e->vert), &(v1e->vert), &(tmpe.vert)))
                {
                    if (side0 == -1) { 	// inner to outer
                        tmpe.edgeId = x0e->edgeId;
                    }
                    else { // side0 == 1, outer to inner
                        tmpe.edgeId = v0e->edgeId;
                    }
                    rese->ve[rese->len++] = tmpe;
                }
        }
        if (i == sube->len - 1) {
            break;
        }
        if (side1 != 1) {
            rese->ve[rese->len++] = *v1e;
        }
        v0e = v1e;
        side0 = side1;
    }
    if (rese->len < 3) {
        rese->len = 0;
    }
}

#if 0
ATTR_BACKEND_RAMFUNC
static poly_e poly_clip_all(poly_e sub, poly_e clip)
{
    poly_e p1e = &p1datae, p2e = &p2datae;

    //int i;
    p1datae.len = 0;
    p2datae.len = 0;

#if 1
    IS_DEBUG_FLAG{
        TRACE_NOP;
    }
    poly_edge_clip_all(sub, clip->ve, clip->ve + 1, p1e);		// [0]->[1]
    if (p1e->len < 3) {
        p2e->len = 0;
        goto L_poly_clip_all_01;
    }

    poly_edge_clip_all(p1e, clip->ve + 2, clip->ve + 3, p2e);	// [2]->[3]
    if (p2e->len < 3) {
        p2e->len = 0;
        goto L_poly_clip_all_01;
    }
    IS_DEBUG_FLAG{
        TRACE_NOP;
    }
#else
    //int dir = left_of(clip->ve, clip->ve + 1, clip->ve + 2);
    //IS_DEBUG_FLAG{ SEND_POLY(0, 0x21, 0); } //nsmoon@170119
    IS_DEBUG_FLAG{ TRACE_POLYGON3(sub, clip); } //nsmoon@170119
    poly_edge_clip_all(sub, clip->ve + clip->len - 1, clip->ve, p2e);
    IS_DEBUG_FLAG{ TRACE_POLYGON3(0, p2e); } //nsmoon@170119
        IS_DEBUG_FLAG{
        TRACE_NOP;
    }

    for (i = 0; i < clip->len - 1; i++) {
        tmpe = p2e; p2e = p1e; p1e = tmpe;
        if (p1e->len == 0) {
            p2e->len = 0;
            break;
        }
        IS_DEBUG_FLAG{ TRACE_POLYGON3(sub, clip); } //nsmoon@170119
        poly_edge_clip_all(p1e, clip->ve + i, clip->ve + i + 1, p2e);
        IS_DEBUG_FLAG{ TRACE_POLYGON3(0, p2e); } //nsmoon@170119
            IS_DEBUG_FLAG{
            TRACE_NOP;
        }
    }
    IS_DEBUG_FLAG{
        TRACE_NOP;
    }
#endif

    L_poly_clip_all_01:
    return p2e;
}
#endif //0
#endif
////////////////////////////////////////////

ATTR_BACKEND_RAMFUNC
float BG_calDistSquare(vec_t p1, vec_t p2)
{
    float diffX, diffY, distSquare;
    diffX = p1.x - p2.x;
    diffY = p1.y - p2.y;
    distSquare = diffX*diffX + diffY*diffY;
    return (distSquare);
}

ATTR_BACKEND_RAMFUNC
float BS_getTouchArea(poly_e pe)
{
    int i, j, points;
    float x1, y1, x2, y2, area;

    points = pe->len;
    area = 0;

    for (i = 0; i < points; i++) {
        j = (i + 1) % points;
        x1 = pe->ve[i].vert.x;
        y1 = pe->ve[i].vert.y;
        x2 = pe->ve[j].vert.x;
        y2 = pe->ve[j].vert.y;
        area += x1 * y2;
        area -= y1 * x2;
    }
    area *= 0.5f;
#if 1 //-Wstrict-aliasing
    area = GET_ABS(area);
#else //-Wno-strict-aliasing
    uint32_t casted = *(uint32_t *)&area;
    casted &= 0x7FFFFFFF;     //clear highest bit
    area = *(float*)&casted;  //re-interpret as float
#endif

    return area;
}

//#define USE_GET_TOUCH_CENTER
#ifdef USE_GET_TOUCH_CENTER
ATTR_BACKEND_RAMFUNC
static void getTouchCenter(poly_e pe, vec_t *cp)
{
    float minX, maxX, minY, maxY;
    int k;

    minX = pe->ve[0].vert.x;
    maxX = pe->ve[0].vert.x;
    minY = pe->ve[0].vert.y;
    maxY = pe->ve[0].vert.y;
    for (k = 1; k < pe->len; k++) {
        if (minX > pe->ve[k].vert.x) minX = pe->ve[k].vert.x;
        if (maxX < pe->ve[k].vert.x) maxX = pe->ve[k].vert.x;
        if (minY > pe->ve[k].vert.y) minY = pe->ve[k].vert.y;
        if (maxY < pe->ve[k].vert.y) maxY = pe->ve[k].vert.y;
    }

    cp->x = (minX + maxX) * 0.5f;
    cp->y = (minY + maxY) * 0.5f;
#ifdef DEBUG_POLYGON_RATIO
    float tmpWx = (maxX - minX);
    float tmpWY = (maxY - minY);
    static int tmpDfMax = 0, failRate = 0;
    int tmpDf;
    if (tmpWx < tmpWY) {
        tmpDf = 100 - (int)((tmpWx * 100) / tmpWY);
    }
    else {
        tmpDf = 100 - (int)((tmpWY * 100) / tmpWx);
    }
    tmpDf = GET_ABS(tmpDf);
    tmpDfMax = FN_MAX(tmpDfMax, tmpDf);
    if (tmpDf > BS_min_max_limit)
    {
        BS_fail_cnt++;
        if (BG_cal_cnt == 0) {
            failRate = 0;
        }
        else {
            failRate = (BS_fail_cnt * 100) / BG_cal_cnt;
        }
    }
    TRACE("BG_back_main_cnt,BG_cal_cnt:(,%d,),%d,(,%d,),%d,:,%d,:", BG_back_main_cnt, BG_cal_cnt, tmpDf, tmpDfMax, failRate);
#endif
}
#endif //USE_GET_TOUCH_CENTER

ATTR_BACKEND_RAMFUNC
int BS_is_edge(axis_t axis, float cent)
{
    int edge = 0;
    if (axis == ENUM_HOR_X) {
        if (cent < (BS_aarea_zero_x + EDGE_DIST_X) && cent >= BS_aarea_zero_x) {
            edge = EDGE_NEAR_PD_X; //near right-aarea (pd)
        }
        else if (cent < BS_aarea_zero_x) {
            edge = EDGE_INNER_PD_X; //inner right-aarea (pd)
        }
        else if (cent > (BS_aarea_end_x - EDGE_DIST_X) && cent <= BS_aarea_end_x) {
            edge = EDGE_NEAR_LED_X; //near left-aarea (led)
        }
        else if (cent > BS_aarea_end_x) {
            edge = EDGE_INNER_LED_X; //inner left-aarea (led)
        }
    }
    else {
        //(axis == ENUM_VER_Y)
        if (cent < (BS_aarea_zero_y + EDGE_DIST_Y) && cent >= BS_aarea_zero_y) {
            edge = EDGE_NEAR_LED_Y; //near bot-aarea (led)
        }
        else if (cent < BS_aarea_zero_y) {
            edge = EDGE_INNER_LED_Y; //inner bot-aarea (led)
        }
        else if (cent > (BS_aarea_end_y - EDGE_DIST_Y) && cent <= BS_aarea_end_y) {
            edge = EDGE_NEAR_PD_Y; //near top-aarea (pd)
        }
        else if (cent > BS_aarea_end_y) {
            edge = EDGE_INNER_PD_Y; //inner top-aarea (pd)
        }
    }
    return edge;
}

ATTR_BACKEND_RAMFUNC
int BS_getSenInBuf(axis_t axis, int inBufIdx, int *pd, int *led)
{
    uint16_t len;
    DEF_PD	*touch_pd;
    int offsetTblLen;
    int8_t *offsetTbl;
    int maxPdNum;

    if (axis == ENUM_HOR_X) {
        len = BS_inBuf->hor_len;
        touch_pd = BS_inBuf->hor_touch_pd;
        offsetTblLen = BS_offsetTblLenX;
        offsetTbl = BS_offsetTblX;
        maxPdNum = BS_maxHorPdNum;
    }
    else {
        len = BS_inBuf->ver_len;
        touch_pd = BS_inBuf->ver_touch_pd;
        offsetTblLen = BS_offsetTblLenY;
        offsetTbl = BS_offsetTblY;
        maxPdNum = BS_maxVerPdNum;
    }

    //int ret = 0;
    if (inBufIdx < 0 || inBufIdx >= len) {
        TRACE_ERROR("ERROR! BS_getSenInBuf-1..invalid idx %d (%d)", inBufIdx, axis);
        return -1; //error
    }
    *pd = touch_pd[inBufIdx].pd;
    if (touch_pd[inBufIdx].led >= offsetTblLen) {
        TRACE_ERROR("ERROR! BS_getSenInBuf-2..invalid led offset %d (%d)", touch_pd[inBufIdx].led, axis);
		return -2; //error
    }
    int slopeVal = offsetTbl[touch_pd[inBufIdx].led];
    *led = touch_pd[inBufIdx].pd + slopeVal;
    if (*pd >= maxPdNum || *led >= maxPdNum || *led < 0) {
        TRACE_ERROR("ERROR! BS_getSenInBuf-3..invalid led,pd %d %d %d (%d)", *led, *pd, slopeVal, axis);
		return -3; //error
    }
    return 0; //no-error
}

int BS_getLinePosInBufSen(axis_t axis, int led, int pd, vec_t *p0, vec_t *p1)
{
	int maxPdNum;

	if (axis == ENUM_HOR_X) {
		maxPdNum = BS_maxHorPdNum - 1;
	}
	else {
		maxPdNum = BS_maxVerPdNum - 1;
	}
	if (pd < 0 || led < 0 || pd > maxPdNum || led > maxPdNum) {
		TRACE_ERROR("ERROR! invalid led,pd %d %d", led, pd);
		return 1; //error
	}

	if (axis == ENUM_HOR_X) {
		p0->x = BS_led_pos_x[led];
		p0->y = BS_sensor_zero_y;
		p1->x = BS_pd_pos_x[pd];
		p1->y = BS_sensor_end_y;
	}
	else {
		//(axis ==ENUM_VER_Y)
		p0->y = BS_led_pos_y[led];
		p0->x = BS_sensor_end_x;
		p1->y = BS_pd_pos_y[pd];
		p1->x = BS_sensor_zero_x;
	}

	return 0; //no-error
}

ATTR_BACKEND_RAMFUNC
int BS_getLinePosInBuf(axis_t axis, int inBufIdx, vec_t *p0, vec_t *p1)
{
	uint16_t len;
	DEF_PD	*touch_pd;
	int offsetTblLen;
	int8_t *offsetTbl;
	int pd, led, ret, maxPdNum;

	if (axis == ENUM_HOR_X) {
		len = BS_inBuf->hor_len;
		touch_pd = BS_inBuf->hor_touch_pd;
		offsetTblLen = BS_offsetTblLenX;
		offsetTbl = BS_offsetTblX;
		maxPdNum = BS_maxHorPdNum;
	}
	else {
		len = BS_inBuf->ver_len;
		touch_pd = BS_inBuf->ver_touch_pd;
		offsetTblLen = BS_offsetTblLenY;
		offsetTbl = BS_offsetTblY;
		maxPdNum = BS_maxVerPdNum;
	}

	ret = 0;
	if (inBufIdx < 0 || inBufIdx >= len) {
		TRACE_ERROR("ERROR! BS_getLinePosInBuf..invalid idx %d (%d)", inBufIdx, axis);
		return -1; //error
	}

	pd = touch_pd[inBufIdx].pd;
	if (touch_pd[inBufIdx].led >= offsetTblLen) {
		TRACE_ERROR("ERROR! BS_getLinePosInBuf.1.invalid led offset %d (%d)", touch_pd[inBufIdx].led, axis);
		ret = -2; //error
	}
	led = touch_pd[inBufIdx].pd + offsetTbl[touch_pd[inBufIdx].led];
	if (led < 0 || led >= maxPdNum) {
		TRACE_ERROR("ERROR! BS_getLinePosInBuf.2.invalid led %d (%d)", led, axis);
		ret = -3; //error
	}

	if (ret == 0) {
		if (axis == ENUM_HOR_X) {
			p0->x = BS_led_pos_x[led];
			p0->y = BS_sensor_zero_y;
			p1->x = BS_pd_pos_x[pd];
			p1->y = BS_sensor_end_y;
		}
		else {
			//(axis ==ENUM_VER_Y)
			p0->y = BS_led_pos_y[led];
			p0->x = BS_sensor_end_x;
			p1->y = BS_pd_pos_y[pd];
			p1->x = BS_sensor_zero_x;
		}
	}

	return ret; //no-error
}

ATTR_BACKEND_RAMFUNC
int BS_getPosSen(axis_t axis, sen_type_t sen, int senNum, vec_t *pR)
{
	int maxPdNum; //pd, led, ret,
	//float *posArry;

	if (axis == ENUM_HOR_X) {
		maxPdNum = BS_maxHorPdNum - 1;
	}
	else {
		maxPdNum = BS_maxVerPdNum - 1;
	}
	if (senNum < 0 || senNum > maxPdNum) {
		TRACE_ERROR("ERROR! BS_getPosSen..invalid senNum %d (%d-%d)", senNum, axis, sen);
		return -1; //error
	}

	if (axis == ENUM_HOR_X) {
		if (sen == ENUM_PD) {
			pR->x = BS_pd_pos_x[senNum];
			pR->y = BS_sensor_end_y;
		}
		else {
			pR->x = BS_led_pos_x[senNum];
			pR->y = BS_sensor_zero_y;
		}
	}
	else {
		if (sen == ENUM_PD) {
			pR->y = BS_pd_pos_y[senNum];
			pR->x = BS_sensor_zero_x;
		}
		else {
			pR->y = BS_led_pos_y[senNum];
			pR->x = BS_sensor_end_x;
		}
	}

	return 0; //no-error
}

ATTR_BACKEND_RAMFUNC
int BS_getSenNumPos(axis_t axis, sen_type_t sen, float pos, int mode)
{
    //mode: 0(before), 1(after), 3(closed)
	int pd, maxPdNum, stPd, edPd, inc;
	float *posArry;
	float defaltPitch, diffCur, diffPrev;
	float tmpPos, tmpPos2;

	if (axis == ENUM_HOR_X) {
		maxPdNum = BS_maxHorPdNum - 1;
		posArry = (sen == ENUM_PD) ? BS_pd_pos_x : BS_led_pos_x;
	}
	else {
		maxPdNum = BS_maxVerPdNum - 1;
		posArry = (sen == ENUM_PD) ? BS_pd_pos_y : BS_led_pos_y;
	}
	if (pos < posArry[0] - EPSILON) { //<
		//TRACE_ERROR("ERROR! invalid pos: %f", pos);
		stPd = 0;
        return stPd;
	}
	else if (pos > posArry[maxPdNum] + EPSILON) {
		//TRACE_ERROR("ERROR! invalid pos: %f", pos);
		stPd = maxPdNum;
        return stPd;
	}
	else {
		defaltPitch = posArry[maxPdNum] / maxPdNum;
		stPd = (int)(pos / defaltPitch);
	}

	tmpPos = posArry[stPd];
	if (pos > tmpPos + EPSILON) { //>
		inc = +1;
		edPd = maxPdNum;
	}
	else {
		inc = -1;
		edPd = 0;
	}

	diffCur = diffPrev = 0;
	for (pd = stPd; (pd * inc) <= edPd; pd += inc) {
		tmpPos = posArry[pd];
		if ((pos * inc) < (tmpPos * inc)) {
			diffCur = GET_ABS(tmpPos - pos);
			if ((pd - inc) >= 0 && (pd - inc) <= maxPdNum) {
				tmpPos2 = posArry[pd - inc];
				diffPrev = GET_ABS(tmpPos2 - pos);
			}
			else {
				diffPrev = diffCur;
			}
			break;
		}
	}
	if (mode == 0) {
		//previous sensor
		if (inc > 0) {
			return GET_MAX((pd - inc), 0); //nsmoon@211231 //(pd - inc);
		}
		else {
			return pd;
		}
	}
	else if (mode == 1) {
		//current sensor
		if (inc > 0) {
			return pd;
		}
		else {
			return GET_MAX((pd - inc), 0); //nsmoon@211231 //(pd - inc);
		}
	}
#if 1 //for test
    else if (mode == 2) {
        //before previous sensor
        if (inc > 0) {
            return GET_MAX((pd - 2), 0); //nsmoon@211231 //(pd - 2);
        }
        else {
            return GET_MIN((pd + 2), maxPdNum); //nsmoon@211231 //(pd + 2);
        }
    }
#endif
    else {
	    //(mode == 3), closed sensor
		if (diffCur < diffPrev + EPSILON) { //<=
			return pd;
		}
	else {
		    return GET_MAX((pd - inc), 0); //nsmoon@211231 //(pd - inc);
	}
}
}

ATTR_BACKEND_RAMFUNC
int BS_getSlopeInBuf(axis_t axis, int inBufIdx, int *slopeValPlus)
{
	uint16_t len;
	DEF_PD	*touch_pd;
	int offsetTblLen;
	int8_t *offsetTbl;
	int pd = 0, led = 0, maxPdNum; //slope
	int ret = 0; //no-error

	if (axis == ENUM_HOR_X) {
		len = BS_inBuf->hor_len;
		touch_pd = BS_inBuf->hor_touch_pd;
		offsetTblLen = BS_offsetTblLenX;
		offsetTbl = BS_offsetTblX;
		maxPdNum = BS_maxHorPdNum;
	}
	else {
		len = BS_inBuf->ver_len;
		touch_pd = BS_inBuf->ver_touch_pd;
		offsetTblLen = BS_offsetTblLenY;
		offsetTbl = BS_offsetTblY;
		maxPdNum = BS_maxVerPdNum;
	}

	if (inBufIdx < 0 || inBufIdx >= len) {
		TRACE_ERROR("ERROR! BS_getSlopeInBuf..invalid idx %d (%d)", inBufIdx, axis);
		ret = -1; //error
	}
	else {
		pd = touch_pd[inBufIdx].pd;
		if (touch_pd[inBufIdx].led >= offsetTblLen) {
			TRACE_ERROR("ERROR! BS_getSlopeInBuf.1.invalid led offset %d (%d)", touch_pd[inBufIdx].led, axis);
        ret = -2; //error
    }
		led = touch_pd[inBufIdx].pd + offsetTbl[touch_pd[inBufIdx].led];
		if (led < 0 || led >= maxPdNum) {
			TRACE_ERROR("ERROR! BS_getSlopeInBuf.2.invalid led %d (%d)", led, axis);
        ret = -3; //error
    }
	}

	if (ret) {
		*slopeValPlus = 0; //error
	}
	else {
		*slopeValPlus = led - pd; //no-error
	}
    return ret;
}

#define CAL_MIN_MAX_TOL		0 //1.0f
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_BS_calMinMaxSen	0
#endif
int BS_calMinMaxSen(axis_t axis, int led, int pd, pos_min_max_t *mM_in, pos_min_max_t *mM_out)
{
	vec_t p0, p1, p2, p3, pR;

	//get sensor pos
	if (BS_getLinePosInBufSen(axis, led, pd, &p0, &p1)) {
		TRACE_ERROR("ERROR! cal_min_max..");
		return 1; //error
	}

	if (axis == ENUM_HOR_X) {
		//y-center
		p2.y = (mM_in->minY + mM_in->maxY) * 0.5f;
		p3.y = p2.y;
        p2.x = mM_in->minX - CAL_MIN_MAX_TOL - EPSILON; //nsmoon@230522 -EPSILON
        p3.x = mM_in->maxX + CAL_MIN_MAX_TOL + EPSILON; //nsmoon@230522 +EPSILON
	}
	else {
		//x-center
		p2.x = (mM_in->minX + mM_in->maxX) * 0.5f;
		p3.x = p2.x;
        p2.y = mM_in->minY - CAL_MIN_MAX_TOL - EPSILON; //nsmoon@230522 -EPSILON
        p3.y = mM_in->maxY + CAL_MIN_MAX_TOL + EPSILON; //nsmoon@230522 +EPSILON
	}
#if (DEBUG_BS_calMinMaxSen == 1)
    //DEBUG_SHOW_LINE_POS(&p0, &p1, 1);
    //DEBUG_SHOW_LINE_POS(&p2, &p3, MY_COLOR);
#endif
	if (BS_line_intersection(&p0, &p1, &p2, &p3, &pR)) {
#if (DEBUG_BS_calMinMaxSen == 1)
		DEBUG_SHOW_POS(&pR, 0.2f, 0.2f, 1);
#endif
		if (axis == ENUM_HOR_X) {
#if (DEBUG_BS_calMinMaxSen == 1)
            IS_DEBUG_FLAG{ TRACE("   pR= %0.3f (%0.3f %0.3f)", pR.x, mM_out->minX, mM_out->maxX); };
#endif
            if (pR.x > mM_in->minX && pR.x < mM_in->maxX) {
                mM_out->minX = GET_MIN(pR.x, mM_out->minX);
                mM_out->maxX = GET_MAX(pR.x, mM_out->maxX);
            }
		}
		else {
#if (DEBUG_BS_calMinMaxSen == 1)
            IS_DEBUG_FLAG{ TRACE("   pR= %0.3f (%0.3f %0.3f)", pR.y, mM_out->minY, mM_out->maxY); };
#endif
            if (pR.y > mM_in->minY && pR.y < mM_in->maxY) {
                mM_out->minY = GET_MIN(pR.y, mM_out->minY);
                mM_out->maxY = GET_MAX(pR.y, mM_out->maxY);
            }
		}
	}
#if (DEBUG_BS_calMinMaxSen == 1)
    else {
        IS_DEBUG_FLAG{ TRACE("   pR= none"); };
    }
#endif
	return 0; //no-error
}

int BS_get_slope_sign(int led0, int pd0)
{
    int slopeSign;
    if (led0 > pd0) {
        slopeSign = 1;
    }
    else if (led0 < pd0) {
        slopeSign = -1;
    }
    else {
        slopeSign = 0;
    }
    return slopeSign;
}

#ifndef FINE_REMOVE_GHOST_NEW //nsmoon@230412
#define DEBUG_BS_get_int_min_max    0
#if (DEBUG_BS_get_int_min_max > 0)
#define TRACE_GIMM(...)    TRACE(__VA_ARGS__)
#else
#define TRACE_GIMM(...)
#endif
int BS_get_float_min_max_init(float *max, int *idxMax, int maxCnt, int minMax)
{
    int i;
    int initVal = (minMax > 0) ? 0 : MIN_INITIAL_VAL;
    for (i = 0; i < maxCnt; i++) {
        max[i] = initVal;
        idxMax[i] = -1;
    }
    return 0;
}

int BS_get_float_min_max(float *max, int *idxMax, int maxCnt, int idx, float val, int idxVal, int minMax)
{
    //minMax(1:max, -1:min)
    int sign = (minMax > 0) ? 1 : -1;
    if (idx < maxCnt) {
        if ((val * sign) > (max[idx] * sign)) {
            TRACE_GIMM("idx=%d %d", idx, val);
            BS_get_float_min_max(max, idxMax, maxCnt, idx + 1, max[idx], idxMax[idx], minMax);
            max[idx] = val;
            idxMax[idx] = idxVal;
            return 1; //found
        }
        else {
            if (BS_get_float_min_max(max, idxMax, maxCnt, idx + 1, val, idxVal, minMax)) {
                return 1; //found
            }
        }
    }
    return 0; //no-found
}
#endif

int BS_init_max4_float(float *max, int maxCnt0)
{
    int i;
    int maxCnt = GET_MIN(4, maxCnt0);
    for (i = 0; i < maxCnt; i++) {
        max[i] = -1;
    }
    return 0;
}

int BS_get_max4_float(float *max, float val, int maxCnt)
{
    if (maxCnt > 0 && val > max[0]) {
        if (maxCnt > 1 && max[0] > max[1]) {
            if (maxCnt > 2 && max[1] > max[2]) {
                if (maxCnt > 3 && max[2] > max[3]) {
                    max[3] = max[2];
                }
                max[2] = max[1];
            }
            max[1] = max[0];
        }
        max[0] = val;
    }
    else if (maxCnt > 1 && val > max[1]) {
        if (maxCnt > 2 && max[1] > max[2]) {
            if (maxCnt > 3 && max[2] > max[3]) {
                max[3] = max[2];
            }
            max[2] = max[1];
        }
        max[1] = val;
    }
    else if (maxCnt > 2 && val > max[2]) {
        if (maxCnt > 3 && max[2] > max[3]) {
            max[3] = max[2];
        }
        max[2] = val;
    }
    else if (maxCnt > 3 && val > max[3]) {
        max[3] = val;
    }
    return 0;
}

int BS_init_max4_int(int *max, int *maxIdx, int maxCnt0)
{
    int i;
    int maxCnt = GET_MIN(4, maxCnt0);
    for (i = 0; i < maxCnt; i++) {
        max[i] = 0;
        maxIdx[i] = -1;
    }
    return 0;
}

int BS_get_max4_int(int *max, int val, int *maxIdx, int idx, int maxCnt)
{
    if (maxCnt > 0 && val > max[0]) {
        if (maxCnt > 1 && max[0] > max[1]) {
            if (maxCnt > 2 && max[1] > max[2]) {
                if (maxCnt > 3 && max[2] > max[3]) {
                    max[3] = max[2];
                    maxIdx[3] = maxIdx[2];
                }
                max[2] = max[1];
                maxIdx[2] = maxIdx[1];
            }
            max[1] = max[0];
            maxIdx[1] = maxIdx[0];
        }
        max[0] = val;
        maxIdx[0] = idx;
    }
    else if (maxCnt > 1 && val > max[1]) {
        if (maxCnt > 2 && max[1] > max[2]) {
            if (maxCnt > 3 && max[2] > max[3]) {
                max[3] = max[2];
                maxIdx[3] = maxIdx[2];
            }
            max[2] = max[1];
            maxIdx[2] = maxIdx[1];
        }
        max[1] = val;
        maxIdx[1] = idx;
    }
    else if (maxCnt > 2 && val > max[2]) {
        if (maxCnt > 3 && max[2] > max[3]) {
            max[3] = max[2];
            maxIdx[3] = maxIdx[2];
        }
        max[2] = val;
        maxIdx[2] = idx;
    }
    else if (maxCnt > 3 && val > max[3]) {
        max[3] = val;
        maxIdx[3] = idx;
    }
    return 0;
}

int BS_get_pos_on_line(vec_t *p0, vec_t *p1, axis_t axis, float *pos, float *posRet)
{
    //m=(y2-y1)/(x2-x1)
    //y=m(x-x1)+y1
    //x=(y-y1)/m+x1
    if (GET_ABS(p1->x - p0->x) < EPSILON) {
        if (axis == ENUM_HOR_X) {
            return -1;
        }
        else {
            *posRet = p0->x;
        }
    }
    else if (GET_ABS(p1->x - p0->x) < EPSILON) {
        if (axis == ENUM_HOR_X) {
            *posRet = p0->y;
        }
        else {
            return -1;
        }
    }
    else {
        float m = (p1->y - p0->y) / (p1->x - p0->x);
        if (axis == ENUM_HOR_X) {
            *posRet = m * (*pos - p0->x) + p0->y;
        }
        else {
            *posRet = (*pos - p0->y) / m + p0->x;
        }
    }
    return 0;
}

#define DEBUG_BS_get_clipping_range	0
#define DEBUG_BS_get_virtual_range	0
#if (DEBUG_BS_get_clipping_range > 0)
#define TRACE_MGCR(...)	TRACE(__VA_ARGS__)
#else
#define TRACE_MGCR(...)
#endif
#if (DEBUG_BS_get_virtual_range > 0)
#define TRACE_MGVR(...)	TRACE(__VA_ARGS__)
#define TRACENR_MGVR(...)	TRACENR(__VA_ARGS__)
#else
#define TRACE_MGVR(...)
#define TRACENR_MGVR(...)
#endif
int BS_get_sensor_via_p2(float x1Pos, float y1Pos, float x2Pos, float y2Pos, float y3Pos, float *senPosRet)
{
    float senTmpPos;
    if (GET_ABS(x2Pos - x1Pos) < EPSILON) {
        return 1; //error
    }
    else {
        if (GET_ABS(y2Pos - y1Pos) < EPSILON) {
            TRACE_ERROR("ERROR! y2,y1: %f %f", y2Pos, y1Pos);
            return 1; //error
        }
        else {
            //m1 = (x2 == x1) ? 0 : (float)(y2 - y1) / (float)(x2 - x1);
            senTmpPos = (y3Pos - y1Pos) * (x2Pos - x1Pos) / (y2Pos - y1Pos) + x1Pos;
            *senPosRet = senTmpPos;
        }
    }

    return 0; //no-error
}

#if 1 //for test
int BS_get_virtual_range(axis_t axis, sen_type_t sen, ep_min_max_t *minMaxEp, pos_min_max_t *minMaxPos, int senNo, int st_ed)
{
    //int senTmp, m1, m2,
    int maxPdNumX = BS_maxHorPdNum - 1;
    int maxPdNumY = BS_maxVerPdNum - 1;
    float maxPdNumXpos = BS_sensor_end_x;
    float maxPdNumYpos = BS_sensor_end_y;
#if 1 //nsmoon@211223 //bug-fix for BS_get_sensor_via_p2
    float senMinXpos = GET_MAX(minMaxPos->minX, 1.0f);
    float senMaxXpos = GET_MIN(minMaxPos->maxX, BS_sensor_end_x-1.0f);
    float senMinYpos = GET_MAX(minMaxPos->minY, 1.0f);
    float senMaxYpos = GET_MIN(minMaxPos->maxY, BS_sensor_end_y-1.0f);
#else
    float senMinXpos = minMaxPos->minX;
    float senMaxXpos = minMaxPos->maxX;
    float senMinYpos = minMaxPos->minY;
    float senMaxYpos = minMaxPos->maxY;
#endif
    float *posX = BS_clipping_range_info[(int)axis].senPosX;
    float *posY = BS_clipping_range_info[(int)axis].senPosY;
    sen_type_t sen2 = (sen == ENUM_PD) ? ENUM_LED : ENUM_PD; //opp sensor
                                                             //float *pos2 = BS_clipping_range_info[(int)axis].senPos2;
    float senPos; //senTmpPos, m1, m2,
    float x1Pos, y1Pos, x2Pos, y2Pos, y3Pos; //x3Pos
    int senRet; //senY
    float senXpos, senXposOrg;

    if (st_ed == 0) {
        //0:start
        senXpos = (axis == ENUM_HOR_X) ? senMinXpos : senMinYpos;
    }
    else {
        //1:end
        senXpos = (axis == ENUM_HOR_X) ? senMaxXpos : senMaxYpos;
    }

#if (DEBUG_BS_get_virtual_range > 0)
    int senX;
    int senMinX = minMaxEp->minX;
    int senMaxX = minMaxEp->maxX;
    int senMinY = minMaxEp->minY;
    int senMaxY = minMaxEp->maxY;
    if (st_ed == 0) {
        //0:start
        senX = (axis == ENUM_HOR_X) ? senMinX : senMinY;
    }
    else {
        //1:end
        senX = (axis == ENUM_HOR_X) ? senMaxX : senMaxY;
    }
    IS_DEBUG_FLAG2{ TRACE_MGVR("BS_get_virtual_range..%d-%d %d (%d) ", axis, sen, st_ed, (senNo < senX)); };
    IS_DEBUG_FLAG2{ TRACE_MGVR("BS_get_virtual_range..%d %d/%d %d/%d", senNo, minMaxEp->minX, minMaxEp->maxX, minMaxEp->minY, minMaxEp->maxY); };
#endif
#if (DEBUG_BS_get_virtual_range > 1)
    int dbgSenX, dbgSenY;
    vec_t pTmp, p0, p1;
    BS_getPosSen(axis, sen, senNo, &p0);
#endif

    x1Pos = y2Pos = 0;
    if (axis == ENUM_HOR_X) {
        //x2Pos = posX[senNo] - posX[senX];
        senXposOrg = posX[senNo];
    }
    else {
        //x2Pos = posY[senNo] - posY[senX];
        senXposOrg = posY[senNo];
    }
    x2Pos = senXposOrg - senXpos;

    if (axis == ENUM_HOR_X) {
        //x-axis
        if (sen == ENUM_PD) {
            //x-pd
            if (st_ed == 0) {
                //x-pd-start
                if (x2Pos < 0) {
                    //x-pd-start-right, e-p0
                    y1Pos = (maxPdNumYpos - senMinYpos);
                    y3Pos = -senMinYpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMinXpos;
                    p1.y = senMinYpos;
#endif
                }
                else {
                    //x-pd-start-left, e-p1
                    y1Pos = (maxPdNumYpos - senMaxYpos);
                    y3Pos = -senMaxYpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMinXpos;
                    p1.y = senMaxYpos;
#endif
                }
            }
            else {
                //x-pd-end
                if (x2Pos < 0) {
                    //x-pd-end-right, e-p2
                    y1Pos = (maxPdNumYpos - senMaxYpos);
                    y3Pos = -senMaxYpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMaxXpos;
                    p1.y = senMaxYpos;
#endif
                }
                else {
                    //x-pd-end-left, e-p3
                    y1Pos = (maxPdNumYpos - senMinYpos);
                    y3Pos = -senMinYpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMaxXpos;
                    p1.y = senMinYpos;
#endif
                }
            }
        }
        else {
            //x-led
            if (st_ed == 0) {
                //x-led-start
                if (x2Pos < 0) {
                    //x-led-start-right, s-p1
                    y3Pos = (maxPdNumYpos - senMaxYpos);
                    y1Pos = -senMaxYpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMinXpos;
                    p1.y = senMaxYpos;
#endif
                }
                else {
                    //x-led-start-left, e-p0
                    y3Pos = (maxPdNumYpos - senMinYpos);
                    y1Pos = -senMinYpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMinXpos;
                    p1.y = senMinYpos;
#endif
                }
            }
            else {
                //x-led-end
                if (x2Pos < 0) {
                    //x-led-end-right, e-p3
                    y3Pos = (maxPdNumYpos - senMinYpos);
                    y1Pos = -senMinYpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMaxXpos;
                    p1.y = senMinYpos;
#endif
                }
                else {
                    //x-led-end-left, e-p2
                    y3Pos = (maxPdNumYpos - senMaxYpos);
                    y1Pos = -senMaxYpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMaxXpos;
                    p1.y = senMaxYpos;
#endif
                }
            }
        }
    }
    else {
        //y-axis
        if (sen == ENUM_PD) {
            //y-pd
            if (st_ed == 0) {
                //y-pd-start
                if (x2Pos < 0) {
                    //y-pd-start-right, e-p3
                    y3Pos = -(maxPdNumXpos - senMaxXpos);
                    y1Pos = senMaxXpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMaxXpos;
                    p1.y = senMinYpos;
#endif
                }
                else {
                    //y-pd-start-left, e-p0
                    y3Pos = -(maxPdNumXpos - senMinXpos);
                    y1Pos = senMinXpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMinXpos;
                    p1.y = senMinYpos;
#endif
                }
            }
            else {
                //y-pd-end
                if (x2Pos < 0) {
                    //y-pd-end-right, e-p1
                    y3Pos = -(maxPdNumXpos - senMinXpos);
                    y1Pos = senMinXpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMinXpos;
                    p1.y = senMaxYpos;
#endif
                }
                else {
                    //y-pd-end-left, e-p2
                    y3Pos = -(maxPdNumXpos - senMaxXpos);
                    y1Pos = senMaxXpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMaxXpos;
                    p1.y = senMaxYpos;
#endif
                }
            }
        }
        else {
            //y-led
            if (st_ed == 0) {
                //y-led-start
                if (x2Pos < 0) {
                    //y-led-start-right, s-p0
                    y1Pos = -(maxPdNumXpos - senMinXpos);
                    y3Pos = senMinXpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMinXpos;
                    p1.y = senMinYpos;
#endif
                }
                else {
                    //y-led-start-left, e-p3
                    y1Pos = -(maxPdNumXpos - senMaxXpos);
                    y3Pos = senMaxXpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMaxXpos;
                    p1.y = senMinYpos;
#endif
                }
            }
            else {
                //y-led-end
                if (x2Pos < 0) {
                    //y-led-end-right, e-p2
                    y1Pos = -(maxPdNumXpos - senMaxXpos);
                    y3Pos = senMaxXpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMaxXpos;
                    p1.y = senMaxYpos;
#endif
                }
                else {
                    //y-led-end-left, e-p1
                    y1Pos = -(maxPdNumXpos - senMinXpos);
                    y3Pos = senMinXpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMinXpos;
                    p1.y = senMaxYpos;
#endif
                }
            }
        }
    }
#if (DEBUG_BS_get_virtual_range > 1)
    DEBUG_SHOW_LINE_POS(&p0, &p1, MY_COLOR);
    TRACE_NOP;
#endif

    float senPosRet;
    if (BS_get_sensor_via_p2(x1Pos, y1Pos, x2Pos, y2Pos, y3Pos, &senPosRet)) {
        senRet = senNo;
    }
    else {
        senPos = senXposOrg - senPosRet;
        if (st_ed == 0) {
            senRet = BS_getSenNumPos(axis, sen2, senPos, 0); //0:before
        }
        else {
            senRet = BS_getSenNumPos(axis, sen2, senPos, 1); //1:after
        }
#if 0 //for test
        if (senNo > senRet) {
            senRet = GET_MAX((senNo - maxSlopeVal), senRet);
        }
        else {
            senRet = GET_MIN((senNo + maxSlopeVal), senRet);
        }
#endif
        senRet = GET_MAX(senRet, 0);
        if (axis == ENUM_HOR_X) {
            senRet = GET_MIN(senRet, maxPdNumX);
        }
        else {
            senRet = GET_MIN(senRet, maxPdNumY);
        }
    }
#if 0 //(DEBUG_BS_get_virtual_range > 1)
    BS_getPosSen(ENUM_HOR_X, sen, dbgSenX, &pTmp);
    p1.x = pTmp.x;
    BS_getPosSen(ENUM_VER_Y, sen, dbgSenY, &pTmp);
    p1.y = pTmp.y;
    DEBUG_SHOW_LINE_POS(&p0, &p1, MY_COLOR);
#endif
#if (DEBUG_BS_get_virtual_range > 0)
    IS_DEBUG_FLAG2{
        if (sen == ENUM_PD) {
            DEBUG_SHOW_LINE_PD_LED(axis, senNo, senRet, MY_COLOR - 2);
        }
        else {
            DEBUG_SHOW_LINE_PD_LED(axis, senRet, senNo, MY_COLOR - 2);
        }
        TRACE_NOP;
    };
#endif

    //must check max slope of senRet before using!!!!
    return senRet;
}
#else
int BS_get_virtual_range(axis_t axis, sen_type_t sen, ep_min_max_t *minMaxEp, pos_min_max_t *minMaxPos, int senNo, int st_ed, int multiLoopCnt)
{
    //int senTmp, m1, m2,
    int maxPdNumX = BS_maxHorPdNum - 1;
    int maxPdNumY = BS_maxVerPdNum - 1;
    float maxPdNumXpos = BS_sensor_end_x;
    float maxPdNumYpos = BS_sensor_end_y;
    float senMinXpos = minMaxPos->minX;
    float senMaxXpos = minMaxPos->maxX;
    float senMinYpos = minMaxPos->minY;
    float senMaxYpos = minMaxPos->maxY;
    float *posX = BS_clipping_range_info[(int)axis].senPosX;
    float *posY = BS_clipping_range_info[(int)axis].senPosY;
    sen_type_t sen2 = (sen == ENUM_PD) ? ENUM_LED : ENUM_PD; //opp sensor
    //float *pos2 = BS_clipping_range_info[(int)axis].senPos2;
    float senPos, senTmpPos; //m1, m2,
    float x1Pos, y1Pos, x2Pos, y2Pos, y3Pos; //x3Pos
    int senRet; //senY
    float senXpos, senXposOrg;

    if (st_ed == 0) {
        //0:start
        senXpos = (axis == ENUM_HOR_X) ? senMinXpos : senMinYpos;
    }
    else {
        //1:end
        senXpos = (axis == ENUM_HOR_X) ? senMaxXpos : senMaxYpos;
    }

#if (DEBUG_BS_get_virtual_range > 0)
    int senX;
	int senMinX = minMaxEp->minX;
	int senMaxX = minMaxEp->maxX;
	int senMinY = minMaxEp->minY;
	int senMaxY = minMaxEp->maxY;
    if (st_ed == 0) {
        //0:start
        senX = (axis == ENUM_HOR_X) ? senMinX : senMinY;
    }
    else {
        //1:end
        senX = (axis == ENUM_HOR_X) ? senMaxX : senMaxY;
    }
    IS_DEBUG_FLAG2{ TRACE_MGVR("BS_get_virtual_range..%d-%d %d (%d) ", axis, sen, st_ed, (senNo < senX)); };
    IS_DEBUG_FLAG2{ TRACE_MGVR("BS_get_virtual_range..%d %d/%d %d/%d", senNo, minMaxEp->minX, minMaxEp->maxX, minMaxEp->minY, minMaxEp->maxY); };
#endif
#if (DEBUG_BS_get_virtual_range > 1)
    int dbgSenX, dbgSenY;
    vec_t pTmp, p0, p1;
    BS_getPosSen(axis, sen, senNo, &p0);
#endif

    x1Pos = y2Pos = 0;
    if (axis == ENUM_HOR_X) {
        //x2Pos = posX[senNo] - posX[senX];
        senXposOrg = posX[senNo];
    }
    else {
        //x2Pos = posY[senNo] - posY[senX];
        senXposOrg = posY[senNo];
    }
    x2Pos = senXposOrg - senXpos;

    if (axis == ENUM_HOR_X) {
        //x-axis
        if (sen == ENUM_PD) {
            //x-pd
            if (st_ed == 0) {
                //x-pd-start
                if (x2Pos < 0) {
                    //x-pd-start-right, e-p0
                    y1Pos = (maxPdNumYpos - senMinYpos);
                    y3Pos = -senMinYpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMinXpos;
                    p1.y = senMinYpos;
#endif
            }
            else {
                    //x-pd-start-left, e-p1
                    y1Pos = (maxPdNumYpos - senMaxYpos);
                    y3Pos = -senMaxYpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMinXpos;
                    p1.y = senMaxYpos;
#endif
                }
            }
            else {
                //x-pd-end
                if (x2Pos < 0) {
                    //x-pd-end-right, e-p2
                    y1Pos = (maxPdNumYpos - senMaxYpos);
                    y3Pos = -senMaxYpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMaxXpos;
                    p1.y = senMaxYpos;
#endif
            }
            else {
                    //x-pd-end-left, e-p3
                    y1Pos = (maxPdNumYpos - senMinYpos);
                    y3Pos = -senMinYpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMaxXpos;
                    p1.y = senMinYpos;
#endif
                }
            }
        }
        else {
            //x-led
            if (st_ed == 0) {
                //x-led-start
                if (x2Pos < 0) {
                    //x-led-start-right, s-p1
                    y3Pos = (maxPdNumYpos - senMaxYpos);
                    y1Pos = -senMaxYpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMinXpos;
                    p1.y = senMaxYpos;
#endif
        }
        else {
                    //x-led-start-left, e-p0
                    y3Pos = (maxPdNumYpos - senMinYpos);
                    y1Pos = -senMinYpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMinXpos;
                    p1.y = senMinYpos;
#endif
                }
            }
            else {
                //x-led-end
                if (x2Pos < 0) {
                    //x-led-end-right, e-p3
                    y3Pos = (maxPdNumYpos - senMinYpos);
                    y1Pos = -senMinYpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMaxXpos;
                    p1.y = senMinYpos;
#endif
            }
                else {
                    //x-led-end-left, e-p2
                    y3Pos = (maxPdNumYpos - senMaxYpos);
                    y1Pos = -senMaxYpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMaxXpos;
                    p1.y = senMaxYpos;
#endif
            }
            }
        }
    }
    else {
        //y-axis
        if (sen == ENUM_PD) {
            //y-pd
            if (st_ed == 0) {
                //y-pd-start
                if (x2Pos < 0) {
                    //y-pd-start-right, e-p3
                    y3Pos = -(maxPdNumXpos - senMaxXpos);
                    y1Pos = senMaxXpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMaxXpos;
                    p1.y = senMinYpos;
#endif
            }
            else {
                    //y-pd-start-left, e-p0
                    y3Pos = -(maxPdNumXpos - senMinXpos);
                    y1Pos = senMinXpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMinXpos;
                    p1.y = senMinYpos;
#endif
                }
            }
            else {
                //y-pd-end
                if (x2Pos < 0) {
                    //y-pd-end-right, e-p1
                    y3Pos = -(maxPdNumXpos - senMinXpos);
                    y1Pos = senMinXpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMinXpos;
                    p1.y = senMaxYpos;
#endif
            }
            else {
                    //y-pd-end-left, e-p2
                    y3Pos = -(maxPdNumXpos - senMaxXpos);
                    y1Pos = senMaxXpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMaxXpos;
                    p1.y = senMaxYpos;
#endif
            }
            }
        }
        else {
            //y-led
            if (st_ed == 0) {
                //y-led-start
                if (x2Pos < 0) {
                    //y-led-start-right, s-p0
                    y1Pos = -(maxPdNumXpos - senMinXpos);
                    y3Pos = senMinXpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMinXpos;
                    p1.y = senMinYpos;
#endif
        }
        else {
                    //y-led-start-left, e-p3
                    y1Pos = -(maxPdNumXpos - senMaxXpos);
                    y3Pos = senMaxXpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMaxXpos;
                    p1.y = senMinYpos;
#endif
                }
            }
            else {
                //y-led-end
                if (x2Pos < 0) {
                    //y-led-end-right, e-p2
                    y1Pos = -(maxPdNumXpos - senMaxXpos);
                    y3Pos = senMaxXpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMaxXpos;
                    p1.y = senMaxYpos;
#endif
            }
                else {
                    //y-led-end-left, e-p1
                    y1Pos = -(maxPdNumXpos - senMinXpos);
                    y3Pos = senMinXpos;
#if (DEBUG_BS_get_virtual_range > 1)
                    p1.x = senMinXpos;
                    p1.y = senMaxYpos;
#endif
            }
            }
        }
    }
#if (DEBUG_BS_get_virtual_range > 1)
    if (multiLoopCnt == 3)
    DEBUG_SHOW_LINE_POS(&p0, &p1, MY_COLOR);
    TRACE_NOP;
#endif

    //m1 = (x2 == x1) ? 0 : (float)(y2 - y1) / (float)(x2 - x1);
    if (GET_ABS(x2Pos - x1Pos) < EPSILON) {
        senRet = senNo;
    }
    else {
        if (GET_ABS(y2Pos - y1Pos) < EPSILON) {
            TRACE_ERROR("ERROR! y2,y1: %f %f", y2Pos, y1Pos);
            senRet = senNo;
        }
        else {
            senTmpPos = (y3Pos - y1Pos) * (x2Pos - x1Pos) / (y2Pos - y1Pos) + x1Pos;
            senPos = senXposOrg - senTmpPos;
#if 0 //(DEBUG_BS_get_virtual_range > 1)
            p1.x = senPos;
            p1.y = BS_sensor_zero_x;
            if (multiLoopCnt == 3)
            DEBUG_SHOW_LINE_POS(&p0, &p1, MY_COLOR);
#endif
            if (st_ed == 0) {
                senRet = BS_getSenNumPos(axis, sen2, senPos, 0); //0:before
            }
            else {
                senRet = BS_getSenNumPos(axis, sen2, senPos, 1); //1:after
            }
#if 0 //for test
            if (senNo > senRet) {
                senRet = GET_MAX((senNo - maxSlopeVal), senRet);
            }
            else {
                senRet = GET_MIN((senNo + maxSlopeVal), senRet);
            }
#endif

            senRet = GET_MAX(senRet, 0);
            if (axis == ENUM_HOR_X) {
                senRet = GET_MIN(senRet, maxPdNumX);
            }
            else {
                senRet = GET_MIN(senRet, maxPdNumY);
            }
        }
    }
#if 0 //(DEBUG_BS_get_virtual_range > 1)
    BS_getPosSen(ENUM_HOR_X, sen, dbgSenX, &pTmp);
    p1.x = pTmp.x;
    BS_getPosSen(ENUM_VER_Y, sen, dbgSenY, &pTmp);
    p1.y = pTmp.y;
    if (multiLoopCnt == 3)
    DEBUG_SHOW_LINE_POS(&p0, &p1, MY_COLOR);
#endif
#if (DEBUG_BS_get_virtual_range > 0)
    IS_DEBUG_FLAG2{
    if (sen == ENUM_PD) {
        if (multiLoopCnt == 3)
            DEBUG_SHOW_LINE_PD_LED(axis, senNo, senRet, MY_COLOR - 2);
    }
    else {
        if (multiLoopCnt == 3)
            DEBUG_SHOW_LINE_PD_LED(axis, senRet, senNo, MY_COLOR - 2);
    }
    TRACE_NOP;
    };
#endif

    //must check max slope of senRet before using!!!!
    return senRet;
}
#endif

static int get_pos_sted(axis_t axis, int mode, int minMaxSen, ep_min_max_t *minMaxEp, pos_min_max_t *minMaxPos, vec_t *p0, vec_t *p1, int calSenNo)
{
    int minMaxSen2;
    int maxPdNumX = BS_maxHorPdNum - 1;
    int maxPdNumY = BS_maxVerPdNum - 1;
    int retSen = minMaxSen;
    sen_type_t sen = BS_clipping_range_info[(int)axis].sensor;
    sen_type_t sen2 = (sen == ENUM_PD) ? ENUM_LED : ENUM_PD; //opp sensor
    float *posX = BS_clipping_range_info[(int)axis].senPosX;
    float *posY = BS_clipping_range_info[(int)axis].senPosY;
    float *pos2 = BS_clipping_range_info[(int)axis].senPos2;

    IS_DEBUG_FLAG{
        if (axis == ENUM_VER_Y && calSenNo) {
            TRACE_NOP;
        }
    };

    if (sen == ENUM_PD) {
        if (axis == ENUM_HOR_X) {
            if (mode == 0) {
                //start line
                minMaxSen2 = minMaxSen + BS_slopeValMaxX;
                if (calSenNo) {
                    if (minMaxSen2 >= maxPdNumX) {
                        minMaxSen2 = maxPdNumX;
                        retSen = BS_get_virtual_range(axis, sen2, minMaxEp, minMaxPos, maxPdNumX, 0);
                        minMaxSen2 = GET_MAX(retSen, 0);
                    }
                }
                else {
                    minMaxSen2 = GET_MIN(minMaxSen2, maxPdNumX);
                    p0->x = posX[retSen];
                    p0->y = BS_sensor_end_y; //pd-side
                    p1->x = pos2[minMaxSen2];
                    p1->y = BS_sensor_zero_y; //led-side
            }
            }
            else {
                //end line
                minMaxSen2 = minMaxSen - BS_slopeValMaxX;
                if (calSenNo) {
                    if (minMaxSen2 <= 0) {
                        minMaxSen2 = 0;
                        retSen = BS_get_virtual_range(axis, sen2, minMaxEp, minMaxPos, 0, 1);
                        minMaxSen2 = GET_MIN(retSen, maxPdNumX);
                    }
                }
                else {
                minMaxSen2 = GET_MAX(minMaxSen2, 0);
                    p0->x = posX[retSen];
                    p0->y = BS_sensor_end_y; //pd-side
                    p1->x = pos2[minMaxSen2];
                    p1->y = BS_sensor_zero_y; //led-side
            }
        }
        }
        else {
            //(axis == ENUM_VER_Y)
            if (mode == 0) {
                //start line
                minMaxSen2 = minMaxSen + BS_slopeValMaxY;
                if (calSenNo) {
                    if (minMaxSen2 >= maxPdNumY) {
                        minMaxSen2 = maxPdNumY;
                        retSen = BS_get_virtual_range(axis, sen2, minMaxEp, minMaxPos, maxPdNumY, 0);
                        minMaxSen2 = GET_MAX(retSen, 0);
                    }
                }
                else {
                    minMaxSen2 = GET_MIN(minMaxSen2, maxPdNumY);
                    p0->y = posY[retSen];
                    p0->x = BS_sensor_zero_x; //pd-side
                    p1->y = pos2[minMaxSen2];
                    p1->x = BS_sensor_end_x; //led-side
            }
            }
            else {
                //end line
                minMaxSen2 = minMaxSen - BS_slopeValMaxY;
                if (calSenNo) {
                    if (minMaxSen2 <= 0) {
                        minMaxSen2 = 0;
                        retSen = BS_get_virtual_range(axis, sen2, minMaxEp, minMaxPos, 0, 1);
                        minMaxSen2 = GET_MIN(retSen, maxPdNumY);
                    }
                }
                else {
                minMaxSen2 = GET_MAX(minMaxSen2, 0);
                    p0->y = posY[retSen];
                    p0->x = BS_sensor_zero_x; //pd-side
                    p1->y = pos2[minMaxSen2];
                    p1->x = BS_sensor_end_x; //led-side
            }
        }
    }
    }
    else {
        //led
        if (axis == ENUM_HOR_X) {
            if (mode == 0) {
                //start line
                minMaxSen2 = minMaxSen + BS_slopeValMaxX;
                if (calSenNo) {
                    if (minMaxSen2 >= maxPdNumX) {
                        minMaxSen2 = maxPdNumX;
                        retSen = BS_get_virtual_range(axis, sen2, minMaxEp, minMaxPos, maxPdNumX, 0);
                        minMaxSen2 = GET_MAX(retSen, 0);
                    }
                }
                else {
                    minMaxSen2 = GET_MIN(minMaxSen2, maxPdNumX);
                    p0->x = posX[retSen];
                    p0->y = BS_sensor_zero_y; //led-side
                    p1->x = pos2[minMaxSen2];
                    p1->y = BS_sensor_end_y; //pd-side
            }
            }
            else {
                //end line
                minMaxSen2 = minMaxSen - BS_slopeValMaxX;
                if (calSenNo) {
                    if (minMaxSen2 <= 0) {
                        minMaxSen2 = 0;
                        retSen = BS_get_virtual_range(axis, sen2, minMaxEp, minMaxPos, 0, 1);
                        minMaxSen2 = GET_MIN(retSen, maxPdNumX);
                    }
                }
                else {
                minMaxSen2 = GET_MAX(minMaxSen2, 0);
                    p0->x = posX[retSen];
                    p0->y = BS_sensor_zero_y; //led-side
                    p1->x = pos2[minMaxSen2];
                    p1->y = BS_sensor_end_y; //pd-side
            }
        }
        }
        else {
            if (mode == 0) {
                //start line
                minMaxSen2 = minMaxSen + BS_slopeValMaxY;
                if (calSenNo) {
                    if (minMaxSen2 >= maxPdNumY) {
                        minMaxSen2 = maxPdNumY;
                        retSen = BS_get_virtual_range(axis, sen2, minMaxEp, minMaxPos, maxPdNumY, 0);
                        minMaxSen2 = GET_MAX(retSen, 0);
                    }
                }
                else {
                    minMaxSen2 = GET_MIN(minMaxSen2, maxPdNumY);
                    p0->y = posY[retSen];
                    p0->x = BS_sensor_end_x; //led-side
                    p1->y = pos2[minMaxSen2];
                    p1->x = BS_sensor_zero_x; //pd-side
            }
            }
            else {
                //end line
                minMaxSen2 = minMaxSen - BS_slopeValMaxY;
                if (calSenNo) {
                    if (minMaxSen2 <= 0) {
                        minMaxSen2 = 0;
                        retSen = BS_get_virtual_range(axis, sen2, minMaxEp, minMaxPos, 0, 1);
                        minMaxSen2 = GET_MAX(retSen, maxPdNumY);
                    }
                }
                else {
                minMaxSen2 = GET_MAX(minMaxSen2, 0);
                    p0->y = posY[retSen];
                    p0->x = BS_sensor_end_x; //led-side
                    p1->y = pos2[minMaxSen2];
                    p1->x = BS_sensor_zero_x; //pd-side
            }
        }
    }
}
    return retSen;
}

int BS_get_clipping_range(axis_t axis, ep_min_max_t *minMaxEp, pos_min_max_t *minMaxEpPos, int *minSenlim, int *maxSenlim)
{
    int dSen, minSen, maxSen;
    int maxPdNum, maxPdNum2, maxSlopeVal;
    int minX, maxX, minY, maxY;
    vec_t p0, p1, p2, p3;
    vec_t pSt0, pSt1, pEd0, pEd1, pDtSt, pDtEd;
    int i, ret, dir;
    sen_type_t sen = BS_clipping_range_info[(int)axis].sensor;

    if (axis == ENUM_HOR_X) {
        maxPdNum = BS_maxHorPdNum - 1;
        maxSlopeVal = BS_slopeValMaxX;
        maxPdNum2 = BS_maxVerPdNum - 1;
        IS_DEBUG_FLAG2{
            TRACE_NOP;
        };
    }
    else {
        maxPdNum = BS_maxVerPdNum - 1;
        maxSlopeVal = BS_slopeValMaxY;
        maxPdNum2 = BS_maxHorPdNum - 1;
        IS_DEBUG_FLAG2{
            TRACE_NOP;
        };
    }

    //get min/max value
    minX = minMaxEp->minX;
    maxX = minMaxEp->maxX;
    minY = minMaxEp->minY;
    maxY = minMaxEp->maxY;

    //get initial range
    if (axis == ENUM_HOR_X) {
        if (sen == ENUM_PD) {
            dSen = (maxSlopeVal * (maxPdNum2 - minY)) / maxPdNum2;
        }
        else {
            dSen = (maxSlopeVal * maxY) / maxPdNum2;
        }
    }
    else {
        if (sen == ENUM_PD) {
            dSen = (maxSlopeVal * maxX) / maxPdNum2;
        }
        else {
            dSen = (maxSlopeVal * (maxPdNum2 - minX)) / maxPdNum2;
        }
    }
    if (axis == ENUM_HOR_X) {
        minSen = GET_MAX((minX - dSen - INITIAL_POL_TOL), 0);
        maxSen = GET_MIN((maxX + dSen + INITIAL_POL_TOL), maxPdNum);
    }
    else {
        minSen = GET_MAX((minY - dSen - INITIAL_POL_TOL), 0);
        maxSen = GET_MIN((maxY + dSen + INITIAL_POL_TOL), maxPdNum);
    }
    *minSenlim = minSen;
    *maxSenlim = maxSen;
    IS_DEBUG_FLAG2{
    TRACE_MGCR("axis,sen, minX,maxX, minY,maxY: (%d-%d) %d,%d %d,%d", axis, sen, minX, maxX, minY, maxY);
    TRACE_MGCR("minSen,maxSen: %d, %d", minSen, maxSen);
    TRACE_NOP;
    };

#if 1 //get fine range
    p0.x = minMaxEpPos->minX;
    p0.y = minMaxEpPos->minY;
    p1.x = minMaxEpPos->minX;
    p1.y = minMaxEpPos->maxY;
    p2.x = minMaxEpPos->maxX;
    p2.y = minMaxEpPos->maxY;
    p3.x = minMaxEpPos->maxX;
    p3.y = minMaxEpPos->minY;
    if (axis == ENUM_HOR_X) {
        if (sen == ENUM_PD) {
            pDtSt = p0;
            pDtEd = p3;
            dir = -1;
        }
        else {
            pDtSt = p1;
            pDtEd = p2;
            dir = 1;
        }
    }
    else {
        if (sen == ENUM_PD) {
            pDtSt = p3;
            pDtEd = p2;
            dir = -1;
        }
        else {
            pDtSt = p0;
            pDtEd = p1;
            dir = 1;
        }
    }
#if (DEBUG_BS_get_clipping_range > 0)
    IS_DEBUG_FLAG2{
    //TRACE_POLYGON_MULTI(0, 0x21, 0);
        //DEBUG_SHOW_BS_INITIAL_POLYGON();
        DEBUG_SHOW_MIN_MAX(minMaxEpPos, 1, 0); //color, mode:1(show it at debug on)
    };
#endif

    //start pos
    minSen = get_pos_sted(axis, 0, minSen, minMaxEp, minMaxEpPos, 0, 0, 1); //cal minSen
    for (i = minSen; i >= 0; i--) {
        get_pos_sted(axis, 0, i, minMaxEp, minMaxEpPos, &pSt0, &pSt1, 0);
#if (DEBUG_BS_get_clipping_range > 0)
        IS_DEBUG_FLAG2{
        DEBUG_SHOW_LINE_POS(&pSt0, &pSt1, 1);
        DEBUG_SHOW_POS(&pDtSt, 0.5f, 0.5f, 1);
        };
#endif
        ret = BS_left_of(&pSt0, &pSt1, &pDtSt);
        if ((ret * dir) < 0) {
            //left
            break;
        }
    }
    if (i == minSen) {
        for (i = (minSen + 1); i <= maxPdNum; i++) {
            get_pos_sted(axis, 0, i, minMaxEp, minMaxEpPos, &pSt0, &pSt1, 0);
#if (DEBUG_BS_get_clipping_range > 0)
            IS_DEBUG_FLAG2{
            DEBUG_SHOW_LINE_POS(&pSt0, &pSt1, 1);
            DEBUG_SHOW_POS(&pDtSt, 0.5f, 0.5f, 1);
            };
#endif
            ret = BS_left_of(&pSt0, &pSt1, &pDtSt);
            if ((ret * dir) >= 0) {
                //right
                break;
            }
        }
        i--;
    }
    if (i < 0) {
        i = 0;
    }
    minSen = i;
    IS_DEBUG_FLAG2{
        TRACE_NOP;
    };

    //end pos
    maxSen = get_pos_sted(axis, 1, maxSen, minMaxEp, minMaxEpPos, 0, 0, 1); //cal maxSen
    for (i = maxSen; i <= maxPdNum; i++) {
        get_pos_sted(axis, 1, i, minMaxEp, minMaxEpPos, &pEd0, &pEd1, 0);
#if (DEBUG_BS_get_clipping_range > 0)
        IS_DEBUG_FLAG2{
        DEBUG_SHOW_LINE_POS(&pEd0, &pEd1, 1);
        DEBUG_SHOW_POS(&pDtEd, 0.5f, 0.5f, 1);
        };
#endif
        ret = BS_left_of(&pEd0, &pEd1, &pDtEd);
        if ((ret * dir) > 0) {
            //right
            break;
        }
    }
    if (i == maxSen) {
        for (i = (maxSen - 1); i >= 0; i--) {
            get_pos_sted(axis, 1, i, minMaxEp, minMaxEpPos, &pEd0, &pEd1, 0);
#if (DEBUG_BS_get_clipping_range > 0)
            IS_DEBUG_FLAG2{
            DEBUG_SHOW_LINE_POS(&pEd0, &pEd1, 1);
            DEBUG_SHOW_POS(&pDtEd, 0.5f, 0.5f, 1);
            };
#endif
            ret = BS_left_of(&pEd0, &pEd1, &pDtEd);
            if ((ret * dir) <= 0) {
                //left
                break;
            }
        }
        i++;
    }
    if (i > maxPdNum) {
        i = maxPdNum;
    }
    maxSen = i;
#endif //1

    *minSenlim = minSen;
    *maxSenlim = maxSen;
    IS_DEBUG_FLAG2{ TRACE_MGCR("=>minSen,maxSen: %d, %d", minSen, maxSen); };
    IS_DEBUG_FLAG2{
    TRACE_NOP;
    };
    return 0;
}

#if 1 //nsmoon@220115 BG_touch_data_edge
#define CROSS_LINE_EDGE_X_SIZE  30
static int is_cross_line_edge_x(vec_t *p2, vec_t *p3)
{
    vec_t p0, p1, pR;

    if (BG_touch_data_edge.y > (BS_aarea_end_y - EPSILON)) {
        //top
        p1.y = p0.y = (BS_sensor_end_y - 1.0f);
    }
    else if (BG_touch_data_edge.y < (BS_aarea_zero_y + EPSILON)) {
        //bottom
        p1.y = p0.y = (BS_sensor_zero_y + 1.0f);
    }
    else {
        TRACE_ERROR("ERROR! is_cross_line_edge_x..invalid BG_touch_data_edge.y %f", BG_touch_data_edge.y);
        return 0; //error
    }

    p0.x = GET_MAX((BG_touch_data_edge.x - CROSS_LINE_EDGE_X_SIZE), 0);
    p1.x = GET_MIN((BG_touch_data_edge.x + CROSS_LINE_EDGE_X_SIZE), BS_sensor_end_x);

    if (BS_line_intersection(&p0, &p1, p2, p3, &pR)) {
#if 1 //for test
        IS_DEBUG_FLAG{
            DEBUG_SHOW_LINE_POS(&p0, &p1, MY_COLOR);
            //DEBUG_SHOW_LINE_POS(p2, p3, MY_COLOR/* - 3*/);
            TRACE_NOP;
        };
#endif
        return 1; //cross
    }

    return 0; //not-cross
}

#define CROSS_LINE_EDGE_Y_SIZE  30
static int is_cross_line_edge_y(vec_t *p2, vec_t *p3)
{
    vec_t p0, p1, pR;

    if (BG_touch_data_edge.x > (BS_aarea_end_x - EPSILON)) {
        //left
        p1.x = p0.x = (BS_sensor_end_x - 1.0f);
    }
    else if (BG_touch_data_edge.x < (BS_aarea_zero_x + EPSILON)) {
        //right
        p1.x = p0.x = (BS_sensor_zero_x + 1.0f);
    }
    else {
        TRACE_ERROR("ERROR! is_cross_line_edge_y..invalid BG_touch_data_edge.x %f", BG_touch_data_edge.x);
        return 0; //error
    }

    p0.y = GET_MAX((BG_touch_data_edge.y - CROSS_LINE_EDGE_Y_SIZE), 0);
    p1.y = GET_MIN((BG_touch_data_edge.y + CROSS_LINE_EDGE_Y_SIZE), BS_sensor_end_y);

    if (BS_line_intersection(&p0, &p1, p2, p3, &pR)) {
#if 1 //for test
        IS_DEBUG_FLAG{
            DEBUG_SHOW_LINE_POS(&p0, &p1, MY_COLOR);
            //DEBUG_SHOW_LINE_POS(p2, p3, MY_COLOR/* - 3*/);
            TRACE_NOP;
        };
#endif
        return 1; //cross
    }

    return 0; //not-cross
}
#endif

static int is_cross_line_tp(axis_t axis, int mode, int tpStart, int tpEnd, vec_t *p2, vec_t *p3)
{
    int tIdx, sideIdx, edge = 0, edge2 = 0;
    vec_t centerPoint, touchHalfWidth;
    float touchArea, cent, cent2;
    float gap_large_touch;
    vec_t p0, p1, pR;
    axis_t axis2 = (axis == ENUM_HOR_X) ? ENUM_VER_Y : ENUM_HOR_X;

#if 1 //nsmoon@220115
    tpStart = GET_MAX(tpStart, 0);
    tpEnd = GET_MIN(tpEnd, BG_touch_count);
#endif

    for (tIdx = tpStart; tIdx < tpEnd; tIdx++) {
        centerPoint.x = BG_touch_data[tIdx].x;
        centerPoint.y = BG_touch_data[tIdx].y;
        touchArea = BG_touch_size[tIdx].xSize * BG_touch_size[tIdx].ySize;
        touchHalfWidth.x = BG_touch_size[tIdx].xSize * 0.5f;
        touchHalfWidth.y = BG_touch_size[tIdx].ySize * 0.5f;

#if (BRUSH_MODE_ENABLE == 1)
        if (BS_brushMode) {
            gap_large_touch = LARGE_TOUCH_GAP_BRUSH;
        }
#endif
#if 1 //nsmoon@220115
        else if (mode == ADJUST_USED_LINE_FINAL) {
            gap_large_touch = LARGE_TOUCH_GAP_BRUSH;
        }
#endif
#if 1 //nsmoon@220116
        else if (mode == ADJUST_USED_LINE_SHADOW) {
            gap_large_touch = LARGE_TOUCH_GAP_SHADOW;
        }
#endif
        else if (touchArea > TOUCH_ERASER_SIZESQ_XLARGE) {
            gap_large_touch = LARGE_TOUCH_GAP_XLARGE;
        }
        else if (touchArea > TOUCH_ERASER_SIZESQ_LARGE) {
            gap_large_touch = LARGE_TOUCH_GAP_LARGE;
        }
        else if (touchArea > TOUCH_ERASER_SIZESQ_NORM) {
            gap_large_touch = LARGE_TOUCH_GAP_NORM;
        }
        else {
            cent = (axis == ENUM_HOR_X) ? centerPoint.x : centerPoint.y;
            edge = BS_is_edge(axis, cent);
#if 1  //nsmoon@200309
            cent2 = (axis2 == ENUM_HOR_X) ? centerPoint.x : centerPoint.y;
            edge2 = BS_is_edge(axis2, cent2);
#endif
            //IS_DEBUG_FLAG{TRACE("..edge,edge2= (%d-%d) %d(%0.1f) %d(%0.1f)", axis2, tIdx, edge, cent, edge2, cent2);};
            if (edge || edge2) {
                if (mode == ADJUST_USED_LINE_MULTI) {
                    gap_large_touch = LARGE_TOUCH_GAP_SMALL_MULTI; //nsmoon@200803
                }
                else {
                    gap_large_touch = LARGE_TOUCH_GAP_LARGE;
                }
            }
            else {
                if (mode == ADJUST_USED_LINE_MULTI) {
                    gap_large_touch = LARGE_TOUCH_GAP_SMALL_MULTI; //nsmoon@200402
                }
                else {
                    gap_large_touch = LARGE_TOUCH_GAP_SMALL;
                }
            }
            //IS_DEBUG_FLAG{TRACE("..edge,edge2= (%d-%d) %d(%0.1f) %d(%0.1f) %0.1f", axis2, tIdx, edge, cent, edge2, cent2, gap_large_touch);};
        }
#if 0 //for test
        IS_DEBUG_FLAG{
            //if (mode == ADJUST_USED_LINE_MULTI)
            {
                TRACE("..touchArea,gap_large_touch= (%d) %0.1f(%0.1f %0.1f) %0.1f (%d-0x%x/0x%x) ", axis, touchArea, BG_touch_size[tIdx].xSize, BG_touch_size[tIdx].ySize, gap_large_touch, tIdx, edge, edge2);
            }
        };
#endif

        for (sideIdx = 0; sideIdx < 4; sideIdx++)
        {
            if (axis == ENUM_HOR_X) {
                if (sideIdx == 0) {
                    //top
                    p0.y = centerPoint.y + touchHalfWidth.y + gap_large_touch;
                }
                else if (sideIdx == 1) {
                    //bottom
                    p0.y = centerPoint.y - touchHalfWidth.y - gap_large_touch;
                }
                else {
                    continue;
                }
                p0.x = centerPoint.x - touchHalfWidth.x - gap_large_touch;
                p1.x = centerPoint.x + touchHalfWidth.x + gap_large_touch;
                p1.y = p0.y;
            }
            else {
                if (sideIdx == 2) {
                    //right
                    p0.x = centerPoint.x - touchHalfWidth.x - gap_large_touch;
                }
                else if (sideIdx == 3) {
                    //left
                    p0.x = centerPoint.x + touchHalfWidth.x + gap_large_touch;
                }
                else {
                    continue;
                }
                p0.y = centerPoint.y - touchHalfWidth.y - gap_large_touch;
                p1.y = centerPoint.y + touchHalfWidth.y + gap_large_touch;
                p1.x = p0.x;
            }
#if 1 //for test
            IS_DEBUG_FLAG{
                if (mode == ADJUST_USED_LINE_MULTI) {
                    DEBUG_SHOW_LINE_POS(&p0, &p1, MY_COLOR);
                    TRACE_NOP;
                }
            };
#endif
            if (BS_line_intersection(&p0, &p1, p2, p3, &pR)) {
#if 0 //for test
                IS_DEBUG_FLAG{
                    //DEBUG_SHOW_LINE_POS(&p0, &p1, MY_COLOR - 2);
                    DEBUG_SHOW_LINE_POS(p2, p3, MY_COLOR/* - 3*/);
                    TRACE_NOP;
                };
#endif
                return 1; //cross
            }
        } //for (sideIdx = 0; sideIdx < 4; sideIdx++)
    }

    return 0; //not-cross
}

ATTR_BACKEND_RAMFUNC
int BS_adj_used_lines_tp(int tpStart, int tpEnd, int mode)
{
    uint16_t *remLine;
    int remLineCnt, remLineIdx;
    int pd0, led0;
    int tmpInBufIdx;
    vec_t p2, p3;
    int axis;

    IS_DEBUG_FLAG{
        //TRACE("BS_adj_used_lines_tp... %d %d %d", tpStart, tpEnd, mode);
        TRACE_NOP;
    };

    for (axis = 0; axis < (int)ENUM_AXIS_END; axis++)
    {
        if (axis == (int)ENUM_HOR_X) {
            remLine = &BS_remained_x[0];
            remLineCnt = BS_remained_x_cnt;
        }
        else {
#if 0 //nsmoon@220127 not-used
            if (mode == ADJUST_USED_LINE_EDGE_X) {
                continue;
            }
#endif
            remLine = &BS_remained_y[0];
            remLineCnt = BS_remained_y_cnt;
        }
        //TRACE("..axis... %d %d", axis, remLineCnt);

        for (remLineIdx = 0; remLineIdx < remLineCnt; remLineIdx++)
        {
            tmpInBufIdx = remLine[remLineIdx];
            if (BS_getSenInBuf((axis_t)axis, tmpInBufIdx, &pd0, &led0)) {
                //TRACE_ERROR("ERROR! BS_adj_used_lines_tp..invalid inBufIdx: %d", tmpInBufIdx);
                continue;
            }
            if (axis == ENUM_HOR_X) {
                p2.x = BS_led_pos_x[led0];
                p3.x = BS_pd_pos_x[pd0];
                p2.y = BS_sensor_zero_y;
                p3.y = BS_sensor_end_y;
            }
            else {
                p2.y = BS_led_pos_y[led0];
                p3.y = BS_pd_pos_y[pd0];
                p2.x = BS_sensor_end_x;
                p3.x = BS_sensor_zero_x;
            }
            if (mode == ADJUST_USED_LINE_EDGE_X) {
                if (is_cross_line_edge_x(&p2, &p3)) {
                    //set used line
                    BS_set_unset2_used_led_pd((axis_t)axis, led0, pd0, 1); //1:set
                }
                else {
    #if 0 //for test
                    IS_DEBUG_FLAG{
                        DEBUG_SHOW_LINE_POS(&p2, &p3, MY_COLOR - 2);
                        TRACE_NOP;
                    };
    #endif
                }
            }
#if 1 //nsmoon@220127
            else if (mode == ADJUST_USED_LINE_EDGE_Y) {
                if (is_cross_line_edge_y(&p2, &p3)) {
                    //set used line
                    BS_set_unset2_used_led_pd((axis_t)axis, led0, pd0, 1); //1:set
                }
                else {
    #if 0 //for test
                    IS_DEBUG_FLAG{
                        DEBUG_SHOW_LINE_POS(&p2, &p3, MY_COLOR - 2);
                        TRACE_NOP;
                    };
    #endif
                }
            }
#endif
            else {
            if (is_cross_line_tp((axis_t)axis, mode, tpStart, tpEnd, &p2, &p3)) {
                //set used line
                BS_set_unset2_used_led_pd((axis_t)axis, led0, pd0, 1); //1:set
            }
            else {
#if 0 //for test
                IS_DEBUG_FLAG{
                    DEBUG_SHOW_LINE_POS(&p2, &p3, MY_COLOR - 2);
                    TRACE_NOP;
                };
#endif
            }
            }
        } //for (remLineIdx = 0; remLineIdx < remLineCnt; remLineIdx++)
    } //for (axis = 0; axis < (int)ENUM_AXIS_END; axis++)
    if (BS_remained_x_cnt && BS_remained_y_cnt) {
        return 2;
    }
    else if (BS_remained_x_cnt || BS_remained_y_cnt) {
        return 1;
    }
    else {
        return 0; //no-remained
    }
}
////////////////////////////////////////////
/*End of File*/
