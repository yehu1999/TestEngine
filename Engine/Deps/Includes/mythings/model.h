#ifndef MODEL_H
#define MODEL_H

#include <mythings/_mesh.h>
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <fstream>
#include <sstream>
#include <map>

unsigned int TextureFromFile(const char* path, const string& directory, bool gammaCorrection = false);

class Model
{
public:
	//ģ������
	vector<Texture> textures_loaded;	//�Ѿ����ص�����(���ظ�����)
	vector<Mesh>    meshes;
	string directory;
	bool gammaCorrection;

	//����
	Model() {}
	Model(string const& path, bool gamma = false) : gammaCorrection(gamma)
	{
		loadModel(path);
	}
	void Draw(Shader shader);
	void Destroy();
private:
	//����
	void loadModel(string const& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);

};


//����ʵ��-------------------------------------------------------------------------------------


void Model::Draw(Shader shader)
{
	//�����������񣬲��������Ǹ��Ե�Draw����
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader);
}

void Model::Destroy()
{
	textures_loaded.clear();
	meshes.clear();
}

void Model::loadModel(string const& path)
{
	//����ģ��
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	//���ڴ���(Post-processing)ѡ��
	//aiProcess_Triangulate�����ģ�Ͳ���ȫ������������ɣ���ת��Ϊ������
	//aiProcess_FlipUVs����תy�����������
	//aiProcess_GenNormals�����ģ�Ͳ������������Ļ�����Ϊÿ�����㴴�����ߡ�
	//aiProcess_SplitLargeMeshes�����Ƚϴ������ָ�ɸ�С����������������Ⱦ����󶥵������ƣ�ֻ����Ⱦ��С��������ô����ǳ����á�
	//aiProcess_OptimizeMeshes�����ϸ�ѡ���෴�����Ὣ���С����ƴ��Ϊһ��������񣬼��ٻ��Ƶ��ôӶ������Ż���
	//... http://assimp.sourceforge.net/lib_html/postprocess_8h.html

	//��鳡��������ڵ㲻Ϊnull�����Ҽ��������һ�����(Flag)�����鿴���ص����ݵ������ԡ�
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	//��ȡ���ļ�·����Ŀ¼·��(�ض��ļ�������ȡ�ļ�����Ŀ¼)
	directory = path.substr(0, path.find_last_of('/'));

	//�������е����нڵ�(�Ӹ��ڵ㿪ʼ)
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// ����ڵ����е���������еĻ���
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		//��ȡ��������������д������������Լ���Mesh���󲢴�����ǵ�Model������
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// �������������ӽڵ��ظ���һ����
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	//�ܵ���˵����ͨ������Aissimp�����ݽṹ�����õ�����������ΪModel����

	//��������Model���������
	vector<Vertex> vertices;               //��������
	vector<unsigned int> indices;          //�������� 
	vector<Texture> textures;              //��������

	//������
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		//������λ��
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		//�����㷨��
		if (mesh->HasNormals())    //��������������
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}

		//������������
		if (mesh->mTextureCoords[0]) //������������������
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		//����vertices��������
		vertices.push_back(vertex);
	}

	// ��������
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// �������
	if (mesh->mMaterialIndex >= 0)//��������������
	{
		//��ȡ����
		aiMaterial* material =
			scene->mMaterials[mesh->mMaterialIndex];
		// 1. diffuse maps
		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}

	//���ش���õ���Mesh����
	return Mesh(vertices, indices, textures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)//ͨ��GetTextureCount������鴢���ڲ��������������
	{
		//��ȡ�����ļ���ַ
		aiString str;
		mat->GetTexture(type, i, &str);//ʹ��GetTexture��ȡÿ��������ļ������Ὣ���������һ��aiString��

		//��������Ƿ��Ѿ����ع���
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			//ͨ���Ƚ��Ѽ��ع��������ַ�뵱ǰ�����Ƿ���ͬ
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{   //��������
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory, gammaCorrection);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
		}
	}
	return textures;
}

unsigned int TextureFromFile(const char* path, const string& directory, bool gammaCorrection)
{
	//�൱��֮ǰ��loadTexture����

	//ͨ��Ŀ¼���ļ�����ϵõ������ļ���ַ
	string filename = string(path);
	filename = directory + '/' + filename;

	//�����������
	unsigned int textureID;
	glGenTextures(1, &textureID);

	//����ͼ��
	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	//����Ƿ�������ȡ��ͼ����Ϣ
	if (data)
	{
		//�ж�ͼ���ȡ��ʽ
		GLenum internalFormat;
		GLenum dataFormat;
		if (nrComponents == 1)
		{
			internalFormat = dataFormat = GL_RED;
		}
		else if (nrComponents == 3)
		{
			internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
			dataFormat = GL_RGB;
		}
		else if (nrComponents == 4)
		{
			internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
			dataFormat = GL_RGBA;
		}

		//���������������Ŀ��
		glBindTexture(GL_TEXTURE_2D, textureID);
		//��������
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		//����MipMap
		glGenerateMipmap(GL_TEXTURE_2D);

		//Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//�ͷ�ͼ���ڴ�
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << filename << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

#endif