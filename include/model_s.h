#pragma once

#include <shader_s.h>

#include <stdlib.h>
#include <string.h>

unsigned int load_texture(char *filename) {
    unsigned int tex_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, num_channels;
    //stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load(filename, &width, &height, &num_channels, 0);
    if (data)
    {
	GLenum image_type = GL_RED;
	if(num_channels == 3) image_type = GL_RGB;
	else if(num_channels == 4) image_type = GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, image_type, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("|%s|\n(%d) ERROR: Failed to load texture \"%s\"", __FILE__, __LINE__, filename);
    }
    stbi_image_free(data);
    return tex_id;
}

typedef struct Vec3 {
	float x;
	float y;
	float z;
} Vec3;

typedef struct Vertex {
	Vec3 position;
	Vec3 normal;
	float tex_u;
	float tex_v;
} Vertex;

typedef struct Mesh {
	Vertex* vertices;
	unsigned int* indices;
	unsigned int num_indices;
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
} Mesh;

typedef struct Model {
	Model** pa_meshes;
	unsigned int size;
} Model;

Mesh* load_mesh(FILE* file, unsigned int beg_i, unsigned int end_i) {
	unsigned int v_size = 0;
	unsigned int i_size = 0;
	char* str_read = (char*)malloc(sizeof(char) * 100);
	while(ftell(file) != end_i) {
		char c_read = fgetc(file);
		if(c_read == 'v') {
			while(c_read == 'v') {
				v_size++;
				fgets(str_read,100,file);
				if(ftell(file) == end_i) break;
				c_read = fgetc(file);
			}
			if(ftell(file) != end_i) fseek(file,-1,SEEK_CUR);
		}
		if(c_read == 'f') {
			while(c_read == 'f') {
				i_size++;
				fgets(str_read,100,file);
				if(ftell(file) == end_i) break;
				c_read = fgetc(file);
			}
			if(ftell(file) != end_i) fseek(file,-1,SEEK_CUR);
		}
		else {
			fgets(str_read,100,file);
		}
	}
	Mesh* new_m = (Mesh*)malloc(sizeof(Mesh));
	new_m->vertices = (Vertex*)malloc(sizeof(Vertex) * v_size);
	new_m->indices = (unsigned int*)malloc(sizeof(unsigned int) * i_size * 3);
	new_m->num_indices = i_size;

	rewind(file);
	while(ftell(file) != end_i) {
		char c_read = fgetc(file);
		if(c_read == 'v') {
			fseek(file, -1, SEEK_CUR);
			for(unsigned int i = 0; i < v_size; i++) {
				fseek(file, 2, SEEK_CUR);
				float* v_pos = &(new_m->vertices[i].position.x);
				for(unsigned int j = 0; j < 3; j++) {
					unsigned int str_i = 0;
					c_read = fgetc(file);
					while(c_read != ' ' && c_read != '\n') {	
						str_read[str_i] = c_read;
						str_i++;
						c_read = fgetc(file);
					}
					str_read[str_i+1] = '\0';
					// this actually has some error in conversion (+/- 0.000001), not sure if it really matters but worth noting
					v_pos[j] = (float)atof(str_read);
				}
			}
		}
		if(c_read == 'f') {
			fseek(file, -1, SEEK_CUR);
			for(unsigned int i = 0; i < i_size; i++) {
				fseek(file, 2, SEEK_CUR);
				fgets(str_read,100,file);
				// put logic here
			}
		}
		else {
			fgets(str_read,100,file);
		}
	}
	free(str_read);

	return new_m;
}

void free_mesh(Mesh* m) {
	free(m->vertices);
	free(m->indices);
	free(m);
}

void load_model(const char* filename) {
	FILE* file = fopen(filename, "r");
	unsigned int region_end_i;
	
	fseek(file, 0, SEEK_END);
	region_end_i = ftell(file);
	rewind(file);

	free_mesh(load_mesh(file, 0, region_end_i));

	fclose(file);
}
