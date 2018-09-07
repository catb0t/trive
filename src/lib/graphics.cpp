#include "../trive.hpp"

namespace trive {

  namespace graphics {

    namespace sdl_gl {
      static const uint32_t window_defaults[2] = { 512, 512 };

      bool set_opengl_attributes (const uint8_t maj, const uint8_t min) {
        static const uint8_t num_settings = 4, setattr_args = 2;

        static const int settings[num_settings][setattr_args] = {
          {SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE},
          {SDL_GL_CONTEXT_MAJOR_VERSION, maj},
          {SDL_GL_CONTEXT_MINOR_VERSION, min},
          {SDL_GL_DOUBLEBUFFER, 1}
        };

        for (size_t i = 0; i < num_settings; i++) {
          if (0 > SDL_GL_SetAttribute(static_cast<SDL_GLattr> (settings[i][0]), settings[i][1])) {
            std::printf(
              "set_opengl_attributes failed '%d = %d' iteration#: %zu (see %s:%d)\n",
              settings[i][0],
              settings[i][1],
              i,
              __FILE__,
              __LINE__
            );
            check_sdl_error();
            return false;
          }
        }
        return true;
      }

      bool init (SDL_Window* * const window, SDL_GLContext* const context) {

        if ( 0 > SDL_Init(SDL_INIT_VIDEO) ) {
          // std::puts("Failed to init SDL");
          check_sdl_error();
          return false;
        }

        *window = SDL_CreateWindow(
          program_name,
          SDL_WINDOWPOS_CENTERED,
          SDL_WINDOWPOS_CENTERED,
          window_defaults[0], window_defaults[1],
          SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
        );

        // Check that everything worked out okay
        if ( nullptr == *window ) {
          // std::puts("Unable to create window");
          check_sdl_error();
          return false;
        }

        // Create our opengl context and attach it to our window
        *context = SDL_GL_CreateContext(*window);

        if ( nullptr == *context || ! set_opengl_attributes(4, 6) ) {
          check_sdl_error();
          return false;
        }

        // This makes our buffer swap syncronized with the monitor's vertical refresh
        SDL_GL_SetSwapInterval(1);

        // Init GLEW
        // Apparently, this is needed for Apple. Thanks to Ross Vander for letting me know
        #ifndef __APPLE__
          glewExperimental = GL_TRUE;
          glewInit();
        #endif

        return true;
      }

      bool setup_buffer_objects (void) {
        static const uint32_t points = 4, dimensions = 3;

        static const GLfloat square[points][dimensions] = {
            { -0.5,  0.5,  0.5 }, // Top left
            {  0.5,  0.5,  0.5 }, // Top right
            {  0.5, -0.5,  0.5 }, // Bottom right
            { -0.5, -0.5,  0.5 }, // Bottom left
        };

        (void) square;

        static const uint32_t count_vbos = 1;
        auto* vbo = alloc(GLuint, count_vbos);
        glGenBuffers(count_vbos, vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

        static const uint32_t bufsize_bytes = nbytes(GLfloat, (points * dimensions));
        glBufferData( GL_ARRAY_BUFFER, bufsize_bytes, square, GL_STATIC_DRAW );

        static const uint32_t count_vaos = 1;
        auto* vao = alloc(GLuint, count_vaos);
        glGenVertexArrays(1, vao);
        glBindVertexArray(vao[0]);

        static const uint32_t pos_attrib_index = 0 /*, color_attrib_index = 1*/;
        glVertexAttribPointer(pos_attrib_index, dimensions, GL_FLOAT, GL_FALSE, 0, nullptr);

        glEnableVertexAttribArray(pos_attrib_index);

        // Note : We didn't enable the colors here!

        // Set up shader ( will be covered in the next part )
        // ===================
        /*if (! shader.init() ) {
          return false;
        }

        shader.use_program();
      */
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return true;
      }

      bool run_game (SDL_Window* const * const window) {
        bool loop = true;

        while (loop) {
          SDL_Event event;
          while ( SDL_PollEvent(&event) ) {
            if (event.type == SDL_QUIT) { loop = false; }

            if (event.type == SDL_KEYDOWN) {
              switch (event.key.keysym.sym) {

                case SDLK_ESCAPE: {
                  loop = false;
                  break;
                }

                case SDLK_r: {
                  // Cover with red and update
                  glClearColor(1.0, 0.0, 0.0, 1.0);
                  glClear(GL_COLOR_BUFFER_BIT);
                  SDL_GL_SwapWindow(*window);
                  break;
                }
                case SDLK_g: {
                  // Cover with green and update
                  glClearColor(0.0, 1.0, 0.0, 1.0);
                  glClear(GL_COLOR_BUFFER_BIT);
                  SDL_GL_SwapWindow(*window);
                  break;
                }
                case SDLK_b: {
                  // Cover with blue and update
                  glClearColor(0.0, 0.0, 1.0, 1.0);
                  glClear(GL_COLOR_BUFFER_BIT);
                  SDL_GL_SwapWindow(*window);
                  break;
                }

                default: { break; }
              }
            }
          }
        }
      // end of function
      return true;
      }

      void cleanup (SDL_Window* * const window, SDL_GLContext* const context) {
        // Delete our OpengL context
        SDL_GL_DeleteContext(context);

        // Destroy our window
        SDL_DestroyWindow(*window);

        // Shutdown SDL 2
        SDL_Quit();
      }
    }

    namespace utils {
      void print_env_attributes() {
        int value = 0;
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &value);
        std::printf("SDL_GL_CONTEXT_MAJOR_VERSION : %d", value);

        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &value);
        std::printf("SDL_GL_CONTEXT_MINOR_VERSION : %d", value);
      }

      void _check_sdl_error (const char* const funcname, const char* const filename, const uint64_t lineno) {
        const char* const error = SDL_GetError();

        if (0 == strncmp(error, "", 2)) {
          std::printf("SDL Error in function %s (%s:%" PRIu64 ")\n\t%s", funcname, filename, lineno, error);

          SDL_ClearError();
        }
      }
    }
  }
}
