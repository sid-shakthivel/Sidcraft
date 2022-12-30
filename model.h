#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

#include "matrix.h"
#include "stb_image.h"
#include "shader.h"
#include "mesh.h"
#include <vector>

class Model
{
private:
    std::vector<Mesh> Meshes;
    std::vector<Texture> LoadedTextures; // Allows reuse of textures which have already been loaded

    void ProcessNodes(aiNode *Node, const aiScene *Scene)
    {
        // Process nodes of mesh

        // std::cout << Node->mNumMeshes << std::endl;
        // std::cout << Node->mNumChildren << std::endl;

        for (unsigned int i = 0; i < Node->mNumMeshes; i++)
        {
            aiMesh *mesh = Scene->mMeshes[Node->mMeshes[i]];
            Meshes.push_back(ProcessMesh(mesh, Scene));
        }

        // Process nodes of children
        for (unsigned int i = 0; i < Node->mNumChildren; i++)
        {
            ProcessNodes(Node->mChildren[i], Scene);
        }
    }

    // If is_rgb = false, assumed it's a png
    unsigned int OldLoadTextureFromFile(const std::string &filepath, bool is_rgb)
    {
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);          // (s, t, r) refers to the coordinate axis
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);          // Mirrors content
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Set filtering method
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_set_flip_vertically_on_load(true);

        int width, height, nrChannels;
        unsigned char *data = stbi_load(("backpack/" + filepath).c_str(), &width, &height, &nrChannels, 0);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);

        return texture;
    }

    std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, TextureType textureType)
    {
        std::vector<Texture> Textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);

            bool skip = false;
            for (unsigned int j = 0; j < LoadedTextures.size(); j++)
            {
                if (std::strcmp(LoadedTextures[j].Path.data(), str.C_Str()) == 0)
                {
                    Textures.push_back(LoadedTextures[j]);
                    skip = true;
                    break;
                }
            }

            if (!skip)
            {
                Texture texture;
                texture.Id = OldLoadTextureFromFile(str.C_Str(), true);
                texture.Type = textureType;
                texture.Path = str.C_Str();
                Textures.push_back(texture);
                LoadedTextures.push_back(texture);
            }
        }
        return Textures;
    }

    Mesh ProcessMesh(aiMesh *AIMesh, const aiScene *Scene)
    {
        // std::cout << "new mesh" << std::endl;

        // Process vertices
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        for (unsigned int i = 0; i < AIMesh->mNumVertices; i++)
        {
            Vertex vertex;

            Vector3f Position; // Get position of vertex
            Position.x = AIMesh->mVertices[i].x;
            Position.y = AIMesh->mVertices[i].y;
            Position.z = AIMesh->mVertices[i].z;

            Vector3f Normal; // Get normal of vertex
            Normal.x = AIMesh->mNormals[i].x;
            Normal.y = AIMesh->mNormals[i].y;
            Normal.z = AIMesh->mNormals[i].z;

            Vector2f TextureCoordinates;

            if (AIMesh->mTextureCoords[0])
                TextureCoordinates = Vector2f(AIMesh->mTextureCoords[0][i].x, AIMesh->mTextureCoords[0][i].y);
            else
                std::cout << "NO TEXTURE COORDINATES" << std::endl;

            vertex.Position = Position;
            vertex.Normal = Normal;
            vertex.TextureCoordinates = TextureCoordinates;

            vertices.push_back(vertex);
        }

        // Process indices
        for (unsigned int i = 0; i < AIMesh->mNumFaces; i++)
        {
            auto const &face = AIMesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        // Process materials
        if (AIMesh->mMaterialIndex > 0)
        {
            aiMaterial *material = Scene->mMaterials[AIMesh->mMaterialIndex];
            std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::Diffuse);
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::Specular);
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }

        return Mesh(vertices, indices, textures);
    }

public:
    Model(const std::string &filepath)
    {
        Assimp::Importer Importer;
        // Triangulate to transform any primitive shape to triangle
        // FlipUV's to flip texture coordiantes
        const aiScene *Scene = Importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << Importer.GetErrorString() << std::endl;
            return;
        }

        ProcessNodes(Scene->mRootNode, Scene);
    }

    void Draw(Shader *MeshShader)
    {
        for (auto &mesh : Meshes)
        {
            mesh.Draw(MeshShader);
        }
    }
};