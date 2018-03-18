#include "data_bulk.h"

typedef struct
{
        vec3 position;
        vec3 normal;
        float uv[2];
        float boneWeights[MAX_BONES_PER_VERTEX];
        uint32_t boneIDs[MAX_BONES_PER_VERTEX];
} vertex_t;

/* Texture
 ####0AA1########
 ####AAAA########
 ####AAAA########
 ####2AA3########
 4BB56CC78DD9aEEb
 BBBBCCCCDDDDEEEE
 BBBBCCCCDDDDEEEE
 cBBdeCCfgDDhEEEi
 ####jFFk########
 ####FFFF########
 ####FFFF########
 ####lFFm########
 */

#define DMVPFR -0.5f
#define DMVPBA 0.5f
#define DMVPBO -0.5f
#define DMVPTO 0.5f
#define DMVPRI -0.5f
#define DMVPLE 0.5f

#define DMVUXONE 0.0f
#define DMVUXTWO 0.25f
#define DMVUXTHR 0.5f
#define DMVUXFOR 0.75f
#define DMVUXFIV 1.0f
#define DMVUYONE 0.0f
#define DMVUYTWO 1.0f / 3.0f
#define DMVUYTHR 2.0f / 3.0f
#define DMVUYFOR 1.0f
const vertex_t model_vertex[] = {
    {// 0
     {DMVPLE, DMVPTO, DMVPBA},
     {0.0f, DMVPTO * 2, 0.0f},
     {DMVUXTWO, DMVUYONE},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {// 1
     {DMVPRI, DMVPTO, DMVPBA},
     {0.0f, DMVPTO * 2, 0.0f},
     {DMVUXTHR, DMVUYONE},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {// 2
     {DMVPLE, DMVPTO, DMVPFR},
     {0.0f, DMVPTO * 2, 0.0f},
     {DMVUXTWO, DMVUYTWO},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {// 3
     {DMVPRI, DMVPTO, DMVPFR},
     {0.0f, DMVPTO * 2, 0.0f},
     {DMVUXTHR, DMVUYTWO},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {// 4
     {DMVPLE, DMVPTO, DMVPBA},
     {DMVPLE * 2, 0.0f, 0.0f},
     {DMVUXONE, DMVUYTWO},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {// 5
     {DMVPLE, DMVPTO, DMVPFR},
     {DMVPLE * 2, 0.0f, 0.0f},
     {DMVUXTWO, DMVUYTWO},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {// 6
     {DMVPLE, DMVPTO, DMVPFR},
     {0.0f, 0.0f, DMVPFR * 2},
     {DMVUXTWO, DMVUYTWO},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {// 7
     {DMVPRI, DMVPTO, DMVPFR},
     {0.0f, 0.0f, DMVPFR * 2},
     {DMVUXTHR, DMVUYTWO},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {// 8
     {DMVPRI, DMVPTO, DMVPFR},
     {DMVPRI * 2, 0.0f, 0.0f},
     {DMVUXTHR, DMVUYTWO},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {// 9
     {DMVPRI, DMVPTO, DMVPBA},
     {DMVPRI * 2, 0.0f, 0.0f},
     {DMVUXFOR, DMVUYTWO},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {// a
     {DMVPRI, DMVPTO, DMVPBA},
     {0.0f, 0.0f, DMVPBA * 2},
     {DMVUXFOR, DMVUYTWO},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {// b
     {DMVPLE, DMVPTO, DMVPBA},
     {0.0f, 0.0f, DMVPBA * 2},
     {DMVUXFIV, DMVUYTWO},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {// c
     {DMVPLE, DMVPBO, DMVPBA},
     {DMVPLE * 2, 0.0f, 0.0f},
     {DMVUXONE, DMVUYTHR},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {// d
     {DMVPLE, DMVPBO, DMVPFR},
     {DMVPLE * 2, 0.0f, 0.0f},
     {DMVUXTWO, DMVUYTHR},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {// e
     {DMVPLE, DMVPBO, DMVPFR},
     {0.0f, 0.0f, DMVPFR * 2},
     {DMVUXTWO, DMVUYTHR},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {// f
     {DMVPRI, DMVPBO, DMVPFR},
     {0.0f, 0.0f, DMVPFR * 2},
     {DMVUXTHR, DMVUYTHR},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {// g
     {DMVPRI, DMVPBO, DMVPFR},
     {DMVPRI * 2, 0.0f, 0.0f},
     {DMVUXTHR, DMVUYTHR},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {// h
     {DMVPRI, DMVPBO, DMVPBA},
     {DMVPRI * 2, 0.0f, 0.0f},
     {DMVUXFOR, DMVUYTHR},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {// i
     {DMVPRI, DMVPBO, DMVPBA},
     {0.0f, 0.0f, DMVPBA * 2},
     {DMVUXFOR, DMVUYTHR},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {// j
     {DMVPLE, DMVPBO, DMVPBA},
     {0.0f, 0.0f, DMVPBA * 2},
     {DMVUXFIV, DMVUYTHR},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {// k
     {DMVPLE, DMVPBO, DMVPFR},
     {0.0f, DMVPBO * 2, 0.0f},
     {DMVUXTWO, DMVUYTHR},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {// k
     {DMVPRI, DMVPBO, DMVPFR},
     {0.0f, DMVPBO * 2, 0.0f},
     {DMVUXTHR, DMVUYTHR},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {// m
     {DMVPLE, DMVPBO, DMVPBA},
     {0.0f, DMVPBO * 2, 0.0f},
     {DMVUXTWO, DMVUYFOR},
     {1.0f, 0.0f, 0.0f, 0.0f},
     {0, 0, 0, 0}},
    {// n
     {DMVPRI, DMVPBO, DMVPBA},
     {0.0f, DMVPBO * 2, 0.0f},
     {DMVUXTHR, DMVUYFOR},
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
        } cube[12];
} *data_model_index = &(struct data_model_index){{
    {2, 0, 1}, // A
    {2, 1, 3},
    {12, 4, 5}, // B
    {12, 5, 13},
    {14, 6, 7}, // C
    {14, 7, 15},
    {16, 8, 9}, // D
    {16, 9, 17},
    {18, 10, 11}, // E
    {18, 11, 19},
    {22, 20, 21}, // F
    {22, 21, 23},
}};
const VkDeviceSize data_model_index_size = sizeof(*data_model_index);

data_model_t data_models[] = {
    {
        "Cube",
        offsetof(struct data_model_index, cube) / sizeof(uint32_t),
        sizeof(data_model_index->cube) / sizeof(uint32_t),
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
