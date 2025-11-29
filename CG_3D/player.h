#include "gameobject.h"

// 플레이어가 조작하는 큐브
class PlayerCube : public GameObject {
public:
    PlayerCube(glm::vec3 pos)
        : GameObject(pos) {
    }

    glm::vec3 dir = { 0.0f,0.0f,1.0f }; // 이동 방향 or 바라보는 방향이라고도 할 수 있음 키보드 조작을 통해 dir이 변경됨
    bool Rolling = false; // 구르는 중인지 아닌지 여부 애니메이션에서 사용
    float  t = 0.0f; // 구르는 애니메이션 진행 시간
    float roll_speed = 3.0f; // 구르는 속도 매개변수 방정식

    // 플레이어 큐브가 바랴보는 방향으로 구르는 함수
    void Rolling_in_the_deep() {
    }

    // 매 프레임마다 호출되는 업데이트 함수
    void Update(float dt) override {
    }

    // 충돌 처리 로직 일단 놔두긴 했는데 쓸거면 쓰고 아님 안 써도 됨 근데 없애진 마
    void OnCollision(GameObject* other) {

    }



};
PlayerCube player({ 0.0f, 2.0f, 0.0f });