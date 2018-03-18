#ifndef HAND_DATA_H__
#define HAND_DATA_H__
#include "list.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

extern GLFWwindow *data_window;

#ifdef NEWER_CGLM
#define GLM_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#endif
#include <cglm/cglm.h>

void data_init(VkExtent2D extent);
void data_update();

// Maximum number of bones per vertex
#define MAX_BONES_PER_VERTEX 4

extern uintptr_t data_uniform_ptr;

extern const void *data_model_vertex;
extern const VkDeviceSize data_model_vertex_size;

extern const VkVertexInputBindingDescription data_binding_description;
extern const VkVertexInputAttributeDescription data_attribute_descriptions[];
extern const uint32_t data_attribute_descriptions_count;

extern const struct data_model_index *data_model_index;
extern const VkDeviceSize data_model_index_size;

extern struct shared_ubo *data_model_shared_ubo;

typedef struct {
  char *name;
  uint32_t first;
  uint32_t count;
  size_t material;
} data_model_t;
extern data_model_t data_models[];

typedef struct data_model_instance {
  char *name;
  size_t model;
  void (*update)(struct data_model_instance *, double);
  void *closure;
  struct list_head list;
  void *ubo;
  struct list_head *uniform_map;
} data_model_instance_t;
extern struct list_head data_model_instances;
extern void (*data_setup_model_instance)(data_model_instance_t *inst);

typedef struct {
  struct {
    const void *spv;
    const size_t *size;
  } vert, frag;
} data_material_shader_t;
extern const data_material_shader_t data_material_shaders[];

typedef struct {
  char *name;
  size_t shader;
  VkDeviceSize ubo_vertex_shader_size;
  mat4 *(*get_model)(void *);
  VkFormat texture_format;
  uint32_t texture_width;
  uint32_t texture_height;
  void *texture;
  VkDeviceSize texture_size;
} data_material_t;
extern const data_material_t data_materials[];

#define UNIFORM_ALIGN (sizeof(float) * 4)
#define FROM_UNIFORM_ALLOC(x) (x * UNIFORM_ALIGN)
#define TO_UNIFORM_ALLOC(x) ((x + UNIFORM_ALIGN - 1) / UNIFORM_ALIGN)
#define UNIFORM_SIZE (4096 * 4096)
#define SHARED_UNIFORM_SIZE (4096 * 32)
typedef struct {
  VkDescriptorSet descriptor_set;
  size_t offset;
  size_t range;
  size_t width;
  struct list_head list;
} uniform_map_t;

#endif
