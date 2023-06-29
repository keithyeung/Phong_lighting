#version 330

uniform sampler2D u_diffuse;

in  vec2 f_texcoord;
in  vec3 f_normal;
in  vec3 f_color;
in	vec3 f_position;

out vec4 frag_color;

uniform vec3 lightDir1;
uniform vec3 camPos;
uniform vec4 lightColor;

void main() {

   //ambient
   vec3 ambientLight = vec3(0.5f,0.5f,0.5f);

   //defuse light
   vec3 posToLightDirVec = normalize(-lightDir1);
   vec3 diffuseColor = vec3(1.0f,1.0f,1.0f);
   float diffuse = clamp(dot(posToLightDirVec, f_normal),0,1);
   vec3 diffuseFinal = diffuseColor * diffuse;

   //Specular light
   vec3 lightToPosDirVec = posToLightDirVec; 
   vec3 reflectDirVec = normalize(reflect(lightToPosDirVec,normalize(f_normal)));
   vec3 posToViewDirVec = normalize(f_position - camPos);
   float specularConstant = pow(max(dot(posToViewDirVec,reflectDirVec),0),50);
   vec3 specularFinal = vec3(1.0f,1.0f,1.0f) * specularConstant;


   //Final light
   frag_color = texture(u_diffuse, f_texcoord) * lightColor 
   * (vec4(ambientLight,1.0f) + vec4(diffuseFinal,1.0f) + vec4(specularFinal,1.0f));
}