#ifndef HEADER_TRIVE_HPP
#define HEADER_TRIVE_HPP

#include <cstring>
#include <cstdio>
#include <cerrno>
#include <vector>
#include <sys/stat.h>

#define GL4_PROTOTYPES 1 // does this do anything?
#include <epoxy/gl.h>
#include <epoxy/glx.h>
#include <GLFW/glfw3.h>

#include <SDL2/SDL.h>

#if defined(GCC) || defined(CLANG)
  #define GCC_COMPAT
#endif

#ifndef __GLIBC__
  #include "gnustring.h"
  #define NOGLIBC
  extern char* strndup(const char* const, const size_t);
  typedef uint64_t u_int64_t;
  typedef uint32_t u_int32_t;
#endif

#undef strdup

#ifdef GCC_COMPAT
  #pragma GCC poison strcpy strdup sprintf gets atoi // poison unsafe functions
#endif

#ifndef alloc
  #define alloc(type, size) (static_cast<type*> (std::malloc(( nbytes(type, size) ))))
#endif

#ifndef set_out_param
  #define set_out_param(name, value) do { if ( NULL != (name) ){ *name = value; } } while(0)
#endif

#ifndef nbytes
  #define nbytes(type, size) ((sizeof (type)) * (size))
#endif

namespace trive {

  static const char
    * const program_name = "SDL2 + OpenGL Thing",
    * const program_version = "0.0.1";

  namespace graphics {

    static const uint32_t
      window_defaults[2] = { 512, 512 },
      space_dimensions = 3, color_dimensions = 4, square_verticies = 4;

    namespace shader {
      class shader_t {
        public:
          static const size_t max_shader_len = 4000;

          std::vector<GLuint>* shader_ids;

          GLuint shader_program = 0;

          int status = 2; // 0 = false, 1 = true, 2 = unset

          shader_t (void) noexcept;
          ~shader_t (void) noexcept;
          char* read_shader_file (const char* const filename);
          void use_program (void);
          void bind_attr_loc (const GLuint index, const char* const attribute);
          bool load_shader (const char* const, const GLenum);
          bool try_compile_shader (const GLuint);
          GLuint create_shader (const char* const, const GLenum);
          bool link_shaders (void);

          void shader_linker_error (const GLuint shader_id);
          void shader_compile_error (const GLuint shader_id);
      };
    }

    bool init (SDL_Window* * const, SDL_GLContext* const, shader::shader_t** const);
    bool run_game (SDL_Window* const * const);
    void render (SDL_Window* const * const, const GLuint);
    void black_window (SDL_Window* const * const);
    bool setup_buffer_objects (shader::shader_t** const, GLuint* const, const size_t, GLuint* const, const size_t, const GLuint, const GLuint);

    namespace metadata {
      bool set_opengl_attributes (const uint8_t, const uint8_t);
      void cleanup (SDL_Window* * const, SDL_GLContext* const, graphics::shader::shader_t** const, GLuint* const, const size_t, GLuint* const, const size_t);
    }

    namespace utils {
      void print_env_attributes (void);
      void _check_sdl_error (const char* const, const char* const, const uint64_t);
    }
  }
}

#define check_sdl_error() trive::graphics::utils::_check_sdl_error(__func__, __FILE__, __LINE__)

#endif /* end of include guard: HEADER_TRIVE_HPP */
