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

void main()
{
    if (turn_off == 1) {
        FragColor = texture(texture1, out_TexCoord);
        return;
    }

    // 텍스처 색상 가져오기
    vec4 texColor = texture(texture1, out_TexCoord);

    // 1. Ambient
    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * lightColor;

    // 2. Diffuse (확산광)
    vec3 norm = normalize(out_Normal);
    vec3 lightDir = normalize(lightPos - out_FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // 3. 최종 색상 계산 (objectColor 대신 out_Color 사용 가능)
    vec3 result = (ambient + diffuse) * texColor.rgb;

    FragColor = vec4(result, texColor.a);
}
/* 이거는 텍스쳐 사용안 한거
void main()
{
    if (turn_off == 1) {
        FragColor = texture(texture1, out_TexCoord);
        return;
    }

    // 텍스처 색상 가져오기
    vec4 texColor = texture(texture1, out_TexCoord);

    // 1. Ambient
    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * lightColor;

    // 2. Diffuse (확산광)
    vec3 norm = normalize(out_Normal);
    vec3 lightDir = normalize(lightPos - out_FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // 3. 최종 색상 계산 (objectColor 대신 out_Color 사용 가능)
    vec3 result = (ambient + diffuse) * out_Color.rgb;

    FragColor = vec4(result, out_Color.a);
}

*/
