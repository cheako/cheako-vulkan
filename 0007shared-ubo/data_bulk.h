#include "data.h"

// Maximum number of bones per mesh
// Must not be higher than same const in skinning shader
#define MAX_BONES 64

typedef struct {
  mat4 model;
  mat4 bones[MAX_BONES];
} material_0_ubo_t;
