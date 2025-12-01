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
    bool Falling = false; // 떨어지는 중인지 여부
    float rollProgress = 0.0f; // 구르는 애니메이션 진행도 (0.0 ~ 1.0)
    float roll_speed = 4.0f; // 구르는 속도 (초당 진행도)
    float fall_speed = 1.0f; // 낙하 속도 (초당 진행도) - 기존 0.1f에서 5.0f로 변경
	glm::vec3 rollDirection = glm::vec3(0.0f);
	glm::vec3 rollStartPos; // 굴림 시작 위치
	glm::vec3 fallStartPos; // 낙하 시작 위치
	glm::vec3 fallTargetPos; // 낙하 목표 위치
	glm::quat rollStartRotation; // 굴림 시작 회전 (쿼터니언)
	glm::vec3 rollAxis; // 회전축
	glm::vec3 rollPivot; // 회전 중심점

    // 플레이어 큐브가 바라보는 방향으로 구르는 함수
    inline void Rolling_in_the_deep(glm::vec3 direction)
    {
		if (Rolling || Falling) return; // 이미 구르는 중이거나 떨어지는 중이면 무시

		dir = direction; // 방향 업데이트
		glm::vec3 nextPos = position + direction * 2.0f;

		// 타일 기반 경계 체크 - 목표 위치에 타일이 있는지 확인
		float targetGroundHeight = FindGroundHeight(nextPos);
		if (targetGroundHeight < -99.0f) return; // 타일이 없으면 이동 불가

		float currentGroundHeight = FindGroundHeight(position);
		
		// 높이 차이 체크: 1칸(2.0f) 이상 높으면 이동 불가
		// 위에서 아래로는 자유롭게, 아래서 위로는 1칸까지만
		float heightDifference = targetGroundHeight - currentGroundHeight;
		if (heightDifference > 2.1f) return; // 2칸 이상 높으면 못 올라감 (여유값 0.1f)

		// 중요: 위에 장애물이 있는지 체크 (천장 충돌 검사)
		if (HasCeilingObstacle(position, nextPos)) return; // 머리 위에 타일이 있으면 이동 불가

		// 구르기 시작
		Rolling = true;
		rollProgress = 0.0f;
		rollDirection = direction;
		rollStartPos = position;
		rollStartRotation = rotation;

		// 높이 차이에 따라 회전 중심점 계산
		if (heightDifference > 0.1f) {
			// 위로 올라갈 때: 목표 타일의 모서리를 기준으로 회전
			float targetTileBottom = targetGroundHeight - 1.0f; // 목표 타일의 하단
			
			if (abs(direction.x) > 0.5f) {
				// 좌우 이동: Z축 회전, 목표 타일의 아래 모서리 기준
				rollAxis = glm::vec3(0.0f, 0.0f, -direction.x);
				rollPivot = glm::vec3(
					position.x + direction.x * 1.0f,  // 타일 모서리 X
					targetTileBottom,                  // 타일 하단 높이
					position.z                         // 현재 Z 유지
				);
			}
			else {
				// 전후 이동: X축 회전, 목표 타일의 아래 모서리 기준
				rollAxis = glm::vec3(direction.z, 0.0f, 0.0f);
				rollPivot = glm::vec3(
					position.x,                        // 현재 X 유지
					targetTileBottom,                  // 타일 하단 높이
					position.z + direction.z * 1.0f   // 타일 모서리 Z
				);
			}
		}
		else {
			// 같은 높이나 아래로 내려갈 때: 기존 방식 (큐브 하단 기준)
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
    }

    // 매 프레임마다 호출되는 업데이트 함수
    inline void Update(float dt) override
	{
		// 떨어지는 애니메이션 처리
		if (Falling) {
			UpdateFalling(dt);
			return;
		}

		// 구르는 애니메이션 처리
		if (!Rolling) {
			// 구르고 있지 않을 때도 낙하 체크
			CheckAndStartFalling();
			return;
		}

		// 진행도 업데이트
		rollProgress += dt * roll_speed;

		if (rollProgress >= 1.0f) {
			// 구르기 완료
			rollProgress = 1.0f;
			Rolling = false;

			// 최종 위치 설정
			glm::vec3 finalPos = rollStartPos + rollDirection * 2.0f;
			
			// 최종 위치의 타일 높이에 맞춰 Y 좌표 조정
			float groundHeight = FindGroundHeight(finalPos);
			finalPos.y = groundHeight + 1.0f; // 타일 위 표면 + 큐브 반지름
			
			position = finalPos;

			// 90도 회전 추가
			glm::quat deltaRotation = glm::angleAxis(glm::radians(90.0f), glm::normalize(rollAxis));
			rotation = glm::normalize(deltaRotation * rollStartRotation);

			// 구르기 완료 후 낙하 체크
			CheckAndStartFalling();
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

	// 충돌 처리 로직
    void OnCollision(GameObject* other) {

    }

private:
	// 지면 높이를 찾는 함수 (큐브 바로 아래 타일만 검색)
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
			if (dx < tileHalfSize && dz < tileHalfSize) {
				// 타일의 위 표면 높이 계산
				float tileTopHeight = tile->position.y + tileHalfSize;
				
				// 큐브 바로 아래에 있는 타일만 선택
				// pos.y는 큐브의 중심이므로, 큐브 하단(pos.y - 1.0f)보다 약간 아래에 있는 타일만 선택
				if (tileTopHeight <= pos.y + 1.0f && tileTopHeight > groundHeight) {
					groundHeight = tileTopHeight;
				}
			}
		}

		return groundHeight;
	}

	// 천장 장애물이 있는지 체크하는 함수
	inline bool HasCeilingObstacle(glm::vec3 currentPos, glm::vec3 nextPos)
	{
		float tileHalfSize = 1.0f; // 타일 크기의 절반
		float cubeHalfSize = 1.0f; // 큐브 크기의 절반
		
		// 현재 큐브의 상단 높이
		float cubeTop = currentPos.y + cubeHalfSize;
		
		// 현재 위치와 다음 위치 사이의 모든 타일 검사
		for (const auto* tile : tileManager.tiles) {
			// 타일이 이동 경로 상에 있는지 확인
			float dx_current = abs(currentPos.x - tile->position.x);
			float dz_current = abs(currentPos.z - tile->position.z);
			float dx_next = abs(nextPos.x - tile->position.x);
			float dz_next = abs(nextPos.z - tile->position.z);

			bool onCurrentPath = (dx_current < tileHalfSize && dz_current < tileHalfSize);
			bool onNextPath = (dx_next < tileHalfSize && dz_next < tileHalfSize);

			if (onCurrentPath || onNextPath) {
				// 타일의 하단 높이
				float tileBottom = tile->position.y - tileHalfSize;
				
				// 타일이 큐브 머리 위에 있는지 확인
				// 큐브 상단이 타일 하단보다 아래에 있고, 
				// 타일이 큐브보다 위에 있으면 천장 장애물
				if (tileBottom > cubeTop - 0.5f && tileBottom < cubeTop + 0.1f) {
					// 이 타일이 서있는 타일이 아닌지 확인
					float currentGround = FindGroundHeight(currentPos);
					float tileTop = tile->position.y + tileHalfSize;
					
					// 현재 서있는 타일이 아니면 천장 장애물
					if (abs(tileTop - currentGround) > 0.1f) {
						return true; // 천장에 장애물 있음
					}
				}
			}
		}

		return false; // 천장 안전
	}

	// 낙하 체크 및 시작
	inline void CheckAndStartFalling()
	{
		if (Falling || Rolling) return; // 이미 떨어지고 있거나 구르고 있으면 체크 안함

		float currentGround = FindGroundHeight(position);
		
		// 타일이 없으면 낙하하지 않음 (맵 밖으로 나가는 것 방지)
		if (currentGround < -99.0f) return;
		
		float expectedY = currentGround + 1.0f; // 타일 위에 있어야 할 Y 위치

		// 현재 위치가 타일 위보다 높으면 떨어져야 함
		if (position.y > expectedY + 0.1f) {
			Falling = true;
			rollProgress = 0.0f;
			fallStartPos = position;
			fallTargetPos = position;
			fallTargetPos.y = expectedY;
		}
	}

	// 떨어지는 애니메이션 업데이트
	inline void UpdateFalling(float dt)
	{
		rollProgress += dt * fall_speed;

		if (rollProgress >= 1.0f) {
			// 낙하 완료
			rollProgress = 1.0f;
			Falling = false;
			position = fallTargetPos;
			
			// 낙하 완료 후에도 추가 낙하가 필요한지 체크
			CheckAndStartFalling();
		}
		else {
			// 부드러운 낙하 (ease-in 가속)
			
			position.y = glm::mix(fallStartPos.y, fallTargetPos.y, rollProgress);
			position.x = fallTargetPos.x;
			position.z = fallTargetPos.z;
		}
	}
};
PlayerCube player({ 0.0f, 2.0f, 0.0f });