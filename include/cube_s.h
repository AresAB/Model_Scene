#ifndef CUBE_H
#define CUBE_H

#include <shader_s.h>
#include <glad/glad.h>

class Cube {
public:
    unsigned int VBO, VAO, EBO;
    Cube() {
        float vertices[] = {   
            // positions          // texture coords
             1.0f,  1.0f,  1.0f,  1.0f, 1.0f, // top right          0  
             1.0f, -1.0f,  1.0f,  1.0f, 0.0f, // bottom right       1  
            -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, // bottom left        2  
            -1.0f,  1.0f,  1.0f,  0.0f, 1.0f, // top left           3  

            -1.0f,  1.0f,  1.0f,  1.0f, 1.0f, // top left           4  
            -1.0f, -1.0f,  1.0f,  1.0f, 0.0f, // bottom left        5  
            -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, // back bottom left   6  
            -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, // back top left      7  

             1.0f,  1.0f, -1.0f,  1.0f, 1.0f, // back top right     8  
             1.0f, -1.0f, -1.0f,  1.0f, 0.0f, // back bottom right  9  
            -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, // back bottom left   10 
            -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, // back top left      11 

             1.0f,  1.0f, -1.0f,  1.0f, 1.0f, // back top right     12 
             1.0f, -1.0f, -1.0f,  1.0f, 0.0f, // back bottom right  13 
             1.0f, -1.0f,  1.0f,  0.0f, 0.0f, // bottom right       14 
             1.0f,  1.0f,  1.0f,  0.0f, 1.0f, // top right          15 

             1.0f,  1.0f, -1.0f,  1.0f, 1.0f, // back top right     16 
             1.0f,  1.0f,  1.0f,  1.0f, 0.0f, // top right          17 
            -1.0f,  1.0f,  1.0f,  0.0f, 0.0f, // top left           18 
            -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, // back top left      19 

             1.0f, -1.0f, -1.0f,  1.0f, 1.0f, // back bottom right  20 
             1.0f, -1.0f,  1.0f,  1.0f, 0.0f, // bottom right       21 
            -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, // bottom left        22 
            -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, // back bottom left   23 
        };
        unsigned int indices[] = {  
            0, 1, 3,     // front:  first triangle
            1, 2, 3,     // front:  second triangle
            4, 5, 7,     // left:   first triangle 
            5, 6, 7,     // left:   second triangle
            8, 9, 11,    // back:   first triangle
            9, 10, 11,   // back:   second triangle
            12, 13, 15,  // right:  first triangle
            13, 14, 15,  // right:  second triangle
            16, 17, 19,  // top:    first triangle
            17, 18, 19,  // top:    second triangle
            20, 21, 23,  // bottom: first triangle
            21, 22, 23   // bottom: second triangle
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
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    
    void deallocate() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
};
#endif