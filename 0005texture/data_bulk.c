#include "data_bulk.h"
#include <stdint.h>

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
     .offset = offsetof(vertex_t, boneIDs)}};
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

#define DMTCLEAR                                                                      \
        {                                                                             \
                {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, \
                    {0, 0, 0, 0}, {0, 0, 0, 0},                                       \
                {                                                                     \
                        0, 0, 0, 0                                                    \
                }                                                                     \
        }
#define DMTCONE 0xff
#define DMTCTWO 0xf0
#define DMTCTHR 0xe0
#define DMTCFOR 0xd0
#define DMTCFIV 0xc0
#define DMTCSIX 0xb0
#define DMTCSEV 0xa0
#define DMTCEIG 0x90
#define DMTCNIN 0x80
#define DMTCTEN 0x70
#define DMTCELE 0x60
#define DMTCTWE 0x50
#define DMTCTHT 0x40
#define DMTCFOT 0x30
#define DMTCFIT 0x20
#define DMTCSIT 0x10
typedef struct
{
        struct
        {
                struct
                {
                        struct
                        {
                                struct
                                {
                                        uint8_t red, green, blue, alpha;
                                } one, two, three, four, five, six, seven, eight;
                        } one, two, three, four;
                } one, two, three, four, five, six, seven, eight;
        } one, two, three;
} texture_0_t;

const data_material_t data_materials[] = {
    {
        "Example",
        0,
        sizeof(material_0_ubo_t),
        VK_FORMAT_R8G8B8A8_UNORM,
        8 * 4,
        8 * 3,
        (void *)&(texture_0_t){{{DMTCLEAR,
                                 {
                                     {DMTCONE, 0, DMTCONE, 0xff},
                                     {DMTCTWO, 0, DMTCONE, 0xff},
                                     {DMTCTHR, 0, DMTCONE, 0xff},
                                     {DMTCFOR, 0, DMTCONE, 0xff},
                                     {0, DMTCFOR, DMTCONE, 0xff},
                                     {0, DMTCTHR, DMTCONE, 0xff},
                                     {0, DMTCTWO, DMTCONE, 0xff},
                                     {0, DMTCONE, DMTCONE, 0xff},
                                 },
                                 DMTCLEAR,
                                 DMTCLEAR},
                                {DMTCLEAR,
                                 {{DMTCTWO, 0, DMTCONE, 0xff},
                                  {DMTCTHR, 0, DMTCONE, 0xff},
                                  {DMTCFOR, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, DMTCFOR, DMTCONE, 0xff},
                                  {0, DMTCTHR, DMTCONE, 0xff},
                                  {0, DMTCTWO, DMTCONE, 0xff}},
                                 DMTCLEAR,
                                 DMTCLEAR},
                                {DMTCLEAR,
                                 {{DMTCTHR, 0, DMTCONE, 0xff},
                                  {DMTCFOR, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, DMTCFOR, DMTCONE, 0xff},
                                  {0, DMTCTHR, DMTCONE, 0xff}},
                                 DMTCLEAR,
                                 DMTCLEAR},
                                {DMTCLEAR,
                                 {{DMTCFOR, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, DMTCFOR, DMTCONE, 0xff}},
                                 DMTCLEAR,
                                 DMTCLEAR},
                                {DMTCLEAR,
                                 {{DMTCEIG, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, DMTCEIG, DMTCONE, 0xff}},
                                 DMTCLEAR,
                                 DMTCLEAR},
                                {DMTCLEAR,
                                 {{DMTCSEV, 0, DMTCONE, 0xff},
                                  {DMTCEIG, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, DMTCEIG, DMTCONE, 0xff},
                                  {0, DMTCSEV, DMTCONE, 0xff}},
                                 DMTCLEAR,
                                 DMTCLEAR},
                                {DMTCLEAR,
                                 {{DMTCSIX, 0, DMTCONE, 0xff},
                                  {DMTCSEV, 0, DMTCONE, 0xff},
                                  {DMTCEIG, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, DMTCEIG, DMTCONE, 0xff},
                                  {0, DMTCSEV, DMTCONE, 0xff},
                                  {0, DMTCSIX, DMTCONE, 0xff}},
                                 DMTCLEAR,
                                 DMTCLEAR},
                                {DMTCLEAR,
                                 {{DMTCFIV, 0, DMTCONE, 0xff},
                                  {DMTCSIX, 0, DMTCONE, 0xff},
                                  {DMTCSEV, 0, DMTCONE, 0xff},
                                  {DMTCEIG, 0, DMTCONE, 0xff},
                                  {0, DMTCEIG, DMTCONE, 0xff},
                                  {0, DMTCSEV, DMTCONE, 0xff},
                                  {0, DMTCSIX, DMTCONE, 0xff},
                                  {0, DMTCFIV, DMTCONE, 0xff}},
                                 DMTCLEAR,
                                 DMTCLEAR}},
                               {{{{DMTCONE, DMTCONE, 0, 0xff},
                                  {DMTCONE, DMTCTWO, 0, 0xff},
                                  {DMTCONE, DMTCTHR, 0, 0xff},
                                  {DMTCONE, DMTCFOR, 0, 0xff},
                                  {DMTCONE, 0, DMTCFOR, 0xff},
                                  {DMTCONE, 0, DMTCTHR, 0xff},
                                  {DMTCONE, 0, DMTCTWO, 0xff},
                                  {DMTCONE, 0, DMTCONE, 0xff}},
                                 {{DMTCONE, DMTCONE, 0, 0xff},
#if 1
                                  {0xff, 0xff, 0xff, 0xff},
#else
                                  {DMTCTWO, DMTCONE, 0, 0xff},
#endif
                                  {DMTCTHR, DMTCONE, 0, 0xff},
                                  {DMTCFOR, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, DMTCFOR, 0xff},
                                  {0, DMTCONE, DMTCTHR, 0xff},
                                  {0, DMTCONE, DMTCTWO, 0xff},
                                  {0, DMTCONE, DMTCONE, 0xff}},
                                 {{DMTCONE, DMTCONE, 0, 0xff},
                                  {DMTCONE, DMTCTWO, 0, 0xff},
                                  {DMTCONE, DMTCTHR, 0, 0xff},
                                  {DMTCONE, DMTCFOR, 0, 0xff},
                                  {DMTCONE, 0, DMTCFOR, 0xff},
                                  {DMTCONE, 0, DMTCTHR, 0xff},
                                  {DMTCONE, 0, DMTCTWO, 0xff},
                                  {DMTCONE, 0, DMTCONE, 0xff}},
                                 {{DMTCONE, DMTCONE, 0, 0xff},
                                  {DMTCTWO, DMTCONE, 0, 0xff},
                                  {DMTCTHR, DMTCONE, 0, 0xff},
                                  {DMTCFOR, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, DMTCFOR, 0xff},
                                  {0, DMTCONE, DMTCTHR, 0xff},
                                  {0, DMTCONE, DMTCTWO, 0xff},
                                  {0, DMTCONE, DMTCONE, 0xff}}},
                                {{{DMTCONE, DMTCTWO, 0, 0xff},
                                  {DMTCONE, DMTCTHR, 0, 0xff},
                                  {DMTCONE, DMTCFOR, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, DMTCFOR, 0xff},
                                  {DMTCONE, 0, DMTCTHR, 0xff},
                                  {DMTCONE, 0, DMTCTWO, 0xff}},
#if 1
                                 {{0xff, 0xff, 0xff, 0xff},
                                  {0xff, 0xff, 0xff, 0xff},
#else
                                 {{DMTCTWO, DMTCONE, 0, 0xff},
                                  {DMTCTHR, DMTCONE, 0, 0xff},
#endif
                                  {DMTCFOR, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, DMTCFOR, 0xff},
                                  {0, DMTCONE, DMTCTHR, 0xff},
                                  {0, DMTCONE, DMTCTWO, 0xff}},
                                 {{DMTCONE, DMTCTWO, 0, 0xff},
                                  {DMTCONE, DMTCTHR, 0, 0xff},
                                  {DMTCONE, DMTCFOR, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, DMTCFOR, 0xff},
                                  {DMTCONE, 0, DMTCTHR, 0xff},
                                  {DMTCONE, 0, DMTCTWO, 0xff}},
                                 {{DMTCTWO, DMTCONE, 0, 0xff},
                                  {DMTCTHR, DMTCONE, 0, 0xff},
                                  {DMTCFOR, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, DMTCFOR, 0xff},
                                  {0, DMTCONE, DMTCTHR, 0xff},
                                  {0, DMTCONE, DMTCTWO, 0xff}}},
                                {{{DMTCONE, DMTCTHR, 0, 0xff},
                                  {DMTCONE, DMTCFOR, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, DMTCFOR, 0xff},
                                  {DMTCONE, 0, DMTCTHR, 0xff}},
                                 {{DMTCTHR, DMTCONE, 0, 0xff},
                                  {DMTCFOR, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, DMTCFOR, 0xff},
                                  {0, DMTCONE, DMTCTHR, 0xff}},
                                 {{DMTCONE, DMTCTHR, 0, 0xff},
                                  {DMTCONE, DMTCFOR, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, DMTCFOR, 0xff},
                                  {DMTCONE, 0, DMTCTHR, 0xff}},
                                 {{DMTCTHR, DMTCONE, 0, 0xff},
                                  {DMTCFOR, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, DMTCFOR, 0xff},
                                  {0, DMTCONE, DMTCTHR, 0xff}}},
                                {{{DMTCONE, DMTCFOR, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, DMTCFOR, 0xff}},
                                 {{DMTCFOR, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, DMTCFOR, 0xff}},
                                 {{DMTCONE, DMTCFOR, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, DMTCFOR, 0xff}},
                                 {{DMTCFOR, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, DMTCFOR, 0xff}}},
                                {{{DMTCONE, DMTCFOR, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, DMTCFOR, 0xff}},
                                 {{DMTCFOR, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, DMTCFOR, 0xff}},
                                 {{DMTCONE, DMTCFOR, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, DMTCFOR, 0xff}},
                                 {{DMTCFOR, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, DMTCFOR, 0xff}}},
                                {{{DMTCONE, DMTCTHR, 0, 0xff},
                                  {DMTCONE, DMTCFOR, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, DMTCFOR, 0xff},
                                  {DMTCONE, 0, DMTCTHR, 0xff}},
                                 {{DMTCTHR, DMTCONE, 0, 0xff},
                                  {DMTCFOR, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, DMTCFOR, 0xff},
                                  {0, DMTCONE, DMTCTHR, 0xff}},
                                 {{DMTCONE, DMTCTHR, 0, 0xff},
                                  {DMTCONE, DMTCFOR, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, DMTCFOR, 0xff},
                                  {DMTCONE, 0, DMTCTHR, 0xff}},
                                 {{DMTCTHR, DMTCONE, 0, 0xff},
                                  {DMTCFOR, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, DMTCFOR, 0xff},
                                  {0, DMTCONE, DMTCTHR, 0xff}}},
                                {{{DMTCONE, DMTCTWO, 0, 0xff},
                                  {DMTCONE, DMTCTHR, 0, 0xff},
                                  {DMTCONE, DMTCFOR, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, DMTCFOR, 0xff},
                                  {DMTCONE, 0, DMTCTHR, 0xff},
                                  {DMTCONE, 0, DMTCTWO, 0xff}},
                                 {{DMTCTWO, DMTCONE, 0, 0xff},
                                  {DMTCTHR, DMTCONE, 0, 0xff},
                                  {DMTCFOR, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, DMTCFOR, 0xff},
                                  {0, DMTCONE, DMTCTHR, 0xff},
                                  {0, DMTCONE, DMTCTWO, 0xff}},
                                 {{DMTCONE, DMTCTWO, 0, 0xff},
                                  {DMTCONE, DMTCTHR, 0, 0xff},
                                  {DMTCONE, DMTCFOR, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, 0, 0xff},
                                  {DMTCONE, 0, DMTCFOR, 0xff},
                                  {DMTCONE, 0, DMTCTHR, 0xff},
                                  {DMTCONE, 0, DMTCTWO, 0xff}},
                                 {{DMTCTWO, DMTCONE, 0, 0xff},
                                  {DMTCTHR, DMTCONE, 0, 0xff},
                                  {DMTCFOR, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, DMTCFOR, 0xff},
                                  {0, DMTCONE, DMTCTHR, 0xff},
                                  {0, DMTCONE, DMTCTWO, 0xff}}},
                                {{{DMTCONE, DMTCONE, 0, 0xff},
                                  {DMTCONE, DMTCTWO, 0, 0xff},
                                  {DMTCONE, DMTCTHR, 0, 0xff},
                                  {DMTCONE, DMTCFOR, 0, 0xff},
                                  {DMTCONE, 0, DMTCFOR, 0xff},
                                  {DMTCONE, 0, DMTCTHR, 0xff},
                                  {DMTCONE, 0, DMTCTWO, 0xff},
                                  {DMTCONE, 0, DMTCONE, 0xff}},
                                 {{DMTCONE, DMTCONE, 0, 0xff},
                                  {DMTCTWO, DMTCONE, 0, 0xff},
                                  {DMTCTHR, DMTCONE, 0, 0xff},
                                  {DMTCFOR, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, DMTCFOR, 0xff},
                                  {0, DMTCONE, DMTCTHR, 0xff},
                                  {0, DMTCONE, DMTCTWO, 0xff},
                                  {0, DMTCONE, DMTCONE, 0xff}},
                                 {{DMTCONE, DMTCONE, 0, 0xff},
                                  {DMTCONE, DMTCTWO, 0, 0xff},
                                  {DMTCONE, DMTCTHR, 0, 0xff},
                                  {DMTCONE, DMTCFOR, 0, 0xff},
                                  {DMTCONE, 0, DMTCFOR, 0xff},
                                  {DMTCONE, 0, DMTCTHR, 0xff},
                                  {DMTCONE, 0, DMTCTWO, 0xff},
                                  {DMTCONE, 0, DMTCONE, 0xff}},
                                 {{DMTCONE, DMTCONE, 0, 0xff},
                                  {DMTCTWO, DMTCONE, 0, 0xff},
                                  {DMTCTHR, DMTCONE, 0, 0xff},
                                  {DMTCFOR, DMTCONE, 0, 0xff},
                                  {0, DMTCONE, DMTCFOR, 0xff},
                                  {0, DMTCONE, DMTCTHR, 0xff},
                                  {0, DMTCONE, DMTCTWO, 0xff},
                                  {0, DMTCONE, DMTCONE, 0xff}}}},
                               {{DMTCLEAR,
                                 {{DMTCONE, 0, DMTCONE, 0xff},
                                  {DMTCTWO, 0, DMTCONE, 0xff},
                                  {DMTCTHR, 0, DMTCONE, 0xff},
                                  {DMTCFOR, 0, DMTCONE, 0xff},
                                  {0, DMTCFOR, DMTCONE, 0xff},
                                  {0, DMTCTHR, DMTCONE, 0xff},
                                  {0, DMTCTWO, DMTCONE, 0xff},
                                  {0, DMTCONE, DMTCONE, 0xff}},
                                 DMTCLEAR,
                                 DMTCLEAR},
                                {DMTCLEAR,
                                 {{DMTCTWO, 0, DMTCONE, 0xff},
                                  {DMTCTHR, 0, DMTCONE, 0xff},
                                  {DMTCFOR, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, DMTCFOR, DMTCONE, 0xff},
                                  {0, DMTCTHR, DMTCONE, 0xff},
                                  {0, DMTCTWO, DMTCONE, 0xff}},
                                 DMTCLEAR,
                                 DMTCLEAR},
                                {DMTCLEAR,
                                 {{DMTCTHR, 0, DMTCONE, 0xff},
                                  {DMTCFOR, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, DMTCFOR, DMTCONE, 0xff},
                                  {0, DMTCTHR, DMTCONE, 0xff}},
                                 DMTCLEAR,
                                 DMTCLEAR},
                                {DMTCLEAR,
                                 {{DMTCFOR, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, DMTCFOR, DMTCONE, 0xff}},
                                 DMTCLEAR,
                                 DMTCLEAR},
                                {DMTCLEAR,
                                 {{DMTCEIG, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, DMTCEIG, DMTCONE, 0xff}},
                                 DMTCLEAR,
                                 DMTCLEAR},
                                {DMTCLEAR,
                                 {{DMTCSEV, 0, DMTCONE, 0xff},
                                  {DMTCEIG, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, DMTCEIG, DMTCONE, 0xff},
                                  {0, DMTCSEV, DMTCONE, 0xff}},
                                 DMTCLEAR,
                                 DMTCLEAR},
                                {DMTCLEAR,
                                 {{DMTCSIX, 0, DMTCONE, 0xff},
                                  {DMTCSEV, 0, DMTCONE, 0xff},
                                  {DMTCEIG, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, 0, DMTCONE, 0xff},
                                  {0, DMTCEIG, DMTCONE, 0xff},
                                  {0, DMTCSEV, DMTCONE, 0xff},
                                  {0, DMTCSIX, DMTCONE, 0xff}},
                                 DMTCLEAR,
                                 DMTCLEAR},
                                {DMTCLEAR,
                                 {{DMTCFIV, 0, DMTCONE, 0xff},
                                  {DMTCSIX, 0, DMTCONE, 0xff},
                                  {DMTCSEV, 0, DMTCONE, 0xff},
                                  {DMTCEIG, 0, DMTCONE, 0xff},
                                  {0, DMTCEIG, DMTCONE, 0xff},
                                  {0, DMTCSEV, DMTCONE, 0xff},
                                  {0, DMTCSIX, DMTCONE, 0xff},
                                  {0, DMTCFIV, DMTCONE, 0xff}},
                                 DMTCLEAR,
                                 DMTCLEAR}}},
        sizeof(texture_0_t),
    },
};
