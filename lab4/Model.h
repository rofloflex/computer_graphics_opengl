#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

#include <string>
#include <iostream>
#include <vector>

using namespace std;

class Model
{
public:
    vector<Mesh> meshes;
    string directory;

    Model(const string& path)
    {
        loadModel(path);
    }

    int findMeshIndexByName(const string& targetName)
    {
        for (int i = 0; i < (int)meshes.size(); i++)
        {
            if (meshes[i].name == targetName)
                return i;
        }
        return -1;
    }

    void drawMeshWithMatrix(int meshIndex, GLuint shaderProgram, const glm::mat4& modelMatrix)
    {
        if (meshIndex < 0 || meshIndex >= (int)meshes.size())
            return;

        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "model"),
            1,
            GL_FALSE,
            glm::value_ptr(modelMatrix)
        );

        meshes[meshIndex].Draw();
    }

    void Draw(GLuint shaderProgram,
        float moveZ,
        float rotateYDeg,
        float moveX,
        float moveY)
    {
        int baseIndex = findMeshIndexByName("Cube");
        int partZIndex = findMeshIndexByName("Cube.001");
        int rotateIndex = findMeshIndexByName("Cube.003");
        int moveXIndex = findMeshIndexByName("Cube.002");
        int partYIndex = findMeshIndexByName("Cylinder.001");

        glm::mat4 identity = glm::mat4(1.0f);

        // Cube Ч основа, неподвижна
        glm::mat4 modelBase = identity;

        // Cube.001 Ч движение по Z
        glm::mat4 modelPartZ = identity;
        modelPartZ = glm::translate(modelPartZ, glm::vec3(0.0f, 0.0f, moveZ));

        // Cube.003 Ч поворот влево-вправо вокруг оси Y
        // “очку вращени€ при необходимости можно потом чуть уточнить
        glm::vec3 pivotRotate(0.0f, 0.5868775f, -0.10051175f);

        glm::mat4 modelRotate = modelPartZ;
        modelRotate = glm::translate(modelRotate, pivotRotate);
        modelRotate = glm::rotate(modelRotate, glm::radians(rotateYDeg), glm::vec3(0.0f, 1.0f, 0.0f));
        modelRotate = glm::translate(modelRotate, -pivotRotate);

        // Cube.002 Ч движение по X
        glm::mat4 modelMoveX = modelRotate;
        modelMoveX = glm::translate(modelMoveX, glm::vec3(moveX, 0.0f, 0.0f));

        // Cylinder.001 Ч движение по Y
        glm::mat4 modelPartY = modelMoveX;
        modelPartY = glm::translate(modelPartY, glm::vec3(0.0f, moveY, 0.0f));

        drawMeshWithMatrix(baseIndex, shaderProgram, modelBase);
        drawMeshWithMatrix(partZIndex, shaderProgram, modelPartZ);
        drawMeshWithMatrix(rotateIndex, shaderProgram, modelRotate);
        drawMeshWithMatrix(moveXIndex, shaderProgram, modelMoveX);
        drawMeshWithMatrix(partYIndex, shaderProgram, modelPartY);
    }

private:
    void loadModel(const string& path)
    {
        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(
            path,
            aiProcess_Triangulate | aiProcess_GenNormals
        );

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }

        directory = path.substr(0, path.find_last_of("/\\"));
        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode* node, const aiScene* scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        vector<Vertex> vertices;
        vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;

            vertex.Position.x = mesh->mVertices[i].x;
            vertex.Position.y = mesh->mVertices[i].y;
            vertex.Position.z = mesh->mVertices[i].z;

            if (mesh->HasNormals())
            {
                vertex.Normal.x = mesh->mNormals[i].x;
                vertex.Normal.y = mesh->mNormals[i].y;
                vertex.Normal.z = mesh->mNormals[i].z;
            }
            else
            {
                vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        string meshName = mesh->mName.C_Str();
        return Mesh(vertices, indices, meshName);
    }
};

#endif