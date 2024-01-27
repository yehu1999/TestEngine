#version 330 core

//�����G��������
layout (location = 0) out vec3 gPosition;  //λ������
layout (location = 1) out vec3 gNormal;    //��������
//Phong
layout (location = 2) out vec4 gColorSpec; //��ɫ+���淴��ǿ������
layout (location = 3) out float gShininess; //�����
//PBR
layout (location = 4) out vec3 gAlbedo; 
layout (location = 5) out vec3 gMetRougAo; 


in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform bool isModel;                                  //�Ƿ�Ϊģ��   
uniform sampler2D texture_diffuse1, texture_specular1, texture_normal1; //ģ��������ɫ������������

struct Material
{
    int type; //0:Phong-Value 1:Phong-Map  2:PBR-Value 3:PBR-Map 4.Model-Phong 5.Model-PBR

    //Phong-Value
    vec3 diffuse;                 //������ֵ/Color
    float specular;               //����ֵ
   
    //Phongͨ�ñ���
    float shininess;              //����ȣ�Ӱ�쾵��߹��ɢ��/�뾶��

    //Phong-Map
	sampler2D diffuseMap;         //�����������ɫ����
	sampler2D specularMap;        //���淴�������ɫ����

    //PBR-Value
    vec3  albedo;
    float metallic;
    float roughness;
    float ao;

    //PBR-Map
    sampler2D albedoMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
    sampler2D aoMap;
    sampler2D normalMap;
};
uniform Material material;                             //������Ϣ

//��������ȡ
vec3 getNormalFromMap(sampler2D NormalMap);

//������-------------------------------------------------------------------------------------------------------------------------------
void main()
{           
    // �洢Ƭ��λ�õ�G����������
	gPosition = FragPos;
	// �洢Ƭ�η��ߵ�G����������
	gNormal = normalize(Normal);

	// �洢Ƭ����������ɫ/����ǿ�ȵ�G����������(��������Ĳ�����Ϣ��Lighting���ϴ�)
	if(isModel) //ģ��
	{
        if(material.type == 4)    //Model-Phong 
		{
            gColorSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
		    gColorSpec.a = texture(texture_specular1, TexCoords).r;
            gShininess = material.shininess; 
        }
        else if(material.type == 5)//Model-PBR
        {
            gAlbedo = texture(texture_diffuse1, TexCoords).rgb; 
            gNormal = getNormalFromMap(texture_normal1);
            gMetRougAo.r = material.metallic;
            gMetRougAo.g = material.roughness;
            gMetRougAo.b = material.ao;
        }
	}
	else        //��ģ��
	{
        if(material.type == 0)        //Phong-Value
        {
            gColorSpec.rgb = material.diffuse;
		    gColorSpec.a = material.specular;
            gShininess = material.shininess; 

        }
        else if(material.type == 1)  //Phong-Map
        {
            gColorSpec.rgb = texture(material.diffuseMap, TexCoords).rgb;
            gColorSpec.a = texture(material.specularMap, TexCoords).r;

        }
        else if(material.type == 2)  //PBR-Value
        {
            gAlbedo = material.albedo; 
            gMetRougAo.r = material.metallic;
            gMetRougAo.g = material.roughness;
            gMetRougAo.b = material.ao;

        }
        else if(material.type == 3)  //PBR-Map
        {
            gAlbedo = texture(material.albedoMap, TexCoords).rgb; 
            gMetRougAo.r = texture(material.metallicMap, TexCoords).r;
            gMetRougAo.g = texture(material.roughnessMap, TexCoords).r;
            gMetRougAo.b = texture(material.aoMap, TexCoords).r;

            gNormal = getNormalFromMap(material.normalMap);
        }

	}
}


//��������ȡ
vec3 getNormalFromMap(sampler2D NormalMap)
{
    // Easy trick to get tangent-normals to world-space to keep PBR code simplified.
    // Don't worry if you don't get what's going on; you generally want to do normal 
    // mapping the usual way for performance anyways; I do plan make a note of this 
    // technique somewhere later in the normal mapping tutorial.
    vec3 tangentNormal = texture(NormalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(FragPos);
    vec3 Q2  = dFdy(FragPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}