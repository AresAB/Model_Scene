#pragma once

#include <assimp/cimport.h>        // Plain-C interface
//#include <assimp/cfileio.h>
//#include <assimp/defs.h>
#include <assimp/scene.h>          // Output data structure
#include <assimp/postprocess.h>    // Post processing flags
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

typedef struct Tex {
	unsigned int id;
	char* type;
	char* path;
} Tex;

typedef struct Mesh {
	Vertex* vertices;
	unsigned int* indices;
	Tex* textures;
	unsigned int num_indices;
	unsigned int num_textures;
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
} Mesh;

void load_material(Tex* textures, unsigned int* offset, aiMaterial* mat, aiTextureType type, char* type_name) {
	unsigned int tex_count = aiGetMaterialTextureCount(mat, type);
	for(unsigned int i = 0; i < tex_count; i++) {
		aiString path;
		aiGetMaterialTexture(mat, type, i, &path);
		textures[*offset] = {
			.id = load_texture((char*)path.C_Str()),
			.type = type_name,
			.path = (char*)path.C_Str()
		};
		(*offset)++;
	}
}

void release_mesh(Mesh* m) {
	glDeleteVertexArrays(1, &(m->VAO));
	glDeleteBuffers(1, &(m->VBO));
	glDeleteBuffers(1, &(m->EBO));
	free(m->vertices);
	free(m->indices);
	free(m->textures);
	free(m);
}

Mesh* load_mesh(aiMesh* m, aiScene* scene) {
	Mesh* mesh = (Mesh*)malloc(sizeof(Mesh));
	mesh->vertices = (Vertex*)malloc(m->mNumVertices * sizeof(Vertex));
	if(m->mTextureCoords[0]) {
		for(unsigned int i = 0; i < m->mNumVertices; i++) {
			mesh->vertices[i] = {
				.position = {
					m->mVertices[i].x,
					m->mVertices[i].y,
					m->mVertices[i].z
				},
				.normal = {
					m->mNormals[i].x,
					m->mNormals[i].y,
					m->mNormals[i].z
				},
				.tex_u = m->mTextureCoords[0][i].x,
				.tex_v = m->mTextureCoords[0][i].y
			};
		}
	}
	else {
		for(unsigned int i = 0; i < m->mNumVertices; i++) {
			mesh->vertices[i] = {
				.position = {
					m->mVertices[i].x,
					m->mVertices[i].y,
					m->mVertices[i].z
				},
				.normal = {
					m->mNormals[i].x,
					m->mNormals[i].y,
					m->mNormals[i].z
				},
				.tex_u = 0,
				.tex_v = 0
			};
		}
	}

	unsigned int indices_size = 0;
	for(unsigned int i = 0; i < m->mNumFaces; i++) {
		indices_size += m->mFaces[i].mNumIndices;
	}
	mesh->indices = (unsigned int*)malloc(indices_size * sizeof(Vec3));
	mesh->num_indices = indices_size + 1;
	unsigned int indice_i = 0;
	for(unsigned int i = 0; i < m->mNumFaces; i++) {
		aiFace face = m->mFaces[i];
		for(unsigned int j = 0; j < face.mNumIndices; j++) {
			mesh->indices[indice_i] = face.mIndices[j];
			indice_i++;
		}
	}
	char diff[] = "texture_diffuse";
	char spec[] = "texture_specular";
	if(m->mMaterialIndex >= 0) {
		aiMaterial* mat = scene->mMaterials[m->mMaterialIndex];
		unsigned int tex_i = 0;
		load_material(mesh->textures, &tex_i, mat, aiTextureType_DIFFUSE, diff);
		load_material(mesh->textures, &tex_i, mat, aiTextureType_DIFFUSE, spec);
		mesh->num_textures = tex_i + 1;
	}

	glGenVertexArrays(1, &(mesh->VAO));
	glGenBuffers(1, &(mesh->VBO));
	glGenBuffers(1, &(mesh->EBO));

	glBindVertexArray(mesh->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
	glBufferData(GL_ARRAY_BUFFER, m->mNumVertices, mesh->vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->EBO);
	glBufferData(GL_ARRAY_BUFFER, indices_size, mesh->indices, GL_STATIC_DRAW);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	// normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture uv
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
	
	//return mesh;
	release_mesh(mesh); // remove once done testing
	return NULL;
}

void render_mesh(Mesh m, unsigned int shader) {
	unsigned int diffuse_i = 0;
	unsigned int specular_i = 0;
	for(unsigned int i = 0; i < m.num_textures; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		unsigned int suffix;
		if(strcmp(m.textures[i].type, "texture_diffuse")) {
			suffix = diffuse_i;
			diffuse_i++;
		}
		else if(strcmp(m.textures[i].type, "texture_specular")) {
			suffix = specular_i;
			specular_i++;
		}
		char suf[3];
		sprintf(suf, "%u", suffix);
		char destination[50] = "material.";
		strcat(destination, m.textures[i].type);
		strcat(destination, suf);
		shader_set_int(shader, destination, i);
		glBindTexture(GL_TEXTURE_2D, m.textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(m.VAO);
	glDrawElements(GL_TRIANGLES, m.num_indices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

typedef struct Model {
	Mesh** meshes;
	unsigned int size;
} Model;

void release_model(Model* m) {
	for(unsigned int i = 0; i < m->size; i++) {
		release_mesh(m->meshes[i]);
	}
	free(m->meshes);
	free(m);
}

Model* load_model(char* filepath) {
	// Assimp loads files in right-hand coordinate system
	// If you need left-hand, there's a flag for that
	const struct aiScene* scene = aiImportFile(filepath,
		aiProcess_Triangulate | // convert all primitives to tri
		aiProcess_GenNormals | // gens normals if none
		aiProcess_FlipUVs | // so we don't have to for every tex
		aiProcess_OptimizeMeshes); // tries to join meshes
	if(scene == NULL || !scene->mRootNode) {
		printf("MODEL LOADING ERROR: ");
		printf(aiGetErrorString());
		return NULL;
	}
	Model* mod = (Model*)malloc(sizeof(Model));
	mod->size = 0;
	mod->meshes = (Mesh**)malloc(sizeof(Mesh) * 10);
	
	aiReleaseImport(scene);
	//return mod;
	release_model(mod);
	return NULL;
}

void render_model(Model m, unsigned int shader) {
	for(unsigned int i = 0; i < m.size; i++) {
		render_mesh(*(m.meshes[i]), shader);
	}
}
