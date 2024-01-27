#ifndef MESH_H
#define MESH_H
#include<mythings/shader.h>
#include<vector>
#include<string>

using std::vector;
using std::string;

#define MAX_BONE_INFLUENCE 4 //最大骨骼影响：可以分配给每个顶点的最大骨骼数量。

struct Vertex    //顶点
{
	glm::vec3 Position;      //位置
	glm::vec3 Normal;        //法线
	glm::vec2 TexCoords;     //纹理坐标
	glm::vec3 Tangent;       //切线
	glm::vec3 Bitangent;     //副切线(也被叫作副法线(Binormal),同时垂直于由法线与切线的向量，可以由法线与切线的叉积计算得出)
	int m_BoneIDs[MAX_BONE_INFLUENCE];  //骨骼索引
	float m_Weights[MAX_BONE_INFLUENCE];//骨骼重量
};

struct Texture   //纹理
{
	unsigned int id;          //纹理id
	string type;              //纹理类型
	string path;              //文件地址
};

class Mesh
{
public:
	//网格数据
	vector<Vertex>       vertices;
	vector<unsigned int> indices;
	vector<Texture>      textures;
	unsigned int VAO;
	//函数
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
	void Draw(Shader shader);
private:
	//渲染数据
	unsigned int VBO, EBO;
	//函数
	void setupMesh();
};

//构造函数
Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setupMesh();  //初始化
}
//初始化网格
void Mesh::setupMesh()
{
	//创建
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	//绑定
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//上传数据
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	//索引
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	//设置顶点属性
	// 顶点位置
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// 顶点法线
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// 顶点纹理坐标
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	// 顶点切线
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	// 顶点副切线
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
	// 骨骼索引
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
	// 骨骼重量
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

	//解绑
	glBindVertexArray(0);
}
//渲染(绘制)
void Mesh::Draw(Shader shader)
{
	//贴图遍历序号
	unsigned int diffuseNr = 1;   //漫反射贴图
	unsigned int specularNr = 1;  //镜面反射贴图
	unsigned int normalNr = 1;    //法线贴图
	unsigned int heightNr = 1;    //高度贴图(视差贴图)

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // 在绑定之前激活相应的纹理单元
		// 获取纹理序号（diffuse_textureN 中的 N）
		string number;
		string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);
		else if (name == "texture_normal")
			number = std::to_string(normalNr++);
		else if (name == "texture_height")
			number = std::to_string(heightNr++);

		//纹理单元设置对应的采样器
		glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
		//绑定纹理
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	// 绘制网格
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//重置至纹理单元0
	glActiveTexture(GL_TEXTURE0);
}

#endif