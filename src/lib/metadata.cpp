#include "../trive.hpp"

namespace trive {

  namespace graphics {

    namespace metadata {

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
              "set_opengl_attributes failed '%d = %d' iteration#: %zu (SDL error follows)\n",
              settings[i][0],
              settings[i][1],
              i
            );
            check_sdl_error();
            return false;
          }
        }
        return true;
      }

      void cleanup (SDL_Window* * const window, SDL_GLContext* const context, shader::shader_t* * const shader_holder, GLuint* const vbo_list, const size_t count_vbo, GLuint* const vao_list, const size_t count_vao) {
        delete *shader_holder;

        glDisableVertexAttribArray(0);
        glDeleteBuffers(static_cast<GLsizei> (count_vbo), vbo_list);
        glDeleteVertexArrays(static_cast<GLsizei> (count_vao), vao_list);

        // Delete our OpengL context
        SDL_GL_DeleteContext(*context);

        // Destroy our window
        SDL_DestroyWindow(*window);

        // Shutdown SDL 2
        SDL_Quit();
      }
    }
  }
}
