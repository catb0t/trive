#include "../trive.hpp"

namespace trive {

  namespace graphics {

    bool init (SDL_Window* * const window, SDL_GLContext* const context, shader::shader_t** const shader_holder) {

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

      if ( nullptr == *context || ! metadata::set_opengl_attributes(4, 6) ) {
        check_sdl_error();
        return false;
      }

      // This makes our buffer swap syncronized with the monitor's vertical refresh
      SDL_GL_SetSwapInterval(1);

      glfwInit();

      if (! (*shader_holder = new shader::shader_t()) ) {
        return false;
      }

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

    bool setup_buffer_objects (shader::shader_t** const shader_holder, GLuint* const vbo_list, const size_t vbo_len, GLuint* const vao_list, const size_t vao_len, const GLuint pos_attr_index, const GLuint color_attr_index) {

      static const GLfloat square[graphics::square_verticies][graphics::space_dimensions] = {
        { -0.5,  0.5,  0.5 }, // Top left
        {  0.5,  0.5,  0.5 }, // Top right
        {  0.5, -0.5,  0.5 }, // Bottom right
        { -0.5, -0.5,  0.5 }, // Bottom left
      };

      const GLfloat colors[graphics::square_verticies][graphics::color_dimensions] = {
        { 0.0, 1.0, 0.0, 1.0 }, // Top left
        { 1.0, 1.0, 0.0, 1.0  }, // Top right
        { 1.0, 0.0, 0.0, 1.0  }, // Bottom right
        { 0.0, 0.0, 1.0, 1.0  }, // Bottom left
      };

      glGenBuffers(static_cast<GLsizei> (vbo_len), vbo_list);
      glBindBuffer(GL_ARRAY_BUFFER, vbo_list[0]);

      static const uint32_t square_bufsize_bytes = nbytes(GLfloat, (graphics::square_verticies * graphics::space_dimensions));
      glBufferData( GL_ARRAY_BUFFER, square_bufsize_bytes, square, GL_STATIC_DRAW );

      glGenVertexArrays(static_cast<GLsizei> (vao_len), vao_list);
      glBindVertexArray(vao_list[0]);

      glVertexAttribPointer(pos_attr_index, graphics::space_dimensions, GL_FLOAT, GL_FALSE, 0, nullptr);

      glEnableVertexAttribArray(pos_attr_index);

      ///// COLORS

      glBindBuffer(GL_ARRAY_BUFFER, vbo_list[1]);

      static const uint32_t color_bufsize_bytes = nbytes(GLfloat, (graphics::square_verticies * graphics::color_dimensions));
      glBufferData( GL_ARRAY_BUFFER, color_bufsize_bytes, colors, GL_STATIC_DRAW );

      glVertexAttribPointer(color_attr_index, graphics::color_dimensions, GL_FLOAT, GL_FALSE, 0, nullptr);

      (*shader_holder)->use_program();

      glBindBuffer(GL_ARRAY_BUFFER, 0);

      return true;
    }


    void render (SDL_Window* const * const window, const GLuint color_attrib_index) {
      // First, render a square without any colors ( all vertexes will be black )
      // ===================
      // Make our background grey
      glClearColor(0.5, 0.5, 0.5, 1.0);
      glClear(GL_COLOR_BUFFER_BIT);

      // Invoke glDrawArrays telling that our data is a line loop and we want to draw 2-4 vertexes
      glDrawArrays(GL_LINE_LOOP, 0, 4);

      std::puts("Press ENTER to render next frame");
      std::fgetc(stdin);

      // Swap our buffers to make our changes visible
      SDL_GL_SwapWindow(*window);

      // Second, enable the colors and draw a solid square
      // ===================
      // Enable our attribute within the current VAO
      glEnableVertexAttribArray(color_attrib_index);

      black_window(window);
      std::puts("Press ENTER to render next frame");
      std::fgetc(stdin);

      // Invoke glDrawArrays telling that our data is a line loop and we want to draw 2-4 vertexes
      glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

      // Swap our buffers to make our changes visible
      SDL_GL_SwapWindow(*window);

      std::puts("Press ENTER to finish");
      std::fgetc(stdin);

    }

    void black_window (SDL_Window* const * const window) {
      glClearColor(0.0, 0.0, 0.0, 1.0);
      glClear(GL_COLOR_BUFFER_BIT);
      SDL_GL_SwapWindow(*window);
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
