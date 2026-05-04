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

typedef struct Vec2 {
	float x;
	float y;
} Vec2;

typedef struct Vertex {
	Vec3 position;
	Vec3 normal;
	Vec2 texcoords;
} Vertex;

typedef struct IndicedVertex {
	Vertex v;
	unsigned int i;
} IndicedVertex;

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

void load_vertices_minimal(Mesh* new_m, FILE* file, unsigned int end_i, char* str_read, unsigned int v_size, unsigned int i_size) {
	new_m->vertices = (Vertex*)calloc(v_size, sizeof(Vertex));
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
			unsigned int indice_i = 0;
			for(unsigned int i = 0; i < i_size; i++) {
				fseek(file, 2, SEEK_CUR);
				for(unsigned int j = 0; j < 3; j++) {
					unsigned int str_i = 0;
					c_read = fgetc(file);
					while(c_read != ' ' && c_read != '\n' && ftell(file) != end_i) {	
						str_read[str_i] = c_read;
						str_i++;
						c_read = fgetc(file);
					}
					str_read[str_i] = '\0';
					new_m->indices[indice_i*3+j] = atoi(str_read) - 1;
				}
				if(c_read == ' ') {
					indice_i++;
					unsigned int str_i = 0;
					c_read = fgetc(file);
					while(c_read != '\n' && ftell(file) != end_i) {
						str_read[str_i] = c_read;
						str_i++;
						c_read = fgetc(file);
					}
					str_read[str_i] = '\0';
					new_m->indices[indice_i*3] = new_m->indices[(indice_i-1)*3];
					new_m->indices[indice_i*3 + 1] = new_m->indices[(indice_i-1)*3 + 2];
					new_m->indices[indice_i*3 + 2] = atoi(str_read) - 1;
				}
				indice_i++;
			}
		}
		else if(c_read != '\n') {
			fgets(str_read,100,file);
		}
	}
}

unsigned int load_vertices_texcoords(Mesh* new_m, FILE* file, unsigned int end_i, char* str_read, unsigned int v_size, unsigned int vt_size, unsigned int i_size) {
	IndicedVertex** vert_pa = (IndicedVertex**)malloc(v_size * sizeof(IndicedVertex*));
	Vec2* texcoords = (Vec2*)malloc(vt_size * sizeof(Vec2));
	unsigned int new_v_size = v_size;
	while(ftell(file) != end_i) {
		char c_read = fgetc(file);
		if(c_read == 'v') {
			c_read = fgetc(file);
			if(c_read == 't') {
				fseek(file, -2, SEEK_CUR);
				for(unsigned int i = 0; i < vt_size; i++) {
					fseek(file, 3, SEEK_CUR);
					float* texcoord = &(texcoords[i].x);
					for(unsigned int j = 0; j < 2; j++) {
						unsigned int str_i = 0;
						c_read = fgetc(file);
						while(c_read != ' ' && c_read != '\n') {	
							str_read[str_i] = c_read;
							str_i++;
							c_read = fgetc(file);
						}
						str_read[str_i] = '\0';
						texcoord[j] = (float)atof(str_read);
					}
					fgets(str_read,100,file);
				}
			}
			else {
				fseek(file, -2, SEEK_CUR);
				for(unsigned int i = 0; i < v_size; i++) {
					fseek(file, 2, SEEK_CUR);
					IndicedVertex* vert_struct = (IndicedVertex*)calloc(1, sizeof(IndicedVertex));
					// This is a clever solution,
					// bad for legibility though,
					// since the indice of
					// the first vert is the same as
					// its indice in the pa, its i
					// parameter will instead store
					// the length of the array in pa.
					// It is set to 0 to trigger
					// an initial condition later
					vert_struct->i = 0;
					vert_pa[i] = vert_struct;
					float* v_pos = &(vert_struct->v.position.x);
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
		}
		if(c_read == 'f') {
			fseek(file, -1, SEEK_CUR);
			unsigned int indice_i = 0;
			for(unsigned int i = 0; i < i_size; i++) {
				fseek(file, 2, SEEK_CUR);
				for(unsigned int j = 0; j < 3; j++) {
					unsigned int str_i = 0;
					c_read = fgetc(file);
					while(c_read != '/') {	
						str_read[str_i] = c_read;
						str_i++;
						c_read = fgetc(file);
					}
					str_read[str_i] = '\0';
					unsigned int v_indice = atoi(str_read) - 1;
					str_i = 0;
					c_read = fgetc(file);
					while(c_read != ' ' && c_read != '\n' && ftell(file) != end_i) {	
						str_read[str_i] = c_read;
						str_i++;
						c_read = fgetc(file);
					}
					str_read[str_i] = '\0';
					unsigned int vt_indice = atoi(str_read) - 1;
					unsigned int indice = 0;
					unsigned int s = vert_pa[v_indice][0].i;
					if(s == 0) {
						vert_pa[v_indice][0].v.texcoords = texcoords[vt_indice];
						vert_pa[v_indice][0].i++;
						indice = v_indice + 1;
					}
					for(unsigned int k = 0; k < s; k++) {
						if(vert_pa[v_indice][k].v.texcoords.x == texcoords[vt_indice].x && vert_pa[v_indice][k].v.texcoords.y == texcoords[vt_indice].y) {
							if(k == 0) indice = v_indice + 1;
							else indice = vert_pa[v_indice][k].i + 1;
							break;
						}
					}
					if(indice == 0) {
						vert_pa[v_indice][0].i++;
						vert_pa[v_indice] = (IndicedVertex*)realloc((void*)(vert_pa[v_indice]), (s+1) * sizeof(IndicedVertex));
						vert_pa[v_indice][s] = vert_pa[v_indice][0];
						vert_pa[v_indice][s].v.texcoords = texcoords[vt_indice];
						vert_pa[v_indice][s].i = new_v_size;
						new_v_size++;
						indice = new_v_size;
					}
					new_m->indices[indice_i*3+j] = indice - 1;
				}
				if(c_read == ' ') {
					indice_i++;
					unsigned int str_i = 0;
					c_read = fgetc(file);
					while(c_read != '/') {	
						str_read[str_i] = c_read;
						str_i++;
						c_read = fgetc(file);
					}
					str_read[str_i] = '\0';
					unsigned int v_indice = atoi(str_read) - 1;
					str_i = 0;
					c_read = fgetc(file);
					while(c_read != '\n' && ftell(file) != end_i) {	
						str_read[str_i] = c_read;
						str_i++;
						c_read = fgetc(file);
					}
					str_read[str_i] = '\0';
					unsigned int vt_indice = atoi(str_read) - 1;
					unsigned int indice = 0;
					unsigned int s = vert_pa[v_indice][0].i;
					if(s == 0) {
						vert_pa[v_indice][0].v.texcoords = texcoords[vt_indice];
						vert_pa[v_indice][0].i++;
						indice = v_indice + 1;
					}
					for(unsigned int k = 0; k < s; k++) {
						if(vert_pa[v_indice][k].v.texcoords.x == texcoords[vt_indice].x && vert_pa[v_indice][k].v.texcoords.y == texcoords[vt_indice].y) {
							if(k == 0) indice = v_indice + 1;
							else indice = vert_pa[v_indice][k].i + 1;
							break;
						}
					}
					if(indice == 0) {
						vert_pa[v_indice][0].i++;
						vert_pa[v_indice] = (IndicedVertex*)realloc((void*)(vert_pa[v_indice]), (s+1) * sizeof(IndicedVertex));
						vert_pa[v_indice][s] = vert_pa[v_indice][0];
						vert_pa[v_indice][s].v.texcoords = texcoords[vt_indice];
						vert_pa[v_indice][s].i = new_v_size;
						new_v_size++;
						indice = new_v_size;
					}
					new_m->indices[indice_i*3] = new_m->indices[(indice_i-1)*3];
					new_m->indices[indice_i*3 + 1] = new_m->indices[(indice_i-1)*3 + 2];
					new_m->indices[indice_i*3 + 2] = indice - 1;
				}
				indice_i++;
			}
		}
		else if(c_read != '\n') {
			fgets(str_read,100,file);
		}
	}
	free(texcoords);
	new_m->vertices = (Vertex*)malloc(new_v_size * sizeof(Vertex));
	for(unsigned int i = 0; i < v_size; i++) {
		new_m->vertices[i] = vert_pa[i][0].v;
		for(unsigned int j = 1; j < vert_pa[i][0].i; j++) {
			new_m->vertices[vert_pa[i][j].i] = vert_pa[i][j].v;
		}
		free(vert_pa[i]);
	}
	free(vert_pa);
	return new_v_size;
}

unsigned int load_vertices_normals(Mesh* new_m, FILE* file, unsigned int end_i, char* str_read, unsigned int v_size, unsigned int vn_size, unsigned int i_size) {
	IndicedVertex** vert_pa = (IndicedVertex**)malloc(v_size * sizeof(IndicedVertex*));
	Vec3* normals = (Vec3*)malloc(vn_size * sizeof(Vec3));
	unsigned int new_v_size = v_size;
	while(ftell(file) != end_i) {
		char c_read = fgetc(file);
		if(c_read == 'v') {
			c_read = fgetc(file);
			if(c_read == 'n') {
				fseek(file, -2, SEEK_CUR);
				for(unsigned int i = 0; i < vn_size; i++) {
					fseek(file, 3, SEEK_CUR);
					float* normal = &(normals[i].x);
					for(unsigned int j = 0; j < 3; j++) {
						unsigned int str_i = 0;
						c_read = fgetc(file);
						while(c_read != ' ' && c_read != '\n') {	
							str_read[str_i] = c_read;
							str_i++;
							c_read = fgetc(file);
						}
						str_read[str_i] = '\0';
						normal[j] = (float)atof(str_read);
					}
				}
			}
			else {
				fseek(file, -2, SEEK_CUR);
				for(unsigned int i = 0; i < v_size; i++) {
					fseek(file, 2, SEEK_CUR);
					IndicedVertex* vert_struct = (IndicedVertex*)calloc(1, sizeof(IndicedVertex));
					vert_struct->i = 0;
					vert_pa[i] = vert_struct;
					float* v_pos = &(vert_struct->v.position.x);
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
		}
		if(c_read == 'f') {
			fseek(file, -1, SEEK_CUR);
			unsigned int indice_i = 0;
			for(unsigned int i = 0; i < i_size; i++) {
				fseek(file, 2, SEEK_CUR);
				for(unsigned int j = 0; j < 3; j++) {
					unsigned int str_i = 0;
					c_read = fgetc(file);
					while(c_read != '/') {	
						str_read[str_i] = c_read;
						str_i++;
						c_read = fgetc(file);
					}
					str_read[str_i] = '\0';
					unsigned int v_indice = atoi(str_read) - 1;
					str_i = 0;
					c_read = fgetc(file);
					c_read = fgetc(file);
					while(c_read != ' ' && c_read != '\n' && ftell(file) != end_i) {	
						str_read[str_i] = c_read;
						str_i++;
						c_read = fgetc(file);
					}
					str_read[str_i] = '\0';
					unsigned int vn_indice = atoi(str_read) - 1;
					unsigned int indice = 0;
					unsigned int s = vert_pa[v_indice][0].i;
					if(s == 0) {
						vert_pa[v_indice][0].v.normal = normals[vn_indice];
						vert_pa[v_indice][0].i++;
						indice = v_indice + 1;
					}
					for(unsigned int k = 0; k < s; k++) {
						if(vert_pa[v_indice][k].v.normal.x == normals[vn_indice].x && vert_pa[v_indice][k].v.normal.y == normals[vn_indice].y && vert_pa[v_indice][k].v.normal.z == normals[vn_indice].z) {
							if(k == 0) indice = v_indice + 1;
							else indice = vert_pa[v_indice][k].i + 1;
							break;
						}
					}
					if(indice == 0) {
						vert_pa[v_indice][0].i++;
						vert_pa[v_indice] = (IndicedVertex*)realloc((void*)(vert_pa[v_indice]), (s+1) * sizeof(IndicedVertex));
						vert_pa[v_indice][s] = vert_pa[v_indice][0];
						vert_pa[v_indice][s].v.normal = normals[vn_indice];
						vert_pa[v_indice][s].i = new_v_size;
						new_v_size++;
						indice = new_v_size;
					}
					new_m->indices[indice_i*3+j] = indice - 1;
				}
				if(c_read == ' ') {
					indice_i++;
					unsigned int str_i = 0;
					c_read = fgetc(file);
					while(c_read != '/') {	
						str_read[str_i] = c_read;
						str_i++;
						c_read = fgetc(file);
					}
					str_read[str_i] = '\0';
					unsigned int v_indice = atoi(str_read) - 1;
					str_i = 0;
					c_read = fgetc(file);
					c_read = fgetc(file);
					while(c_read != '\n' && ftell(file) != end_i) {	
						str_read[str_i] = c_read;
						str_i++;
						c_read = fgetc(file);
					}
					str_read[str_i] = '\0';
					unsigned int vn_indice = atoi(str_read) - 1;
					unsigned int indice = 0;
					unsigned int s = vert_pa[v_indice][0].i;
					if(s == 0) {
						vert_pa[v_indice][0].v.normal = normals[vn_indice];
						vert_pa[v_indice][0].i++;
						indice = v_indice + 1;
					}
					for(unsigned int k = 0; k < s; k++) {
						if(vert_pa[v_indice][k].v.normal.x == normals[vn_indice].x && vert_pa[v_indice][k].v.normal.y == normals[vn_indice].y && vert_pa[v_indice][k].v.normal.z == normals[vn_indice].z) {
							if(k == 0) indice = v_indice + 1;
							else indice = vert_pa[v_indice][k].i + 1;
							break;
						}
					}
					if(indice == 0) {
						vert_pa[v_indice][0].i++;
						vert_pa[v_indice] = (IndicedVertex*)realloc((void*)(vert_pa[v_indice]), (s+1) * sizeof(IndicedVertex));
						vert_pa[v_indice][s] = vert_pa[v_indice][0];
						vert_pa[v_indice][s].v.normal = normals[vn_indice];
						vert_pa[v_indice][s].i = new_v_size;
						new_v_size++;
						indice = new_v_size;
					}
					new_m->indices[indice_i*3] = new_m->indices[(indice_i-1)*3];
					new_m->indices[indice_i*3 + 1] = new_m->indices[(indice_i-1)*3 + 2];
					new_m->indices[indice_i*3 + 2] = indice_i - 1;
				}
				indice_i++;
			}
		}
		else if(c_read != '\n') {
			fgets(str_read,100,file);
		}
	}
	free(normals);
	new_m->vertices = (Vertex*)malloc(new_v_size * sizeof(Vertex));
	for(unsigned int i = 0; i < v_size; i++) {
		new_m->vertices[i] = vert_pa[i][0].v;
		for(unsigned int j = 1; j < vert_pa[i][0].i; j++) {
			new_m->vertices[vert_pa[i][j].i] = vert_pa[i][j].v;
		}
		free(vert_pa[i]);
	}
	free(vert_pa);
	return new_v_size;
}

unsigned int load_vertices(Mesh* new_m, FILE* file, unsigned int end_i, char* str_read, unsigned int v_size, unsigned int vt_size, unsigned int vn_size, unsigned int i_size) {
	IndicedVertex** vert_pa = (IndicedVertex**)malloc(v_size * sizeof(IndicedVertex*));
	Vec2* texcoords = (Vec2*)malloc(vt_size * sizeof(Vec2));
	Vec3* normals = (Vec3*)malloc(vn_size * sizeof(Vec3));
	unsigned int new_v_size = v_size;
	while(ftell(file) != end_i) {
		char c_read = fgetc(file);
		if(c_read == 'v') {
			c_read = fgetc(file);
			if(c_read == 't') {
				fseek(file, -2, SEEK_CUR);
				for(unsigned int i = 0; i < vt_size; i++) {
					fseek(file, 3, SEEK_CUR);
					float* texcoord = &(texcoords[i].x);
					for(unsigned int j = 0; j < 2; j++) {
						unsigned int str_i = 0;
						c_read = fgetc(file);
						while(c_read != ' ' && c_read != '\n') {	
							str_read[str_i] = c_read;
							str_i++;
							c_read = fgetc(file);
						}
						str_read[str_i] = '\0';
						texcoord[j] = (float)atof(str_read);
					}
					fgets(str_read,100,file);
				}
			}
			else if(c_read == 'n') {
				fseek(file, -2, SEEK_CUR);
				for(unsigned int i = 0; i < vn_size; i++) {
					fseek(file, 3, SEEK_CUR);
					float* normal = &(normals[i].x);
					for(unsigned int j = 0; j < 3; j++) {
						unsigned int str_i = 0;
						c_read = fgetc(file);
						while(c_read != ' ' && c_read != '\n') {	
							str_read[str_i] = c_read;
							str_i++;
							c_read = fgetc(file);
						}
						str_read[str_i] = '\0';
						normal[j] = (float)atof(str_read);
					}
				}
			}
			else {
				fseek(file, -2, SEEK_CUR);
				for(unsigned int i = 0; i < v_size; i++) {
					fseek(file, 2, SEEK_CUR);
					IndicedVertex* vert_struct = (IndicedVertex*)calloc(1, sizeof(IndicedVertex));
					vert_struct->i = 0;
					vert_pa[i] = vert_struct;
					float* v_pos = &(vert_struct->v.position.x);
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
		}
		if(c_read == 'f') {
			fseek(file, -1, SEEK_CUR);
			unsigned int indice_i = 0;
			for(unsigned int i = 0; i < i_size; i++) {
				fseek(file, 2, SEEK_CUR);
				for(unsigned int j = 0; j < 3; j++) {
					unsigned int str_i = 0;
					c_read = fgetc(file);
					while(c_read != '/') {	
						str_read[str_i] = c_read;
						str_i++;
						c_read = fgetc(file);
					}
					str_read[str_i] = '\0';
					unsigned int v_indice = atoi(str_read) - 1;
					str_i = 0;
					c_read = fgetc(file);
					while(c_read != '/') {	
						str_read[str_i] = c_read;
						str_i++;
						c_read = fgetc(file);
					}
					str_read[str_i] = '\0';
					unsigned int vt_indice = atoi(str_read) - 1;
					str_i = 0;
					c_read = fgetc(file);
					while(c_read != ' ' && c_read != '\n' && ftell(file) != end_i) {	
						str_read[str_i] = c_read;
						str_i++;
						c_read = fgetc(file);
					}
					str_read[str_i] = '\0';
					unsigned int vn_indice = atoi(str_read) - 1;
					unsigned int indice = 0;
					unsigned int s = vert_pa[v_indice][0].i;
					if(s == 0) {
						vert_pa[v_indice][0].v.normal = normals[vn_indice];
						vert_pa[v_indice][0].v.texcoords = texcoords[vt_indice];
						vert_pa[v_indice][0].i++;
						indice = v_indice + 1;
					}
					for(unsigned int k = 0; k < s; k++) {
						if(vert_pa[v_indice][k].v.texcoords.x == texcoords[vt_indice].x && vert_pa[v_indice][k].v.texcoords.y == texcoords[vt_indice].y && vert_pa[v_indice][k].v.normal.x == normals[vn_indice].x && vert_pa[v_indice][k].v.normal.y == normals[vn_indice].y && vert_pa[v_indice][k].v.normal.z == normals[vn_indice].z) {
							if(k == 0) indice = v_indice + 1;
							else indice = vert_pa[v_indice][k].i + 1;
							break;
						}
					}
					if(indice == 0) {
						vert_pa[v_indice][0].i++;
						vert_pa[v_indice] = (IndicedVertex*)realloc((void*)(vert_pa[v_indice]), (s+1) * sizeof(IndicedVertex));
						vert_pa[v_indice][s] = vert_pa[v_indice][0];
						vert_pa[v_indice][s].v.texcoords = texcoords[vt_indice];
						vert_pa[v_indice][s].v.normal = normals[vn_indice];
						vert_pa[v_indice][s].i = new_v_size;
						new_v_size++;
						indice = new_v_size;
					}
					new_m->indices[indice_i*3+j] = indice - 1;
				}
				if(c_read == ' ') {
					indice_i++;
					unsigned int str_i = 0;
					c_read = fgetc(file);
					while(c_read != '/') {	
						str_read[str_i] = c_read;
						str_i++;
						c_read = fgetc(file);
					}
					str_read[str_i] = '\0';
					unsigned int v_indice = atoi(str_read) - 1;
					str_i = 0;
					c_read = fgetc(file);
					while(c_read != '/') {	
						str_read[str_i] = c_read;
						str_i++;
						c_read = fgetc(file);
					}
					str_read[str_i] = '\0';
					unsigned int vt_indice = atoi(str_read) - 1;
					str_i = 0;
					c_read = fgetc(file);
					while(c_read != '\n' && ftell(file) != end_i) {	
						str_read[str_i] = c_read;
						str_i++;
						c_read = fgetc(file);
					}
					str_read[str_i] = '\0';
					unsigned int vn_indice = atoi(str_read) - 1;
					unsigned int indice = 0;
					unsigned int s = vert_pa[v_indice][0].i;
					if(s == 0) {
						vert_pa[v_indice][0].v.normal = normals[vn_indice];
						vert_pa[v_indice][0].v.texcoords = texcoords[vt_indice];
						vert_pa[v_indice][0].i++;
						indice = v_indice + 1;
					}
					for(unsigned int k = 0; k < s; k++) {
						if(vert_pa[v_indice][k].v.texcoords.x == texcoords[vt_indice].x && vert_pa[v_indice][k].v.texcoords.y == texcoords[vt_indice].y && vert_pa[v_indice][k].v.normal.x == normals[vn_indice].x && vert_pa[v_indice][k].v.normal.y == normals[vn_indice].y && vert_pa[v_indice][k].v.normal.z == normals[vn_indice].z) {
							if(k == 0) indice = v_indice + 1;
							else indice = vert_pa[v_indice][k].i + 1;
							break;
						}
					}
					if(indice == 0) {
						vert_pa[v_indice][0].i++;
						vert_pa[v_indice] = (IndicedVertex*)realloc((void*)(vert_pa[v_indice]), (s+1) * sizeof(IndicedVertex));
						vert_pa[v_indice][s] = vert_pa[v_indice][0];
						vert_pa[v_indice][s].v.texcoords = texcoords[vt_indice];
						vert_pa[v_indice][s].v.normal = normals[vn_indice];
						vert_pa[v_indice][s].i = new_v_size;
						new_v_size++;
						indice = new_v_size;
					}
					new_m->indices[indice_i*3] = new_m->indices[(indice_i-1)*3];
					new_m->indices[indice_i*3 + 1] = new_m->indices[(indice_i-1)*3 + 2];
					new_m->indices[indice_i*3 + 2] = indice - 1;
				}
				indice_i++;
			}
		}
		else if(c_read != '\n') {
			fgets(str_read,100,file);
		}
	}
	free(normals);
	free(texcoords);
	new_m->vertices = (Vertex*)malloc(new_v_size * sizeof(Vertex));
	for(unsigned int i = 0; i < v_size; i++) {
		new_m->vertices[i] = vert_pa[i][0].v;
		for(unsigned int j = 1; j < vert_pa[i][0].i; j++) {
			new_m->vertices[vert_pa[i][j].i] = vert_pa[i][j].v;
		}
		free(vert_pa[i]);
	}
	free(vert_pa);
	return new_v_size;
}

Mesh* load_mesh(FILE* file, unsigned int beg_i, unsigned int end_i) {
	unsigned int v_size = 0;
	unsigned int vn_size = 0;
	unsigned int vt_size = 0;
	unsigned int i_size = 0;
	unsigned int new_i_size = 0;
	fseek(file, beg_i, SEEK_SET);
	char* str_read = (char*)malloc(sizeof(char) * 100);
	while(ftell(file) != end_i) {
		char c_read = fgetc(file);
		if(c_read == 'v') {
			c_read = fgetc(file);
			if(c_read == 't') {
				c_read = 'v';
				while(c_read == 'v') {
					vt_size++;
					fgets(str_read,100,file);
					if(ftell(file) == end_i) break;
					c_read = fgetc(file);
				}
			}
			else if(c_read == 'n') {
				c_read = 'v';
				while(c_read == 'v') {
					vn_size++;
					fgets(str_read,100,file);
					if(ftell(file) == end_i) break;
					c_read = fgetc(file);
				}
			}
			else {
				c_read = 'v';
				while(c_read == 'v') {
					v_size++;
					fgets(str_read,100,file);
					if(ftell(file) == end_i) break;
					c_read = fgetc(file);
				}
			}
			if(ftell(file) != end_i) fseek(file,-1,SEEK_CUR);
		}
		if(c_read == 'f') {
			while(c_read == 'f') {
				i_size++;
				unsigned int count = 0;
				while(c_read != '\n' && ftell(file) != end_i) {
					c_read = fgetc(file);
					if(c_read == ' ') count++;
				}
				if(count == 4) new_i_size++;
				else if(count > 4) {
					free(str_read);
					printf("ERROR: mesh has n-polygons larger than quads\n");
					return (Mesh*)NULL;
				}
				if(ftell(file) == end_i) break;
				c_read = fgetc(file);
			}
			if(ftell(file) != end_i) fseek(file,-1,SEEK_CUR);
		}
		else if(c_read != '\n') {
			fgets(str_read,100,file);
		}
	}
	new_i_size += i_size;
	Mesh* new_m = (Mesh*)malloc(sizeof(Mesh));
	new_m->indices = (unsigned int*)malloc(sizeof(unsigned int) * new_i_size * 3);
	new_m->num_indices = new_i_size * 3;

	fseek(file, beg_i, SEEK_SET);
	if(vt_size == 0 && vn_size == 0) {
		load_vertices_minimal(new_m, file, end_i, str_read, v_size, i_size);
	}
	else if(vt_size != 0 && vn_size != 0) {
		v_size = load_vertices(new_m, file, end_i, str_read, v_size, vt_size, vn_size, i_size);
	}
	else if(vt_size != 0) {
		v_size = load_vertices_texcoords(new_m, file, end_i, str_read, v_size, vt_size, i_size);
	}
	else if(vn_size != 0) {
		v_size = load_vertices_normals(new_m, file, end_i, str_read, v_size, vn_size, i_size);
	}
	else {
		printf("VERTEX LOADING ERROR\n");
	}
	free(str_read);

    	glGenVertexArrays(1, &new_m->VAO);
    	glGenBuffers(1, &new_m->VBO);
    	glGenBuffers(1, &new_m->EBO);

    	glBindVertexArray(new_m->VAO);

    	glBindBuffer(GL_ARRAY_BUFFER, new_m->VBO);
    	glBufferData(GL_ARRAY_BUFFER, v_size * sizeof(Vertex), new_m->vertices, GL_STATIC_DRAW);

    	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, new_m->EBO);
    	glBufferData(GL_ELEMENT_ARRAY_BUFFER, new_i_size * 3 * sizeof(unsigned int), new_m->indices, GL_STATIC_DRAW);

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
