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
	Mesh** pa_meshes;
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
		else if(c_read != '\n') {
			fgets(str_read,100,file);
		}
	}
	Mesh* new_m = (Mesh*)malloc(sizeof(Mesh));
	new_m->vertices = (Vertex*)calloc(v_size, sizeof(Vertex));
	new_m->indices = (unsigned int*)malloc(sizeof(unsigned int) * i_size * 3);
	new_m->num_indices = i_size * 3;

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
					str_read[str_i] = '\0';
					v_pos[j] = (float)atof(str_read);
				}
			}
		}
		if(c_read == 'f') {
			fseek(file, -1, SEEK_CUR);
			for(unsigned int i = 0; i < i_size; i++) {
				fseek(file, 2, SEEK_CUR);
				for(unsigned int j = 0; j < 3; j++) {
					unsigned int str_i = 0;
					c_read = fgetc(file);
					while(c_read != ' ' && c_read != '\n') {	
						str_read[str_i] = c_read;
						str_i++;
						c_read = fgetc(file);
					}
					str_read[str_i] = '\0';
					new_m->indices[i*3+j] = atoi(str_read) - 1;
				}
			}
		}
		else if(c_read != '\n') {
			fgets(str_read,100,file);
		}
	}
	free(str_read);

	/*for(unsigned int i = 0; i < v_size; i++) {
		printf("p: (%f, %f, %f), n: (%f, %f, %f), uv: (%f, %f)\n", new_m->vertices[i].position.x, new_m->vertices[i].position.y, new_m->vertices[i].position.z, new_m->vertices[i].normal.x, new_m->vertices[i].normal.y, new_m->vertices[i].tex_u, new_m->vertices[i].tex_v);
	}*/
	/*for(unsigned int i = 0; i < i_size; i++) {
		printf("f: (%u, %u, %u)\n", new_m->indices[3*i], new_m->indices[3*i+1], new_m->indices[3*i+2]);
	}*/

    	glGenVertexArrays(1, &new_m->VAO);
    	glGenBuffers(1, &new_m->VBO);
    	glGenBuffers(1, &new_m->EBO);

    	glBindVertexArray(new_m->VAO);

    	glBindBuffer(GL_ARRAY_BUFFER, new_m->VBO);
    	glBufferData(GL_ARRAY_BUFFER, v_size * sizeof(Vertex), new_m->vertices, GL_STATIC_DRAW);

    	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, new_m->EBO);
    	glBufferData(GL_ELEMENT_ARRAY_BUFFER, i_size * 3 * sizeof(unsigned int), new_m->indices, GL_STATIC_DRAW);

    	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    	glEnableVertexAttribArray(0);
    
    	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vec3)));
    	glEnableVertexAttribArray(1);

    	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vec3) * 2));
    	glEnableVertexAttribArray(2);

	return new_m;
}

void render_mesh(Mesh* m) {
	glBindVertexArray(m->VAO);
	glDrawElements(GL_TRIANGLES, m->num_indices, GL_UNSIGNED_INT, 0);
}

void free_mesh(Mesh* m) {
	free(m->vertices);
	free(m->indices);
	glDeleteVertexArrays(1, &m->VAO);
	glDeleteBuffers(1, &m->VBO);
	glDeleteBuffers(1, &m->EBO);
	free(m);
}

Model* load_model(const char* filename) {
	FILE* file = fopen(filename, "r");
	unsigned int region_end_i;
	
	fseek(file, 0, SEEK_END);
	region_end_i = ftell(file);
	rewind(file);
	
	Model* new_m = (Model*)malloc(sizeof(Model));
	new_m->pa_meshes = (Mesh**)malloc(sizeof(Mesh*));
	new_m->size = 1;

	new_m->pa_meshes[0] = load_mesh(file, 0, region_end_i);

	fclose(file);

	return new_m;
}

void render_model(Model* m) {
	for(unsigned int i = 0; i < m->size; i++) {
		render_mesh(m->pa_meshes[i]);
	}
}

void free_model(Model* m) {
	for(unsigned int i = 0; i < m->size; i++) {
		free_mesh(m->pa_meshes[i]);
	}
	free(m);
}
