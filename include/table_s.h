#ifndef TABLE_H
#define TABLE_H

#include <shader_s.h>
#include <glad/glad.h>

class Table {
public:
    unsigned int VBO, VAO, EBO;
    Table(float t_w = 0.25, float l_w = 0.2) { // table and leg width
        float vertices[] = {   
            // positions                           // texture coords
             1.0f,        1.0f,        1.0f,       1.0f, 1.0f, // top right          0  
             1.0f,        1.0f - t_w,  1.0f,       1.0f, 0.0f, // bottom right       1  
            -1.0f,        1.0f - t_w,  1.0f,       0.0f, 0.0f, // bottom left        2  
            -1.0f,        1.0f,        1.0f,       0.0f, 1.0f, // top left           3  

            -1.0f,        1.0f,        1.0f,       1.0f, 1.0f, // top left           4  
            -1.0f,        1.0f - t_w,  1.0f,       1.0f, 0.0f, // bottom left        5  
            -1.0f,        1.0f - t_w, -1.0f,       0.0f, 0.0f, // back bottom left   6  
            -1.0f,        1.0f,       -1.0f,       0.0f, 1.0f, // back top left      7  

             1.0f,        1.0f,       -1.0f,       1.0f, 1.0f, // back top right     8  
             1.0f,        1.0f - t_w, -1.0f,       1.0f, 0.0f, // back bottom right  9  
            -1.0f,        1.0f - t_w, -1.0f,       0.0f, 0.0f, // back bottom left   10 
            -1.0f,        1.0f,       -1.0f,       0.0f, 1.0f, // back top left      11 

             1.0f,        1.0f,       -1.0f,       1.0f, 1.0f, // back top right     12 
             1.0f,        1.0f - t_w, -1.0f,       1.0f, 0.0f, // back bottom right  13 
             1.0f,        1.0f - t_w,  1.0f,       0.0f, 0.0f, // bottom right       14 
             1.0f,        1.0f,        1.0f,       0.0f, 1.0f, // top right          15 

             1.0f,        1.0f,       -1.0f,       1.0f, 1.0f, // back top right     16 
             1.0f,        1.0f,        1.0f,       1.0f, 0.0f, // top right          17 
            -1.0f,        1.0f,        1.0f,       0.0f, 0.0f, // top left           18 
            -1.0f,        1.0f,       -1.0f,       0.0f, 1.0f, // back top left      19 

             1.0f,        1.0f - t_w, -1.0f,       1.0f, 1.0f, // back bottom right  20 
             1.0f,        1.0f - t_w,  1.0f,       1.0f, 0.0f, // bottom right       21 
            -1.0f,        1.0f - t_w,  1.0f,       0.0f, 0.0f, // bottom left        22 
            -1.0f,        1.0f - t_w, -1.0f,       0.0f, 1.0f, // back bottom left   23 

            // positions                           // texture coords
            -0.9f,        1.0f - t_w, -0.9f,       1.0f, 1.0f, // top right          0  
            -0.9f,       -1.0f,       -0.9f,       1.0f, 0.0f, // bottom right       1  
            -0.9f + l_w, -1.0,        -0.9f,       0.0f, 0.0f, // bottom left        2  
            -0.9f + l_w,  1.0f - t_w, -0.9f,       0.0f, 1.0f, // top left           3  

            -0.9f + l_w,  1.0f - t_w, -0.9f,       1.0f, 1.0f, // top left           4  
            -0.9f + l_w, -1.0f,       -0.9f,       1.0f, 0.0f, // bottom left        5  
            -0.9f + l_w, -1.0f,       -0.9f + l_w, 0.0f, 0.0f, // back bottom left   6  
            -0.9f + l_w,  1.0f - t_w, -0.9f + l_w, 0.0f, 1.0f, // back top left      7  

            -0.9f,        1.0f - t_w, -0.9f + l_w, 1.0f, 1.0f, // back top right     8  
            -0.9f,       -1.0f,       -0.9f + l_w, 1.0f, 0.0f, // back bottom right  9  
            -0.9f + l_w, -1.0f,       -0.9f + l_w, 0.0f, 0.0f, // back bottom left   10 
            -0.9f + l_w,  1.0f - t_w, -0.9f + l_w, 0.0f, 1.0f, // back top left      11 

            -0.9f,        1.0f - t_w, -0.9f + l_w, 1.0f, 1.0f, // back top right     12 
            -0.9f,       -1.0f,       -0.9f + l_w, 1.0f, 0.0f, // back bottom right  13 
            -0.9f,       -1.0f,       -0.9f,       0.0f, 0.0f, // bottom right       14 
            -0.9f,        1.0f - t_w, -0.9f,       0.0f, 1.0f, // top right          15 

            -0.9f,        1.0f - t_w, -0.9f + l_w, 1.0f, 1.0f, // back top right     16 
            -0.9f,        1.0f - t_w, -0.9f,       1.0f, 0.0f, // top right          17 
            -0.9f + l_w,  1.0f - t_w, -0.9f,       0.0f, 0.0f, // top left           18 
            -0.9f + l_w,  1.0f - t_w, -0.9f + l_w, 0.0f, 1.0f, // back top left      19 

            -0.9f,       -1.0f,       -0.9f + l_w, 1.0f, 1.0f, // back bottom right  20 
            -0.9f,       -1.0f,       -0.9f,       1.0f, 0.0f, // bottom right       21 
            -0.9f + l_w, -1.0f,       -0.9f,       0.0f, 0.0f, // bottom left        22 
            -0.9f + l_w, -1.0f,       -0.9f + l_w, 0.0f, 1.0f, // back bottom left   23 

            // positions                           // texture coords
            -0.9f,        1.0f - t_w,  0.9f,       1.0f, 1.0f, // top right          0  
            -0.9f,       -1.0f,        0.9f,       1.0f, 0.0f, // bottom right       1  
            -0.9f + l_w, -1.0f,        0.9f,       0.0f, 0.0f, // bottom left        2  
            -0.9f + l_w,  1.0f - t_w,  0.9f,       0.0f, 1.0f, // top left           3  

            -0.9f + l_w,  1.0f - t_w,  0.9f,       1.0f, 1.0f, // top left           4  
            -0.9f + l_w, -1.0f,        0.9f,       1.0f, 0.0f, // bottom left        5  
            -0.9f + l_w, -1.0f,        0.9f - l_w, 0.0f, 0.0f, // back bottom left   6  
            -0.9f + l_w,  1.0f - t_w,  0.9f - l_w, 0.0f, 1.0f, // back top left      7  

            -0.9f,        1.0f - t_w,  0.9f - l_w, 1.0f, 1.0f, // back top right     8  
            -0.9f,       -1.0f,        0.9f - l_w, 1.0f, 0.0f, // back bottom right  9  
            -0.9f + l_w, -1.0f,        0.9f - l_w, 0.0f, 0.0f, // back bottom left   10 
            -0.9f + l_w,  1.0f - t_w,  0.9f - l_w, 0.0f, 1.0f, // back top left      11 

            -0.9f,        1.0f - t_w,  0.9f - l_w, 1.0f, 1.0f, // back top right     12 
            -0.9f,       -1.0f,        0.9f - l_w, 1.0f, 0.0f, // back bottom right  13 
            -0.9f,       -1.0f,        0.9f,       0.0f, 0.0f, // bottom right       14 
            -0.9f,        1.0f - t_w,  0.9f,       0.0f, 1.0f, // top right          15 

            -0.9f,        1.0f - t_w,  0.9f - l_w, 1.0f, 1.0f, // back top right     16 
            -0.9f,        1.0f - t_w,  0.9f,       1.0f, 0.0f, // top right          17 
            -0.9f + l_w,  1.0f - t_w,  0.9f,       0.0f, 0.0f, // top left           18 
            -0.9f + l_w,  1.0f - t_w,  0.9f - l_w, 0.0f, 1.0f, // back top left      19 

            -0.9f,       -1.0f,        0.9f - l_w, 1.0f, 1.0f, // back bottom right  20 
            -0.9f,       -1.0f,        0.9f,       1.0f, 0.0f, // bottom right       21 
            -0.9f + l_w, -1.0f,        0.9f,       0.0f, 0.0f, // bottom left        22 
            -0.9f + l_w, -1.0f,        0.9f - l_w, 0.0f, 1.0f, // back bottom left   23 

            // positions          // texture coords
             0.9f,        1.0f - t_w, -0.9f,       1.0f, 1.0f, // top right          0  
             0.9f,       -1.0f,       -0.9f,       1.0f, 0.0f, // bottom right       1  
             0.9f - l_w, -1.0f,       -0.9f,       0.0f, 0.0f, // bottom left        2  
             0.9f - l_w,  1.0f - t_w, -0.9f,       0.0f, 1.0f, // top left           3  

             0.9f - l_w,  1.0f - t_w, -0.9f,       1.0f, 1.0f, // top left           4  
             0.9f - l_w, -1.0f,       -0.9f,       1.0f, 0.0f, // bottom left        5  
             0.9f - l_w, -1.0f,       -0.9f + l_w, 0.0f, 0.0f, // back bottom left   6  
             0.9f - l_w,  1.0f - t_w, -0.9f + l_w, 0.0f, 1.0f, // back top left      7  

             0.9f,        1.0f - t_w, -0.9f + l_w, 1.0f, 1.0f, // back top right     8  
             0.9f,       -1.0f,       -0.9f + l_w, 1.0f, 0.0f, // back bottom right  9  
             0.9f - l_w, -1.0f,       -0.9f + l_w, 0.0f, 0.0f, // back bottom left   10 
             0.9f - l_w,  1.0f - t_w, -0.9f + l_w, 0.0f, 1.0f, // back top left      11 

             0.9f,        1.0f - t_w, -0.9f + l_w, 1.0f, 1.0f, // back top right     12 
             0.9f,       -1.0f,       -0.9f + l_w, 1.0f, 0.0f, // back bottom right  13 
             0.9f,       -1.0f,       -0.9f,       0.0f, 0.0f, // bottom right       14 
             0.9f,        1.0f - t_w, -0.9f,       0.0f, 1.0f, // top right          15 

             0.9f,        1.0f - t_w, -0.9f + l_w, 1.0f, 1.0f, // back top right     16 
             0.9f,        1.0f - t_w, -0.9f,       1.0f, 0.0f, // top right          17 
             0.9f - l_w,  1.0f - t_w, -0.9f,       0.0f, 0.0f, // top left           18 
             0.9f - l_w,  1.0f - t_w, -0.9f + l_w, 0.0f, 1.0f, // back top left      19 

             0.9f,       -1.0f,       -0.9f + l_w, 1.0f, 1.0f, // back bottom right  20 
             0.9f,       -1.0f,       -0.9f,       1.0f, 0.0f, // bottom right       21 
             0.9f - l_w, -1.0f,       -0.9f,       0.0f, 0.0f, // bottom left        22 
             0.9f - l_w, -1.0f,       -0.9f + l_w, 0.0f, 1.0f, // back bottom left   23 

            // positions          // texture coords
             0.9f,        1.0f - t_w,  0.9f,       1.0f, 1.0f, // top right          0  
             0.9f,       -1.0f,        0.9f,       1.0f, 0.0f, // bottom right       1  
             0.9f - l_w, -1.0f,        0.9f,       0.0f, 0.0f, // bottom left        2  
             0.9f - l_w,  1.0f - t_w,  0.9f,       0.0f, 1.0f, // top left           3  

             0.9f - l_w,  1.0f - t_w,  0.9f,       1.0f, 1.0f, // top left           4  
             0.9f - l_w, -1.0f,        0.9f,       1.0f, 0.0f, // bottom left        5  
             0.9f - l_w, -1.0f,        0.9f - l_w, 0.0f, 0.0f, // back bottom left   6  
             0.9f - l_w,  1.0f - t_w,  0.9f - l_w, 0.0f, 1.0f, // back top left      7  

             0.9f,        1.0f - t_w,  0.9f - l_w, 1.0f, 1.0f, // back top right     8  
             0.9f,       -1.0f,        0.9f - l_w, 1.0f, 0.0f, // back bottom right  9  
             0.9f - l_w, -1.0f,        0.9f - l_w, 0.0f, 0.0f, // back bottom left   10 
             0.9f - l_w,  1.0f - t_w,  0.9f - l_w, 0.0f, 1.0f, // back top left      11 

             0.9f,        1.0f - t_w,  0.9f - l_w, 1.0f, 1.0f, // back top right     12 
             0.9f,       -1.0f,        0.9f - l_w, 1.0f, 0.0f, // back bottom right  13 
             0.9f,       -1.0f,        0.9f,       0.0f, 0.0f, // bottom right       14 
             0.9f,        1.0f - t_w,  0.9f,       0.0f, 1.0f, // top right          15 

             0.9f,        1.0f - t_w,  0.9f - l_w, 1.0f, 1.0f, // back top right     16 
             0.9f,        1.0f - t_w,  0.9f,       1.0f, 0.0f, // top right          17 
             0.9f - l_w,  1.0f - t_w,  0.9f,       0.0f, 0.0f, // top left           18 
             0.9f - l_w,  1.0f - t_w,  0.9f - l_w, 0.0f, 1.0f, // back top left      19 

             0.9f,       -1.0f,        0.9f - l_w, 1.0f, 1.0f, // back bottom right  20 
             0.9f,       -1.0f,        0.9f,       1.0f, 0.0f, // bottom right       21 
             0.9f - l_w, -1.0f,        0.9f,       0.0f, 0.0f, // bottom left        22 
             0.9f - l_w, -1.0f,        0.9f - l_w, 0.0f, 1.0f, // back bottom left   23 
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
            21, 22, 23,   // bottom: second triangle

            0 + 24, 1 + 24, 3 + 24,     // front:  first triangle
            1 + 24, 2 + 24, 3 + 24,     // front:  second triangle
            4 + 24, 5 + 24, 7 + 24,     // left:   first triangle 
            5 + 24, 6 + 24, 7 + 24,     // left:   second triangle
            8 + 24, 9 + 24, 11 + 24,    // back:   first triangle
            9 + 24, 10 + 24, 11 + 24,   // back:   second triangle
            12 + 24, 13 + 24, 15 + 24,  // right:  first triangle
            13 + 24, 14 + 24, 15 + 24,  // right:  second triangle
            16 + 24, 17 + 24, 19 + 24,  // top:    first triangle
            17 + 24, 18 + 24, 19 + 24,  // top:    second triangle
            20 + 24, 21 + 24, 23 + 24,  // bottom: first triangle
            21 + 24, 22 + 24, 23 + 24,   // bottom: second triangle

            0 + 48, 1 + 48, 3 + 48,     // front:  first triangle
            1 + 48, 2 + 48, 3 + 48,     // front:  second triangle
            4 + 48, 5 + 48, 7 + 48,     // left:   first triangle 
            5 + 48, 6 + 48, 7 + 48,     // left:   second triangle
            8 + 48, 9 + 48, 11 + 48,    // back:   first triangle
            9 + 48, 10 + 48, 11 + 48,   // back:   second triangle
            12 + 48, 13 + 48, 15 + 48,  // right:  first triangle
            13 + 48, 14 + 48, 15 + 48,  // right:  second triangle
            16 + 48, 17 + 48, 19 + 48,  // top:    first triangle
            17 + 48, 18 + 48, 19 + 48,  // top:    second triangle
            20 + 48, 21 + 48, 23 + 48,  // bottom: first triangle
            21 + 48, 22 + 48, 23 + 48,   // bottom: second triangle

            0 + 72, 1 + 72, 3 + 72,     // front:  first triangle
            1 + 72, 2 + 72, 3 + 72,     // front:  second triangle
            4 + 72, 5 + 72, 7 + 72,     // left:   first triangle 
            5 + 72, 6 + 72, 7 + 72,     // left:   second triangle
            8 + 72, 9 + 72, 11 + 72,    // back:   first triangle
            9 + 72, 10 + 72, 11 + 72,   // back:   second triangle
            12 + 72, 13 + 72, 15 + 72,  // right:  first triangle
            13 + 72, 14 + 72, 15 + 72,  // right:  second triangle
            16 + 72, 17 + 72, 19 + 72,  // top:    first triangle
            17 + 72, 18 + 72, 19 + 72,  // top:    second triangle
            20 + 72, 21 + 72, 23 + 72,  // bottom: first triangle
            21 + 72, 22 + 72, 23 + 72,   // bottom: second triangle

            0 + 96, 1 + 96, 3 + 96,     // front:  first triangle
            1 + 96, 2 + 96, 3 + 96,     // front:  second triangle
            4 + 96, 5 + 96, 7 + 96,     // left:   first triangle 
            5 + 96, 6 + 96, 7 + 96,     // left:   second triangle
            8 + 96, 9 + 96, 11 + 96,    // back:   first triangle
            9 + 96, 10 + 96, 11 + 96,   // back:   second triangle
            12 + 96, 13 + 96, 15 + 96,  // right:  first triangle
            13 + 96, 14 + 96, 15 + 96,  // right:  second triangle
            16 + 96, 17 + 96, 19 + 96,  // top:    first triangle
            17 + 96, 18 + 96, 19 + 96,  // top:    second triangle
            20 + 96, 21 + 96, 23 + 96,  // bottom: first triangle
            21 + 96, 22 + 96, 23 + 96   // bottom: second triangle
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
        glDrawElements(GL_TRIANGLES, 180, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    
    void deallocate() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
};
#endif