#ifndef QUAD_H
#define QUAD_H

#include <shader_s.h>
#include <glad/glad.h>

class Quad {
public:
    unsigned int VBO, VAO, EBO;
    Quad() {
        float vertices[] = {   
            // positions          // texture coords
             1.0f,  1.0f,  0.0f,  1.0f, 1.0f, // top right          0  
             1.0f, -1.0f,  0.0f,  1.0f, 0.0f, // bottom right       1  
            -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, // bottom left        2  
            -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, // top left           3  
        };
        unsigned int indices[] = {  
            0, 1, 3,     // first triangle
            1, 2, 3,     // second triangle
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    
    void render() {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    
    void deallocate() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
};
#endif