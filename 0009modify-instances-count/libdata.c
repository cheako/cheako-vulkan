#include "data_bulk.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

inline static void player_shoot();

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods)
{
        if (action == GLFW_PRESS)
                switch (key)
                {
                case GLFW_KEY_ESCAPE:
                        glfwSetWindowShouldClose(window, true);
                        break;
                case GLFW_KEY_X:
                        player_shoot();
                        break;
                }
}

typedef struct shared_ubo
{
        mat4 vp;
        vec3 lightPos;
        float pada;
        vec3 viewPos;
        float padb;
} shared_ubo_t;

float center_dir = 0;

GLFWwindow *data_window = NULL;

const uint8_t material_shader_0_vert_spv[1];
const size_t material_shader_0_vert_size;
const uint8_t material_shader_0_frag_spv[1];
const size_t material_shader_0_frag_size;
const uint8_t material_shader_1_vert_spv[1];
const size_t material_shader_1_vert_size;
const uint8_t material_shader_1_frag_spv[1];
const size_t material_shader_1_frag_size;
const data_material_shader_t data_material_shaders[] = {
    {{
         material_shader_0_vert_spv,
         &material_shader_0_vert_size,
     },
     {
         material_shader_0_frag_spv,
         &material_shader_0_frag_size,
     }},
    {{
         material_shader_1_vert_spv,
         &material_shader_1_vert_size,
     },
     {
         material_shader_1_frag_spv,
         &material_shader_1_frag_size,
     }},
};
const size_t data_material_shaders_count =
    sizeof(data_material_shaders) / sizeof(data_material_shaders[0]);

void shoot_forward(data_model_instance_t *ptr, double t)
{
        mat4 rot0,
            *model =
                data_materials[data_models[ptr->model].material].get_model(ptr->ubo);
        glm_translate_z(*model, 3 * t);
        glm_rotate_z(*model, t, rot0);
        glm_mat4_copy(rot0, *model);
}

inline static void setup_model_instance(data_model_instance_t *inst);
#define SUBO data_model_shared_ubo
struct shared_ubo *SUBO = NULL;
inline static void player_shoot()
{
        data_model_instance_t *this_instance = NULL,
                              bullet_instance = {
                                  "Bullet Object",
                                  1,
                                  shoot_forward,
                                  NULL,
                                  {NULL, NULL},
                                  NULL,
                                  VK_NULL_HANDLE,
                              };

        this_instance = malloc(sizeof(bullet_instance));
        assert(this_instance && "Out of memory.");
        *this_instance = bullet_instance;
        setup_model_instance(this_instance);

        {
                material_1_ubo_t *this_ubo = this_instance->ubo;
                {
                        static const material_1_ubo_t empty_data_material_ubo_s;
                        *this_ubo = empty_data_material_ubo_s;
                }
                glm_mat4_copy(GLM_MAT4_IDENTITY, this_ubo->bones[0]);

                {
                        mat4 trans0;
                        glm_translate_to(GLM_MAT4_IDENTITY, SUBO->viewPos, trans0);
                        glm_rotate_y(trans0, center_dir, this_ubo->model);
                        glm_translate_z(this_ubo->model, 1.25f);
                }
        }

        list_add_tail(&this_instance->list, &data_model_instances);
}

typedef struct
{
        data_model_instance_t obj;
} model_instance_t;
model_instance_t model_instance;
struct list_head data_model_instances = {&(model_instance.obj.list),
                                        &(model_instance.obj.list)};
model_instance_t model_instance = {
    {
        "Object",
        0,
        NULL,
        NULL,
        {&(data_model_instances), &(data_model_instances)},
        NULL,
        VK_NULL_HANDLE,
    },
};

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
                             struct list_head **ptr)
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
                *ptr = &this_map->list;
        }
        else
        {
                this_map->offset = map->offset;
                map->offset += this_map->width = this_map->range = size;
                map->width -= size;
                list_add(&this_map->list, &map->list);
                *ptr = &this_map->list;
        }
}

void (*data_setup_model_instance)(data_model_instance_t *inst);
uintptr_t data_uniform_ptr = 0;
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
                                inst->uniform_map = &map->list;
                                data_setup_model_instance(inst);
                                break;
                        }
                        else if (size < free)
                        {
                                split_map(size, map, &inst->uniform_map);
                                data_setup_model_instance(inst);
                                break;
                        }
                        else if (size < free + contigious)
                        {
                                assert(!count && 0 && "TODO: Support for combining uniform maps.");
                        }
                }
        }
        assert(inst->uniform_map && "Out of uniform space.");
        inst->ubo = (void *)(FROM_UNIFORM_ALLOC(list_entry(inst->uniform_map,
                                                           uniform_map_t, list)
                                                    ->offset) +
                             data_uniform_ptr);
}

mat4 projection;
mat4 view;
inline static void update_vp()
{
        glm_lookat(SUBO->viewPos,
                   (vec3){SUBO->viewPos[0] + sinf(center_dir), SUBO->viewPos[1],
                          SUBO->viewPos[2] + cosf(center_dir)},
                   GLM_YUP, view);
        glm_mat4_mul(projection, view, SUBO->vp);
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
        data_model_instance_t *inst = list_entry(data_model_instances.next, data_model_instance_t, list);
        setup_model_instance(inst);
        ubo_0 = inst->ubo;

        glfwSetKeyCallback(data_window, key_callback);

        {
                static const material_0_ubo_t empty_data_material_ubo_s;
                *ubo_0 = empty_data_material_ubo_s;
                static const shared_ubo_t empty_shared_ubo_s;
                *SUBO = empty_shared_ubo_s;
        }
        glm_mat4_copy(GLM_MAT4_IDENTITY, ubo_0->bones[0]);
        SUBO->lightPos[0] = 0.0f;
        SUBO->lightPos[1] = 250.0f;
        SUBO->lightPos[2] = -250.0f;
        SUBO->viewPos[0] = 0.0f;
        SUBO->viewPos[1] = 0.0f;
        SUBO->viewPos[2] = -3.0f;

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

        update_vp();

        previous_frame = glfwGetTime();
}

inline static void handle_input(double this_frame)
{
        float diagnal2 = 1;
        bool need_lookat = false;

#define GET_KEY(key) glfwGetKey(data_window, (key))
#define GET_KEY2(a, b) (GET_KEY(a) || GET_KEY(b))
        if (GET_KEY2(GLFW_KEY_W, GLFW_KEY_UP) ^ GET_KEY2(GLFW_KEY_S, GLFW_KEY_DOWN))
        {
                double dist = 1.5 * (GET_KEY2(GLFW_KEY_W, GLFW_KEY_UP) ? this_frame - previous_frame
                                                             : previous_frame - this_frame);
                if (GET_KEY(GLFW_KEY_R) ^ GET_KEY(GLFW_KEY_V))
                        diagnal2 = 0.70710678119f;
                SUBO->viewPos[0] += sinf(center_dir) * dist * diagnal2;
                SUBO->viewPos[2] += cosf(center_dir) * dist * diagnal2;
                need_lookat = true;
        }

        if (GET_KEY2(GLFW_KEY_A, GLFW_KEY_LEFT) ^ GET_KEY2(GLFW_KEY_D, GLFW_KEY_RIGHT))
        {
                center_dir += GET_KEY2(GLFW_KEY_A, GLFW_KEY_LEFT) ? this_frame - previous_frame
                                  : previous_frame - this_frame;
                need_lookat = true;
        }

        if (GET_KEY(GLFW_KEY_R) ^ GET_KEY(GLFW_KEY_V))
        {
                double dist = 1.5 * (GET_KEY(GLFW_KEY_R) ? this_frame - previous_frame
                                                         : previous_frame - this_frame);
                SUBO->viewPos[1] += dist * diagnal2;
                need_lookat = true;
        }

        if (need_lookat)
                update_vp();
}

void _glm_atan3(vec3 zvec, float zopposite, float zadjacent, vec3 rot)
{
        rot[0] = atan2f(zvec[1], zvec[2]);
        rot[1] = atan2f(zvec[0], fabsf(zvec[2]));
        rot[2] = atan2f(zopposite, zadjacent);
}

void _glm_decompose(mat4 model, vec3 pos, vec3 rot, vec3 zvec, float *zopposite,
                    float *zadjacent)
{
        mat4 rot0, rot1;
        vec4 mul1, mul2;
        if (pos)
        {
                vec4 mul0;
                glm_mat4_mulv(model, (vec4){0.0f, 0.0f, 0.0f, 1.0f}, mul0);
                glm_vec_copy(mul0, pos);
        }
        glm_mat4_mulv(model, (vec4){0.0f, 0.0f, 1.0f, 0.0f}, mul1);
        glm_rotate_y(model, -atan2f(mul1[0], fabsf(mul1[2])), rot0);
        glm_rotate_x(rot0, -atan2f(mul1[1], mul1[2]), rot1);
        glm_mat4_mulv(rot1, (vec4){0.0f, 1.0f, 0.0f, 0.0f}, mul2);
        if (rot)
                _glm_atan3(mul1, mul2[2], mul2[1], rot);
        if (zvec)
                glm_vec_copy(mul1, zvec);
        if (zopposite && zadjacent)
        {
                *zopposite = mul2[2];
                *zadjacent = mul2[1];
        }
}

inline static void take_turn(data_model_instance_t *ptr, double this_frame)
{
        if (ptr->update)
        {
                ptr->update(ptr, this_frame - previous_frame);
                /* Check out of bounds and reflect. */
                {
                        vec3 pos, rot;
                        mat4 *model =
                            data_materials[data_models[ptr->model].material].get_model(ptr->ubo);
                        _glm_decompose(*model, pos, rot, NULL, NULL, NULL);
                        bool new_model = false;
                        for (size_t i = 0; i < 3; i++)
                        {
#define BOUND 7.0f
                                if (pos[i] < -BOUND || pos[i] > BOUND)
                                {
                                        switch (i)
                                        {
                                        case 0:
                                                rot[1] = -rot[1];
                                                break;
                                        case 1:
                                                rot[0] = -rot[0];
                                                break;
                                        case 2:
                                                rot[1] = CGLM_PI - rot[1];
                                                break;
                                        default:
                                                assert(0 && "Out of bounds.");
                                        }
                                        pos[i] += pos[i] < -BOUND ? (pos[i] + BOUND) * -2.0f
                                                                  : (pos[i] - BOUND) * -2.0f;
                                        new_model = true;
                                }
                        }
                        if (new_model)
                        {
                                mat4 rot0, rot1, trans0;
                                glm_rotate_x(GLM_MAT4_IDENTITY, rot[0], rot0);
                                glm_rotate_y(rot0, rot[1], rot1);
                                glm_rotate_z(rot1, rot[2], rot0);
                                glm_translate_make(trans0, pos);
                                glm_mat4_mul(trans0, rot0, *model);
                        }
                }
        }
}

void data_update()
{
        double this_frame = glfwGetTime();

        handle_input(this_frame);

        data_model_instance_t *ptr;
        list_for_each_entry(ptr, &data_model_instances, list)
            take_turn(ptr, this_frame);

        if (0)
        {
                material_0_ubo_t *ubo_0 = model_instance.obj.ubo;
                mat4 model;
                glm_mat4_copy(ubo_0->model, model);
                glm_rotate_y(model, this_frame - previous_frame, ubo_0->model);
        }

        previous_frame = this_frame;
}
