#pragma once

#include <shader_s.h>

#include <stdlib.h>
#include <string.h>

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
	unsigned int* indices;
	unsigned int num_indices;
	unsigned int VAO;
	unsigned int EBO;
} Mesh;

typedef struct Model {
	Vertex* vertices;
	Mesh** pa_meshes;
	unsigned int size;
	unsigned int VBO;
} Model;

void render_mesh(Mesh* m) {
	glBindVertexArray(m->VAO);
	glDrawElements(GL_TRIANGLES, m->num_indices, GL_UNSIGNED_INT, 0);
}

void free_mesh(Mesh* m) {
	free(m->indices);
	glDeleteVertexArrays(1, &m->VAO);
	glDeleteBuffers(1, &m->EBO);
	free(m);
}

void initialize_pa_vert(IndicedVertex** pa_vert, Vec3* normals, Vec2* texcoords, unsigned int v_size, FILE* file, unsigned int end_i) {
	char* str_read = (char*)malloc(100 * sizeof(char));
	unsigned int v_i = 0;
	unsigned int vn_i = 0;
	unsigned int vt_i = 0;
	while(ftell(file) != end_i) {
		char c_read = fgetc(file);
		if(c_read == 'v') {
			c_read = fgetc(file);
			if(c_read == 't') {
				fseek(file, 1, SEEK_CUR);
				float* texcoord = &(texcoords[vt_i].x);
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
				if(c_read != '\n') fgets(str_read,100,file);
				vt_i++;
			}
			else if(c_read == 'n') {
				fseek(file, 1, SEEK_CUR);
				float* normal = &(normals[vn_i].x);
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
				vn_i++;
			}
			else {
				IndicedVertex* vert_struct = (IndicedVertex*)calloc(1, sizeof(IndicedVertex));
				vert_struct->i = 0;
				pa_vert[v_i] = vert_struct;
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
				v_i++;
			}
		}
		else if(c_read != '\n') {
			fgets(str_read,100,file);
		}
	}
	free(str_read);
}

void generate_mesh_minimal(Mesh** pp_mesh, IndicedVertex** pa_vert, unsigned int v_size, unsigned int VBO, FILE* file, unsigned int* beg_indices_and_f_sizes) {
	unsigned int section_size = sizeof(beg_indices_and_f_sizes) / sizeof(unsigned int);
	unsigned int f_size = 0;
	for(unsigned int i = 1; i < section_size; i += 2) {
		f_size += beg_indices_and_f_sizes[i];
	}
	Mesh* new_m = (Mesh*)malloc(sizeof(Mesh));
	*pp_mesh = new_m;
	new_m->indices = (unsigned int*)malloc(sizeof(unsigned int) * f_size * 3);
	unsigned int indice_i = 0;
	char c_read;
	char* str_read = (char*)malloc(100 * sizeof(char));
	for(unsigned int i = 0; i < section_size; i += 2){
		fseek(file, beg_indices_and_f_sizes[i], SEEK_SET);
		c_read = fgetc(file);
		while(c_read == 'f') {
			fseek(file, 1, SEEK_CUR);
			for(unsigned int j = 0; j < 3; j++) {
				unsigned int str_i = 0;
				c_read = fgetc(file);
				while(c_read != ' ' && c_read != '\n') {	
					str_read[str_i] = c_read;
					str_i++;
					c_read = fgetc(file);
				}
				str_read[str_i] = '\0';
				new_m->indices[indice_i*3+j] = atoi(str_read) - 1;
			}
			Vec3 a = pa_vert[new_m->indices[3*indice_i]]->v.position;
			Vec3 ab = pa_vert[new_m->indices[3*indice_i+1]]->v.position;
			ab.x -= a.x;
			ab.y -= a.y;
			ab.z -= a.z;
			Vec3 ac = pa_vert[new_m->indices[3*indice_i+2]]->v.position;
			ac.x -= a.x;
			ac.y -= a.y;
			ac.z -= a.z;
			Vec3 normal = {
				ab.y * ac.z - ab.z * ac.y,
				ab.z * ac.x - ab.x * ac.z,
				ab.x * ac.y - ab.y * ac.x
			};
			float mag = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
			normal.x /= mag;
			normal.y /= mag;
			normal.z /= mag;
			pa_vert[new_m->indices[3*indice_i]]->v.normal.x +=normal.x;
			pa_vert[new_m->indices[3*indice_i]]->v.normal.y +=normal.y;
			pa_vert[new_m->indices[3*indice_i]]->v.normal.z +=normal.z;
			pa_vert[new_m->indices[3*indice_i]]->i++;
			pa_vert[new_m->indices[3*indice_i+1]]->v.normal.x +=normal.x;
			pa_vert[new_m->indices[3*indice_i+1]]->v.normal.y +=normal.y;
			pa_vert[new_m->indices[3*indice_i+1]]->v.normal.z +=normal.z;
			pa_vert[new_m->indices[3*indice_i+1]]->i++;
			pa_vert[new_m->indices[3*indice_i+2]]->v.normal.x +=normal.x;
			pa_vert[new_m->indices[3*indice_i+2]]->v.normal.y +=normal.y;
			pa_vert[new_m->indices[3*indice_i+2]]->v.normal.z +=normal.z;
			pa_vert[new_m->indices[3*indice_i+2]]->i++;
			indice_i++;
			c_read = fgetc(file);
		}
	}
	free(str_read);
	new_m->num_indices = indice_i * 3;

    	glGenVertexArrays(1, &new_m->VAO);
    	glGenBuffers(1, &new_m->EBO);

    	glBindVertexArray(new_m->VAO);

    	glBindBuffer(GL_ARRAY_BUFFER, VBO);

    	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, new_m->EBO);
    	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indice_i * 3 * sizeof(unsigned int), new_m->indices, GL_STATIC_DRAW);

    	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    	glEnableVertexAttribArray(0);
    
    	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vec3)));
    	glEnableVertexAttribArray(1);

    	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vec3) * 2));
    	glEnableVertexAttribArray(2);
}

unsigned int generate_mesh_given_normals(Mesh** pp_mesh, IndicedVertex** pa_vert, Vec3* normals, unsigned int v_size, unsigned int VBO, FILE* file, unsigned int* beg_indices_and_f_sizes) {
	unsigned int section_size = sizeof(beg_indices_and_f_sizes) / sizeof(unsigned int);
	unsigned int f_size = 0;
	for(unsigned int i = 1; i < section_size; i += 2) {
		f_size += beg_indices_and_f_sizes[i];
	}
	Mesh* new_m = (Mesh*)malloc(sizeof(Mesh));
	*pp_mesh = new_m;
	new_m->indices = (unsigned int*)malloc(sizeof(unsigned int) * f_size * 6);
	unsigned int new_v_size = v_size;
	unsigned int indice_i = 0;
	char c_read;
	char* str_read = (char*)malloc(100 * sizeof(char));
	for(unsigned int i = 0; i < section_size; i += 2){
		fseek(file, beg_indices_and_f_sizes[i], SEEK_SET);
		c_read = fgetc(file);
		while(c_read == 'f') {
			fseek(file, 1, SEEK_CUR);
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
				while(c_read != ' ' && c_read != '\n') {	
					str_read[str_i] = c_read;
					str_i++;
					c_read = fgetc(file);
				}
				str_read[str_i] = '\0';
				unsigned int vn_indice = atoi(str_read) - 1;
				unsigned int indice = 0;
				unsigned int s = pa_vert[v_indice][0].i;
				if(s == 0) {
					pa_vert[v_indice][0].v.normal = normals[vn_indice];
					pa_vert[v_indice][0].i++;
					indice = v_indice + 1;
				}
				for(unsigned int k = 0; k < s; k++) {
					if(pa_vert[v_indice][k].v.normal.x == normals[vn_indice].x && pa_vert[v_indice][k].v.normal.y == normals[vn_indice].y && pa_vert[v_indice][k].v.normal.z == normals[vn_indice].z) {
						if(k == 0) indice = v_indice + 1;
						else indice = pa_vert[v_indice][k].i + 1;
						break;
					}
				}
				if(indice == 0) {
					pa_vert[v_indice][0].i++;
					pa_vert[v_indice] = (IndicedVertex*)realloc((void*)(pa_vert[v_indice]), (s+1) * sizeof(IndicedVertex));
					pa_vert[v_indice][s] = pa_vert[v_indice][0];
					pa_vert[v_indice][s].v.normal = normals[vn_indice];
					pa_vert[v_indice][s].i = new_v_size;
					new_v_size++;
					indice = new_v_size;
				}
				new_m->indices[indice_i*3+j] = indice - 1;
			}
			if(c_read == ' ') {
			c_read = fgetc(file);
			if(c_read != '\n') {
				indice_i++;
				unsigned int str_i = 0;
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
				while(c_read != '\n') {	
					str_read[str_i] = c_read;
					str_i++;
					c_read = fgetc(file);
				}
				str_read[str_i] = '\0';
				unsigned int vn_indice = atoi(str_read) - 1;
				unsigned int indice = 0;
				unsigned int s = pa_vert[v_indice][0].i;
				if(s == 0) {
					pa_vert[v_indice][0].v.normal = normals[vn_indice];
					pa_vert[v_indice][0].i++;
					indice = v_indice + 1;
				}
				for(unsigned int j = 0; j < s; j++) {
					if(pa_vert[v_indice][j].v.normal.x == normals[vn_indice].x && pa_vert[v_indice][j].v.normal.y == normals[vn_indice].y && pa_vert[v_indice][j].v.normal.z == normals[vn_indice].z) {
						if(j == 0) indice = v_indice + 1;
						else indice = pa_vert[v_indice][j].i + 1;
						break;
					}
				}
				if(indice == 0) {
					pa_vert[v_indice][0].i++;
					pa_vert[v_indice] = (IndicedVertex*)realloc((void*)(pa_vert[v_indice]), (s+1) * sizeof(IndicedVertex));
					pa_vert[v_indice][s] = pa_vert[v_indice][0];
					pa_vert[v_indice][s].v.normal = normals[vn_indice];
					pa_vert[v_indice][s].i = new_v_size;
					new_v_size++;
					indice = new_v_size;
				}
				new_m->indices[indice_i*3] = new_m->indices[(indice_i-1)*3];
				new_m->indices[indice_i*3 + 1] = new_m->indices[(indice_i-1)*3 + 2];
				new_m->indices[indice_i*3 + 2] = indice - 1;
			}
			}
			indice_i++;
			c_read = fgetc(file);
		}
	}
	free(str_read);
	new_m->indices = (unsigned int*)realloc(new_m->indices, sizeof(unsigned int) * indice_i * 3);
	new_m->num_indices = indice_i * 3;

    	glGenVertexArrays(1, &new_m->VAO);
    	glGenBuffers(1, &new_m->EBO);

    	glBindVertexArray(new_m->VAO);

    	glBindBuffer(GL_ARRAY_BUFFER, VBO);

    	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, new_m->EBO);
    	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indice_i * 3 * sizeof(unsigned int), new_m->indices, GL_STATIC_DRAW);

    	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    	glEnableVertexAttribArray(0);
    
    	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vec3)));
    	glEnableVertexAttribArray(1);

    	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vec3) * 2));
    	glEnableVertexAttribArray(2);
	
	return new_v_size;
}

unsigned int generate_mesh_given_texcoords(Mesh** pp_mesh, IndicedVertex** pa_vert, Vec2* texcoords, unsigned int v_size, unsigned int VBO, FILE* file, unsigned int* beg_indices_and_f_sizes) {
	unsigned int section_size = sizeof(beg_indices_and_f_sizes) / sizeof(unsigned int);
	unsigned int f_size = 0;
	for(unsigned int i = 1; i < section_size; i += 2) {
		f_size += beg_indices_and_f_sizes[i];
	}
	Mesh* new_m = (Mesh*)malloc(sizeof(Mesh));
	*pp_mesh = new_m;
	new_m->indices = (unsigned int*)malloc(sizeof(unsigned int) * f_size * 6);
	unsigned int new_v_size = v_size;
	unsigned int indice_i = 0;
	char c_read;
	char* str_read = (char*)malloc(100 * sizeof(char));
	unsigned int* face = (unsigned int*)malloc(8 * sizeof(unsigned int));
	for(unsigned int i = 0; i < section_size; i += 2){
		fseek(file, beg_indices_and_f_sizes[i], SEEK_SET);
		c_read = fgetc(file);
		while(c_read == 'f') {
			fseek(file, 1, SEEK_CUR);
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
				face[j*2] = v_indice;
				str_i = 0;
				c_read = fgetc(file);
				while(c_read != ' ' && c_read != '\n') {	
					str_read[str_i] = c_read;
					str_i++;
					c_read = fgetc(file);
				}
				str_read[str_i] = '\0';
				unsigned int vt_indice = atoi(str_read) - 1;
				unsigned int indice = 0;
				unsigned int s = pa_vert[v_indice][0].i;
				if(s == 0) {
					pa_vert[v_indice][0].v.texcoords = texcoords[vt_indice];
					pa_vert[v_indice][0].i++;
					indice = v_indice + 1;
					face[j*2+1] = 0;
				}
				for(unsigned int k = 0; k < s; k++) {
					if(pa_vert[v_indice][k].v.texcoords.x == texcoords[vt_indice].x && pa_vert[v_indice][k].v.texcoords.y == texcoords[vt_indice].y) {
						if(k == 0) indice = v_indice + 1;
						else indice = pa_vert[v_indice][k].i + 1;
						face[j*2+1] = k;
						break;
					}
				}
				if(indice == 0) {
					pa_vert[v_indice][0].i++;
					pa_vert[v_indice] = (IndicedVertex*)realloc((void*)(pa_vert[v_indice]), (s+1) * sizeof(IndicedVertex));
					pa_vert[v_indice][s] = pa_vert[v_indice][0];
					pa_vert[v_indice][s].v.texcoords = texcoords[vt_indice];
					pa_vert[v_indice][s].i = new_v_size;
					new_v_size++;
					indice = new_v_size;
					face[j*2+1] = s;
				}
				new_m->indices[indice_i*3+j] = indice - 1;
			}
			Vec3 a = pa_vert[face[0]][face[1]].v.position;
			Vec3 ab = pa_vert[face[2]][face[3]].v.position;
			ab.x -= a.x;
			ab.y -= a.y;
			ab.z -= a.z;
			Vec3 ac = pa_vert[face[4]][face[5]].v.position;
			ac.x -= a.x;
			ac.y -= a.y;
			ac.z -= a.z;
			Vec3 normal = {
				ab.y * ac.z - ab.z * ac.y,
				ab.z * ac.x - ab.x * ac.z,
				ab.x * ac.y - ab.y * ac.x
			};
			float mag = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
			normal.x /= mag;
			normal.y /= mag;
			normal.z /= mag;
			pa_vert[face[0]][face[1]].v.normal.x +=normal.x;
			pa_vert[face[0]][face[1]].v.normal.y +=normal.y;
			pa_vert[face[0]][face[1]].v.normal.z +=normal.z;
			pa_vert[face[0]][face[1]].v.texcoords.x += 10;
			pa_vert[face[2]][face[3]].v.normal.x +=normal.x;
			pa_vert[face[2]][face[3]].v.normal.y +=normal.y;
			pa_vert[face[2]][face[3]].v.normal.z +=normal.z;
			pa_vert[face[2]][face[3]].v.texcoords.x += 10;
			pa_vert[face[4]][face[5]].v.normal.x +=normal.x;
			pa_vert[face[4]][face[5]].v.normal.y +=normal.y;
			pa_vert[face[4]][face[5]].v.normal.z +=normal.z;
			pa_vert[face[4]][face[5]].v.texcoords.x += 10;
			if(c_read == ' ') {
			c_read = fgetc(file);
			if(c_read != '\n') {
				indice_i++;
				unsigned int str_i = 0;
				while(c_read != '/') {	
					str_read[str_i] = c_read;
					str_i++;
					c_read = fgetc(file);
				}
				str_read[str_i] = '\0';
				unsigned int v_indice = atoi(str_read) - 1;
				face[6] = v_indice;
				str_i = 0;
				c_read = fgetc(file);
				while(c_read != '\n') {	
					str_read[str_i] = c_read;
					str_i++;
					c_read = fgetc(file);
				}
				str_read[str_i] = '\0';
				unsigned int vt_indice = atoi(str_read) - 1;
				unsigned int indice = 0;
				unsigned int s = pa_vert[v_indice][0].i;
				if(s == 0) {
					pa_vert[v_indice][0].v.texcoords = texcoords[vt_indice];
					pa_vert[v_indice][0].i++;
					indice = v_indice + 1;
					face[7] = 0;
				}
				for(unsigned int k = 0; k < s; k++) {
					if(pa_vert[v_indice][k].v.texcoords.x == texcoords[vt_indice].x && pa_vert[v_indice][k].v.texcoords.y == texcoords[vt_indice].y) {
						if(k == 0) indice = v_indice + 1;
						else indice = pa_vert[v_indice][k].i + 1;
						face[7] = k;
						break;
					}
				}
				if(indice == 0) {
					pa_vert[v_indice][0].i++;
					pa_vert[v_indice] = (IndicedVertex*)realloc((void*)(pa_vert[v_indice]), (s+1) * sizeof(IndicedVertex));
					pa_vert[v_indice][s] = pa_vert[v_indice][0];
					pa_vert[v_indice][s].v.texcoords = texcoords[vt_indice];
					pa_vert[v_indice][s].i = new_v_size;
					new_v_size++;
					indice = new_v_size;
					face[7] = s;
				}
				new_m->indices[indice_i*3] = new_m->indices[(indice_i-1)*3];
				new_m->indices[indice_i*3 + 1] = new_m->indices[(indice_i-1)*3 + 2];
				new_m->indices[indice_i*3 + 2] = indice - 1;
				Vec3 a = pa_vert[face[0]][face[1]].v.position;
				Vec3 ab = pa_vert[face[4]][face[5]].v.position;
				ab.x -= a.x;
				ab.y -= a.y;
				ab.z -= a.z;
				Vec3 ac = pa_vert[face[6]][face[7]].v.position;
				ac.x -= a.x;
				ac.y -= a.y;
				ac.z -= a.z;
				Vec3 normal = {
					ab.y * ac.z - ab.z * ac.y,
					ab.z * ac.x - ab.x * ac.z,
					ab.x * ac.y - ab.y * ac.x
				};
				float mag = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
				normal.x /= mag;
				normal.y /= mag;
				normal.z /= mag;
				pa_vert[face[0]][face[1]].v.normal.x +=normal.x;
				pa_vert[face[0]][face[1]].v.normal.y +=normal.y;
				pa_vert[face[0]][face[1]].v.normal.z +=normal.z;
				pa_vert[face[0]][face[1]].v.texcoords.x += 10;
				pa_vert[face[4]][face[5]].v.normal.x +=normal.x;
				pa_vert[face[4]][face[5]].v.normal.y +=normal.y;
				pa_vert[face[4]][face[5]].v.normal.z +=normal.z;
				pa_vert[face[4]][face[5]].v.texcoords.x += 10;
				pa_vert[face[6]][face[7]].v.normal.x +=normal.x;
				pa_vert[face[6]][face[7]].v.normal.y +=normal.y;
				pa_vert[face[6]][face[7]].v.normal.z +=normal.z;
				pa_vert[face[6]][face[7]].v.texcoords.x += 10;
			}
			}
			indice_i++;
			c_read = fgetc(file);
		}
	}
	free(str_read);
	free(face);
	new_m->indices = (unsigned int*)realloc(new_m->indices, sizeof(unsigned int) * indice_i * 3);
	new_m->num_indices = indice_i * 3;

    	glGenVertexArrays(1, &new_m->VAO);
    	glGenBuffers(1, &new_m->EBO);

    	glBindVertexArray(new_m->VAO);

    	glBindBuffer(GL_ARRAY_BUFFER, VBO);

    	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, new_m->EBO);
    	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indice_i * 3 * sizeof(unsigned int), new_m->indices, GL_STATIC_DRAW);

    	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    	glEnableVertexAttribArray(0);
    
    	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vec3)));
    	glEnableVertexAttribArray(1);

    	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vec3) * 2));
    	glEnableVertexAttribArray(2);
	
	return new_v_size;
}

unsigned int generate_mesh(Mesh** pp_mesh, IndicedVertex** pa_vert, Vec3* normals, Vec2* texcoords, unsigned int v_size, unsigned int VBO, FILE* file, unsigned int* beg_indices_and_f_sizes) {
	unsigned int section_size = sizeof(beg_indices_and_f_sizes) / sizeof(unsigned int);
	unsigned int f_size = 0;
	for(unsigned int i = 1; i < section_size; i += 2) {
		f_size += beg_indices_and_f_sizes[i];
	}
	Mesh* new_m = (Mesh*)malloc(sizeof(Mesh));
	*pp_mesh = new_m;
	new_m->indices = (unsigned int*)malloc(sizeof(unsigned int) * f_size * 6);
	unsigned int new_v_size = v_size;
	unsigned int indice_i = 0;
	char c_read;
	char* str_read = (char*)malloc(100 * sizeof(char));
	for(unsigned int i = 0; i < section_size; i += 2){
		fseek(file, beg_indices_and_f_sizes[i], SEEK_SET);
		c_read = fgetc(file);
		while(c_read == 'f') {
			fseek(file, 1, SEEK_CUR);
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
				while(c_read != ' ' && c_read != '\n') {	
					str_read[str_i] = c_read;
					str_i++;
					c_read = fgetc(file);
				}
				str_read[str_i] = '\0';
				unsigned int vn_indice = atoi(str_read) - 1;
				unsigned int indice = 0;
				unsigned int s = pa_vert[v_indice][0].i;
				if(s == 0) {
					pa_vert[v_indice][0].v.normal = normals[vn_indice];
					pa_vert[v_indice][0].v.texcoords = texcoords[vt_indice];
					pa_vert[v_indice][0].i++;
					indice = v_indice + 1;
				}
				for(unsigned int k = 0; k < s; k++) {
					if(pa_vert[v_indice][k].v.texcoords.x == texcoords[vt_indice].x && pa_vert[v_indice][k].v.texcoords.y == texcoords[vt_indice].y && pa_vert[v_indice][k].v.normal.x == normals[vn_indice].x && pa_vert[v_indice][k].v.normal.y == normals[vn_indice].y && pa_vert[v_indice][k].v.normal.z == normals[vn_indice].z) {
						if(k == 0) indice = v_indice + 1;
						else indice = pa_vert[v_indice][k].i + 1;
						break;
					}
				}
				if(indice == 0) {
					pa_vert[v_indice][0].i++;
					pa_vert[v_indice] = (IndicedVertex*)realloc((void*)(pa_vert[v_indice]), (s+1) * sizeof(IndicedVertex));
					pa_vert[v_indice][s] = pa_vert[v_indice][0];
					pa_vert[v_indice][s].v.texcoords = texcoords[vt_indice];
					pa_vert[v_indice][s].v.normal = normals[vn_indice];
					pa_vert[v_indice][s].i = new_v_size;
					new_v_size++;
					indice = new_v_size;
				}
				new_m->indices[indice_i*3+j] = indice - 1;
			}
			if(c_read == ' ') {
			c_read = fgetc(file);
			if(c_read != '\n') {
				indice_i++;
				unsigned int str_i = 0;
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
				while(c_read != '\n') {	
					str_read[str_i] = c_read;
					str_i++;
					c_read = fgetc(file);
				}
				str_read[str_i] = '\0';
				unsigned int vn_indice = atoi(str_read) - 1;
				unsigned int indice = 0;
				unsigned int s = pa_vert[v_indice][0].i;
				if(s == 0) {
					pa_vert[v_indice][0].v.normal = normals[vn_indice];
					pa_vert[v_indice][0].v.texcoords = texcoords[vt_indice];
					pa_vert[v_indice][0].i++;
					indice = v_indice + 1;
				}
				for(unsigned int j = 0; j < s; j++) {
					if(pa_vert[v_indice][j].v.texcoords.x == texcoords[vt_indice].x && pa_vert[v_indice][j].v.texcoords.y == texcoords[vt_indice].y && pa_vert[v_indice][j].v.normal.x == normals[vn_indice].x && pa_vert[v_indice][j].v.normal.y == normals[vn_indice].y && pa_vert[v_indice][j].v.normal.z == normals[vn_indice].z) {
						if(j == 0) indice = v_indice + 1;
						else indice = pa_vert[v_indice][j].i + 1;
						break;
					}
				}
				if(indice == 0) {
					pa_vert[v_indice][0].i++;
					pa_vert[v_indice] = (IndicedVertex*)realloc((void*)(pa_vert[v_indice]), (s+1) * sizeof(IndicedVertex));
					pa_vert[v_indice][s] = pa_vert[v_indice][0];
					pa_vert[v_indice][s].v.texcoords = texcoords[vt_indice];
					pa_vert[v_indice][s].v.normal = normals[vn_indice];
					pa_vert[v_indice][s].i = new_v_size;
					new_v_size++;
					indice = new_v_size;
				}
				new_m->indices[indice_i*3] = new_m->indices[(indice_i-1)*3];
				new_m->indices[indice_i*3 + 1] = new_m->indices[(indice_i-1)*3 + 2];
				new_m->indices[indice_i*3 + 2] = indice - 1;
			}
			}
			indice_i++;
			c_read = fgetc(file);
		}
	}
	free(str_read);
	new_m->indices = (unsigned int*)realloc(new_m->indices, sizeof(unsigned int) * indice_i * 3);
	new_m->num_indices = indice_i * 3;

    	glGenVertexArrays(1, &new_m->VAO);
    	glGenBuffers(1, &new_m->EBO);

    	glBindVertexArray(new_m->VAO);

    	glBindBuffer(GL_ARRAY_BUFFER, VBO);

    	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, new_m->EBO);
    	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indice_i * 3 * sizeof(unsigned int), new_m->indices, GL_STATIC_DRAW);

    	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    	glEnableVertexAttribArray(0);
    
    	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vec3)));
    	glEnableVertexAttribArray(1);

    	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vec3) * 2));
    	glEnableVertexAttribArray(2);
	
	return new_v_size;
}

Model* load_model(const char* filename) {
	FILE* file = fopen(filename, "r");
	unsigned int end_i;
	
	fseek(file, 0, SEEK_END);
	end_i = ftell(file);
	rewind(file);

	unsigned int v_size = 0;
	unsigned int vn_size = 0;
	unsigned int vt_size = 0;
	unsigned int group_size = 0;
	unsigned int group_beg_indice_i = 0;
	unsigned int** pa_group_beg_indices_and_f_sizes = (unsigned int**)malloc(10 * sizeof(unsigned int*));
	char str_read[100];
	char c_read;
	while(ftell(file) != end_i) {
		c_read = fgetc(file);
		if(c_read == 'v') {
			c_read = fgetc(file);
			if(c_read == 'n') vn_size++;
			else if(c_read == 't') vt_size++;
			else v_size++;
		}
		else if(c_read == 'f') {
			if(group_size == 0) {
				pa_group_beg_indices_and_f_sizes[group_size] = (unsigned int*)malloc(2*sizeof(unsigned int));
				group_size = 1;
			}
			if(group_beg_indice_i > 0) pa_group_beg_indices_and_f_sizes[group_size-1] = (unsigned int*)realloc(pa_group_beg_indices_and_f_sizes[group_size-1], (2*group_beg_indice_i+2) * sizeof(unsigned int));
			pa_group_beg_indices_and_f_sizes[group_size-1][2*group_beg_indice_i] = ftell(file)-1;
			unsigned int f_size = 1;
			while(c_read == 'f') {
				f_size++;
				if(ftell(file) == end_i) break;
				fgets(str_read,100,file);
				c_read = fgetc(file);
			}
			fseek(file,-2,SEEK_CUR);
			pa_group_beg_indices_and_f_sizes[group_size-1][2*group_beg_indice_i+1] = f_size;
			group_beg_indice_i++;
		}
		else if(c_read == 'm') {
			fseek(file, -1, SEEK_CUR);
			fgets(str_read,8,file);
			if(strcmp(str_read, "mtllib ") == 0) {
				// load material via filename
				// immplement later
			}
			else {
				fseek(file, -7, SEEK_CUR);
			}
		}
		else if(c_read == 'u') {
			fseek(file, -1, SEEK_CUR);
			fgets(str_read,8,file);
			if(strcmp(str_read, "usemtl ") == 0) {
				group_size++;
				if((group_size-1) % 10 == 0 && group_size != 1) pa_group_beg_indices_and_f_sizes = (unsigned int**)realloc(pa_group_beg_indices_and_f_sizes, (group_size + 9) * sizeof(unsigned int*));
				group_beg_indice_i = 0;
				pa_group_beg_indices_and_f_sizes[group_size-1] = (unsigned int*)malloc(2*sizeof(unsigned int));
			}
			else {
				fseek(file, -7, SEEK_CUR);
			}
		}
		else if(c_read == 'p') {
			printf("ERROR: obj model uses points, which isn't supported\n");
		}
		else if(c_read == 'l') {
			printf("ERROR: obj model uses a polyline, which isn't supported\n");
		}
		if(c_read != '\n') fgets(str_read,100,file);
	}
	rewind(file);

	IndicedVertex** pa_vert = (IndicedVertex**)malloc(v_size * sizeof(IndicedVertex*));
	Vec3* normals = (Vec3*)malloc(vn_size * sizeof(Vec3));
	Vec2* texcoords = (Vec2*)malloc(vt_size * sizeof(Vec2));
	initialize_pa_vert(pa_vert, normals, texcoords, v_size, file, end_i);
	
	Model* new_m = (Model*)malloc(sizeof(Model));
    	glGenBuffers(1, &new_m->VBO);
	new_m->pa_meshes = (Mesh**)malloc(group_size * sizeof(Mesh*));
	new_m->size = group_size;

	unsigned int old_v_size = v_size;
	if(vn_size == 0 && vt_size == 0) {
		for(unsigned int i = 0; i < group_size; i++) {
			generate_mesh_minimal(new_m->pa_meshes+i, pa_vert, v_size, new_m->VBO, file, pa_group_beg_indices_and_f_sizes[i]);
			free(pa_group_beg_indices_and_f_sizes[i]);
		}
	}
	else if(vt_size == 0) {
		for(unsigned int i = 0; i < group_size; i++) {
			v_size = generate_mesh_given_normals(new_m->pa_meshes+i, pa_vert, normals, v_size, new_m->VBO, file, pa_group_beg_indices_and_f_sizes[i]);
			free(pa_group_beg_indices_and_f_sizes[i]);
		}
	}
	else if(vn_size == 0) {
		for(unsigned int i = 0; i < group_size; i++) {
			v_size = generate_mesh_given_texcoords(new_m->pa_meshes+i, pa_vert, texcoords, v_size, new_m->VBO, file, pa_group_beg_indices_and_f_sizes[i]);
			free(pa_group_beg_indices_and_f_sizes[i]);
		}
	}
	else {
		for(unsigned int i = 0; i < group_size; i++) {
			v_size = generate_mesh(new_m->pa_meshes+i, pa_vert, normals, texcoords, v_size, new_m->VBO, file, pa_group_beg_indices_and_f_sizes[i]);
			free(pa_group_beg_indices_and_f_sizes[i]);
		}
	}
	fclose(file);
	
	new_m->vertices = (Vertex*)malloc(v_size * sizeof(Vertex));
	if(vt_size == 0 && vn_size == 0){
		for(unsigned int i = 0; i < v_size; i++) {
			// since the indice of pa_vert[i][0] is = to i,
			// pa_vert[i][0].i is instead used to store sum
			// of every instance of pa_vert[i][*] in indices
			// used to calculate mean normal approximation
			new_m->vertices[i] = pa_vert[i][0].v;
			unsigned int sum = pa_vert[i][0].i;
			new_m->vertices[i].normal.x /= sum;
			new_m->vertices[i].normal.y /= sum;
			new_m->vertices[i].normal.z /= sum;
			free(pa_vert[i]);
		}
	}
	else if(vn_size == 0) {
		for(unsigned int i = 0; i < old_v_size; i++) {
			new_m->vertices[i] = pa_vert[i][0].v;
			// sum is encoded in texcoords.x as
			// texcoords.x + sum * 10
			unsigned int sum = floor(new_m->vertices[i].texcoords.x / 10);
			
			new_m->vertices[i].texcoords.x -= 10 * sum;
			new_m->vertices[i].normal.x /= sum;
			new_m->vertices[i].normal.y /= sum;
			new_m->vertices[i].normal.z /= sum;
			for(unsigned int j = 1; j < pa_vert[i][0].i; j++) {
				new_m->vertices[pa_vert[i][j].i] = pa_vert[i][j].v;
				sum = floor(new_m->vertices[pa_vert[i][j].i].texcoords.x / 10);
				new_m->vertices[pa_vert[i][j].i].texcoords.x -= 10 * sum;
				new_m->vertices[pa_vert[i][j].i].normal.x /= sum;
				new_m->vertices[pa_vert[i][j].i].normal.y /= sum;
				new_m->vertices[pa_vert[i][j].i].normal.z /= sum;
			}
		}
	}
	else {
		for(unsigned int i = 0; i < old_v_size; i++) {
			// since the indice of pa_vert[i][0] is = to i,
			// pa_vert[i][0].i is instead used to store len
			new_m->vertices[i] = pa_vert[i][0].v;
			for(unsigned int j = 1; j < pa_vert[i][0].i; j++) {
				new_m->vertices[pa_vert[i][j].i] = pa_vert[i][j].v;
			}
			free(pa_vert[i]);
		}
	}
	free(pa_vert);

    	glBindBuffer(GL_ARRAY_BUFFER, new_m->VBO);
    	glBufferData(GL_ARRAY_BUFFER, v_size * sizeof(Vertex), new_m->vertices, GL_STATIC_DRAW);

	return new_m;
}

void render_model(Model* m) {
	for(unsigned int i = 0; i < m->size; i++) {
		render_mesh(m->pa_meshes[i]);
	}
}

void free_model(Model* m) {
	free(m->vertices);
	glDeleteBuffers(1, &m->VBO);
	for(unsigned int i = 0; i < m->size; i++) {
		free_mesh(m->pa_meshes[i]);
	}
	free(m);
}
