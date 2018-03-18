#include "data_bulk.h"
#include <asm/errno.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

inline static void player_shoot();

struct
{
        unsigned char forward;
        unsigned char backward;
        unsigned char turn_l;
        unsigned char turn_r;
        unsigned char climb_up;
        unsigned char climb_down;
} keys_count = {false, false, false, false, false, false};

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

typedef struct
{
        float mass;
        float speed;
        bool solid;
        size_t hp;
        float scale_left;
} instance_closure_t;

void shoot_forward(data_model_instance_t *ptr, double t)
{
        mat4 rot0,
            *model =
                data_materials[data_models[ptr->model].material].get_model(ptr->ubo);
        glm_translate_z(*model, ((instance_closure_t *)ptr->closure)->speed * t);
        glm_rotate_z(*model, t, rot0);
        glm_mat4_copy(rot0, *model);
}

void shrink_forward(data_model_instance_t *ptr, double t)
{
        mat4 *bones =
            data_materials[data_models[ptr->model].material].get_bones(ptr->ubo);
        float *scale_left = &((instance_closure_t *)ptr->closure)->scale_left;
        shoot_forward(ptr, t);
        if (*scale_left == 0.0f)
                *scale_left = 1.0f;
        *scale_left -= t / 3.0f;
        if (*scale_left <= 0.0f)
        {
                ptr->update = NULL;
                *scale_left = 0.0f;
                glm_scale1(bones[0], 0);
        }
        else
                glm_scale1(bones[0], (*scale_left - t / 3.0f) / *scale_left);
}

void reduce_hp_shrink(data_model_instance_t *a, data_model_instance_t *b)
{
        assert(((instance_closure_t *)a->closure)->hp &&
               "Shouldn't have called collision().");
        if (--((instance_closure_t *)a->closure)->hp == 0)
        {
                ((instance_closure_t *)a->closure)->solid = false;
                a->collision = NULL;
                a->update = &shrink_forward;
        }
}

inline static void setup_model_instance(data_model_instance_t *inst);
#define SUBO data_model_shared_ubo
struct shared_ubo *SUBO = NULL;
pthread_mutex_t data_data_mutex = PTHREAD_MUTEX_INITIALIZER;
inline static void player_shoot()
{
        static const instance_closure_t bullet_closure = {
            0.02f,
            1.0f,
            true,
            3,
            0.0f,
        };

        data_model_instance_t *this_instance = NULL,
                              bullet_instance = {
                                  "Bullet Object",
                                  1,
                                  shoot_forward,
                                  reduce_hp_shrink,
                                  NULL,
                                  {NULL, NULL},
                                  NULL,
                                  VK_NULL_HANDLE,
                              };

        this_instance = malloc(sizeof(bullet_instance));
        assert(this_instance && "Out of memory.");
        *this_instance = bullet_instance;
        this_instance->closure = malloc(sizeof(bullet_closure));
        assert(this_instance->closure && "Out of memory.");
        *((instance_closure_t *)this_instance->closure) = bullet_closure;
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

        assert((!pthread_mutex_lock(&data_data_mutex)) && "Player Shoot");
        list_add_tail(&this_instance->list, &data_model_instances);
        assert((!pthread_mutex_unlock(&data_data_mutex)) && "Player Shoot");
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
        reduce_hp_shrink,
        &(instance_closure_t){
            1.0f,
            0.0f,
            true,
            5,
            0.0f,
        },
        {&(data_model_instances), &(data_model_instances)},
        NULL,
        VK_NULL_HANDLE,
    },
};

bool intersect(vec3 amin, vec3 amax, vec3 bmin, vec3 bmax)
{
        return (amin[0] <= bmax[0] && amax[0] >= bmin[0]) &&
               (amin[1] <= bmax[1] && amax[1] >= bmin[1]) &&
               (amin[2] <= bmax[2] && amax[2] >= bmin[2]);
}

pthread_t while_rendering_0;
pthread_cond_t data_data_cond = PTHREAD_COND_INITIALIZER;
#define MAX_COLLISIONS (64 * 64)
typedef struct
{
        data_model_instance_t *x, *y;
} collision_t;
collision_t collisions[MAX_COLLISIONS];
size_t collisions_count = 0;
pthread_mutex_t collisions_mutex = PTHREAD_MUTEX_INITIALIZER;
void *while_rendering_0_func(void *vargp)
{
        while (!glfwWindowShouldClose(data_window))
        {

                assert((!pthread_mutex_lock(&data_data_mutex)) && "While Rendering 0");
                pthread_cond_wait(&data_data_cond, &data_data_mutex);

                size_t instance_count = 0;
                struct list_head *ptr;
                list_for_each(ptr, &data_model_instances) instance_count++;
                {
                        struct
                        {
                                data_model_instance_t *id;
                                mat4 model;
                                float radius;
                                vec4 pos;
                                vec3 pos_high;
                                vec3 pos_low;
                        } _data[instance_count];

                        {
                                size_t i = 0;
                                data_model_instance_t *ptr;
                                list_for_each_entry(ptr, &data_model_instances, list)
                                {
                                        if (!((instance_closure_t *)ptr->closure)->solid)
                                        {
                                                --instance_count;
                                                continue;
                                        }
                                        _data[i].id = ptr;
                                        glm_mat4_copy(
                                            *(data_materials[data_models[ptr->model].material].get_model(
                                                ptr->ubo)),
                                            _data[i].model);
                                        _data[i++].radius = data_models[ptr->model].radius;
                                }
                        }

                        assert((!pthread_mutex_unlock(&data_data_mutex)) && "While Rendering 0");

                        for (size_t i = 0; i < instance_count; i++)
                        {
                                glm_mat4_mulv(_data[i].model, (vec4){0.0f, 0.0f, 0.0f, 1.0f},
                                              _data[i].pos);
                                for (size_t a = 0; a < 3; a++)
                                {
                                        _data[i].pos_high[a] = _data[i].pos[a] + _data[i].radius;
                                        _data[i].pos_low[a] = _data[i].pos[a] - _data[i].radius;
                                }
                        }

                        assert((!pthread_mutex_lock(&collisions_mutex)) && "While Rendering 0");

                        collisions_count = 0;

                        for (size_t x = 0; x < instance_count; x++)
                        {
                                if (x + 1 < instance_count)
                                {
                                        for (size_t y = x + 1; y < instance_count; y++)
                                        {
                                                if (intersect(_data[x].pos_low, _data[x].pos_high, _data[y].pos_low,
                                                              _data[y].pos_high))
                                                {
                                                        if (glm_vec_distance(_data[x].pos, _data[y].pos) -
                                                                _data[x].radius - _data[y].radius <
                                                            0)
                                                        {
                                                                assert((++collisions_count < MAX_COLLISIONS) &&
                                                                       "Overflow collisions");
                                                                collisions[collisions_count - 1] =
                                                                    (collision_t){_data[x].id, _data[y].id};
                                                        }
                                                }
                                        }
                                }
                        }
                }
                assert((!pthread_mutex_unlock(&collisions_mutex)) && "While Rendering 0");
        }
        return (NULL);
}

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

        assert(
            !pthread_create(&while_rendering_0, NULL, while_rendering_0_func, NULL));

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

        assert((!pthread_mutex_lock(&data_data_mutex)) && "Init");
        assert((!pthread_cond_broadcast(&data_data_cond)) && "Init");
        assert((!pthread_mutex_unlock(&data_data_mutex)) && "Init");
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
                if ((!keys_count.climb_up) ^ !keys_count.climb_down)
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
                                mat4 scale0, trans0, rot0, rot1;
#define SCALE ((instance_closure_t *)ptr->closure)->scale_left
#define VALUE SCALE == 0.0f ? 1.0f : SCALE
                                glm_scale_make(scale0, (vec3){VALUE, VALUE, VALUE});
#undef VALUE
#undef SCALE
                                glm_rotate_x(scale0, rot[0], rot0);
                                glm_rotate_y(rot0, rot[1], rot1);
                                glm_rotate_z(rot1, rot[2], rot0);
                                glm_translate_make(trans0, pos);
                                glm_mat4_mul(trans0, rot0, *model);
                        }
                }
        }
}

inline static void process_collisions()
{
        for (size_t i = 0; i < collisions_count; i++)
        {
                data_model_instance_t *ids[2];
                vec3 pos[2], zrot[2];
                float zop[2], zadj[2];
                mat4 *model[2];
                ids[0] = collisions[i].x;
                ids[1] = collisions[i].y;
                if (ids[0]->collision)
                        ids[0]->collision(ids[0], ids[1]);
                if (ids[1]->collision)
                        ids[1]->collision(ids[1], ids[0]);
                for (size_t a = 0; a < 2; a++)
                {
                        /* TODO: Skip no-longer available objects */
                        model[a] = data_materials[data_models[ids[a]->model].material].get_model(
                            ids[a]->ubo);
                        _glm_decompose(*model[a], pos[a], NULL, zrot[a], &zop[a], &zadj[a]);
                }
                {
                        vec3 N /* Collision */, add0[2] /* overlapping points */;
                        glm_vec_sub(pos[0], pos[1], N);
                        for (size_t a = 0; a < 2; a++)
                        {
                                vec3 scale0;
                                float table[] = {
                                    -data_models[ids[a]->model].radius,
                                    data_models[ids[a]->model].radius,
                                };
                                glm_vec_scale_as(N, table[a], scale0);
                                glm_vec_add(scale0, pos[a], add0[a]);
                        }
                        vec3 sub1 /* overlapping influence */;
                        glm_vec_sub(add0[0], add0[1], sub1);
                        {
#define MASS(x) ((instance_closure_t *)ids[x]->closure)->mass
#define SPEED(x) ((instance_closure_t *)ids[x]->closure)->speed
                                vec3 scale0, scale1, add0[2];
                                float div0 = 1 / (MASS(0) + MASS(1));
                                glm_vec_scale(zrot[0], SPEED(0) * (MASS(0) - MASS(1)), scale0);
                                glm_vec_scale(zrot[1], SPEED(1) * 2 * MASS(1), scale1);
                                glm_vec_add(scale0, scale1, add0[0]);
                                glm_vec_scale(zrot[1], SPEED(1) * (MASS(1) - MASS(0)), scale0);
                                glm_vec_scale(zrot[0], SPEED(0) * 2 * MASS(0), scale1);
                                glm_vec_scale(add0[0], div0, zrot[0]);
                                glm_vec_add(scale0, scale1, add0[1]);
                                glm_vec_scale(add0[1], div0, zrot[1]);
                        }
                        for (size_t a = 0; a < 2; a++)
                        {
                                vec3 scale1, add1, rot;
                                float table[] = {
                                    -1.2f * MASS(0) / (MASS(0) + MASS(1)),
                                    1.2f * MASS(1) / (MASS(0) + MASS(1)),
                                };
#undef MASS
#undef SPEED
                                glm_vec_scale(sub1, table[a], scale1);
                                glm_vec_add(scale1, pos[a], add1);
                                glm_vec_copy(add1, pos[a]);

                                _glm_atan3(zrot[a], zop[a], zadj[a], rot);

                                {
                                        mat4 scale0, trans0, rot0, rot1, rot2;
#define SCALE ((instance_closure_t *)ids[a]->closure)->scale_left
#define VALUE SCALE == 0.0f ? 1.0f : SCALE
                                        glm_scale_make(scale0, (vec3){VALUE, VALUE, VALUE});
#undef VALUE
#undef SCALE
                                        glm_rotate_x(scale0, rot[0], rot0);
                                        glm_rotate_y(rot0, rot[1], rot1);
                                        glm_rotate_z(rot1, rot[2], rot2);
                                        glm_translate_make(trans0, pos[a]);
                                        glm_mat4_mul(trans0, rot2, *model[a]);
                                }
                        }
                }
        }
        collisions_count = 0;
}

void data_update()
{
        double this_frame = glfwGetTime();

        handle_input(this_frame);

        assert((!pthread_mutex_lock(&data_data_mutex)) && "Update");
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

        int err;
        err = pthread_mutex_trylock(&collisions_mutex);
        switch (err)
        {
        case 0:
                if (collisions_count)
                        process_collisions();

                assert((!pthread_cond_broadcast(&data_data_cond)) && "Update");
                assert((!pthread_mutex_unlock(&collisions_mutex)) && "Update");

                break;
        case EBUSY:
                break;
        default:
                assert((!err) && "pthread_mutex_trylock(&collisions_mutex): Update");
        }

        assert((!pthread_mutex_unlock(&data_data_mutex)) && "Update");
}

void data_exit()
{
        assert((!pthread_mutex_lock(&collisions_mutex)) && "Exit");
        assert((!pthread_cond_broadcast(&data_data_cond)) && "Exit");
        assert((!pthread_mutex_unlock(&collisions_mutex)) && "Exit");
        assert(!pthread_join(while_rendering_0, NULL));
}
