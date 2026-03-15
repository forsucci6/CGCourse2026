#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../common/debugging.h"

void create_box2d(int xsize, int ysize){
    
	GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return;


    // Request OpenGL 4.1
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    // Ask specifically for the core profile (recommended)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // macOS requires this for 3.2+ contexts
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(512, 512, "code_021_my_first_triangle_mod", NULL, NULL);


    if (!window)
    {
        glfwTerminate();
        return;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window); 


    // Load GL symbols *after* the context is current
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::fprintf(stderr, "Failed to initialize GLAD\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return;
    }


    /* query for the hardware and software specs and print the result on the console*/
    printout_opengl_glsl_info();

    // --- CREA VERTICI DEL QUADRATO ---
    float xMin = -0.5f, xMax = 0.5f;
    float yMin = -0.5f, yMax = 0.5f;
    //distanza vertici su x e y 
    float dx = (xMax - xMin) / xsize;
    float dy = (yMax - yMin) / ysize;

    /* create render data in RAM */
    GLuint positionAttribIndex = 0;
    std::vector<float> vec = {};
    
    for (int r = 0; r <= ysize; ++r) {
        for (int c = 0; c <= xsize; ++c) {
            float x = xMin + c * dx;
            float y = yMin + r * dy;

            // aggiungo solo se non c'è già
            bool exists = false;
            for (size_t i = 0; i < vec.size(); i += 2) {
                if (vec[i] == x && vec[i+1] == y) {
                    exists = true;
                    break;
                }
            }

            if (!exists) {
                vec.push_back(x);
                vec.push_back(y);
            }
        }
    }
    float* positions = vec.data();
    ///* create  a vertex array object */
    GLuint va;
    glGenVertexArrays(1, &va);
    glBindVertexArray(va);

    ///* create a buffer for the render data in video RAM */
    GLuint positionsBuffer;
    glGenBuffers(1, &positionsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, positionsBuffer);

    int vertici = (xsize + 1) * (ysize +1);

    ///* declare what data in RAM are filling the bufferin video RAM */
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertici*2, positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(positionAttribIndex);
    ///* specify the data format */
    glVertexAttribPointer(positionAttribIndex, 2, GL_FLOAT, false, 0, 0);

    ///* create render data in RAM */
    GLuint colorAttribIndex = 1;
    std::vector<float> colori(vertici * 3);  // grande N*3

    for(int r=0; r<=ysize; ++r) {
        for(int c=0; c<=xsize; ++c) {
            int i = r*(xsize+1)+c;
            colori[i*3 + 0] = (c%2) * 1.0f;   // rosso alternato
            colori[i*3 + 1] = (r%2) * 1.0f;   // verde alternato
            colori[i*3 + 2] = 0.5f;           // blu fisso
    }
}


    float* colors = colori.data();
    ///* create a buffer for the render data in video RAM */
    GLuint colorBuffer;
    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);


    ///* declare what data in RAM are filling the bufferin video RAM */
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertici*3, colors, GL_STATIC_DRAW);

    glEnableVertexAttribArray(colorAttribIndex);
    ///* specify the data format */
    glVertexAttribPointer(colorAttribIndex, 3, GL_FLOAT, false, 0, 0);

    std::vector<GLuint> index = {};

    for (int r = 0; r < ysize; r++) {
        for (int c = 0; c < xsize; c++) {

            int v0 = r * (xsize+1) + c;
            int v1 = v0 + 1;
            int v2 = v0 + (xsize+1);
            int v3 = v2 + 1;

            // triangolo 1
            index.push_back(v0);
            index.push_back(v1);
            index.push_back(v2);

            // triangolo 2
            index.push_back(v1);
            index.push_back(v3);
            index.push_back(v2);
        }
    }

    int numIndices = 6 * xsize * ysize;


    GLuint indexBuffer;
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numIndices, index.data(), GL_STATIC_DRAW);


    ///* create a vertex shader */
    std::string  vertex_shader_src = "#version 410\n \
        in vec2 aPosition;\
        in vec3 aColor;\
        out vec3 vColor;\
        uniform float uDelta;\
        void main(void)\
        {\
         gl_Position = vec4(aPosition+vec2(uDelta, uDelta), 0.0, 1.0);\
         vColor = aColor;\
        }\
       ";
    const GLchar* vs_source = (const GLchar*)vertex_shader_src.c_str();
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vs_source, NULL);
    glCompileShader(vertex_shader);
  

    ///* create a fragment shader */
    std::string   fragment_shader_src = "#version 410 \n \
        layout (location = 0) out vec4 color;\
        in vec3 vColor;\
        uniform float uDelta;\
        void main(void)\
        {\
            color = vec4(vColor+vec3(uDelta,0.0,0.0), 1.0);\
        }";
    const GLchar* fs_source = (const GLchar*)fragment_shader_src.c_str();
    
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fs_source, NULL);
    glCompileShader(fragment_shader);
    

    GLuint program_shader = glCreateProgram();
    glAttachShader(program_shader, vertex_shader);
    glAttachShader(program_shader, fragment_shader);
     
    glBindAttribLocation(program_shader, positionAttribIndex, "aPosition");
    glBindAttribLocation(program_shader, colorAttribIndex, "aColor");
    glLinkProgram(program_shader);
   

    GLint linked;
    validate_shader_program(program_shader);
    glGetProgramiv(program_shader, GL_LINK_STATUS, &linked);
    if (linked) {
        glUseProgram(program_shader);
    }
    
    GLint loc = glGetUniformLocation(program_shader, "uDelta");

    /* cal glGetError and print out the result in a more verbose style
    * __LINE__ and __FILE__ are precompiler directive that replace the value with the
    * line and file of this call, so you know where the error happened
    */
    check_gl_errors(__LINE__, __FILE__);
    
    float d = 0.01;
    float delta = 0;

    glClearColor(0.2, 0.2, 0.2, 1);
    while (!glfwWindowShouldClose(window))
    {
        /*if (delta < -0.5 || delta > 0.5)
            d = -d;
        delta += d;

        glUniform1f(loc, delta); */

        /* Render here */
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, NULL);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

}

int main(int argc, char** argv) {

    create_box2d(3,2); 

    glfwTerminate();

	return 0;
}