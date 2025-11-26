#version 330 core
layout(location = 0) in vec3 inPos;   // VBO에서 위치 입력
layout(location = 1) in vec4 inColor; // VBO에서 색상 입력
layout(location = 2) in vec3 inNormal; // VBO에서 법선 입력
layout(location = 3) in vec2 intexCoord; // VBO에서 텍스처 좌표 입력

out vec4 out_Color;                   // 프래그먼트 셰이더로 색상 전달
out vec3 out_Normal;
out vec3 out_FragPos;
out vec2 out_TexCoord;

uniform mat4 m;     // 모델 변환
uniform mat4 v;      // 뷰
uniform mat4 p;// 투영
uniform mat3 n; // 모델의 법선 변환 행렬 (transpose(inverse(mat3(model))))

void main()
{
    vec4 worldPos = m * vec4(inPos, 1.0);
    gl_Position = p * v * worldPos;

    out_Color = inColor;
    out_FragPos = worldPos.xyz;
    out_Normal = normalize(n * inNormal);   // 법선 변환
    out_TexCoord = intexCoord;
}
