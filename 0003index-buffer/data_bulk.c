#include "data_bulk.h"

typedef struct
{
        vec3 position;
        vec3 normal;
        float uv[2];
        float boneWeights[MAX_BONES_PER_VERTEX];
        uint32_t boneIDs[MAX_BONES_PER_VERTEX];
} vertex_t;
const vertex_t model_vertex[] = {
    {{-0.5f, 0.5f, -0.5f},
     {0.0f, 0.0f, -1.0f},
     {0.0f, 2.0f / 3.0f},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {{0.5f, -0.5f, -0.5f},
     {0.0f, 0.0f, -1.0f},
     {0.0f, 1.0f / 3.0f},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {{-0.5f, -0.5f, -0.5f},
     {0.0f, 0.0f, -1.0f},
     {0.25f, 1.0f / 3.0f},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {{0.5f, 0.5f, -0.5f},
     {0.0f, 0.0f, -1.0f},
     {0.25f, 2.0f / 3.0f},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
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
    {.location = 1,
     .binding = 0,
     .format = VK_FORMAT_R32G32B32_SFLOAT,
     .offset = offsetof(vertex_t, normal)},
    {.location = 2,
     .binding = 0,
     .format = VK_FORMAT_R32G32_SFLOAT,
     .offset = offsetof(vertex_t, uv)},
    {.location = 3,
     .binding = 0,
     .format = VK_FORMAT_R32G32B32A32_SFLOAT,
     .offset = offsetof(vertex_t, boneWeights)},
    {.location = 4,
     .binding = 0,
     .format = VK_FORMAT_R32G32B32A32_SINT,
     .offset = offsetof(vertex_t, boneIDs)},
};
const uint32_t data_attribute_descriptions_count =
    sizeof(data_attribute_descriptions) /
    sizeof(data_attribute_descriptions[0]);

const struct data_model_index
{
        struct
        {
                uint32_t first, second, third;
        } square[2];
} *data_model_index = &(struct data_model_index){{
    {0, 2, 1},
    {0, 1, 3},
}};
const VkDeviceSize data_model_index_size = sizeof(*data_model_index);

data_model_t data_models[] = {
    {
        "Square",
        offsetof(struct data_model_index, square) / sizeof(uint32_t),
        sizeof(data_model_index->square) / sizeof(uint32_t),
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
