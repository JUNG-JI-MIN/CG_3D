#version 330 core

struct LIGHT{
    vec3 position;  // pos → position으로 변경
    vec3 dir;
    vec3 color;
    int type;
};

in vec4 out_Color;
in vec3 out_Normal;
in vec3 out_FragPos;
in vec2 out_TexCoord;
out vec4 FragColor;

uniform LIGHT light[2];
uniform sampler2D texture1;
uniform vec3 viewPos;
uniform int whatColor;

void main()
{
    vec4 selectColor = out_Color;
    if (whatColor == 0){
        selectColor = texture(texture1, out_TexCoord);
    }
    else{
        selectColor = out_Color;
    }

    vec3 resultlight = vec3(0.0f);
    vec3 norm = normalize(out_Normal);

    for (int i = 0; i < 2; i++){
        // 1. Ambient
        float ambientStrength = 1;
        vec3 ambient = ambientStrength * light[i].color;

        // 2. Diffuse (확산광)
        vec3 lightDir;
        float attenuation = 1.0;
        
        if (light[i].type == 0){    
            // 방향광: dir을 반대로 (빛이 오는 방향)
            lightDir = normalize(-light[i].dir);
        }
        else{
            // 점광원: 위치에서 fragment로의 방향 + 감쇠 계산
            lightDir = normalize(light[i].position - out_FragPos);
            float distance = length(light[i].position - out_FragPos);
            attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        }
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * light[i].color * attenuation;
        
        // ambient에도 점광원이면 감쇠 적용
        vec3 finalAmbient = ambient;
        if (light[i].type == 1) {
            finalAmbient *= attenuation;
        }
        
        resultlight += finalAmbient + diffuse;
    }

    vec3 result = resultlight * selectColor.rgb;
    FragColor = vec4(result, selectColor.a);
}