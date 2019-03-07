/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

/*!
 * @brief SIMD like functions
 */

/*
 Functions:
   CGLM_INLINE void  glm_vec_mulv(vec3 a, vec3 b, vec3 d);
   CGLM_INLINE void  glm_vec_broadcast(float val, vec3 d);
   CGLM_INLINE bool  glm_vec_eq(vec3 v, float val);
   CGLM_INLINE bool  glm_vec_eq_eps(vec4 v, float val);
   CGLM_INLINE bool  glm_vec_eq_all(vec3 v);
   CGLM_INLINE bool  glm_vec_eqv(vec3 v1, vec3 v2);
   CGLM_INLINE bool  glm_vec_eqv_eps(vec3 v1, vec3 v2);
   CGLM_INLINE float glm_vec_max(vec3 v);
   CGLM_INLINE float glm_vec_min(vec3 v);
 */

#ifndef cglm_vec3_ext_h
#define cglm_vec3_ext_h

#include "common.h"
#include <stdbool.h>
#include <math.h>
#include <float.h>

/*!
 * @brief multiplies individual items, just for convenient like SIMD
 *
 * @param a vec1
 * @param b vec2
 * @param d vec3 = (v1[0] * v2[0],  v1[1] * v2[1], v1[2] * v2[2])
 */
CGLM_INLINE
void
glm_vec_mulv(vec3 a, vec3 b, vec3 d) {
  d[0] = a[0] * b[0];
  d[1] = a[1] * b[1];
  d[2] = a[2] * b[2];
}

/*!
 * @brief fill a vector with specified value
 *
 * @param val value
 * @param d   dest
 */
CGLM_INLINE
void
glm_vec_broadcast(float val, vec3 d) {
  d[0] = d[1] = d[2] = val;
}

/*!
 * @brief check if vector is equal to value (without epsilon)
 *
 * @param v   vector
 * @param val value
 */
CGLM_INLINE
bool
glm_vec_eq(vec3 v, float val) {
  return v[0] == val && v[0] == v[1] && v[0] == v[2];
}

/*!
 * @brief check if vector is equal to value (with epsilon)
 *
 * @param v   vector
 * @param val value
 */
CGLM_INLINE
bool
glm_vec_eq_eps(vec4 v, float val) {
  return fabsf(v[0] - val) <= FLT_EPSILON
         && fabsf(v[1] - val) <= FLT_EPSILON
         && fabsf(v[2] - val) <= FLT_EPSILON;
}

/*!
 * @brief check if vectors members are equal (without epsilon)
 *
 * @param v   vector
 */
CGLM_INLINE
bool
glm_vec_eq_all(vec3 v) {
  return v[0] == v[1] && v[0] == v[2];
}

/*!
 * @brief check if vector is equal to another (without epsilon)
 *
 * @param v1 vector
 * @param v2 vector
 */
CGLM_INLINE
bool
glm_vec_eqv(vec3 v1, vec3 v2) {
  return v1[0] == v2[0]
        && v1[1] == v2[1]
        && v1[2] == v2[2];
}

/*!
 * @brief check if vector is equal to another (with epsilon)
 *
 * @param v1 vector
 * @param v2 vector
 */
CGLM_INLINE
bool
glm_vec_eqv_eps(vec3 v1, vec3 v2) {
  return fabsf(v1[0] - v2[0]) <= FLT_EPSILON
         && fabsf(v1[1] - v2[1]) <= FLT_EPSILON
         && fabsf(v1[2] - v2[2]) <= FLT_EPSILON;
}

/*!
 * @brief max value of vector
 *
 * @param v vector
 */
CGLM_INLINE
float
glm_vec_max(vec3 v) {
  float max;

  max = v[0];
  if (v[1] > max)
    max = v[1];
  if (v[2] > max)
    max = v[2];

  return max;
}

/*!
 * @brief min value of vector
 *
 * @param v vector
 */
CGLM_INLINE
float
glm_vec_min(vec3 v) {
  float min;

  min = v[0];
  if (v[1] < min)
    min = v[1];
  if (v[2] < min)
    min = v[2];

  return min;
}

#endif /* cglm_vec3_ext_h */
