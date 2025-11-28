#pragma once
#include "gameobject.h"
#include "tilemanager.h"

// 플레이어가 조작하는 큐브
class PlayerCube : public GameObject {
public:
    PlayerCube(glm::vec3 pos)
        : GameObject(pos) {
    }

    glm::vec3 dir = { 0.0f,0.0f,1.0f }; // 이동 방향 or 바라보는 방향이라고도 할 수 있음 키보드 조작을 통해 dir이 변경됨
    bool Rolling = false; // 구르는 중인지 아닌지 여부 애니메이션에서 사용
    float rollProgress = 0.0f; // 구르는 애니메이션 진행도 (0.0 ~ 1.0)
    float roll_speed = 4.0f; // 구르는 속도 (초당 진행도)
	glm::vec3 rollDirection = glm::vec3(0.0f);
	glm::vec3 rollStartPos; // 굴림 시작 위치
	glm::quat rollStartRotation; // 굴림 시작 회전 (쿼터니언)
	glm::vec3 rollAxis; // 회전축
	glm::vec3 rollPivot; // 회전 중심점

    // 플레이어 큐브가 바라보는 방향으로 구르는 함수
    inline void Rolling_in_the_deep(glm::vec3 direction)
    {
		if (Rolling) return; // 이미 구르는 중이면 무시

		dir = direction; // 방향 업데이트
		glm::vec3 nextPos = position + direction * 2.0f;

		// 맵 경계 체크
		if (abs(nextPos.x) > 20.0f || abs(nextPos.z) > 20.0f) return;

		float targetGroundHeight = FindGroundHeight(nextPos);
		float currentGroundHeight = FindGroundHeight(position);

		// 타일이 없거나 너무 높으면 이동 불가
		if (targetGroundHeight < -99.0f) return;
		if (targetGroundHeight > currentGroundHeight + 0.1f) return;

		// 구르기 시작
		Rolling = true;
		rollProgress = 0.0f;
		rollDirection = direction;
		rollStartPos = position;
		rollStartRotation = rotation;

		float cubeBottomY = position.y - 1.0f; // 큐브 크기가 2.0이므로 반지름은 1.0

		if (abs(direction.x) > 0.5f) {
			// 좌우 이동: Z축 회전
			rollAxis = glm::vec3(0.0f, 0.0f, -direction.x);
			rollPivot = glm::vec3(position.x + direction.x * 1.0f, cubeBottomY, position.z);
		}
		else {
			// 전후 이동: X축 회전
			rollAxis = glm::vec3(direction.z, 0.0f, 0.0f);
			rollPivot = glm::vec3(position.x, cubeBottomY, position.z + direction.z * 1.0f);
		}
    }

    // 매 프레임마다 호출되는 업데이트 함수
    inline void Update(float dt) override
	{
		if (!Rolling) return;

		// 진행도 업데이트
		rollProgress += dt * roll_speed;

		if (rollProgress >= 1.0f) {
			// 구르기 완료
			rollProgress = 1.0f;
			Rolling = false;

			// 최종 위치와 회전 설정
			position = rollStartPos + rollDirection * 2.0f;

			// 90도 회전 추가
			glm::quat deltaRotation = glm::angleAxis(glm::radians(90.0f), glm::normalize(rollAxis));
			rotation = glm::normalize(deltaRotation * rollStartRotation);
		}
		else {
			// 보간된 위치 계산 (원호 운동)
			float angle = rollProgress * glm::half_pi<float>(); // 0 ~ 90도

			// 회전 중심점 기준으로 원호 운동
			glm::vec3 fromPivot = rollStartPos - rollPivot;
			glm::quat rotationDelta = glm::angleAxis(angle, glm::normalize(rollAxis));
			glm::vec3 rotatedFromPivot = rotationDelta * fromPivot;
			position = rollPivot + rotatedFromPivot;

			// 회전 애니메이션
			glm::quat deltaRotation = glm::angleAxis(angle, glm::normalize(rollAxis));
			rotation = glm::normalize(deltaRotation * rollStartRotation);
		}
	}

	// 충돌 처리 로직 일단 놔두긴 했는데 쓸거면 쓰고 아님 안 써도 됨 근데 없애진 마
    void OnCollision(GameObject* other) {

    }

private:
	inline float FindGroundHeight(glm::vec3 pos)
	{
		float groundHeight = -100.0f; // 기본값: 매우 낮은 위치
		float tileHalfSize = 1.0f; // 타일 크기의 절반 (tileSize = 2.0f이므로)

		// 모든 타일을 검사하여 큐브 아래 가장 높은 타일 찾기
		for (const auto* tile : tileManager.tiles) {
			// 타일의 위치와 큐브의 위치를 비교하여 큐브가 타일 위에 있는지 확인
			float dx = abs(pos.x - tile->position.x);
			float dz = abs(pos.z - tile->position.z);

			// 큐브가 타일 범위 안에 있는지 확인
			if (dx <= tileHalfSize && dz <= tileHalfSize) {
				// 타일의 위 표면 높이 계산
				float tileTopHeight = tile->position.y + tileHalfSize;

				if (tileTopHeight > groundHeight) {
					groundHeight = tileTopHeight;
				}
			}
		}

		return groundHeight;
	}

};
// 플레이어를 타일 위에 올려놓기 위해 Y 좌표를 1.0으로 설정 
PlayerCube player({ 0.0f, 2.0f, 0.0f });