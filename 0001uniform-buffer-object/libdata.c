#include "data_bulk.h"
#include <math.h>
#include <stdbool.h>

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods)
{
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, true);
                break;
        }
}

float center_dir = 0;

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

struct list_head data_model_instances;
struct
{
        data_model_instance_t obj;
} model_instance = {
    {"Object",
     0,
     {&(data_model_instances), &(data_model_instances)},
     NULL,
     NULL},
};
struct list_head data_model_instances = {&(model_instance.obj.list),
                                         &(model_instance.obj.list)};

uniform_map_t starting_map;
struct list_head uniform_maps = {&(starting_map.list), &(starting_map.list)};
uniform_map_t starting_map = {
    VK_NULL_HANDLE,
    0,
    0,
    TO_UNIFORM_ALLOC(UNIFORM_SIZE),
    {&(uniform_maps), &(uniform_maps)},
};

inline static void split_map(size_t size, uniform_map_t *map,
                             uniform_map_t **ptr)
{
        uniform_map_t *this_map;
        this_map = malloc(sizeof(uniform_map_t));
        assert(this_map && "malloc(sizeof(uniform_map_t))");
        this_map->descriptor_set = VK_NULL_HANDLE;
        if (map->range)
        {
                this_map->offset = map->offset + map->range;
                this_map->range = size;
                this_map->width = map->width - map->range;
                map->width = map->range;
                list_add(&this_map->list, map->list.next);
                *ptr = this_map;
        }
        else
        {
                this_map->offset = map->offset;
                map->offset += this_map->width = this_map->range = size;
                map->width -= size;
                list_add(&this_map->list, &map->list);
                *ptr = this_map;
        }
}

uintptr_t data_uniform_ptr = 0;
void (*data_setup_model_instance)(uniform_map_t *uniform_map);
inline static void setup_model_instance(data_model_instance_t *inst)
{
        size_t size = TO_UNIFORM_ALLOC(
            data_materials[data_models[inst->model].material].ubo_vertex_shader_size);
        size_t contigious = 0;
        size_t count = 0;
        inst->uniform_map = NULL;
        uniform_map_t *map;
        list_for_each_entry(map, &uniform_maps, list)
        {
                if (map->range)
                {
                        contigious = 0;
                        count = 0;
                }
                size_t free = map->width - map->range;
                if (free)
                {
                        contigious += free;
                        count++;
                        if ((!map->range) && (free >= size) && (size * 2 > free))
                        {
                                map->range = size;
                                inst->uniform_map = map;
                                data_setup_model_instance(inst->uniform_map);
                                break;
                        }
                        else if (size < free)
                        {
                                split_map(size, map, &inst->uniform_map);
                                data_setup_model_instance(inst->uniform_map);
                                break;
                        }
                        else if (size < free + contigious)
                        {
                                assert(!count && 0 && "TODO: Support for combining uniform maps.");
                        }
                }
        }
        assert(inst->uniform_map && "Out of uniform space.");
        inst->ubo = (void *)(FROM_UNIFORM_ALLOC(inst->uniform_map->offset) +
                             data_uniform_ptr);
}

mat4 projection;
mat4 view;
inline static void update_vp(material_0_ubo_t *ubo_0)
{
        glm_lookat(ubo_0->viewPos,
                   (vec3){ubo_0->viewPos[0] + sinf(center_dir), ubo_0->viewPos[1],
                          ubo_0->viewPos[2] + cosf(center_dir)},
                   GLM_YUP, view);
        glm_mat4_mul(projection, view, ubo_0->vp);
}

/*
 * This code uses a float point frame counter.  To make your code easier
 * u may want to resolve this to an integer.  One frame is the set of all
 * player's turns.
 */
double previous_frame;

void data_init(VkExtent2D extent)
{
        material_0_ubo_t *ubo_0;
        data_model_instance_t *inst =
            list_entry(data_model_instances.next, data_model_instance_t, list);

        setup_model_instance(inst);
        ubo_0 = inst->ubo;

        glfwSetKeyCallback(data_window, key_callback);

        {
                static const material_0_ubo_t empty_data_material_ubo_s;
                *ubo_0 = empty_data_material_ubo_s;
        }
        glm_mat4_copy(GLM_MAT4_IDENTITY, ubo_0->bones[0]);
        ubo_0->viewPos[0] = 0.0f;
        ubo_0->viewPos[1] = 0.0f;
        ubo_0->viewPos[2] = -2.0f;

        glm_mat4_copy(GLM_MAT4_IDENTITY, ubo_0->model);

#ifdef NEWER_CGLM
        glm_perspective_default((float)extent.width / (float)extent.height,
                                projection);
#else
        mat4 oglproj;
        glm_perspective_default((float)extent.width / (float)extent.height, oglproj);
        /* This is for Vulkan, cglm is for OpenGL. */
        glm_mat4_mul((mat4){{1.0f, 0.0f, 0.0f, 0.0f},
                            {0.0f, -1.0f, 0.0f, 0.0f},
                            {0.0f, 0.0f, 0.5f, 0.5f},
                            {0.0f, 0.0f, 0.0f, 1.0f}},
                     oglproj, projection);
#endif

        update_vp(ubo_0);

        previous_frame = glfwGetTime();
}

inline static void handle_input(double this_frame, material_0_ubo_t *ubo_0)
{
        bool need_lookat = false;

#define GET_KEY(key) glfwGetKey(data_window, (key))
#define GET_KEY2(a, b) (GET_KEY(a) || GET_KEY(b))
        if (GET_KEY2(GLFW_KEY_W, GLFW_KEY_UP) ^ GET_KEY2(GLFW_KEY_S, GLFW_KEY_DOWN))
        {
                double dist = 1.5 * (GET_KEY2(GLFW_KEY_W, GLFW_KEY_UP) ? this_frame - previous_frame
                                                                       : previous_frame - this_frame);
                ubo_0->viewPos[0] += sinf(center_dir) * dist;
                ubo_0->viewPos[2] += cosf(center_dir) * dist;
                need_lookat = true;
        }

        if (GET_KEY2(GLFW_KEY_A, GLFW_KEY_LEFT) ^ GET_KEY2(GLFW_KEY_D, GLFW_KEY_RIGHT))
        {
                center_dir += GET_KEY2(GLFW_KEY_A, GLFW_KEY_LEFT) ? this_frame - previous_frame
                                                                  : previous_frame - this_frame;
                need_lookat = true;
        }

        if (need_lookat)
                update_vp(ubo_0);
}

void data_update()
{
        material_0_ubo_t *ubo_0 =
            list_entry(data_model_instances.next, data_model_instance_t, list)->ubo;
        double this_frame = glfwGetTime();

        handle_input(this_frame, ubo_0);

        /* TODO: Take turns. */

        if (0)
        {
                mat4 model;
                glm_mat4_copy(ubo_0->model, model);
                glm_rotate_y(model, this_frame - previous_frame, ubo_0->model);
        }

        previous_frame = this_frame;
}
