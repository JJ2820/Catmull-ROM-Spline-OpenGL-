#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

// Use GLM's vec2 for points
using glm::vec2;

// Control points for the spline
std::vector<vec2> controlPoints = {
    vec2(0.0f, 0.0f), // Control Point 1
    vec2(1.0f, 1.0f), // Control Point 2
    vec2(2.0f, 3.0f), // Control Point 3
    vec2(5.0f, 1.0f), // Control Point 4
    vec2(7.0f, 8.0f)  // Control Point 5
};

// Stroke width for the outline
float strokeWidth = 0.05f;

// Catmull-Rom spline interpolation using GLM vectors
vec2 interpolateCatmullRom(const vec2& p0, const vec2& p1, const vec2& p2, const vec2& p3, float t) {
    float t2 = t * t;
    float t3 = t2 * t;

    return 0.5f * ((2.0f * p1) + (-p0 + p2) * t + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
                   (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
}

// Load shader from file
std::string loadShaderSource(const char* filePath) {
    std::ifstream shaderFile(filePath);
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    return shaderStream.str();
}

// Compile shader
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Error: Shader Compilation Failed\n" << infoLog << std::endl;
    }
    return shader;
}

// Initialize and return the shader program
GLuint initShaders() {
    std::string vertexCode = loadShaderSource("vertex_shader.glsl");
    std::string fragmentCode = loadShaderSource("fragment_shader.glsl");

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode.c_str());
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode.c_str());

    // Create shader program and link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Error: Shader Program Linking Failed\n" << infoLog << std::endl;
    }

    // Cleanup shaders (they are now linked into the program and can be deleted)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// Generate spline vertices
std::vector<float> generateSplineVertices() {
    std::vector<float> vertices;

    if (controlPoints.size() < 4) return vertices;

    for (size_t i = 1; i < controlPoints.size() - 2; ++i) {
        const vec2& p0 = controlPoints[i - 1];
        const vec2& p1 = controlPoints[i];
        const vec2& p2 = controlPoints[i + 1];
        const vec2& p3 = controlPoints[i + 2];

        for (float t = 0; t <= 1.0f; t += 0.01f) {
            vec2 interpolatedPoint = interpolateCatmullRom(p0, p1, p2, p3, t);
            vertices.push_back(interpolatedPoint.x);
            vertices.push_back(interpolatedPoint.y);
        }
    }

    return vertices;
}

// Generate stroke geometry based on the spline vertices
std::vector<float> generateStrokeGeometry(const std::vector<float>& splineVertices) {
    std::vector<float> strokeVertices;

    for (size_t i = 0; i < splineVertices.size() / 2 - 1; ++i) {
        float x1 = splineVertices[i * 2];
        float y1 = splineVertices[i * 2 + 1];
        float x2 = splineVertices[(i + 1) * 2];
        float y2 = splineVertices[(i + 1) * 2 + 1];

        // Calculate the perpendicular direction for the stroke
        float dx = x2 - x1;
        float dy = y2 - y1;
        float length = std::sqrt(dx * dx + dy * dy);
        if (length > 0) {
            dx /= length;  // Normalize
            dy /= length;

            // Perpendicular vector
            float perpX = -dy;
            float perpY = dx;

            // Offset points for the stroke width
            strokeVertices.push_back(x1 + perpX * strokeWidth / 2);
            strokeVertices.push_back(y1 + perpY * strokeWidth / 2);
            strokeVertices.push_back(x1 - perpX * strokeWidth / 2);
            strokeVertices.push_back(y1 - perpY * strokeWidth / 2);
            strokeVertices.push_back(x2 + perpX * strokeWidth / 2);
            strokeVertices.push_back(y2 + perpY * strokeWidth / 2);

            strokeVertices.push_back(x2 + perpX * strokeWidth / 2);
            strokeVertices.push_back(y2 + perpY * strokeWidth / 2);
            strokeVertices.push_back(x2 - perpX * strokeWidth / 2);
            strokeVertices.push_back(y2 - perpY * strokeWidth / 2);
            strokeVertices.push_back(x1 - perpX * strokeWidth / 2);
            strokeVertices.push_back(y1 - perpY * strokeWidth / 2);
            strokeVertices.push_back(x1 + perpX * strokeWidth / 2);
            strokeVertices.push_back(y1 + perpY * strokeWidth / 2);
        }
    }

    return strokeVertices;
}

int main() {
    // Initialize GLFW and GLEW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Catmull-Rom Spline with Stroke Geometry", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        return -1;
    }

    // Generate spline vertices
    std::vector<float> splineVertices = generateSplineVertices();
    
    // Generate stroke geometry
    std::vector<float> strokeVertices = generateStrokeGeometry(splineVertices);

    // Vertex buffer and array objects for spline
    GLuint splineVBO, splineVAO;
    glGenVertexArrays(1, &splineVAO);
    glGenBuffers(1, &splineVBO);

    glBindVertexArray(splineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, splineVBO);
    glBufferData(GL_ARRAY_BUFFER, splineVertices.size() * sizeof(float), splineVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Vertex buffer and array objects for stroke
    GLuint strokeVBO, strokeVAO;
    glGenVertexArrays(1, &strokeVAO);
    glGenBuffers(1, &strokeVBO);

    glBindVertexArray(strokeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, strokeVBO);
    glBufferData(GL_ARRAY_BUFFER, strokeVertices.size() * sizeof(float), strokeVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Initialize shaders
    GLuint shaderProgram = initShaders();
    glUseProgram(shaderProgram);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw stroke outline
        glBindVertexArray(strokeVAO);
        glDrawArrays(GL_TRIANGLES, 0, strokeVertices.size() / 2);
        glBindVertexArray(0);

        // Draw spline for reference (optional)
        glBindVertexArray(splineVAO);
        glDrawArrays(GL_LINE_STRIP, 0, splineVertices.size() / 2);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &splineVAO);
    glDeleteBuffers(1, &splineVBO);
    glDeleteVertexArrays(1, &strokeVAO);
    glDeleteBuffers(1, &strokeVBO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
