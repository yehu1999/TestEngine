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
	//模型数据
	vector<Texture> textures_loaded;	//已经加载的纹理(不重复加载)
	vector<Mesh>    meshes;
	string directory;
	bool gammaCorrection;

	//函数
	Model() {}
	Model(string const& path, bool gamma = false) : gammaCorrection(gamma)
	{
		loadModel(path);
	}
	void Draw(Shader shader);
	void Destroy();
private:
	//函数
	void loadModel(string const& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);

};


//函数实现-------------------------------------------------------------------------------------


void Model::Draw(Shader shader)
{
	//遍历所有网格，并调用它们各自的Draw函数
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
	//加载模型
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	//后期处理(Post-processing)选项
	//aiProcess_Triangulate：如果模型不是全部由三角形组成，则转换为三角形
	//aiProcess_FlipUVs：翻转y轴的纹理坐标
	//aiProcess_GenNormals：如果模型不包含法向量的话，就为每个顶点创建法线。
	//aiProcess_SplitLargeMeshes：将比较大的网格分割成更小的子网格，如果你的渲染有最大顶点数限制，只能渲染较小的网格，那么它会非常有用。
	//aiProcess_OptimizeMeshes：和上个选项相反，它会将多个小网格拼接为一个大的网格，减少绘制调用从而进行优化。
	//... http://assimp.sourceforge.net/lib_html/postprocess_8h.html

	//检查场景和其根节点不为null，并且检查了它的一个标记(Flag)，来查看返回的数据的完整性。
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	//获取了文件路径的目录路径(截短文件名，获取文件所在目录)
	directory = path.substr(0, path.find_last_of('/'));

	//处理场景中的所有节点(从根节点开始)
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// 处理节点所有的网格（如果有的话）
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		//获取网格的索引并进行处理，返回我们自己的Mesh对象并存进我们的Model对象中
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// 接下来对它的子节点重复这一过程
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	//总的来说就是通过访问Aissimp的数据结构，将得到的数据整合为Model对象

	//用于生成Model对象的数据
	vector<Vertex> vertices;               //顶点数据
	vector<unsigned int> indices;          //索引数据 
	vector<Texture> textures;              //纹理数据

	//处理顶点
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		//处理顶点位置
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		//处理顶点法线
		if (mesh->HasNormals())    //如果网格包含法线
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}

		//处理纹理坐标
		if (mesh->mTextureCoords[0]) //如果网格包含纹理坐标
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

		//整合vertices顶点数据
		vertices.push_back(vertex);
	}

	// 处理索引
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// 处理材质
	if (mesh->mMaterialIndex >= 0)//如果网格包含材质
	{
		//获取材质
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

	//返回处理得到的Mesh对象
	return Mesh(vertices, indices, textures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)//通过GetTextureCount函数检查储存在材质中纹理的数量
	{
		//获取纹理文件地址
		aiString str;
		mat->GetTexture(type, i, &str);//使用GetTexture获取每个纹理的文件，它会将结果储存在一个aiString中

		//检测纹理是否已经加载过了
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			//通过比较已加载过的纹理地址与当前纹理是否相同
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{   //加载纹理
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
	//相当于之前的loadTexture函数

	//通过目录和文件名组合得到纹理文件地址
	string filename = string(path);
	filename = directory + '/' + filename;

	//创建纹理对象
	unsigned int textureID;
	glGenTextures(1, &textureID);

	//加载图像
	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	//检测是否正常读取到图像信息
	if (data)
	{
		//判断图像读取格式
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

		//绑定纹理对象至纹理目标
		glBindTexture(GL_TEXTURE_2D, textureID);
		//生成纹理
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		//生成MipMap
		glGenerateMipmap(GL_TEXTURE_2D);

		//为当前绑定的纹理对象设置环绕、过滤方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//释放图像内存
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