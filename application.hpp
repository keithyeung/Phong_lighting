// application.hpp

#pragma once

#include "system.hpp"
#include "graphics.hpp"

#include "vertex.hpp"
#include "shared.hpp"
#include "mesh.hpp"
#include "input.hpp"
#include "camera.hpp"
#include "skybox.hpp"
#include "overlay.hpp"
#include "scene.hpp"


class application_t {
public:
   application_t();

   // note: enter/exit
   bool on_initialize();
   void on_shutdown();

   // note: mainloop
   bool on_update(const timespan_t &deltatime,
                  const timespan_t &apptime);
   void on_render();

   // note: events
   void on_event(const mouse_moved_t &event);
   void on_event(const key_pressed_t &event);
   void on_event(const key_released_t &event);
   void on_event(const button_pressed_t &event);
   void on_event(const button_released_t &event);
   void on_event(const resized_event_t &event);

private:
   bool create_skybox();
   bool create_render_target(const int width, const int height);
   bool create_screen_quad();
   bool create_overlay();
   bool create_cube_mesh();

private:
   void render_pass_world(const color_t &clear_color);
   void render_pass_post_world(const color_t &clear_color);
   void render_pass_overlay(const viewport_t &viewport);

private:
   bool                             m_running = true;
   int32_t                          m_window_width = 0;
   int32_t                          m_window_height = 0;
   mouse_t                          m_mouse;
   keyboard_t                       m_keyboard;
   input_handler_t                  m_input_handler;
   renderer_t                       m_renderer;
   framebuffer_t                    m_backbuffer; // backbuffer has id 0

   shared_t                         m_shared;
   skybox_t                         m_skybox;
   camera_t                         m_camera;
   first_person_camera_controller_t m_controller;
   overlay_t                        m_overlay;

   texture_t                        m_wallAndfloor_texture;
   mesh_t                           m_wall_mesh;
   glm::vec3                        m_wall_position = {};
   float                            m_wall_rotation = 0.0f;
   glm::mat4                        m_wall_transform = {};
   
   mesh_t                           m_floor_mesh;
   glm::vec3                        m_floor_position = {};
   float                            m_floor_rotation = 0.0f;
   glm::mat4                        m_floor_transform = {};

   scene                            sceneManager;

   // note: framebuffer attachments and framebuffer (called render target)
   texture_t                        m_render_color;
   texture_t                        m_render_depth;
   framebuffer_t                    m_render_target;

   // note: big sprite covering whole screen using whole m_render_color as texture.
   vertex_buffer_t                  m_screen_quad_buffer;
   int                              m_screen_quad_primitive_count = 0;
};
