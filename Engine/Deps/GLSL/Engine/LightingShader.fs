#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

//��Դ
struct Light 
{
    int type;                  // 0: Point, 1: Directional, 2: Spot

    vec3 position;
    vec3 direction;            //Directional specific

	//���ղ���
    vec3 diffuse;              //or Color
    vec3 specular;

	//����˥������
	float constant;
    float linear;
    float quadratic;

	//�۹����
    float cutOff;               //���䷶Χ���нǵ�cosֵ
    float outerCutOff;          //���䷶Χ���нǵ�cosֵ(�����Ե)

    //������뾶
     float Radius;
};

//�����Դ��Ϣ
const int MAX_LIGHTSNUM = 50;               //����Դ����
uniform int LightsNum;                      //��ǰ��Դ����
uniform Light lights[MAX_LIGHTSNUM];        //��Դ�б�

//������� 
uniform int  renderMode;                    //��Ⱦģʽ    //0:Phong  1:PBR
uniform vec3 camPos;                        //�����λ��
uniform vec3 Ambient;                       //����������ɫ����
uniform float AmbientIntensity;             //��������ǿ��

//G���弸����ϢMap
uniform sampler2D gPosition;                //G����Ƭ������
uniform sampler2D gNormal;                  //G���巨������
uniform sampler2D gColorSpec;               //G����Phong��ɫ����ֵ
uniform sampler2D gShininess;               //G����Phong�����

uniform sampler2D gAlbedo;                  //G����PBR��ɫ��Ϣ
uniform sampler2D gMetRougAo;               //G����PBR�����ȴֲڶȻ����ڱ���Ϣ

//IBL
uniform samplerCube irradianceMap;          //���նȻ�����ͼ
uniform samplerCube prefilterMap;           //Ԥ�˲�������ͼ
uniform sampler2D   brdfLUT;                //BRDF������ͼ

//��ѧ����
const float PI = 3.14159265359;

//PBR----------------
//���߷ֲ�����
float DistributionGGX(vec3 N, vec3 H, float roughness);
//�����ڱκ���
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
//����������
vec3 fresnelSchlick(float cosTheta, vec3 F0);
//���������̣����ֲڶȣ�
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

//Phong����-------------
vec4 CalcPhong(vec3 Normal ,vec3 FragPos, vec3 Diffuse, float Specular, float Shininess);
vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir, vec3 Diffuse, float Specular, float Shininess);//�����
vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, float Shininess);//���Դ
vec3 CalcSpotLight(Light light, vec3 Normal, vec3 FragPos, vec3 viewDir, vec3 Diffuse, float Specular, float Shininess);//�۹�


//������
void main()
{
    //retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;

    vec3 Diffuse;
    float Specular;
    float Shininess;

    if(renderMode == 0) //Phong
    {
        Diffuse = texture(gColorSpec, TexCoords).rgb;
        Specular = texture(gColorSpec, TexCoords).a;
        Shininess = texture(gShininess, TexCoords).r;

        FragColor = CalcPhong(Normal ,FragPos, Diffuse, Specular, Shininess);
    }
    else if(renderMode == 1) //PBR����Ҫ��g�����������Ӧalbedo�Ƚ������
    {
        vec3 albedo = pow(texture(gAlbedo, TexCoords).rgb, vec3(2.2));  //gamma����
        //vec3 albedo = texture(gAlbedo, TexCoords).rgb;
        float metallic = texture(gMetRougAo, TexCoords).r;
        float roughness = texture(gMetRougAo, TexCoords).g;
        float ao = texture(gMetRougAo, TexCoords).b;

        vec3 N = normalize(Normal);
        vec3 V = normalize(camPos - FragPos);
        vec3 R = reflect(-V, N); 

        //������������ʣ���������ʱ�ķ����ʣ�
        vec3 F0 = vec3(0.04);             //�ǽ���ʹ��0.04����������(��ɫ)
        F0 = mix(F0, albedo, metallic);   //��������ݽ����ȶ�F0��albedo���в�ֵ�õ���������ɫ

        //�����ʷ���
        vec3 Lo = vec3(0.0);
        for(int i = 0; i < LightsNum; ++i) 
        {
            //����ÿ�����ߵķ�����
            vec3 L = normalize(lights[i].position - FragPos);
            vec3 H = normalize(V + L);
            float distance = length(lights[i].position - FragPos);
            float attenuation = 1.0 / (distance * distance);
            vec3 radiance = lights[i].diffuse * attenuation;

            //Cook-Torrance BRDF
            float NDF = DistributionGGX(N, H, roughness);   
            float G   = GeometrySmith(N, V, L, roughness);      
            vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
           
            vec3 numerator    = NDF * G * F; 
            float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
            vec3 specular = numerator / denominator;
        
            //kS(���淴����)����Fresnel
            vec3 kS = F;
            //kD(��������)����1 - kS �������غ㣩
            vec3 kD = vec3(1.0) - kS;
            //���ƽ�����   (������Ϊ1����������)
            kD *= 1.0 - metallic;	  

            // scale light by NdotL
            float NdotL = max(dot(N, L), 0.0);        

            //���շ�����radiance Lo
            Lo += (kD * albedo / PI + specular) * radiance * NdotL; 
            //��ע�⣬�����Ѿ��� BRDF ���Է����� ��kS����������ǲ����ٴγ��� kS
        }   
    
        // ambient lighting (we now use IBL as the ambient term)
        vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
        vec3 kS = F;
        vec3 kD = 1.0 - kS;
        kD *= 1.0 - metallic;
        vec3 irradiance = texture(irradianceMap, N).rgb;
        vec3 diffuse      = irradiance * albedo;

        // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
        const float MAX_REFLECTION_LOD = 4.0;
        vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
        vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
        vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

        vec3 ambient = (kD * diffuse + specular) * ao;

        vec3 color = ambient + Lo;

        //HDRɫ��ӳ��
        color = color / (color + vec3(1.0));
        //gammaУ��
        color = pow(color, vec3(1.0/2.2)); 

        FragColor = vec4(color, 1.0);
    }
}

//PBR----------
//���߷ֲ�����
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
//�����ڱκ���
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
//����������
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 2.0);
}
//���������̣����ֲڶȣ�
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 2.0);
} 

//Phong---------
vec4 CalcPhong(vec3 Normal ,vec3 FragPos, vec3 Diffuse, float Specular, float Shininess)
{
    vec3 lighting  = AmbientIntensity * Ambient * Diffuse; //������
    vec3 viewDir  = normalize(camPos - FragPos);

    for(int i = 0; i < LightsNum; ++i)
    {
        if(lights[i].type == 0)  //���
        {
            float distance = length(lights[i].position - FragPos);
            if(distance < lights[i].Radius) //�����������Ƭ�β��������(ʵ���ϸ�if��䲻������)
            {
                // diffuse
                vec3 lightDir = normalize(lights[i].position - FragPos);
                vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].diffuse;
                // specular
                vec3 halfwayDir = normalize(lightDir + viewDir);  
                //float spec = pow(max(dot(Normal, halfwayDir), 0.0), Shininess);
                float spec = pow(max(dot(reflect(-lightDir, Normal), viewDir), 0.0), Shininess);

                vec3 specular = lights[i].diffuse * spec * Specular;
                // attenuation
                float attenuation = 1.0 / (1.0 + lights[i].linear * distance + lights[i].quadratic * distance * distance);
                diffuse *= attenuation;
                specular *= attenuation;
                lighting += (diffuse + specular);   
            }
        }
        else if(lights[i].type == 1)  //�����
        {
            lighting += CalcDirLight(lights[i], Normal, viewDir, Diffuse, Specular, Shininess);
        }
        else if(lights[i].type == 2)  //�۹�
        {
            lighting += CalcSpotLight(lights[i], Normal, FragPos, viewDir, Diffuse, Specular, Shininess);
        } 
    }

    return vec4(lighting, 1.0);
}
vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir, vec3 Diffuse, float Specular, float Shininess)
{
	vec3 lightDir = normalize(-light.direction);
	// ��������ɫ
	float diff = max(dot(normal, lightDir), 0.0);
	// �������ɫ
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);
	// �ϲ����
	vec3 diffuse  = light.diffuse  * diff * Diffuse;
	vec3 specular = light.specular * spec * Specular;
	return (diffuse + specular);
}
vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 Diffuse, float Specular, float Shininess)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// ��������ɫ
	float diff = max(dot(normal, lightDir), 0.0);
	// �������ɫ
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);
	// ˥��
	float distance    = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance +
					light.quadratic * (distance * distance));
	// �ϲ����
	vec3 diffuse  = light.diffuse  * diff * Diffuse;
	vec3 specular = light.specular * spec * Specular;

	diffuse  *= attenuation;
	specular *= attenuation;

	return ( diffuse + specular);
}
vec3 CalcSpotLight(Light light, vec3 Normal, vec3 FragPos, vec3 viewDir, vec3 Diffuse, float Specular, float Shininess)
{
	vec3 lightDir = normalize(light.position - FragPos);//���߷�������

	//������
	vec3 norm = normalize(Normal);//��������
	float diff = max(dot(norm, lightDir), 0.0f);//���diffֵ�������γɵļн����
	vec3 diffuse  = light.diffuse  * diff * Diffuse;
	//���淴��
	vec3 reflectDir = reflect(-lightDir, norm);//���߷��䷽������
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), Shininess);//ָ���Ǹ߹�ķ����(Shininess)
	vec3 specular = light.specular * spec * Specular;
	//�ⷢ��
	//vec3 emission = vec3(0.5f) * vec3(texture(material.emissionMap, TexCoords));

	//�۹�����Լ��
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon   = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	diffuse  *= intensity;
	specular *= intensity;

	//����˥��
	float distance    = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
	diffuse  *= attenuation;
	specular *= attenuation;
		
	return (diffuse + specular); //+ emission
}