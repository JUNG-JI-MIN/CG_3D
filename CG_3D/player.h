#pragma once
#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
#include <cmath>
#include "gameobject.h"
#include "tilemanager.h"

// 플레이어가 조작하는 큐브
class PlayerCube : public GameObject {
public:
    PlayerCube(glm::vec3 pos)
        : GameObject(pos), stageStartPos(pos) {
    }

    glm::vec3 dir = { 0.0f,0.0f,1.0f }; // 이동 방향 or 바라보는 방향이라고 할 수 있음 키보드 조작을 통해 dir이 변경됨
    bool Rolling = false; // 구르는 중인지 아닌지 여부 애니메이션에서 사용
    bool Falling = false; // 떨어지는 중인지 여부
    float rollProgress = 0.0f; // 구르는 애니메이션 진행도 (0.0 ~ 1.0)
	float EndrollProgress = 1.0f;
	float roll_speed = 4.0f; // 구르는 속도 (초당 진행도)
    float fall_speed = 2.5f; // 낙하 속도 (초당 진행도) - 값을 줄이면 더 느리게 떨어짐
	glm::vec3 rollDirection = glm::vec3(0.0f);
	glm::vec3 rollStartPos; // 굴림 시작 위치
	glm::vec3 fallStartPos; // 낙하 시작 위치
	glm::vec3 fallTargetPos; // 낙하 목표 위치
	glm::quat rollStartRotation; // 굴림 시작 회전 (쿼터니언)
	glm::vec3 rollAxis; // 회전축
	glm::vec3 rollPivot; // 회전 중심점
	glm::vec3 stageStartPos; // 스테이지 시작 위치 (리셋용)

	// 플레이어 큐브가 바라보는 방향으로 구르는 함수
	inline void Rolling_in_the_deep(glm::vec3 direction)
	{
		if (Rolling || Falling) return;
		if (fmod(position.x, 2) != 0 || fmod(position.z, 2) != 0) return;

		if (fmod(position.x, 2) != 0 || fmod(position.z, 2) != 0) return;

		dir = direction;

		float targetGroundHeight = 0.0f;
		int tileState = CheckTileAtDirection(direction, targetGroundHeight);

		if (tileState == -1) {
			// 위에 타일이 있음 - 이동 불가
			return;
		}
		else if (tileState == 2) {
			// 올라가기 (1칸 높이 차이)
			float currentGroundHeight = FindGroundHeight(position);
			float heightDifference = targetGroundHeight - currentGroundHeight;

			// 천장 체크
			glm::vec3 nextPos = position + direction * 2.0f;
			if (HasCeilingObstacle(position, nextPos)) return;

			// 회전 경로 체크
			if (HasRollPathObstacle(position, nextPos, heightDifference)) return;

			// 구르기 시작
			Rolling = true;
			rollProgress = 0.0f;
			rollDirection = { direction.x, direction.y + 1, direction.z };
			rollStartPos = position;
			rollStartRotation = rotation;
			EndrollProgress = 2.0f;
			tileManager.CubeExit({ position.x,position.y - 2,position.z });

			// 올라갈 때: 목표 타일의 모서리를 기준으로 회전
			float targetTileBottom = targetGroundHeight - 1.0f;

			if (abs(direction.x) > 0.5f) {
				rollAxis = glm::vec3(0.0f, 0.0f, -direction.x);
				
			}
			else {
				rollAxis = glm::vec3(direction.z, 0.0f, 0.0f);
				
			}
			rollPivot = glm::vec3(
				position.x + direction.x,
				position.y + direction.y + 1.0f,
				position.z + direction.z );
		}
		else if (tileState == 1) {
			// 같은 높이 이동
			float currentGroundHeight = FindGroundHeight(position);

			// 천장 체크
			glm::vec3 nextPos = position + direction * 2.0f;
			if (HasCeilingObstacle(position, nextPos)) return;

			// 구르기 시작
			Rolling = true;
			rollProgress = 0.0f;
			rollDirection = direction;
			rollStartPos = position;
			rollStartRotation = rotation;
			EndrollProgress = 1.0f;
			tileManager.CubeExit({ position.x,position.y - 2,position.z });

			// 같은 높이: 큐브 하단 기준으로 회전
			float cubeBottomY = position.y - 1.0f;

			if (abs(direction.x) > 0.5f) {
				rollAxis = glm::vec3(0.0f, 0.0f, -direction.x);
				rollPivot = glm::vec3(position.x + direction.x * 1.0f, cubeBottomY, position.z);
			}
			else {
				rollAxis = glm::vec3(direction.z, 0.0f, 0.0f);
				rollPivot = glm::vec3(position.x, cubeBottomY, position.z + direction.z * 1.0f);
			}
		}
		else {
			

			// 낙하 시작
			glm::vec3 nextPos = position + direction * 2.0f;
			Rolling = true;
			rollProgress = 0.0f;
			rollDirection = direction;
			rollStartPos = position;
			rollStartRotation = rotation;
			EndrollProgress = 1.0f;
			tileManager.CubeExit({position.x,position.y-2,position.z});

			// 이동은 수평으로 진행
			float cubeBottomY = position.y - 1.0f;

			if (abs(direction.x) > 0.5f) {
				rollAxis = glm::vec3(0.0f, 0.0f, -direction.x);
				rollPivot = glm::vec3(position.x + direction.x * 1.0f, cubeBottomY, position.z);
			}
			else {
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

		if (rollProgress >= EndrollProgress) {
			// 구르기 완료
			Rolling = false;

			// 최종 위치 (수평 좌표)
			glm::vec3 finalPos = rollStartPos + rollDirection * 2.0f;
			float groundHeight = FindGroundHeight(finalPos);
			float startGroundHeight = FindGroundHeight(rollStartPos);
			float dropAmount = (startGroundHeight + 1.0f) - (groundHeight + 1.0f); // 내려가는 양 (>0 이면 내려감)

			// 90도 회전 적용
			glm::quat deltaRotation = glm::angleAxis(glm::radians(90.0f), glm::normalize(rollAxis));
			rotation = glm::normalize(deltaRotation * rollStartRotation);

			if (groundHeight > -99.0f && dropAmount > 0.1f) {
				// 큰 높이 차이로 내려갈 때: 바로 아래 높이에 맞춰 스냅하지 않고 낙하 애니메이션 시작
				position = glm::vec3(finalPos.x, rollStartPos.y, finalPos.z); // 수평 이동만 완료, 높이는 기존 유지
				Falling = true;
				rollProgress = 0.0f; // 낙하 진행도로 재사용
				fallStartPos = position;
				fallTargetPos = glm::vec3(finalPos.x, groundHeight + 1.0f, finalPos.z);
				return; // 낙하 시작했으므로 종료
			}
			else {
				// 같은 높이거나 아주 작은 높이 차이면 즉시 위치 스냅
				if (groundHeight > -99.0f) {
					finalPos.y = groundHeight + 1.0f; // 타일 위 표면 + 큐브 반지름
				}
				position = finalPos;
				// 구르기 완료 후 낙하 체크 (얕은 틈도 처리)
				CheckAndStartFalling();
			}
			CubeEnter({ position.x,position.y - 2,position.z });
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

	// 스테이지 시작 위치 설정
	void SetStageStartPosition(glm::vec3 pos) {
        stageStartPos = pos;
    }

    // 스테이지 초기 위치로 리셋
    void ResetToStageStart() {
        position = stageStartPos;
        Rolling = false;
        Falling = false;
        rollProgress = 0.0f;
        rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        velocity = glm::vec3(0.0f);
        cout << "플레이어가 스테이지 시작 위치로 리셋됨: (" 
             << stageStartPos.x << ", " << stageStartPos.y << ", " << stageStartPos.z << ")" << endl;
    }

	// 외부에서 낙하 체크를 강제로 호출할 수 있도록 public 함수 추가
	void CheckFalling() {
		if (!Rolling && !Falling) {
			CheckAndStartFalling();
		}
	}
	void CubeEnter(glm::vec3 pos) {
		for (TileBase* t : tileManager.tiles) {
			if (t->position == pos) {
				t->OnCubeEnter();
				if (t->type == "QuitTile") {
					exit(0);
				}
				else if (t->type == "GoToOneTile") {
					tileManager.LoadFromJSON("json/stage.json");
					position = tileManager.playerPos;
					SetStageStartPosition(tileManager.playerPos);
					light.light[1].position = position;
					light.player_position_update();
				}
				else if (t->type == "GoToTwoTile") {
					tileManager.LoadFromJSON("json/stage2.json");
					position = tileManager.playerPos;
					SetStageStartPosition(tileManager.playerPos);
					light.light[1].position = position;
					light.player_position_update();
				}
				else if (t->type == "switchtile") {
					tileManager.current_switch_tile = dynamic_cast<SwitchTile*>(t);
					position = tileManager.current_switch_tile->switch_position;
					cout << "스위치 타일 순간 이동" << endl;
				}
			}
		}
	}

private:
	inline int CheckTileAtDirection(glm::vec3 direction, float& outTargetHeight)
	{

		float tileHalfSize = 1.0f;
		glm::vec3 player_y = position;
		player_y.y += 2.0f;

		// 1단계: 방향 기준 아래 높이 체크 (position.y - 2)
		glm::vec3 checkPos = position + direction * 2.0f;
		glm::vec3 checkPos2 = position + direction * 2.0f;
		checkPos.y -= 2.0f;

		float belowHeight = FindGroundHeight(checkPos);
		float belowHeight2 = FindGroundHeight(checkPos2);

		// 타일이 있으면: 1단계 통과
		if (belowHeight > -99.0f || belowHeight2 > -99.0f) {
			outTargetHeight = belowHeight;

			// 2단계: 현재 높이 (position.y)에서 타일 확인
			checkPos = position + direction * 2.0f;
			checkPos.y = position.y;
			float currentLevelHeight = FindGroundHeight(checkPos);

			if (currentLevelHeight > -99.0f && currentLevelHeight > belowHeight + 0.1f) {
				checkPos.y += 2.0f;
				for (const auto& t : tileManager.tiles) {
					if (t->position == checkPos) return -1; // 앞 타일 위에 타일이 있음 - 이동 불가
					if (t->position == player_y) return -1; // 위에 타일이 있음 - 이동 불가
				}
				// 3단계: 위에 타일이 있는지 확인
				checkPos = position + direction * 2.0f;
				checkPos.y += 2.0f;
				float aboveHeight = FindGroundHeight(checkPos);
				// 올라가기 (1칸 높이)
				outTargetHeight = currentLevelHeight;
				return 2;
			}
			else {
				// 같은 높이 이동
				return 1;
			}
		}

		// 타일이 없으면: 1단계 실패 → 낙하할 타일 찾기
		return 0;
	}

	// 입력 벡터 방향에서 낙하할 가장 높은 타일 찾기
	// 반환값: 낙하 목표 높이 (타일이 없으면 -100.0f)
	inline float FindFallTargetHeight(glm::vec3 direction)
	{
		float targetX = position.x + direction.x * 2.0f;
		float targetZ = position.z + direction.z * 2.0f;
		float tileHalfSize = 1.0f;
		float minHeight = -10.0f; // 기준값

		for (const auto* tile : tileManager.tiles) {
			// X, Z 좌표가 일치하는 타일만 검사
			float dx = abs(targetX - tile->position.x);
			float dz = abs(targetZ - tile->position.z);

			if (dx < tileHalfSize && dz < tileHalfSize) {
				float tileTop = tile->position.y + tileHalfSize;

				// 현재 위치보다 아래에 있고, 가장 높은 타일 찾기
				if (tileTop < position.y && tileTop > minHeight) {
					minHeight = tileTop;
				}
			}
		}

		// 타일을 찾았으면 타일 위의 위치, 아니면 -100.0f 반환
		return (minHeight > -10.0f) ? minHeight : -100.0f;
	}
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
				if (tileBottom > cubeTop - 0.5f && tileBottom < cubeTop) {
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

	// **새로 추가: 회전 경로에 장애물(벽)이 있는지 체크하는 함수**
	inline bool HasRollPathObstacle(glm::vec3 currentPos, glm::vec3 nextPos, float heightDiff)
	{
		float tileHalfSize = 1.0f;
		float cubeHalfSize = 1.0f;
		
		// 위로 올라갈 때만 중간 장애물 체크 (벽 감지)
		if (heightDiff <= 0.1f) return false; // 같은 높이나 내려갈 때는 체크 안함

		float currentGround = FindGroundHeight(currentPos);
		float targetGround = FindGroundHeight(nextPos);
		
		// 현재 위치와 목표 위치 사이의 중간 높이 범위
		float minCheckHeight = currentGround;
		float maxCheckHeight = targetGround;

		for (const auto* tile : tileManager.tiles) {
			// 타일이 이동 경로 상에 있는지 확인
			float dx_current = abs(currentPos.x - tile->position.x);
			float dz_current = abs(currentPos.z - tile->position.z);
			float dx_next = abs(nextPos.x - tile->position.x);
			float dz_next = abs(nextPos.z - tile->position.z);

			bool onCurrentPath = (dx_current < tileHalfSize && dz_current < tileHalfSize);
			bool onNextPath = (dx_next < tileHalfSize && dz_next < tileHalfSize);

			// 현재 위치나 목표 위치 중 하나에 겹치는 타일만 검사
			if (onCurrentPath || onNextPath) {
				float tileTop = tile->position.y + tileHalfSize;
				float tileBottom = tile->position.y - tileHalfSize;

				// 중간 높이에 있는 타일인지 확인
				// 현재 서있는 타일보다 위에 있고, 목표 타일보다 아래에 있으면 중간 장애물
				if (tileBottom > minCheckHeight + 0.1f && tileTop < maxCheckHeight - 0.1f) {
					// 회전 경로를 막는 벽 발견
					return true;
				}

				// 목표 위치의 바로 아래층(목표-1칸)에 타일이 있는지 체크
				// 이 경우도 올라갈 수 없음 (벽에 막힘)
				if (onNextPath && tileTop < targetGround - 0.1f && tileTop > currentGround + 0.1f) {
					return true;
				}
			}
		}

		return false; // 경로 안전
	}

	// 낙하 체크 및 시작
	inline void CheckAndStartFalling()
	{
		if (Falling || Rolling) return; // 이미 떨어지고 있거나 구르고 있으면 체크 안함

		float currentGround = FindGroundHeight(position);
		
		// 타일이 없으면 즉시 낙하 시작 (맵 밖으로 떨어짐)
		if (currentGround < -99.0f) {
			Falling = true;
			rollProgress = 0.0f;
			fallStartPos = position;
			fallTargetPos = position;
			fallTargetPos.y = -30.0f; // 매우 깊이 떨어지도록 설정
			cout << "타일 없음! 낙하 시작!" << endl;
			return;
		}
		
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

            // 낙하 후 땅 타일이 있는지 확인
            float groundHeight = FindGroundHeight(position);
            
            // 땅 타일이 없으면 (낙하 깊이가 너무 깊으면) 스테이지 초기 위치로 리셋
            if (groundHeight < -99.0f || position.y < -50.0f) {
                cout << "플레이어가 맵 밖으로 떨어짐!" << endl;
                ResetToStageStart();
                light.light[1].position = position;
                light.player_position_update();
                return;
            }

			CubeEnter({ position.x,position.y - 2,position.z });
			
			// 낙하 완료 후에도 추가적으로 필요하면 체크
			CheckAndStartFalling();
		}
		else {
			// 부드럽게 낙하 (ease-in 효과)
			
			position.y = glm::mix(fallStartPos.y, fallTargetPos.y, rollProgress);
			position.x = fallTargetPos.x;
			position.z = fallTargetPos.z;
		}
	}

	

	// 충돌 처리 로직
	void OnCollision(GameObject* other) {

	}
};
PlayerCube player({ 0.0f, 2.0f, 0.0f });