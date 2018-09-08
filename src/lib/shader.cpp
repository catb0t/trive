#include "../trive.hpp"

namespace trive {
  namespace graphics {
    namespace shader {

      static const char* const shader_source_vertex = R"(
      #version 150

      // in_Position was bound to attribute index 0 and in_Color was bound to attribute index 1
      attribute vec3 in_Position;
      attribute vec4 in_Color;

      // We output the ex_Color variable to the next shader in the chain
      out vec4 ex_Color;

      void main(void) {
          // Since we are using flat lines, our input only had two points: x and y.
          // Set the Z coordinate to 0 and W coordinate to 1
          gl_Position = vec4(in_Position.x, in_Position.y, in_Position.z, 1.0);

          // Pass the color on to the fragment shader
          ex_Color = in_Color;
      }
      )";

      static const char* const shader_source_fragment = R"(
      #version 120
      // It was expressed that some drivers required this next line to function properly
      precision highp float;

      in  vec4 ex_Color;

      void main(void) {
          gl_FragColor = vec4(ex_Color);
      }
      )";

      void shader_t::bind_attr_loc (const GLuint index, const char* const attribute) {
        // Bind attribute index 0 (coordinates) to in_Position and attribute index 1 (color) to in_Color
        // Attribute locations must be setup before calling glLinkProgram
        glBindAttribLocation(this->shader_program, index, attribute);
      }

      void shader_t::use_program (void) {
        // Load the shader into the rendering pipeline
        glUseProgram(this->shader_program);
      }

      shader_t::shader_t (void) noexcept {
        // Generate our shader. This is similar to glGenBuffers() and glGenVertexArray(), except that this returns the ID
        this->shader_program = glCreateProgram();

        // Bind the location of our attributes
        bind_attr_loc(0, "in_Position");
        bind_attr_loc(1, "in_Color");

        if ( ! load_vertex_shader() || ! load_fragment_shader() ) {
          this->status = 0;
          //return nullptr;
        }

        // All shaders has been create, now we must put them together into one large object
        //return
        if ( ! link_shaders() ) {
          this->status = 0;
        }
      }

      bool shader_t::load_vertex_shader (void) {
        std::puts("Linking Vertex shader");

        const GLint vert_len = static_cast<GLint> ( strnlen(shader_source_vertex, max_shader_len) );

        // Create an empty vertex shader handle
        this->vertex_shader = glCreateShader(GL_VERTEX_SHADER);

        // Send the vertex shader source code to OpenGL
        glShaderSource(this->vertex_shader, 1, &shader_source_vertex, &vert_len);

        // Compile the vertex shader
        glCompileShader(this->vertex_shader);

        GLint wasCompiled = 0;
        glGetShaderiv(this->vertex_shader, GL_COMPILE_STATUS, &wasCompiled );

        if (! wasCompiled) {
          shader_compile_error(this->vertex_shader);
          return false;
        }

        glAttachShader(this->shader_program, this->vertex_shader);
        return true;
      }

      bool shader_t::load_fragment_shader (void) {
        std::puts("Loading Fragment Shader");

        const GLint frag_len = static_cast<GLint> ( strnlen(shader_source_fragment, max_shader_len) );

        // Create an empty vertex shader handle
        this->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

        // Send the vertex shader source code to OpenGL
        glShaderSource(this->fragment_shader, 1, &shader_source_fragment, &frag_len);

        // Compile the vertex shader
        glCompileShader(this->fragment_shader);

        GLint wasCompiled = 0;
        glGetShaderiv(this->fragment_shader, GL_COMPILE_STATUS, &wasCompiled);

        if (! wasCompiled) {
          shader_compile_error(this->fragment_shader);
          return false;
        }

        glAttachShader(this->shader_program, this->fragment_shader);
        return true;
      }

      bool shader_t::link_shaders (void) {
        // Link. At this point, our shaders will be inspected/optized and the binary code generated
        // The binary code will then be uploaded to the GPU
        glLinkProgram(this->shader_program);

        // Verify that the linking succeeded
        GLint is_linked = false;
        glGetProgramiv(this->shader_program, GL_LINK_STATUS, &is_linked);

        if (false == is_linked){
          shader_linker_error(this->shader_program);
          return false;
        }

        return true;
      }

      void shader_t::shader_linker_error (const GLuint shader_id) {
        std::puts("Shader linking failed : ");

        // Find length of shader info log
        GLint max_length = 0;
        glGetProgramiv(shader_id, GL_INFO_LOG_LENGTH, &max_length);

        std::printf("Info Length : %d", max_length);

        // Get shader info log
        char* shader_program_log = alloc(char, static_cast<size_t> (max_length));
        glGetProgramInfoLog(this->shader_program, max_length, &max_length, shader_program_log);

        std::printf("Linker error message : %s", shader_program_log);

        free(shader_program_log);
      }

      // If something went wrong whil compiling the shaders, we'll use this function to find the error
      void shader_t::shader_compile_error (const GLuint shader_id) {
        std::puts("Shader compilation failed : ");

        // Find length of shader info log
        GLint max_length = 0;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &max_length);

        // Get shader info log
        char* shader_info_log = alloc(char, static_cast<size_t> (max_length));
        glGetShaderInfoLog(shader_id, max_length, &max_length, shader_info_log );

        // Print shader info log
        std::printf("\tError info : %s\n", shader_info_log);

        free(shader_info_log);
      }

      void shader_t::cleanup (void) {
        /* Cleanup all the things we bound and allocated */
        glUseProgram(0);
        glDetachShader(this->shader_program, this->vertex_shader);
        glDetachShader(this->shader_program, this->fragment_shader);

        glDeleteProgram(this->shader_program);

        glDeleteShader(this->vertex_shader);
        glDeleteShader(this->fragment_shader);
      }
    }
  }
}
