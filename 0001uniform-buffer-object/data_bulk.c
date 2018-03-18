#include "data_bulk.h"

typedef struct
{
        vec3 position;
} vertex_t;
const vertex_t model_vertex[] = {
    {{0.5f, -0.5f, -0.5f}},
    {{0.5f, 0.5f, -0.5f}},
    {{-0.5f, 0.5f, -0.5f}},
    {{0.5f, -0.5f, -0.5f}},
    {{-0.5f, 0.5f, -0.5f}},
    {{-0.5f, -0.5f, -0.5f}},
};
const void *data_model_vertex = model_vertex;
const VkDeviceSize data_model_vertex_size = sizeof(model_vertex);
const VkVertexInputBindingDescription data_binding_description = {
    .binding = 0,
    .stride = sizeof(vertex_t),
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};
const VkVertexInputAttributeDescription data_attribute_descriptions[] = {
    {.location = 0,
     .binding = 0,
     .format = VK_FORMAT_R32G32B32_SFLOAT,
     .offset = offsetof(vertex_t, position)},
};
const uint32_t data_attribute_descriptions_count =
    sizeof(data_attribute_descriptions) /
    sizeof(data_attribute_descriptions[0]);

data_model_t data_models[] = {
    {
        "Square",
        0,
        sizeof(model_vertex) / sizeof(uint32_t),
        0,
    },
};

const data_material_t data_materials[] = {
    {
        "Example",
        0,
        sizeof(material_0_ubo_t),
    },
};
