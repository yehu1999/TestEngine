#ifndef MESH_H
#define MESH_H
#include<mythings/shader.h>
#include<vector>
#include<string>

using std::vector;
using std::string;

#define MAX_BONE_INFLUENCE 4 //������Ӱ�죺���Է����ÿ�������������������

struct Vertex    //����
{
	glm::vec3 Position;      //λ��
	glm::vec3 Normal;        //����
	glm::vec2 TexCoords;     //��������
	glm::vec3 Tangent;       //����
	glm::vec3 Bitangent;     //������(Ҳ������������(Binormal),ͬʱ��ֱ���ɷ��������ߵ������������ɷ��������ߵĲ������ó�)
	int m_BoneIDs[MAX_BONE_INFLUENCE];  //��������
	float m_Weights[MAX_BONE_INFLUENCE];//��������
};

struct Texture   //����
{
	unsigned int id;          //����id
	string type;              //��������
	string path;              //�ļ���ַ
};

class Mesh
{
public:
	//��������
	vector<Vertex>       vertices;
	vector<unsigned int> indices;
	vector<Texture>      textures;
	unsigned int VAO;
	//����
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
	void Draw(Shader shader);
private:
	//��Ⱦ����
	unsigned int VBO, EBO;
	//����
	void setupMesh();
};

//���캯��
Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setupMesh();  //��ʼ��
}
//��ʼ������
void Mesh::setupMesh()
{
	//����
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	//��
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//�ϴ�����
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	//����
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	//���ö�������
	// ����λ��
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// ���㷨��
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// ������������
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	// ��������
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	// ���㸱����
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
	// ��������
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
	// ��������
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

	//���
	glBindVertexArray(0);
}
//��Ⱦ(����)
void Mesh::Draw(Shader shader)
{
	//��ͼ�������
	unsigned int diffuseNr = 1;   //��������ͼ
	unsigned int specularNr = 1;  //���淴����ͼ
	unsigned int normalNr = 1;    //������ͼ
	unsigned int heightNr = 1;    //�߶���ͼ(�Ӳ���ͼ)

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // �ڰ�֮ǰ������Ӧ������Ԫ
		// ��ȡ������ţ�diffuse_textureN �е� N��
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

		//����Ԫ���ö�Ӧ�Ĳ�����
		glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
		//������
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	// ��������
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//����������Ԫ0
	glActiveTexture(GL_TEXTURE0);
}

#endif