const char* vertex_shader_code_150 = R"*(
#version 150

in vec3 pos;
in vec3 normal;
in vec3 col;

uniform mat4 projMatrix;
uniform mat4 modelviewMatrix;
uniform mat3 normalMatrix;

uniform vec3 lightpos;
uniform vec4 matAmbientColor;
uniform vec4 lightAmbientColor;
uniform vec4 mycolor;
uniform int uselighting;
uniform bool selected;

smooth out vec3 color;
out vec4 ambient;
out vec3 viewEye,lightEye, normalEye;
flat out vec3 normalEyeflat;


    
    void main() {
      
      vec4 vert4=vec4(pos,1.0);
      color=col;
  
      viewEye   = vec3(modelviewMatrix * vert4);
      normalEye = normalize(normalMatrix * normal);
      lightEye  = normalize(lightpos-viewEye);
      
      normalEyeflat=normalize(normalMatrix * normal);
      ambient = matAmbientColor*lightAmbientColor;
      
      gl_Position = projMatrix * modelviewMatrix* vert4;
    }

)*";


const char* fragment_shader_code_150 = R"*(
#version 150
precision highp float;
out vec4 fragColor;

in vec3 color;
in vec4 ambient;
in vec3 viewEye,lightEye, normalEye;
flat in vec3 normalEyeflat;


uniform vec4 matDiffuseColor;
uniform vec4 lightDiffuseColor;
uniform vec4 matSpecularColor;
uniform vec4 lightSpecularColor;
uniform float shininess;
uniform int uselighting;
uniform int flatshading;
uniform vec4 mycolor;
uniform bool selected;

vec4 calcDiffusePhongIllumination(vec3 L, vec3 N)
{
    // Compute the diffuse term
	float NdotL = max(dot(N,L),0.0);
	vec4 diffuse = matDiffuseColor * lightDiffuseColor * NdotL;

	return diffuse;
}


vec4 calcSpecularPhongIllumination(vec3 L, vec3 N, vec3 V)
{
	vec3 R = reflect(L,N); 
	float NdotHV = max(dot(R,V),0.0);
	vec4 specular = matSpecularColor * lightSpecularColor * pow(NdotHV,shininess);
	
	return specular;
}



void main() {
     
    vec3 normal,light,view;
    if(flatshading==0)
        normal=normalize(normalEye);
    else
        normal=normalize(normalEyeflat);

	light = normalize(lightEye);
	view = normalize(viewEye);

	vec4 col= vec4(0.0,0.0,0.0,0.0);
	
	if(uselighting==1)
	{
	  col+=ambient;
	  col+=calcDiffusePhongIllumination(light,normal);
	  col+=calcSpecularPhongIllumination(light,normal,view);
	}
	else
	{
		col=vec4(color,0.0);
	}
	fragColor = col;
       
}
)*";


/****************************************************************************************************************/

const char* vertex_shader_code_130 = R"*(
#version 130


in vec3 pos;
in vec3 normal;
in vec3 col;

uniform mat4 projMatrix;
uniform mat4 modelviewMatrix;
uniform mat3 normalMatrix;

uniform vec3 lightpos;
uniform vec4 matAmbientColor;
uniform vec4 lightAmbientColor;
uniform vec4 mycolor;
uniform int uselighting;
uniform bool selected;

out vec3 color;
out vec4 ambient;
out vec3 viewEye,lightEye, normalEye;

    
    void main() {
      
      vec4 vert4=vec4(pos,1.0);
      color=col;
  
      viewEye   = vec3(modelviewMatrix * vert4);
      normalEye = normalize(normalMatrix * normal);
      lightEye  = normalize(lightpos-viewEye);      
      ambient = matAmbientColor*lightAmbientColor;

      gl_Position = projMatrix * modelviewMatrix* vert4;
    }

)*";


const char* fragment_shader_code_130 = R"*(
#version 130
precision highp float;
out vec4 fragColor;

in vec3 color;
in vec4 ambient;
in vec3 viewEye,lightEye, normalEye;

uniform vec4 matDiffuseColor;
uniform vec4 lightDiffuseColor;
uniform vec4 matSpecularColor;
uniform vec4 lightSpecularColor;
uniform float shininess;
uniform int uselighting;
uniform int flatshading;
uniform vec4 mycolor;
uniform bool selected;

vec4 calcDiffusePhongIllumination(vec3 L, vec3 N)
{
    // Compute the diffuse term
    float NdotL = max(dot(N,L),0.0);
    vec4 diffuse = matDiffuseColor * lightDiffuseColor * NdotL;

    return diffuse;
}


vec4 calcSpecularPhongIllumination(vec3 L, vec3 N, vec3 V)
{
    vec3 R = reflect(L,N); 
    float NdotHV = max(dot(R,V),0.0);
    vec4 specular = matSpecularColor * lightSpecularColor * pow(NdotHV,shininess);
    
    return specular;
}



void main() {
     
    vec3 normal,light,view;

    normal=normalize(normalEye);
    light = normalize(lightEye);
    view = normalize(viewEye);

    vec4 col= vec4(0.0,0.0,0.0,0.0);
    
    if(uselighting==1)
    {
      col+=ambient;
      col+=calcDiffusePhongIllumination(light,normal);
      col+=calcSpecularPhongIllumination(light,normal,view);
    }
    else
    {
        col=vec4(color,0.0);
    }
    fragColor = col;
       
}
)*";

