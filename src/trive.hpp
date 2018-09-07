#ifndef HEADER_TRIVE_HPP
#define HEADER_TRIVE_HPP

#endif /* end of include guard: HEADER_TRIVE_HPP */
#include <cstring>
#include <cstdio>

#define GL4_PROTOTYPES 1
#include <GL/glew.h>

#include <SDL2/SDL.h>

#ifndef alloc
  #define alloc(type, size) (static_cast<type*> (std::malloc(( nbytes(type, size) ))))
#endif

#ifndef set_out_param
  #define set_out_param(name, value) do { if ( NULL != (name) ){ *name = value; } } while(0)
#endif

#ifndef nbytes
  #define nbytes(type, size) ((sizeof (type)) * (size))
#endif

//Shader shader;

namespace trive {

  const char
    * const program_name = "SDL2 + OpenGL Thing",
    * const program_version = "0.0.1";

  namespace config {}

  namespace graphics {
    namespace sdl_gl {
      bool init (SDL_Window* * const, SDL_GLContext* const);
      bool set_opengl_attributes (const uint8_t, const uint8_t);
      bool setup_buffer_objects (void);
      bool run_game (SDL_Window* const * const);
      void cleanup (SDL_Window* * const, SDL_GLContext* const);
    }

    namespace utils {
      void print_env_attributes (void);
      void _check_sdl_error (const char* const, const char* const, const uint64_t);
    }
  }

}

#define check_sdl_error() trive::graphics::utils::_check_sdl_error(__func__, __FILE__, __LINE__)