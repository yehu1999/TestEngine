#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

//光源
struct Light 
{
    int type;                  // 0: Point, 1: Directional, 2: Spot

    vec3 position;
    vec3 direction;            //Directional specific

	//光照参数
    vec3 diffuse;              //or Color
    vec3 specular;

	//光线衰弱参数
	float constant;
    float linear;
    float quadratic;

	//聚光参数
    float cutOff;               //照射范围内切角的cos值
    float outerCutOff;          //照射范围外切角的cos值(渐变边缘)

    //光体积半径
     float Radius;
};

//传入光源信息
const int MAX_LIGHTSNUM = 50;               //最大光源数量
uniform int LightsNum;                      //当前光源数量
uniform Light lights[MAX_LIGHTSNUM];        //光源列表

//传入变量 
uniform int  renderMode;                    //渲染模式    //0:Phong  1:PBR
uniform vec3 camPos;                        //摄像机位置
uniform vec3 Ambient;                       //环境光照颜色分量
uniform float AmbientIntensity;             //环境光照强度

//G缓冲几何信息Map
uniform sampler2D gPosition;                //G缓冲片段坐标
uniform sampler2D gNormal;                  //G缓冲法线纹理
uniform sampler2D gColorSpec;               //G缓冲Phong颜色镜面值
uniform sampler2D gShininess;               //G缓冲Phong反光度

uniform sampler2D gAlbedo;                  //G缓冲PBR颜色信息
uniform sampler2D gMetRougAo;               //G缓冲PBR金属度粗糙度环境遮蔽信息

//IBL
uniform samplerCube irradianceMap;          //辐照度环境贴图
uniform samplerCube prefilterMap;           //预滤波环境贴图
uniform sampler2D   brdfLUT;                //BRDF环境贴图

//数学常数
const float PI = 3.14159265359;

//PBR----------------
//法线分布函数
float DistributionGGX(vec3 N, vec3 H, float roughness);
//几何遮蔽函数
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
//菲涅尔方程
vec3 fresnelSchlick(float cosTheta, vec3 F0);
//菲涅尔方程（含粗糙度）
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

//Phong光照-------------
vec4 CalcPhong(vec3 Normal ,vec3 FragPos, vec3 Diffuse, float Specular, float Shininess);
vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir, vec3 Diffuse, float Specular, float Shininess);//定向光
vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, float Shininess);//点光源
vec3 CalcSpotLight(Light light, vec3 Normal, vec3 FragPos, vec3 viewDir, vec3 Diffuse, float Specular, float Shininess);//聚光


//主函数
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
    else if(renderMode == 1) //PBR（需要再g缓冲中添加相应albedo等结果纹理）
    {
        vec3 albedo = pow(texture(gAlbedo, TexCoords).rgb, vec3(2.2));  //gamma矫正
        //vec3 albedo = texture(gAlbedo, TexCoords).rgb;
        float metallic = texture(gMetRougAo, TexCoords).r;
        float roughness = texture(gMetRougAo, TexCoords).g;
        float ao = texture(gMetRougAo, TexCoords).b;

        vec3 N = normalize(Normal);
        vec3 V = normalize(camPos - FragPos);
        vec3 R = reflect(-V, N); 

        //计算基础反射率（法向入射时的反射率）
        vec3 F0 = vec3(0.04);             //非金属使用0.04基础反射率(黑色)
        F0 = mix(F0, albedo, metallic);   //金属则根据金属度对F0到albedo进行插值得到反射率颜色

        //反射率方程
        vec3 Lo = vec3(0.0);
        for(int i = 0; i < LightsNum; ++i) 
        {
            //计算每条光线的辐射量
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
        
            //kS(镜面反射率)等于Fresnel
            vec3 kS = F;
            //kD(漫反射率)等于1 - kS （能量守恒）
            vec3 kD = vec3(1.0) - kS;
            //控制金属性   (金属性为1则无漫反射)
            kD *= 1.0 - metallic;	  

            // scale light by NdotL
            float NdotL = max(dot(N, L), 0.0);        

            //最终辐射量radiance Lo
            Lo += (kD * albedo / PI + specular) * radiance * NdotL; 
            //请注意，我们已经将 BRDF 乘以菲涅耳 （kS），因此我们不会再次乘以 kS
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

        //HDR色调映射
        color = color / (color + vec3(1.0));
        //gamma校正
        color = pow(color, vec3(1.0/2.2)); 

        FragColor = vec4(color, 1.0);
    }
}

//PBR----------
//法线分布函数
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
//几何遮蔽函数
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
//菲涅尔方程
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 2.0);
}
//菲涅尔方程（含粗糙度）
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 2.0);
} 

//Phong---------
vec4 CalcPhong(vec3 Normal ,vec3 FragPos, vec3 Diffuse, float Specular, float Shininess)
{
    vec3 lighting  = AmbientIntensity * Ambient * Diffuse; //环境光
    vec3 viewDir  = normalize(camPos - FragPos);

    for(int i = 0; i < LightsNum; ++i)
    {
        if(lights[i].type == 0)  //点光
        {
            float distance = length(lights[i].position - FragPos);
            if(distance < lights[i].Radius) //超出光体积的片段不做光计算(实际上该if语句不起作用)
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
        else if(lights[i].type == 1)  //有向光
        {
            lighting += CalcDirLight(lights[i], Normal, viewDir, Diffuse, Specular, Shininess);
        }
        else if(lights[i].type == 2)  //聚光
        {
            lighting += CalcSpotLight(lights[i], Normal, FragPos, viewDir, Diffuse, Specular, Shininess);
        } 
    }

    return vec4(lighting, 1.0);
}
vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir, vec3 Diffuse, float Specular, float Shininess)
{
	vec3 lightDir = normalize(-light.direction);
	// 漫反射着色
	float diff = max(dot(normal, lightDir), 0.0);
	// 镜面光着色
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);
	// 合并结果
	vec3 diffuse  = light.diffuse  * diff * Diffuse;
	vec3 specular = light.specular * spec * Specular;
	return (diffuse + specular);
}
vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 Diffuse, float Specular, float Shininess)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// 漫反射着色
	float diff = max(dot(normal, lightDir), 0.0);
	// 镜面光着色
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);
	// 衰减
	float distance    = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance +
					light.quadratic * (distance * distance));
	// 合并结果
	vec3 diffuse  = light.diffuse  * diff * Diffuse;
	vec3 specular = light.specular * spec * Specular;

	diffuse  *= attenuation;
	specular *= attenuation;

	return ( diffuse + specular);
}
vec3 CalcSpotLight(Light light, vec3 Normal, vec3 FragPos, vec3 viewDir, vec3 Diffuse, float Specular, float Shininess)
{
	vec3 lightDir = normalize(light.position - FragPos);//光线方向向量

	//漫反射
	vec3 norm = normalize(Normal);//法线向量
	float diff = max(dot(norm, lightDir), 0.0f);//求得diff值与两者形成的夹角相关
	vec3 diffuse  = light.diffuse  * diff * Diffuse;
	//镜面反射
	vec3 reflectDir = reflect(-lightDir, norm);//光线反射方向向量
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), Shininess);//指数是高光的反光度(Shininess)
	vec3 specular = light.specular * spec * Specular;
	//外发光
	//vec3 emission = vec3(0.5f) * vec3(texture(material.emissionMap, TexCoords));

	//聚光照射约束
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon   = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	diffuse  *= intensity;
	specular *= intensity;

	//光线衰弱
	float distance    = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
	diffuse  *= attenuation;
	specular *= attenuation;
		
	return (diffuse + specular); //+ emission
}