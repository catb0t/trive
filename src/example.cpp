#include "trive.hpp"

auto main (const int argc, char* const * const argv) -> int {
  (void) argc;
  (void) argv;

  std::printf("%s %s", trive::program_name, trive::program_version);

  SDL_Window* main_window;
  SDL_GLContext main_context;

  if ( ! trive::graphics::sdl_gl::init(&main_window, &main_context) ) {
    return EXIT_FAILURE;
  }
  // Clear our buffer with a black background
  // This is the same as :
  //     SDL_SetRenderDrawColor(&renderer, 255, 0, 0, 255);
  //     SDL_RenderClear(&renderer);
  //

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_GL_SwapWindow(main_window);

  trive::graphics::sdl_gl::setup_buffer_objects();
  // run_game();

  trive::graphics::sdl_gl::cleanup(&main_window, &main_context);

  return EXIT_SUCCESS;
}
