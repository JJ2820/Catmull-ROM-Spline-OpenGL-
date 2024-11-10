#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include <vector>

const std::vector<glm::vec2> controlPoints = {
    glm::vec2(0.0f, 0.0f),
    glm::vec2(1.0f, 1.0f),
    glm::vec2(2.0f, 3.0f),
    glm::vec2(5.0f, 1.0f),
    glm::vec2(7.0f, 8.0f)
};

// Catmull-Rom interpolation function
glm::vec2 catmullRom(float t, glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3) {
    float t2 = t * t;
    float t3 = t2 * t;

    glm::vec2 result = 0.5f * ((2.0f * p1) +
                               (-p0 + p2) * t +
                               (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
                               (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
    return result;
}

void generateSplinePoints(std::vector<glm::vec2>& splinePoints, int numSegments = 100) {
    for (size_t i = 1; i < controlPoints.size() - 2; ++i) {
        for (int j = 0; j <= numSegments; ++j) {
            float t = (float)j / (float)numSegments;
            glm::vec2 point = catmullRom(t, controlPoints[i - 1], controlPoints[i], controlPoints[i + 1], controlPoints[i + 2]);
            splinePoints.push_back(point);
        }
    }
}

// Generate the skeleton offset points along the spline
void generateSkeletonLayer(const std::vector<glm::vec2>& splinePoints, std::vector<glm::vec2>& leftSkeleton, std::vector<glm::vec2>& rightSkeleton, float offset = 0.1f) {
    for (size_t i = 1; i < splinePoints.size() - 1; ++i) {
        glm::vec2 prev = splinePoints[i - 1];
        glm::vec2 curr = splinePoints[i];
        glm::vec2 next = splinePoints[i + 1];

        glm::vec2 tangent = glm::normalize(next - prev);
        glm::vec2 normal(-tangent.y, tangent.x);

        // Calculate offset points
        leftSkeleton.push_back(curr + normal * offset);
        rightSkeleton.push_back(curr - normal * offset);
    }
}

// Render the Catmull-Rom spline
void renderSpline(const std::vector<glm::vec2>& splinePoints) {
    glBegin(GL_LINE_STRIP);
    for (const auto& point : splinePoints) {
        glVertex2f(point.x, point.y);
    }
    glEnd();
}

// Render the skeleton layer
void renderSkeleton(const std::vector<glm::vec2>& leftSkeleton, const std::vector<glm::vec2>& rightSkeleton) {
    glBegin(GL_LINE_STRIP);
    for (const auto& point : leftSkeleton) {
        glVertex2f(point.x, point.y);
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    for (const auto& point : rightSkeleton) {
        glVertex2f(point.x, point.y);
    }
    glEnd();
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Catmull-Rom Spline with Skeleton", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewInit();

    // Set up viewport and orthographic projection
    glViewport(0, 0, 800, 600);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 8, -1, 9, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    std::vector<glm::vec2> splinePoints;
    generateSplinePoints(splinePoints);

    std::vector<glm::vec2> leftSkeleton, rightSkeleton;
    generateSkeletonLayer(splinePoints, leftSkeleton, rightSkeleton);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // Render spline in red
        glColor3f(1.0f, 0.0f, 0.0f);
        renderSpline(splinePoints);

        // Render skeleton layer in blue
        glColor3f(0.0f, 0.0f, 1.0f);
        renderSkeleton(leftSkeleton, rightSkeleton);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
