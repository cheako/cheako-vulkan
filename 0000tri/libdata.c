#include "data.h"

GLFWwindow *data_window = NULL;

const uint8_t material_shader_0_vert_spv[1];
const size_t material_shader_0_vert_size;
const uint8_t material_shader_0_frag_spv[1];
const size_t material_shader_0_frag_size;
const data_material_shader_t data_material_shaders[] = {
    {{
         material_shader_0_vert_spv,
         &material_shader_0_vert_size,
     },
     {
         material_shader_0_frag_spv,
         &material_shader_0_frag_size,
     }},
};
