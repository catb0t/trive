#include "trive.hpp"

auto main (const int argc, char* const * const argv) -> int {
  (void) argc;
  (void) argv;

  std::printf("%s %s\n", trive::program_name, trive::program_version);

  SDL_Window* main_window;
  SDL_GLContext main_context;
  trive::graphics::shader::shader_t* shader_holder;

  if ( ! trive::graphics::init(&main_window, &main_context, &shader_holder) ) {
    check_sdl_error();
    return EXIT_FAILURE;
  }

  trive::graphics::black_window(&main_window);

  size_t vbo_len = 2, vao_len = 1;

  auto* vbo_list = alloc(GLuint, vbo_len);
  auto* vao_list = alloc(GLuint, vao_len);

  static const uint32_t pos_attr_index = 0, color_attr_index = 1;

  trive::graphics::setup_buffer_objects(&shader_holder, vbo_list, vbo_len, vao_list, vao_len, pos_attr_index, color_attr_index);
  trive::graphics::render(&main_window, color_attr_index);
  // run_game();

  trive::graphics::metadata::cleanup(&main_window, &main_context, &shader_holder, vbo_list, 1, vao_list, 1);

  free(vao_list);
  free(vbo_list);

  return EXIT_SUCCESS;
}
