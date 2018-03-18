#ifndef HAND_DATA_H__
#define HAND_DATA_H__

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

extern GLFWwindow *data_window;

#ifdef NEWER_CGLM
#define GLM_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#endif
#include <cglm/cglm.h>

extern const void *data_model_vertex;
extern const size_t data_model_vertex_count;
extern const VkDeviceSize data_model_vertex_size;

extern const VkVertexInputBindingDescription data_binding_description;
extern const VkVertexInputAttributeDescription data_attribute_descriptions[];
extern const uint32_t data_attribute_descriptions_count;

typedef struct
{
        struct
        {
                const void *spv;
                const size_t *size;
        } vert, frag;
} data_material_shader_t;
extern const data_material_shader_t data_material_shaders[];

#endif
