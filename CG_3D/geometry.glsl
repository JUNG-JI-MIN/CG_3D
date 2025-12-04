#version 430 core

layout(points) in;
layout(points, max_vertices = 50) out;

in VS_OUT {
    vec3 pos;
    vec3 vel;
    float life;
} gs_in[];

void main() {

    vec3 pos = gs_in[0].pos;
    vec3 vel = gs_in[0].vel;
    float life = gs_in[0].life;

    // life가 양수이면 = 올라가는 중
    if (life > 0.0) {
        gl_Position = vec4(pos, 1.0);
        EmitVertex();
        EndPrimitive();
    }
    else {
        // life <= 0  →  여기에서 폭발!
        for (int i=0;i<30;i++) {
            float angle = radians(float(i) * (360.0 / 30.0));
            vec3 dir = vec3(cos(angle), sin(angle), 0.0);

            gl_Position = vec4(pos + dir * 0.05, 1.0);
            EmitVertex();
        }
        EndPrimitive();
    }
}
