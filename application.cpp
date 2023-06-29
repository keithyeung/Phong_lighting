// application.cpp

#include "application.hpp"
#include <format>
#include <numbers> 
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

application_t::application_t()
   : m_input_handler(m_mouse, m_keyboard)
   , m_controller(m_camera)
{
}

bool application_t::on_initialize()
{
   // note: register event callbacks
   event_dispatcher_t::add_listener<mouse_moved_t>(*this);
   event_dispatcher_t::add_listener<key_pressed_t>(*this);
   event_dispatcher_t::add_listener<key_released_t>(*this);
   event_dispatcher_t::add_listener<button_pressed_t>(*this);
   event_dispatcher_t::add_listener<button_released_t>(*this);
   event_dispatcher_t::add_listener<resized_event_t>(*this);

   if (!m_shared.create()) {
      return false;
   }

   if (!create_skybox()) {
      return false;
   }

   if (!create_overlay()) {
      return false;
   }

   if (!create_render_target(1280, 720)) {
      return false;
   }

   if (!create_screen_quad()) {
      return false;
   }

   if (!create_cube_mesh()) {
      return false;
   }

   return true;
}

void application_t::on_shutdown()
{
   m_skybox.destroy();
}

bool application_t::on_update(const timespan_t &deltatime,
                              const timespan_t &apptime)
{
   // note: easy exit mode - 'on'
   if (m_keyboard.pressed(GLFW_KEY_ESCAPE)) {
      m_running = false;
   }

   if (m_keyboard.pressed(GLFW_KEY_1))
   {
       sceneManager.lightDir1 = sceneManager.lightDir2;
   }
   if (m_keyboard.pressed(GLFW_KEY_2))
   {
       sceneManager.lightDir1 = sceneManager.lightDir3;
   }
   if (m_keyboard.pressed(GLFW_KEY_3))
   {
       sceneManager.lightDir1 = sceneManager.lightDir4;
   }
   if (m_keyboard.pressed(GLFW_KEY_7))
   {
       sceneManager.lightColor = sceneManager.lightColor2;
   }
   if (m_keyboard.pressed(GLFW_KEY_8))
   {
       sceneManager.lightColor = sceneManager.lightColor3;
   }
   if (m_keyboard.pressed(GLFW_KEY_9))
   {
       sceneManager.lightColor = sceneManager.lightColor4;
   }

   // note: calculate camera perspective projection
   constexpr float kPI = std::numbers::pi_v<float>;
   constexpr float camera_default_znear = 1.0f;
   constexpr float camera_default_zfar = 100.0f;
   const float aspect_ration = float(m_window_width) / m_window_height;
   m_camera.set_perspective(kPI * 0.25f,
                            aspect_ration,
                            camera_default_znear,
                            camera_default_zfar);

   // note: update first-person camera controller
   m_controller.update(m_mouse, m_keyboard, deltatime);

   // note: post frame input update
   m_input_handler.post_frame_update();

   sceneManager.on_update(deltatime, apptime);

   
   

   { // note: some overlay text
      // note: color format 0xAABBGGRR
      const uint32_t color = 0xffffffff;
      const glm::vec2 position{ 2,2 };
      std::string text = std::format("AppTime: {:.2f}s", apptime.elapsed_seconds());
      m_overlay.add_text_line(position, color, text);
   }

   return m_running;
}

void application_t::on_render()
{
   const color_t clear_color{ 0x00, 0x00, 0x00, 0xff };
   const viewport_t viewport{ 0, 0, m_window_width, m_window_height };

   render_pass_world(clear_color);
   render_pass_post_world(clear_color);
   render_pass_overlay(viewport);
}

void application_t::on_event(const mouse_moved_t &event)
{
   m_input_handler.on_mouse_move(event.x, event.y);
}

void application_t::on_event(const key_pressed_t &event)
{
   m_input_handler.on_key_pressed(event.keycode);
}

void application_t::on_event(const key_released_t &event)
{
   m_input_handler.on_key_released(event.keycode);
}

void application_t::on_event(const button_pressed_t &event)
{
   m_input_handler.on_button_pressed(event.button);
}

void application_t::on_event(const button_released_t &event)
{
   m_input_handler.on_button_released(event.button);
}

void application_t::on_event(const resized_event_t &event)
{
   m_window_width = event.width;
   m_window_height = event.height;

   // note: we need to keep the backbuffer size in sync
   m_backbuffer.m_width = m_window_width;
   m_backbuffer.m_height = m_window_height;
}

bool application_t::create_skybox()
{
   if (!m_skybox.create_shader_program_from_file("assets/skybox/shader.vs.glsl",
                                                 "assets/skybox/shader.fs.glsl")) {
      return false;
   }

   const std::vector<std::string> cubemap_image_paths =
   {
      "assets/skybox/x_negative.jpg",
      "assets/skybox/x_positive.jpg",
      "assets/skybox/y_negative.jpg",
      "assets/skybox/y_positive.jpg",
      "assets/skybox/z_negative.jpg",
      "assets/skybox/z_positive.jpg",
   };

   if (!m_skybox.create_cubemap_from_files(cubemap_image_paths)) {
      return false;
   }

   if (!m_skybox.create_geometry()) {
      return false;
   }

   return true;
}\

bool application_t::create_render_target(const int width, const int height)
{
   // note: we cannot have a framebuffer bound and use one of its attachments as a texture when rendering!
   if (!m_render_color.create(width, height, nullptr, texture_t::pixel_format_t::rgba8)) {
      return false;
   }
   if (!m_render_depth.create(width, height, nullptr, texture_t::pixel_format_t::depth24_stencil8)) {
      return false;
   }
   if (!m_render_target.create(width, height, { m_render_color, m_render_depth })) {
      return false;
   }

   return true;
}

bool application_t::create_screen_quad()
{
   // note: construct a screen quad using NDC coordinates (i.e., identity matrix for projection).
   const vertex2d_t vertices[] =
   {
      { { -1.0f, 1.0f },  { 0.0f, 1.0f }, 0xffffffff },
      { {  1.0f, 1.0f },  { 1.0f, 1.0f }, 0xffffffff },
      { {  1.0f,-1.0f },  { 1.0f, 0.0f }, 0xffffffff },
      { {  1.0f,-1.0f },  { 1.0f, 0.0f }, 0xffffffff },
      { { -1.0f,-1.0f },  { 0.0f, 0.0f }, 0xffffffff },
      { { -1.0f, 1.0f },  { 0.0f, 1.0f }, 0xffffffff },
   };

   m_screen_quad_primitive_count = sizeof(vertices) / sizeof(vertices[0]);
   if (!m_screen_quad_buffer.create(sizeof(vertices), vertices)) {
      return false;
   }

   return true;
}

bool application_t::create_overlay()
{
   return m_overlay.create_from_file("assets/overlay/font8x8.png", glm::vec2{ 8.0f, 8.0f });
}

bool application_t::create_cube_mesh()
{
   if (!m_wallAndfloor_texture.create_from_file("assets/wallTexture.jpg", true)) {
      return false;
   }
   if (!mesh_t::make_cube_indexed(m_wall_mesh, {5.0f,5.0f,1.0f})) {
      return false;
   }
   if (!mesh_t::make_cube_indexed(m_floor_mesh, {5.0f,0.1f,3.0f})) {
      return false;
   }
   if (!mesh_t::make_cube_indexed(sceneManager.m_mesh, { 0.2f,0.2f,0.2f })) {
       return false;
   }

   m_wall_position = glm::vec3(0.0f, 0.0f, -10.0f);
   m_wall_rotation = 0.0f;
   m_wall_transform = glm::translate(glm::mat4(1.0f), m_wall_position);

   m_floor_position = glm::vec3(0.0f, -5.0f, -6.0f);
   m_floor_rotation = 0.0f;
   m_floor_transform = glm::translate(glm::mat4(1.0f), m_floor_position);

   return true;
}

void application_t::render_pass_world(const color_t &clear_color)
{
   // note: we set the framebuffer that we want to render to
   m_renderer.set_framebuffer(m_render_target);

   // note: clear framebuffer
   m_renderer.clear(clear_color);

   // note: render skybox
   m_skybox.render(m_renderer, m_shared, m_camera);

   // note: shared render state for world objects
   m_renderer.set_shader_program(m_shared.m_world_program);
   m_renderer.set_uniform("u_projection", m_camera.m_projection);
   m_renderer.set_uniform("u_view", m_camera.m_view);
   m_renderer.set_sampler_state(m_shared.m_linear_sampler);
   m_renderer.set_blend_state(m_shared.m_blend_alpha);
   m_renderer.set_depth_stencil_state(m_shared.m_depth_read_write);
   m_renderer.set_rasterizer_state(m_shared.m_rasterizer_back_culling);

   // note: render state unique per mesh.
   //       ... and to render more cubes we change texture and world transform!
   for (int i = 0; i < sceneManager.cubeVec.size(); i++)
   {
       m_renderer.set_texture(sceneManager.cubeVec[i].m_texture);
       m_renderer.set_uniform("u_world", sceneManager.cubeVec[i].m_transform);
       sceneManager.m_mesh.render(m_renderer, m_shared);
   }
   
   m_renderer.set_texture(m_wallAndfloor_texture);
   m_renderer.set_uniform("u_world", m_wall_transform);
   m_wall_mesh.render(m_renderer, m_shared);

   m_renderer.set_texture(m_wallAndfloor_texture);
   m_renderer.set_uniform("u_world", m_floor_transform);
   m_floor_mesh.render(m_renderer, m_shared);

   m_renderer.set_uniform("lightDir1", sceneManager.lightDir1);
   m_renderer.set_uniform("camPos", m_camera.m_position);
   m_renderer.set_uniform("lightColor", sceneManager.lightColor);


}

void application_t::render_pass_post_world(const color_t &clear_color)
{
   // note: we reset framebuffer back to backbuffer
   m_renderer.set_framebuffer(m_backbuffer);

   // note: clear framebuffer
   m_renderer.clear(clear_color);

   // note: render world framebuffer texture to screen (covers whole screen)
   m_renderer.set_shader_program(m_shared.m_canvas_program);
   m_renderer.set_uniform("u_projection", glm::mat4(1.0f));
   m_renderer.set_texture(m_render_color);
   m_renderer.set_sampler_state(m_shared.m_nearest_sampler);
   m_renderer.set_blend_state(m_shared.m_blend_none);
   m_renderer.set_depth_stencil_state(m_shared.m_depth_disabled);
   m_renderer.set_rasterizer_state(m_shared.m_rasterizer_back_culling);
   m_renderer.set_vertex_buffer_and_layout(m_screen_quad_buffer, m_shared.m_vertex_layout_2d);
   m_renderer.draw(topology_t::triangle_list, 0, m_screen_quad_primitive_count);
}

void application_t::render_pass_overlay(const viewport_t &viewport)
{
   m_overlay.render(m_renderer, m_shared, viewport);
}
