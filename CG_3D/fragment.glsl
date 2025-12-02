#version 330 core
in vec4 out_Color;
in vec3 out_Normal;
in vec3 out_FragPos;
in vec2 out_TexCoord;
out vec4 FragColor;

uniform sampler2D texture1; // 텍스처 유니폼
uniform vec3 lightPos;     // 빛 위치
uniform vec3 viewPos;      // 카메라 위치
uniform vec3 lightColor;  // 빛 색상
uniform int turn_off;    // 조명 끄기 변수
uniform int whatColor; // 색상 선택 변수
void main()
{
    vec4 selectColor = out_Color;
    /*
    if (turn_off == 1) {
        if (whatColor == 0){        
            FragColor = texture(texture1, out_TexCoord);
        }
        else {
             FragColor = out_Color;
        }
        return;
    }
    */
    if (whatColor == 0){
        // 텍스처 색상 가져오기
        selectColor = texture(texture1, out_TexCoord);
    }
    else{
        selectColor = out_Color;
    }

    

    // 1. Ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    // 2. Diffuse (확산광)
    vec3 norm = normalize(out_Normal);
    vec3 lightDir;
    if (turn_off == 0){    
        lightDir = normalize(vec3(1.0, 1.0, 1.0));
    }
    else{
        lightDir = normalize(lightPos - out_FragPos);
    }
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // 3. 최종 색상 계산 (objectColor 대신 out_Color 사용 가능)
    vec3 result = (ambient + diffuse) * selectColor.rgb;

    FragColor = vec4(result, selectColor.a);
}