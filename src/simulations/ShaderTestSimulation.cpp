#include "ShaderTestSimulation.h"
#include <imgui.h>
#include <string>

static const char* vertexShaderSource = R"(#version 300 es
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;
out vec2 vTexCoord;

void main() {
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    vTexCoord = aTexCoord;
}
)";

static const char* fragmentShaderSource = R"(#version 300 es
precision highp float;
in vec2 vTexCoord;
uniform float iTime;
out vec4 FragColor;

void main() {
    vec2 uv = vTexCoord;
    vec2 center = vec2(0.5);
    
    float dist = distance(uv, center);
    float wave = sin(dist * 20.0 - iTime * 2.0) * 0.5 + 0.5;
    
    FragColor = vec4(wave, wave * 0.5, wave * 0.2, 1.0);
}
)";

static const char* defaultFragmentShader = R"(#version 300 es
precision highp float;
in vec2 vTexCoord;
uniform float iTime;
uniform vec3 uPrimaryColor;
uniform vec3 uSecondaryColor;
out vec4 FragColor;

void main() {
    vec2 uv = vTexCoord;
    vec2 center = vec2(0.5);
    
    float dist = distance(uv, center);
    float wave = sin(dist * 20.0 - iTime * 2.0) * 0.5 + 0.5;
    
    vec3 color = mix(uSecondaryColor, uPrimaryColor, wave);
    FragColor = vec4(color, 1.0);
}
)";

ShaderTestSimulation::ShaderTestSimulation() {
    fragmentShaderText = defaultFragmentShader;
    strcpy(editorBuffer, fragmentShaderText.c_str());
}

ShaderTestSimulation::~ShaderTestSimulation() {
    if (renderTexture) glDeleteTextures(1, &renderTexture);
    if (frameBuffer) glDeleteFramebuffers(1, &frameBuffer);
    if (shaderProgram) glDeleteProgram(shaderProgram);
    if (quadVAO) glDeleteVertexArrays(1, &quadVAO);
    if (quadVBO) glDeleteBuffers(1, &quadVBO);
}

void ShaderTestSimulation::init(int w, int h) {
    width = w;
    height = h;

    // Skip initial shader compilation - we'll do it in constructor
    fragmentShaderText = defaultFragmentShader;
    if (!compileShader()) {
        errorLog = "Failed to compile initial shader";
    }

    // Create quad
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // Create framebuffer and texture
    glGenFramebuffers(1, &frameBuffer);
    glGenTextures(1, &renderTexture);
    glBindTexture(GL_TEXTURE_2D, renderTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);
}

bool ShaderTestSimulation::compileShader() {
    if (fragmentShaderText.empty()) return false;

    // Create new fragment shader
    GLuint newFragShader = glCreateShader(GL_FRAGMENT_SHADER);
    if (newFragShader == 0) {
        errorLog = "Failed to create fragment shader";
        return false;
    }

    // Compile fragment shader
    const char* source = fragmentShaderText.c_str();
    glShaderSource(newFragShader, 1, &source, nullptr);
    glCompileShader(newFragShader);

    GLint success;
    glGetShaderiv(newFragShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(newFragShader, sizeof(infoLog), NULL, infoLog);
        errorLog = "Fragment shader compilation failed:\n";
        errorLog += infoLog;
        glDeleteShader(newFragShader);
        return false;
    }

    // Create new vertex shader
    GLuint newVertShader = glCreateShader(GL_VERTEX_SHADER);
    if (newVertShader == 0) {
        errorLog = "Failed to create vertex shader";
        glDeleteShader(newFragShader);
        return false;
    }

    // Compile vertex shader
    glShaderSource(newVertShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(newVertShader);
    glGetShaderiv(newVertShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(newVertShader, sizeof(infoLog), NULL, infoLog);
        errorLog = "Vertex shader compilation failed:\n";
        errorLog += infoLog;
        glDeleteShader(newFragShader);
        glDeleteShader(newVertShader);
        return false;
    }

    // Create and link program
    GLuint newProgram = glCreateProgram();
    if (newProgram == 0) {
        errorLog = "Failed to create shader program";
        glDeleteShader(newFragShader);
        glDeleteShader(newVertShader);
        return false;
    }

    glAttachShader(newProgram, newVertShader);
    glAttachShader(newProgram, newFragShader);
    glLinkProgram(newProgram);

    // Check linking status
    glGetProgramiv(newProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(newProgram, sizeof(infoLog), NULL, infoLog);
        errorLog = "Shader program linking failed:\n";
        errorLog += infoLog;
        glDeleteShader(newFragShader);
        glDeleteShader(newVertShader);
        glDeleteProgram(newProgram);
        return false;
    }

    // Cleanup old program and store new one
    if (shaderProgram) {
        glDeleteProgram(shaderProgram);
    }
    shaderProgram = newProgram;

    // Cleanup shaders
    glDeleteShader(newFragShader);
    glDeleteShader(newVertShader);
    
    errorLog = "Shader compilation successful!";
    return true;
}

void ShaderTestSimulation::update(float dt) {
    time += dt;
}

void ShaderTestSimulation::render() {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glViewport(0, 0, width, height);
    
    glUseProgram(shaderProgram);
    glUniform1f(glGetUniformLocation(shaderProgram, "iTime"), time);
    glUniform3fv(glGetUniformLocation(shaderProgram, "uPrimaryColor"), 1, primaryColor);
    glUniform3fv(glGetUniformLocation(shaderProgram, "uSecondaryColor"), 1, secondaryColor);
    
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShaderTestSimulation::ui() {
    ImGui::Begin("Shader Editor");
    
    // Add color controls
    if (ImGui::ColorEdit3("Primary Color", primaryColor)) {
        // Colors updated automatically in render()
    }
    if (ImGui::ColorEdit3("Secondary Color", secondaryColor)) {
        // Colors updated automatically in render()
    }
    
    // Changed to prevent auto-recompile
    ImGui::InputTextMultiline("##source", editorBuffer, sizeof(editorBuffer), 
        ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 20));
    
    if (ImGui::Button("Compile")) {
        fragmentShaderText = editorBuffer;
        if (!compileShader()) {
            // If compilation fails, keep using the old program
            ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Compilation Error:");
        } else {
            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Compilation Success!");
        }
    }
    
    if (!errorLog.empty()) {
        ImGui::TextWrapped("%s", errorLog.c_str());
    }
    ImGui::End();

    ImGui::Begin("Shader Controls");
    ImGui::Text("Time: %.1f", time);
    ImGui::End();
}
