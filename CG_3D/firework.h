enum class ParticleEffectType {
    EXPLOSION,      // 기존 폭발 (빠르게 터짐)
    RISING_CUBES    // 새로운 이펙트 (느리게 올라감)
};
struct Particle {
    glm::vec3 pos;      // 현재 위치
    glm::vec3 vel;      // 속도 (위로 슈욱 올라가는 값)
    float life;         // 남은 수명 (0되면 터짐)
};
class Firework {
public:
    Particle rocket;
    std::vector<Particle> sparks;
    bool exploded = false;
    ParticleEffectType effectType;  // ← 추가
    float duration;                 // ← 지속시간

    Firework(const glm::vec3& startPos,float l, ParticleEffectType type = ParticleEffectType::EXPLOSION) {
        rocket.pos = startPos;
        rocket.vel = glm::vec3(0.0f, 5.0f, 0.0f);
        rocket.life = l;
        effectType = type;
        duration = 0.0f;
    }

    bool isDead() const {
        return exploded && sparks.empty();
    }

    void update(float dt) {
        if (!exploded) {
            rocket.pos += rocket.vel * dt;
            rocket.life -= dt;

            if (rocket.life <= 0.0f) {
                if (effectType == ParticleEffectType::EXPLOSION) {
                    explode();
                }
                else {
                    risingCubesEffect();  // ← 새로운 이펙트
                }
            }
        }
        else {
            for (int i = (int)sparks.size() - 1; i >= 0; i--) {
                sparks[i].pos += sparks[i].vel * dt;

                if (effectType == ParticleEffectType::EXPLOSION) {
                    sparks[i].life -= dt * 0.5f;
                }
                else {
                    sparks[i].life -= dt * 0.3f;  // 더 천천히 사라짐
                }

                if (sparks[i].life <= 0.0f) {
                    sparks.erase(sparks.begin() + i);
                }
            }
        }
    }

private:
    void explode() {
        exploded = true;
        for (int i = 0; i < 50; i++) {
            Particle p;
            p.pos = rocket.pos;
            float angle = glm::linearRand(0.0f, glm::two_pi<float>());
            float speed = glm::linearRand(2.0f, 6.0f);
            p.vel = glm::vec3(
                cos(angle) * speed,
                glm::linearRand(1.0f, 4.0f),
                sin(angle) * speed
            );
            p.life = 1.0f;
            sparks.push_back(p);
        }
    }

    void risingCubesEffect() {
        exploded = true;
        for (int i = 0; i < 30; i++) {
            Particle p;
            p.pos = rocket.pos;

            // 수평 이동은 거의 없음 (느린 드리프트)
            float drift = glm::linearRand(-0.5f, 0.5f);
            p.vel = glm::vec3(
                drift,
                glm::linearRand(1.0f, 5.0f),  // 천천히 올라감
                glm::linearRand(-0.5f, 0.5f)
            );

            p.life = 1.0f;  // 더 오래 지속
            sparks.push_back(p);
        }
    }
};

class FireworkManager {
public:
    std::vector<Firework> fireworks;
    GLuint particleVAO, particleVBO;
    bool initialized = false;

    void init() {
        if (initialized) return;
        glGenVertexArrays(1, &particleVAO);
        glGenBuffers(1, &particleVBO);
        initialized = true;
    }

    void createFirework(const glm::vec3& pos) {
        fireworks.emplace_back(pos,1.5f, ParticleEffectType::EXPLOSION);
        cout << "폭죽 생성! 위치: " << pos.x << ", " << pos.y << ", " << pos.z << endl;
    }

    void createRisingCubesEffect(const glm::vec3& pos) {
        fireworks.emplace_back(pos, 0.0f, ParticleEffectType::RISING_CUBES );
        cout << "올라가는 큐브 이펙트 생성! 위치: " << pos.x << ", " << pos.y << ", " << pos.z << endl;
    }

    void update(float dt) {
        for (int i = (int)fireworks.size() - 1; i >= 0; i--) {
            fireworks[i].update(dt);
            if (fireworks[i].isDead())
                fireworks.erase(fireworks.begin() + i);
        }
    }

    void Draw(Camera& camera) {
        if (fireworks.empty()) return;
        if (!initialized) init();

        vector<glm::vec3> positions;
        vector<glm::vec4> colors;

        for (const auto& firework : fireworks) {
            if (!firework.exploded) {
                positions.push_back(firework.rocket.pos);
                colors.push_back(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
            }
            else {
                for (const auto& spark : firework.sparks) {
                    positions.push_back(spark.pos);

                    if (firework.effectType == ParticleEffectType::EXPLOSION) {
                        // 기존: 노랑 → 주황 → 빨강
                        float life_ratio = spark.life;
                        colors.push_back(glm::vec4(1.0f, life_ratio * 0.5f, 0.0f, life_ratio));
                    }
                    else {
                        // 새로운: 다양한 색상 (무지개)
                        float hue = glm::linearRand(0.0f, 1.0f);
                        colors.push_back(glm::vec4(
                            fract(hue + 1.0f) < 0.333f ? 1.0f : (fract(hue + 1.0f) < 0.666f ? hue : 0.0f),
                            fract(hue) > 0.333f && fract(hue) < 0.666f ? 1.0f : fract(hue),
                            fract(hue + 2.0f / 3.0f) > 0.333f ? 1.0f : 0.0f,
                            spark.life  // 수명에 따라 투명도 변화
                        ));
                    }
                }
            }
        }

        if (positions.empty()) return;

        // VAO/VBO 설정
        glBindVertexArray(particleVAO);
        glBindBuffer(GL_ARRAY_BUFFER, particleVBO);

        // 정점 데이터 업로드
        struct ParticleVertex {
            glm::vec3 position;
            glm::vec4 color;
        };

        vector<ParticleVertex> vertices;
        for (size_t i = 0; i < positions.size(); ++i) {
            vertices.push_back({ positions[i], colors[i] });
        }

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ParticleVertex),
            vertices.data(), GL_DYNAMIC_DRAW);

        // 정점 속성 설정
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex),
            (void*)offsetof(ParticleVertex, color));
        glEnableVertexAttribArray(1);

        // 행렬 설정
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.View_matrix_update();
        glm::mat4 proj = camera.Projection_matrix_update();

        GLuint u = glGetUniformLocation(shaderProgramID, "m");
        glUniformMatrix4fv(u, 1, GL_FALSE, glm::value_ptr(model));
        u = glGetUniformLocation(shaderProgramID, "v");
        glUniformMatrix4fv(u, 1, GL_FALSE, glm::value_ptr(view));
        u = glGetUniformLocation(shaderProgramID, "p");
        glUniformMatrix4fv(u, 1, GL_FALSE, glm::value_ptr(proj));
        u = glGetUniformLocation(shaderProgramID, "whatColor");
        glUniform1i(u, 1);  // 색상 사용

        // 블렌딩 활성화
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);  // 파티클은 깊이 테스트 끄기

        // 점 크기 설정 및 렌더링
        glPointSize(8.0f);
        glDrawArrays(GL_POINTS, 0, vertices.size());

        // 상태 복원
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glPointSize(1.0f);
        glBindVertexArray(0);
    }

private:
    float fract(float x) {
        return x - floor(x);
    }
};
FireworkManager fireworkmanager;