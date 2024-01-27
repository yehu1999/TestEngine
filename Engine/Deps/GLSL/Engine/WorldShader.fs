#version 330 core

//输出的G缓冲纹理
layout (location = 0) out vec3 gPosition;  //位置纹理
layout (location = 1) out vec3 gNormal;    //法线纹理
//Phong
layout (location = 2) out vec4 gColorSpec; //颜色+镜面反射强度纹理
layout (location = 3) out float gShininess; //反光度
//PBR
layout (location = 4) out vec3 gAlbedo; 
layout (location = 5) out vec3 gMetRougAo; 


in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform bool isModel;                                  //是否为模型   
uniform sampler2D texture_diffuse1, texture_specular1, texture_normal1; //模型物体颜色纹理，镜面纹理

struct Material
{
    int type; //0:Phong-Value 1:Phong-Map  2:PBR-Value 3:PBR-Map 4.Model-Phong 5.Model-PBR

    //Phong-Value
    vec3 diffuse;                 //漫反射值/Color
    float specular;               //镜面值
   
    //Phong通用变量
    float shininess;              //反光度：影响镜面高光的散射/半径。

    //Phong-Map
	sampler2D diffuseMap;         //漫反射光照颜色分量
	sampler2D specularMap;        //镜面反射光照颜色分量

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
uniform Material material;                             //材质信息

//法向量获取
vec3 getNormalFromMap(sampler2D NormalMap);

//主函数-------------------------------------------------------------------------------------------------------------------------------
void main()
{           
    // 存储片段位置到G缓冲纹理中
	gPosition = FragPos;
	// 存储片段法线到G缓冲纹理中
	gNormal = normalize(Normal);

	// 存储片段漫反射颜色/镜面强度到G缓冲纹理中(基本物体的材质信息在Lighting中上传)
	if(isModel) //模型
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
	else        //非模型
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


//法向量获取
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