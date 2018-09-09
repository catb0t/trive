#include "../trive.hpp"

namespace trive {
  namespace graphics {
    namespace shader {

      shader_t::shader_t (void) noexcept {
        // Generate our shader. This is similar to glGenBuffers() and glGenVertexArray(), except that this returns the ID
        this->shader_program = glCreateProgram();

        // Bind the location of our attributes
        bind_attr_loc(0, "in_Position");
        bind_attr_loc(1, "in_Color");

        this->shader_ids = new std::vector<GLuint>();

        if ( ! this->load_shader("src/shader/vert.vert", GL_VERTEX_SHADER) || ! this->load_shader("src/shader/frag.frag", GL_FRAGMENT_SHADER) || ! this->load_shader("src/shader/geom.geom", GL_GEOMETRY_SHADER) ) { // ! load_vertex_shader() || ! load_fragment_shader() ) {
          this->status = 0;
          //return nullptr;
        }

        // All shaders has been create, now we must put them together into one large object
        //return
        if ( ! link_shaders() ) {
          this->status = 0;
        }
      }

      char* shader_t::read_shader_file (const char* const filename) {

        struct stat file_info;

        if ( -1 == stat(filename, &file_info) ) {
          char *errno_str = strerror(errno);
          std::fprintf(stderr, "%s: %s: %s\n", __func__, filename, errno_str);
          return nullptr;
        }

        FILE* shader_fp = std::fopen(filename, "r");

        char* shader_contents = alloc(char, max_shader_len);

        if (nullptr != shader_fp) {
          size_t new_len = std::fread(shader_contents, nbytes(char, 1), max_shader_len, shader_fp);
          if (0 == new_len) {
            char *errno_str = strerror(errno);
            std::fprintf(stderr, "%s: %s: %s\n", __func__, filename, errno_str);
            return nullptr;
          } else {
            shader_contents[ new_len ] = '\0'; /* Just to be safe. */
          }

          std::fclose(shader_fp);
        }

        return shader_contents;
      }

      void shader_t::use_program (void) {
        // Load the shader into the rendering pipeline
        glUseProgram(this->shader_program);
      }

      void shader_t::bind_attr_loc (const GLuint index, const char* const attribute) {
        // Bind attribute index 0 (coordinates) to in_Position and attribute index 1 (color) to in_Color
        // Attribute locations must be setup before calling glLinkProgram
        glBindAttribLocation(this->shader_program, index, attribute);
      }

/*
      bool shader_t::load_vertex_shader (void) {
        std::puts("Loading Vertex shader");

        char* vertex_source = this->read_shader_file("src/shader/shader1.vert");

        if (nullptr == vertex_source) {
          return false;
        }

        const GLint vert_len = static_cast<GLint> ( strnlen(vertex_source, max_shader_len) );

        // Create an empty vertex shader handle
        this->vertex_shader = glCreateShader(GL_VERTEX_SHADER);

        // Send the vertex shader source code to OpenGL
        glShaderSource(this->vertex_shader, 1, &vertex_source, &vert_len);

        std::free(vertex_source);

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

        char* fragment_source = this->read_shader_file("src/shader/frag1.frag");

        if (nullptr == fragment_source) {
          return false;
        }

        const GLint frag_len = static_cast<GLint> ( strnlen(fragment_source, max_shader_len) );

        // Create an empty vertex shader handle
        this->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

        // Send the vertex shader source code to OpenGL
        glShaderSource(this->fragment_shader, 1, &fragment_source, &frag_len);

        std::free(fragment_source);

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
*/
      bool shader_t::load_shader (const char* const filename, const GLenum shader_type) {
        std::printf("Loading shader : %s\n", filename);

        const GLuint shader_id = this->create_shader(filename, shader_type);

        if ( this->try_compile_shader(shader_id) ) {
          glAttachShader(this->shader_program, shader_id);
          this->shader_ids->push_back(shader_id);

          return true;
        }

        return false;
      }

      bool shader_t::try_compile_shader (const GLuint shader_id) {
        // Compile the vertex shader
        glCompileShader(shader_id);

        // Ask OpenGL if the shaders was compiled
        GLint wasCompiled = false;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &wasCompiled);

        if (! wasCompiled) {
          shader_compile_error(shader_id);
          return false;
        }

        // Return false if compilation failed
        return (wasCompiled != 0);

      }

      GLuint shader_t::create_shader (const char* const filename, const GLenum shader_type) {
          char* const shader_source = this->read_shader_file(filename);

          GLint shader_len = static_cast<GLint> (strnlen(shader_source, max_shader_len));

          const GLuint shader_id = glCreateShader(shader_type);

          glShaderSource(shader_id , 1, &shader_source, &shader_len);
          free(shader_source);

          return shader_id;
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

      shader_t::~shader_t (void) noexcept {

        glUseProgram(0);

        for (auto shi: *(this->shader_ids)) {
          glDetachShader(this->shader_program, shi);
        }

        glDeleteProgram(this->shader_program);

        for (auto shi: *(this->shader_ids)) {
          glDeleteShader(shi);
        }

        delete this->shader_ids;
      }
    }
  }
}
