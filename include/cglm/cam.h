/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

/*
 Functions:
   CGLM_INLINE void glm_frustum(float left,
                                float right,
                                float bottom,
                                float top,
                                float nearVal,
                                float farVal,
                                mat4  dest);
   CGLM_INLINE void glm_ortho(float left,
                              float right,
                              float bottom,
                              float top,
                              float nearVal,
                              float farVal,
                              mat4  dest);
   CGLM_INLINE void glm_ortho_default(float aspect, mat4  dest);
   CGLM_INLINE void glm_ortho_default_s(float aspect, float size, mat4  dest);
   CGLM_INLINE void glm_perspective(float fovy,
                                    float aspect,
                                    float nearVal,
                                    float farVal,
                                    mat4  dest);
   CGLM_INLINE void glm_perspective_default(float aspect, mat4  dest);
   CGLM_INLINE void glm_perspective_resize(float aspect, mat4  proj);
   CGLM_INLINE void glm_lookat(vec3 eye, vec3 center, vec3 up, mat4 dest);
   CGLM_INLINE void glm_persp_decomp(mat4 proj,
                                     float * __restrict nearVal,
                                     float * __restrict farVal,
                                     float * __restrict top,
                                     float * __restrict bottom,
                                     float * __restrict left,
                                     float * __restrict right);
   CGLM_INLINE void glm_persp_decompv(mat4  proj, float dest[6]);
   CGLM_INLINE void glm_persp_decomp_x(mat4 proj,
                                       float * __restrict left,
                                       float * __restrict right);
   CGLM_INLINE void glm_persp_decomp_y(mat4 proj,
                                       float * __restrict top,
                                       float * __restrict bottom);
   CGLM_INLINE void glm_persp_decomp_z(mat4 proj,
                                       float * __restrict nearVal,
                                       float * __restrict farVal);
   CGLM_INLINE void glm_persp_decomp_far(mat4 proj, float * __restrict farVal);
   CGLM_INLINE void glm_persp_decomp_near(mat4 proj, float *__restrict nearVal);
 */

#ifndef cglm_vcam_h
#define cglm_vcam_h

#include "common.h"

/*!
 * @brief set up perspective peprojection matrix
 *
 * @param[in]  left    viewport.left
 * @param[in]  right   viewport.right
 * @param[in]  bottom  viewport.bottom
 * @param[in]  top     viewport.top
 * @param[in]  nearVal near clipping plane
 * @param[in]  farVal  far clipping plane
 * @param[out] dest    result matrix
 */
CGLM_INLINE
void
glm_frustum(float left,
            float right,
            float bottom,
            float top,
            float nearVal,
            float farVal,
            mat4  dest) {
  float rl, tb, fn, nv;

  glm__memzero(float, dest, sizeof(mat4));

  rl = 1.0f / (right  - left);
  tb = 1.0f / (top    - bottom);
  fn =-1.0f / (farVal - nearVal);
  nv = 2.0f * nearVal;

  dest[0][0] = nv * rl;
  dest[1][1] = nv * tb;
  dest[2][0] = (right  + left)    * rl;
  dest[2][1] = (top    + bottom)  * tb;
  dest[2][2] = (farVal + nearVal) * fn;
  dest[2][3] =-1.0f;
  dest[3][2] = farVal * nv * fn;
}

/*!
 * @brief set up orthographic projection matrix
 *
 * @param[in]  left    viewport.left
 * @param[in]  right   viewport.right
 * @param[in]  bottom  viewport.bottom
 * @param[in]  top     viewport.top
 * @param[in]  nearVal near clipping plane
 * @param[in]  farVal  far clipping plane
 * @param[out] dest    result matrix
 */
CGLM_INLINE
void
glm_ortho(float left,
          float right,
          float bottom,
          float top,
          float nearVal,
          float farVal,
          mat4  dest) {
  float rl, tb, fn;

  glm__memzero(float, dest, sizeof(mat4));

  rl = 1.0f / (right  - left);
  tb = 1.0f / (top    - bottom);
  fn =-1.0f / (farVal - nearVal);

  dest[0][0] = 2.0f * rl;
  dest[1][1] = 2.0f * tb;
  dest[2][2] = 2.0f * fn;
  dest[3][0] =-(right  + left)    * rl;
  dest[3][1] =-(top    + bottom)  * tb;
  dest[3][2] = (farVal + nearVal) * fn;
  dest[3][3] = 1.0f;
}

/*!
 * @brief set up unit orthographic projection matrix
 *
 * @param[in]  aspect aspect ration ( width / height )
 * @param[out] dest   result matrix
 */
CGLM_INLINE
void
glm_ortho_default(float aspect,
                  mat4  dest) {
  if (aspect >= 1.0f) {
    glm_ortho(-1.0f * aspect,
               1.0f * aspect,
              -1.0f,
               1.0f,
              -100.0f,
               100.0f,
               dest);
	  return;
  }

  glm_ortho(-1.0f,
             1.0f,
            -1.0f / aspect,
             1.0f / aspect,
            -100.0f,
             100.0f,
             dest);
}

/*!
 * @brief set up orthographic projection matrix with given CUBE size
 *
 * @param[in]  aspect aspect ratio ( width / height )
 * @param[in]  size   cube size
 * @param[out] dest   result matrix
 */
CGLM_INLINE
void
glm_ortho_default_s(float aspect,
                    float size,
                    mat4  dest) {
  if (aspect >= 1.0f) {
    glm_ortho(-size * aspect,
               size * aspect,
              -size,
               size,
              -size - 100.0f,
               size + 100.0f,
               dest);
	  return;
  }

  glm_ortho(-size,
             size,
            -size / aspect,
             size / aspect,
            -size - 100.0f,
             size + 100.0f,
             dest);
}

/*!
 * @brief set up perspective projection matrix
 *
 * @param[in]  fovy    field of view angle
 * @param[in]  aspect  aspect ratio ( width / height )
 * @param[in]  nearVal near clipping plane
 * @param[in]  farVal  far clipping planes
 * @param[out] dest    result matrix
 */
CGLM_INLINE
void
glm_perspective(float fovy,
                float aspect,
                float nearVal,
                float farVal,
                mat4  dest) {
  float f, fn;

  glm__memzero(float, dest, sizeof(mat4));

  f  = 1.0f / tanf(fovy * 0.5f);
  fn = 1.0f / (nearVal - farVal);

  dest[0][0] = f / aspect;
  dest[1][1] = f;
  dest[2][2] = (nearVal + farVal) * fn;
  dest[2][3] =-1.0f;
  dest[3][2] = 2.0f * nearVal * farVal * fn;
}

/*!
 * @brief set up perspective projection matrix with default near/far
 *        and angle values
 *
 * @param[in]  aspect aspect ratio ( width / height )
 * @param[out] dest   result matrix
 */
CGLM_INLINE
void
glm_perspective_default(float aspect,
                        mat4  dest) {
  glm_perspective((float)CGLM_PI_4,
                  aspect,
                  0.01f,
                  100.0f,
                  dest);
}

/*!
 * @brief resize perspective matrix by aspect ratio ( width / height )
 *        this very make easy to resize proj matrix when window, viewport
 *        reized
 *
 * @param[in]      aspect aspect ratio ( width / height )
 * @param[in, out] proj   perspective projection matrix
 */
CGLM_INLINE
void
glm_perspective_resize(float aspect,
                       mat4  proj) {
  if (proj[0][0] == 0.0f)
    return;

  proj[0][0] = proj[1][1] / aspect;
}

/*!
 * @brief set up view matrix
 *
 * @param[in]  eye    eye vector
 * @param[in]  center center vector
 * @param[in]  up     up vector
 * @param[out] dest   result matrix
 */
CGLM_INLINE
void
glm_lookat(vec3 eye,
           vec3 center,
           vec3 up,
           mat4 dest) {
  vec3 f, u, s;

  glm_vec_sub(center, eye, f);
  glm_vec_normalize(f);

  glm_vec_cross(f, up, s);
  glm_vec_normalize(s);

  glm_vec_cross(s, f, u);

  dest[0][0] = s[0];
  dest[0][1] = u[0];
  dest[0][2] =-f[0];
  dest[1][0] = s[1];
  dest[1][1] = u[1];
  dest[1][2] =-f[1];
  dest[2][0] = s[2];
  dest[2][1] = u[2];
  dest[2][2] =-f[2];
  dest[3][0] =-glm_vec_dot(s, eye);
  dest[3][1] =-glm_vec_dot(u, eye);
  dest[3][2] = glm_vec_dot(f, eye);
  dest[0][3] = dest[1][3] = dest[2][3] = 0.0f;
  dest[3][3] = 1.0f;
}

/*!
 * @brief decomposes frustum values of perspective projection.
 *
 * @param[in]  proj    perspective projection matrix
 * @param[out] nearVal near
 * @param[out] farVal  far
 * @param[out] top     top
 * @param[out] bottom  bottom
 * @param[out] left    left
 * @param[out] right   right
 */
CGLM_INLINE
void
glm_persp_decomp(mat4 proj,
                 float * __restrict nearVal,
                 float * __restrict farVal,
                 float * __restrict top,
                 float * __restrict bottom,
                 float * __restrict left,
                 float * __restrict right) {
  *nearVal = proj[3][2] / (proj[2][2] - 1);
  *farVal  = proj[3][2] / (proj[2][2] + 1);
  *bottom  = *nearVal * (proj[2][1] - 1) / proj[1][1];
  *top     = *nearVal * (proj[2][1] + 1) / proj[1][1];
  *left    = *nearVal * (proj[2][0] - 1) / proj[0][0];
  *right   = *nearVal * (proj[2][0] + 1) / proj[0][0];
}

/*!
 * @brief decomposes frustum values of perspective projection.
 *        this makes easy to get all values at once
 *
 * @param[in]  proj   perspective projection matrix
 * @param[out] dest   array
 */
CGLM_INLINE
void
glm_persp_decompv(mat4 proj, float dest[6]) {
  glm_persp_decomp(proj, &dest[0], &dest[1], &dest[2],
                         &dest[3], &dest[4], &dest[5]);
}

/*!
 * @brief decomposes left and right values of perspective projection.
 *        x stands for x axis (left / right axis)
 *
 * @param[in]  proj perspective projection matrix
 * @param[out] left  left
 * @param[out] right right
 */
CGLM_INLINE
void
glm_persp_decomp_x(mat4 proj,
                   float * __restrict left,
                   float * __restrict right) {
  float nearVal;

  nearVal = proj[3][2] / (proj[3][3] - 1);
  *left   = nearVal * (proj[2][0] - 1) / proj[0][0];
  *right  = nearVal * (proj[2][0] + 1) / proj[0][0];
}

/*!
 * @brief decomposes top and bottom values of perspective projection.
 *        y stands for y axis (top / botom axis)
 *
 * @param[in]  proj   perspective projection matrix
 * @param[out] top    top
 * @param[out] bottom bottom
 */
CGLM_INLINE
void
glm_persp_decomp_y(mat4 proj,
                   float * __restrict top,
                   float * __restrict bottom) {
  float nearVal;

  nearVal = proj[3][2] / (proj[3][3] - 1);
  *bottom = nearVal * (proj[2][1] - 1) / proj[1][1];
  *top    = nearVal * (proj[2][1] + 1) / proj[1][1];
}

/*!
 * @brief decomposes near and far values of perspective projection.
 *        z stands for z axis (near / far axis)
 *
 * @param[in]  proj    perspective projection matrix
 * @param[out] nearVal near
 * @param[out] farVal  far
 */
CGLM_INLINE
void
glm_persp_decomp_z(mat4 proj,
                   float * __restrict nearVal,
                   float * __restrict farVal) {
  *nearVal = proj[3][2] / (proj[2][2] - 1);
  *farVal  = proj[3][2] / (proj[2][2] + 1);
}

/*!
 * @brief decomposes far value of perspective projection.
 *
 * @param[in]  proj   perspective projection matrix
 * @param[out] farVal far
 */
CGLM_INLINE
void
glm_persp_decomp_far(mat4 proj, float * __restrict farVal) {
  *farVal = proj[3][2] / (proj[2][2] + 1);
}

/*!
 * @brief decomposes near value of perspective projection.
 *
 * @param[in]  proj    perspective projection matrix
 * @param[out] nearVal near
 */
CGLM_INLINE
void
glm_persp_decomp_near(mat4 proj, float * __restrict nearVal) {
  *nearVal = proj[3][2] / (proj[2][2] - 1);
}
#endif /* cglm_vcam_h */
