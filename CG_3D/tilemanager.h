#pragma once
#include "gameobject.h"
#include "firework.h"

// 타일의 기본 클래스
class TileBase : public GameObject {
public:
    TileBase(glm::vec3 pos)
        : GameObject(pos) {
    }
    string type;
    // 플레이어 큐브가 타일에 들어갔을 때 호출되는 함수
    virtual void OnCubeEnter() {}
    // 플레이어 큐브가 타일위에 머무를 때 호출되는 함수    
    virtual void OnCubeStay() {}
    // 플레이어 큐브가 타일에서 나갔을 때 호출되는 함수
    virtual void OnCubeExit() {}
};

// 기본 땅 타일
class GroundTile : public TileBase {
public:
    GroundTile(glm::vec3 pos)
		: TileBase(pos) {
		type = "groundtile";
    }
    void OnCubeEnter() override {
        cout << "Enter the " << type << endl;
    }
    void OnCubeStay() override {
        cout << "Stay in " << type << endl;
    }
    void OnCubeExit() override {
        cout << "Exit from " << type << endl;
    }
    void Update(float dt) override {
    }
};


class GoToOneTile : public TileBase {
public:
    GoToOneTile(glm::vec3 pos)
        : TileBase(pos) {
        type = "GoToOneTile";
    }
    void OnCubeEnter() override {
        cout << "Enter the " << type << endl;
    }
    void OnCubeStay() override {
        cout << "Stay in " << type << endl;
    }
    void OnCubeExit() override {
        cout << "Exit from " << type << endl;
    }
    void Update(float dt) override {
    }
};


class GoToTwoTile : public TileBase {
public:
    GoToTwoTile(glm::vec3 pos)
        : TileBase(pos) {
        type = "GoToTwoTile";
    }
    void OnCubeEnter() override {
        cout << "Enter the " << type << endl;
    }
    void OnCubeStay() override {
        cout << "Stay in " << type << endl;
    }
    void OnCubeExit() override {
        cout << "Exit from " << type << endl;
    }
    void Update(float dt) override {
    }
};

class QuitTile : public TileBase {
public:
    QuitTile(glm::vec3 pos)
        : TileBase(pos) {
        type = "QuitTile";
    }
    void OnCubeEnter() override {
        cout << "Enter the " << type << endl;
    }
    void OnCubeStay() override {
        cout << "Stay in " << type << endl;
    }
    void OnCubeExit() override {
        cout << "Exit from " << type << endl;
    }
    void Update(float dt) override {
    }
};

// 스위치 땅 타일
class SwitchTile : public TileBase {
public:
    bool switched_map = false;
    SwitchTile(glm::vec3 pos)
        : TileBase(pos) {
        type = "switchtile";
    }
    void OnCubeEnter() override {
        cout << "Enter the " << type << endl;
        if (switched_map == true) return;
        switched_map = true;
    }
    void OnCubeStay() override {
        cout << "Stay in " << type << endl;
    }
    void OnCubeExit() override {
        cout << "Exit from " << type << endl;
    }
    void Update(float dt) override {
        if (!switched_map)
    }
};

// 움직이는 땅 타일
class MoveTile : public TileBase {
public:
    vector<glm::vec3> moveCommend; // 움직임에 대한 명령어들을 모아놓은 벡터
	int currentMoveIndex = 0; // 현재 수행 중인 명령어 인덱스
	float speed = 0.5f; // 이동 속도
	bool moving = true; // 현재 움직이고 있는지 여부
    glm::vec3 previousPosition; // 이전 프레임의 위치
    glm::vec3 movementDelta; // 이번 프레임의 이동량
    float waitTime = 1.0f; // 각 목표 지점에서 대기할 시간 (초)
    float currentWaitTime = 0.0f; // 현재 대기한 시간
    bool isWaiting = false; // 현재 대기 중인지 여부
    
    MoveTile(glm::vec3 pos)
        : TileBase(pos), previousPosition(pos), movementDelta(0.0f) {
        type = "movetile";
    }
    void OnCubeEnter() override {
        cout << "Enter the " << type << endl;
    }
    void OnCubeStay() override {
        cout << "Stay in " << type << endl;
    }
    void OnCubeExit() override {
        cout << "Exit from " << type << endl;
    }
    void add_movement(glm::vec3 moving) {
		moveCommend.push_back(moving);
    }
	// 구조를 설명하자면 moveCommend 벡터에 이동할 위치들을 순서대로 넣어놓고 
    // Update 함수에서 그 위치들로 순차적으로 이동하는 방식임
    void Update(float dt) override{
		if (moveCommend.empty()) {
            movementDelta = glm::vec3(0.0f);
            return;
        }

        previousPosition = position;
        
        // 대기 중이면 대기 시간 증가
        if (isWaiting) {
            currentWaitTime += dt;
            movementDelta = glm::vec3(0.0f); // 대기 중에는 움직이지 않음
            
            // 대기 시간이 끝나면 다음 목표로 이동
            if (currentWaitTime >= waitTime) {
                isWaiting = false;
                currentWaitTime = 0.0f;
                currentMoveIndex = (currentMoveIndex + 1) % moveCommend.size();
            }
            return;
        }

        // 이동 중
		glm::vec3 targetPos = moveCommend[currentMoveIndex];
        glm::vec3 direction = targetPos - position;
        float distanceToTarget = glm::length(direction);
        
        // 목표 위치에 도달했는지 확인
        if (distanceToTarget < 0.1f) {
            // 목표 지점에 정확히 위치시키기
            position = targetPos;
            movementDelta = position - previousPosition;
            
            // 대기 상태로 전환
            isWaiting = true;
            currentWaitTime = 0.0f;
        } else {
            // 계속 이동
            position += glm::normalize(direction) * speed;
            movementDelta = position - previousPosition;
        }
    }
};

// 회전하는 땅 타일
class RotateTile : public TileBase {
public:
    RotateTile(glm::vec3 pos)
        : TileBase(pos) {
        type = "rotatetile";
    }
    void OnCubeEnter() override {
        cout << "Enter the " << type << endl;
    }
    void OnCubeStay() override {
        cout << "Stay in " << type << endl;
    }
    void OnCubeExit() override {
        cout << "Exit from " << type << endl;
    }
    void Update(float dt) override {
    }
};

class BackgroundTile : public TileBase {
public:
    BackgroundTile(glm::vec3 pos)
        : TileBase(pos) {
		type = "background";
        addRotation(0, 45, 0);
    }
    void OnCubeEnter() override {
        cout << "Enter the " << type << endl;
    }
    void OnCubeStay() override {
        cout << "Stay in " << type << endl;
    }
    void OnCubeExit() override {
        cout << "Exit from " << type << endl;
    }
    void Update(float dt) override {
	}
};
class EndTile : public TileBase {
public:
    EndTile(glm::vec3 pos)
        : TileBase(pos) {
        type = "endtile";
    }
    void OnCubeEnter() override {
        cout << "Enter the " << type << endl;
        fireworkmanager.createFirework(position);
    }
    void OnCubeStay() override {
        cout << "Stay in " << type << endl;
    }
    void OnCubeExit() override {
        cout << "Exit from " << type << endl;
    }
    void Update(float dt) override {
    }
};

class MakeTile : public TileBase {
public:
    MakeTile()
        : TileBase(glm::vec3(0.0f, 6.0f, 0.0f)) {
        color_type = 1;
    }
    void OnCubeEnter() override {
    }
    void OnCubeStay() override {
    }
    void OnCubeExit() override {
    }
    void switching_make_tile(int key) {
        switch (key)
        {
        case GLUT_KEY_F1: 
            type = "groundtile";  
			texture = &ground_cube_texture;
            model = &public_cube;
            color_type = 1;
            break;
		case GLUT_KEY_F2: 
            type = "GoToOneTile";  
			texture = &One_cube_texture;
            model = &stage_cube;
            color_type = 0;
            break;
		case GLUT_KEY_F3: 
            type = "switchtile"; 
			texture = &switch_cube_texture;
            model = &public_cube;
            color_type = 0;
            break;
		case GLUT_KEY_F4: 
            type = "movetile";    
			texture = &moving_cube_texture;
            model = &public_cube;
            color_type = 0;
            break;
		case '1':
            type = "endtile";
            texture = &BackGround_cube_texture;
            model = &stage_cube;
            color_type = 0;
            break;
        case '2':
            type = "GoToTwoTile";
            texture = &Two_cube_texture;
            model = &stage_cube;
            color_type = 0;
            break;
        case '3':
            type = "QuitTile";
            texture = &quit_texture;
            model = &stage_cube;
            color_type = 0;
            break;
        }
    }
};

// TileManager 클래스 추가 이건 json 저장 및 불러오기 기능 포함
class TileManager {
public:
    vector<TileBase*> tiles;
	MakeTile make_tile;
    glm::vec3 playerPos = glm::vec3{ 0,0,0 };
    int gridWidth = 10;
    int gridHeight = 10;
    float tileSize = 2.0f; // 타일 하나의 크기
	bool editing_mode = true;
    bool making_move_tile = false;
	TileBase* selected_move_tile = nullptr;

    // 플레이어가 현재 서있는 타일을 찾는 함수
    TileBase* GetTileUnderPlayer(glm::vec3 playerPos) {
        float tileHalfSize = 1.0f;
        
        // 플레이어의 바닥 위치 (플레이어는 중심이 position이고, 크기가 2x2x2 큐브)
        float playerBottomY = playerPos.y - 1.0f;

        for (auto* tile : tiles) {
            if (tile->type == "background") continue;
            
            float dx = abs(playerPos.x - tile->position.x);
            float dz = abs(playerPos.z - tile->position.z);
            
            // XZ 평면에서 플레이어가 타일 위에 있는지 확인
            if (dx < tileHalfSize && dz < tileHalfSize) {
                float tileTop = tile->position.y + tileHalfSize;
                
                // 플레이어 바닥이 타일 표면 바로 위에 있는지 확인
                float heightDiff = abs(playerBottomY - tileTop);
                if (heightDiff < 0.5f) {
                    return tile;
                }
            }
        }
        return nullptr;
    }

	// 타일 만드는 함수
    void tile_make() {
        for (auto* tile : tiles) {
            if (tile->type == "background") continue;
            if (tile->position == make_tile.position) {
                cout << "거기 못 둔다." << endl;
                return;
            }
        }
		glm::vec3 pos = make_tile.position;
        if (make_tile.type == "groundtile") {
            GroundTile* tile = new GroundTile(pos);
            tile->InitializeRendering(&public_cube, &ground_cube_texture);
			tile->color_type = 1;
            tiles.push_back(tile);
        }
        else if (make_tile.type == "GoToOneTile") {
            GoToOneTile* tile = new GoToOneTile(pos);
            tile->InitializeRendering(&stage_cube, &One_cube_texture);
            tiles.push_back(tile);
        }
        else if (make_tile.type == "GoToTwoTile") {
            GoToTwoTile* tile = new GoToTwoTile(pos);
            tile->InitializeRendering(&stage_cube, &Two_cube_texture);
            tiles.push_back(tile);
        }
        else if (make_tile.type == "QuitTile") {
            QuitTile* tile = new QuitTile(pos);
            tile->InitializeRendering(&stage_cube, &quit_texture);
            tiles.push_back(tile);
        }
        else if (make_tile.type == "switchtile") {
            SwitchTile* tile = new SwitchTile(pos);
            tile->InitializeRendering(&public_cube, &switch_cube_texture);
            tiles.push_back(tile);
        }
        else if (make_tile.type == "movetile") {
            MoveTile* tile = new MoveTile(pos);
            tile->InitializeRendering(&public_cube, &moving_cube_texture);
            tiles.push_back(tile);
		}
        else if (make_tile.type == "endtile") {
            EndTile* tile = new EndTile(pos);
            tile->InitializeRendering(&public_cube, &BackGround_cube_texture);
            tiles.push_back(tile);
        }
        else {
            RotateTile* tile = new RotateTile(pos);
            tile->InitializeRendering(&public_cube, &rotate_cube_texture);
            tiles.push_back(tile);
        }
    }
	// 타일 삭제 함수
    void delete_tile() {
        for (auto it = tiles.begin(); it != tiles.end(); ) {
            if ((*it)->type == "background") {
                ++it;
                continue;
            }
            if ((*it)->position == make_tile.position) {
                delete* it;  // 메모리 해제
                it = tiles.erase(it);  // 벡터에서 제거 후 iterator 갱신
                cout << "타일 삭제됨: " << make_tile.position.x << ", "
                    << make_tile.position.y << ", " << make_tile.position.z << endl;
                return;  // 하나만 삭제하고 종료
            }
            else {
                ++it;
            }
        }
        cout << "해당 위치에 타일이 없습니다." << endl;
    }
    // 타일 포인트박스 생성 함수
    void make_init() {
		make_tile.InitializeRendering(&public_cube, &ground_cube_texture);
		make_tile.type = "groundtile";
    }
    // 움직이는 타일 동선 추가 함수 
    void move_tile_add_command() {
        if (!making_move_tile) {
            for (auto& t : tiles) {
                if (t->type != "movetile") continue;

                if (make_tile.position == t->position) {
                    MoveTile* moveTile = dynamic_cast<MoveTile*>(t);
                    if (!moveTile) continue;  // 캐스팅 실패 시 건너뜀
                    selected_move_tile = moveTile;  // MoveTile*로 저장
                    moveTile->moveCommend.clear();
                    making_move_tile = true;

                    
					move_cube_line.vertices.clear();
					move_cube_line.vertices.push_back({ moveTile->position, glm::vec4{1.0f, 0.0f, 0.0f, 1.0f} });
					cout <<  move_cube_line.vbo << endl;
                    move_cube_line.Update();
                }
            }
        }
        else {
            MoveTile* moveTile = dynamic_cast<MoveTile*>(selected_move_tile);
            if (!moveTile) {
                cout << "ERROR: selected_move_tile이 MoveTile이 아닙니다!" << endl;
                making_move_tile = false;
                return;
            }
            moveTile->add_movement(make_tile.position);
            move_cube_line.vertices.push_back({ make_tile.position, glm::vec4{0.0f, 0.0f, 0.0f, 1.0f} });
            move_cube_line.Update();
            cout << move_cube_line.vbo << endl;
        }
    }

    void GenerateGrid() {
        Clear();
        tiles.reserve(gridWidth * gridHeight + 1);
        for (int z = 0; z < gridHeight; ++z) {
            for (int x = 0; x < gridWidth; ++x) {
                glm::vec3 pos = glm::vec3(
                    x * tileSize - (gridWidth * tileSize / 2.0f),
                    0.0f,
                    z * tileSize - (gridHeight * tileSize / 2.0f)
                );
                GroundTile* tile = new GroundTile(pos);
                tile->InitializeRendering(&public_cube, &ground_cube_texture);
                tiles.push_back(tile);
            }
        }
        make_init();
    }
    void GenerateGrid_harf() {
        Clear();
        tiles.reserve(gridWidth * gridHeight + 1);
        for (int z = 0; z < gridHeight; ++z) {
            for (int x = 0; x < gridWidth; ++x) {
                glm::vec3 pos = glm::vec3(
                    x * tileSize - (gridWidth * tileSize / 2.0f),
                    0.0f,
                    z * tileSize - (gridHeight * tileSize / 2.0f)
                );
                GroundTile* tile = new GroundTile(pos);
                tile->InitializeRendering(&harf_cube, &ground_cube_texture);
				tile->add_position(0.0f, 0.25f, 0.0f);
                tiles.push_back(tile);
            }
        }
    }
    void GenerateBackground() {
        glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
        BackgroundTile* backgroundTile = new BackgroundTile(pos);
        backgroundTile->InitializeRendering(&BackGround_cube, &BackGround_cube_texture);
        tiles.push_back(backgroundTile);
    }
    void GenerateMoveTile() {
    }

    void SaveToJSON(const char* filepath) {
        ofstream file(filepath);
        file << "{\n";
        file << "  \"playerPos\": [" << (int)make_tile.position.x << ", " << (int)make_tile.position.y << ", " << (int)make_tile.position.z << "],\n";
        file << "  \"gridWidth\": " << gridWidth << ",\n";
        file << "  \"gridHeight\": " << gridHeight << ",\n";
        file << "  \"tileSize\": " << tileSize << ",\n";
        file << "  \"tiles\": [\n";

        for (int i = 0; i < tiles.size(); ++i) {
            file << "    {\n";
            file << "      \"x\": " << (int)tiles[i]->position.x << ",\n";
            file << "      \"y\": " << (int)tiles[i]->position.y << ",\n";
            file << "      \"z\": " << (int)tiles[i]->position.z << ",\n";
            file << "      \"type\": \"" << tiles[i]->type << "\"";

            // MoveTile인 경우 이동 경로 저장
            if (tiles[i]->type == "movetile") {
                MoveTile* moveTile = dynamic_cast<MoveTile*>(tiles[i]);
                if (moveTile && !moveTile->moveCommend.empty()) {
                    file << ",\n      \"moveCommands\": [\n";
                    for (int j = 0; j < moveTile->moveCommend.size(); ++j) {
                        file << "        { \"x\": " << (int)moveTile->moveCommend[j].x
                            << ", \"y\": " << (int)moveTile->moveCommend[j].y
                            << ", \"z\": " << (int)moveTile->moveCommend[j].z << " }";
                        if (j < moveTile->moveCommend.size() - 1) file << ",\n";
                        else file << "\n";
                    }
                    file << "      ]";
                }
                file << ",\n      \"speed\": " << moveTile->speed;
            }

            file << "\n    }";
            if (i < tiles.size() - 1) file << ",";
            file << "\n";
        }

        file << "  ]\n";
        file << "}\n";
        file.close();
        cout << "Stage saved to " << filepath << endl;
    }
    void LoadFromJSON(const char* filepath) {
        Clear();

        ifstream file(filepath);
        if (!file.is_open()) {
            std::cout << "파일 열기 실패: " << filepath << std::endl;
            return;
        }

        string line;
        float x = 0, y = 0, z = 0;
        float speed = 1.0f;
        string tileType;
        vector<glm::vec3> moveCommands;
        bool readingTiles = false;
        bool readingMoveCommands = false;
        int braceDepth = 0;  // { } 깊이 추적
        bool inTileObject = false;  // 타일 객체 내부 여부

        while (getline(file, line)) {
            // 기본 설정 파싱
            if (line.find("\"playerPos\":") != string::npos) {
                size_t start = line.find("[");
                size_t end = line.find("]");
                if (start != string::npos && end != string::npos) {
                    string arr = line.substr(start + 1, end - start - 1);
                    float px = 0, py = 0, pz = 0;
                    sscanf(arr.c_str(), "%f, %f, %f", &px, &py, &pz);
                    playerPos = glm::vec3(px, py, pz);
                }
            }
            else if (line.find("\"gridWidth\":") != string::npos) {
                size_t pos = line.find(":");
                gridWidth = stoi(line.substr(pos + 1));
            }
            else if (line.find("\"gridHeight\":") != string::npos) {
                size_t pos = line.find(":");
                gridHeight = stoi(line.substr(pos + 1));
            }
            else if (line.find("\"tileSize\":") != string::npos) {
                size_t pos = line.find(":");
                tileSize = stof(line.substr(pos + 1));
            }

            if (line.find("\"tiles\":") != string::npos) {
                readingTiles = true;
                continue;
            }

            if (!readingTiles) continue;

            // 타일 배열 끝 감지
            if (line.find("]") != string::npos && !inTileObject) {
                break;  // 타일 배열 파싱 완료
            }

            // { 카운팅
            for (size_t i = 0; i < line.length(); ++i) {
                if (line[i] == '{') {
                    braceDepth++;
                    if (braceDepth == 1) {
                        // 새로운 타일 객체 시작
                        inTileObject = true;
                        moveCommands.clear();
                        x = 0;
                        y = 0;
                        z = 0;
                        speed = 1.0f;
                        tileType = "";
                    }
                }
                else if (line[i] == '}') {
                    braceDepth--;
                    if (braceDepth == 0 && inTileObject) {
                        //타일 객체 끝: 타일 생성
                        inTileObject = false;

                        glm::vec3 pos(x, y, z);
                        TileBase* tile = nullptr;

                        if (tileType == "groundtile") {
                            tile = new GroundTile(pos);
                            tile->InitializeRendering(&public_cube, &ground_cube_texture);
							tile->color_type = 1;
                        }
                        else if (tileType == "GoToOneTile") {
                            tile = new GoToOneTile(pos);
                            tile->InitializeRendering(&public_cube, &One_cube_texture);
                            tile->color_type = 0;
                        }
                        else if (tileType == "GoToTwoTile") {
                            tile = new GoToTwoTile(pos);
                            tile->InitializeRendering(&public_cube, &Two_cube_texture);
                            tile->color_type = 0;
                        }
                        else if (tileType == "QuitTile") {
                            tile = new QuitTile(pos);
                            tile->InitializeRendering(&public_cube, &quit_texture);
                            tile->color_type = 0;
                        }
                        else if (tileType == "switchtile") {
                            tile = new SwitchTile(pos);
                            tile->InitializeRendering(&public_cube, &switch_cube_texture);
                            tile->color_type = 0;
                        }
                        else if (tileType == "endtile") {
                            tile = new EndTile(pos);
                            tile->InitializeRendering(&public_cube, &BackGround_cube_texture);
                            tile->color_type = 0;
                        }
                        else if (tileType == "movetile") {
                            MoveTile* moveTile = new MoveTile(pos);
                            moveTile->InitializeRendering(&public_cube, &moving_cube_texture);
							moveTile->color_type = 0;
                            moveTile->speed = speed;

                            // 저장된 이동 경로 복원
                            for (const auto& cmd : moveCommands) {
                                moveTile->add_movement(cmd);
                            }

                            std::cout << "MoveTile 로드: 위치(" << pos.x << ", " << pos.y << ", " << pos.z
                                << "), 경로 개수(" << moveTile->moveCommend.size() << ")" << std::endl;

                            tile = moveTile;
                        }
                        else if (tileType == "background") {
                            tile = new BackgroundTile(pos);
                            tile->InitializeRendering(&BackGround_cube, &BackGround_cube_texture);
                        }

                        if (tile) {
                            tiles.push_back(tile);
                        }

                        moveCommands.clear();
                    }
                }
            }

            if (!inTileObject) continue;  // 타일 객체 외부에서는 파싱 스킵

            // 이동 명령어 파싱 시작
            if (line.find("\"moveCommands\":") != string::npos) {
                readingMoveCommands = true;
                continue;
            }

            // 이동 명령어 파싱 중
            if (readingMoveCommands) {
                if (line.find("]") != string::npos) {
                    readingMoveCommands = false;
                }
                else if (line.find("{") != string::npos) {
                    float cmdX = 0, cmdY = 0, cmdZ = 0;

                    size_t posX = line.find("\"x\":");
                    if (posX != string::npos) {
                        cmdX = stof(line.substr(posX + 4));
                    }

                    size_t posY = line.find("\"y\":");
                    if (posY != string::npos) {
                        cmdY = stof(line.substr(posY + 4));
                    }

                    size_t posZ = line.find("\"z\":");
                    if (posZ != string::npos) {
                        cmdZ = stof(line.substr(posZ + 4));
                    }

                    moveCommands.push_back(glm::vec3(cmdX, cmdY, cmdZ));
                    std::cout << "이동 명령어 로드: (" << cmdX << ", " << cmdY << ", " << cmdZ << ")" << std::endl;
                }
                continue;
            }

            // 타일 정보 파싱
            if (line.find("\"x\":") != string::npos && !readingMoveCommands) {
                size_t pos = line.find(":");
                x = stof(line.substr(pos + 1));
            }
            else if (line.find("\"y\":") != string::npos && !readingMoveCommands) {
                size_t pos = line.find(":");
                y = stof(line.substr(pos + 1));
            }
            else if (line.find("\"z\":") != string::npos && !readingMoveCommands) {
                size_t pos = line.find(":");
                z = stof(line.substr(pos + 1));
            }
            else if (line.find("\"speed\":") != string::npos) {
                size_t pos = line.find(":");
                speed = stof(line.substr(pos + 1));
            }
            else if (line.find("\"type\":") != string::npos) {
                size_t first = line.find("\"", line.find(":") + 1);
                size_t second = line.find("\"", first + 1);
                tileType = line.substr(first + 1, second - first - 1);
            }
        }

        file.close();

        std::cout << "로드 완료: " << tiles.size() << "개 타일" << std::endl;
        for (auto& t : tiles) {
            if (t->type == "movetile") {
                MoveTile* moveTile = dynamic_cast<MoveTile*>(t);
                std::cout << "MoveTile 확인: 경로 개수 = " << moveTile->moveCommend.size() << std::endl;
            }
        }
        
        make_init();
    }

    void UpdateALl(float dt) {
        for (auto* tile : tiles) {
            tile->Update(dt);
		}
    }
    // 일반 화면 출력하는 거
    void DrawAll(Camera& cam) {
        for (auto* tile : tiles) {
            GLuint u;
            if (tile->type == "background") {
                glFrontFace(GL_CW);
                //u = glGetUniformLocation(shaderProgramID, "turn_off");
                //glUniform1i(u, 1);
            }
            tile->result_matrix(cam);
            tile->Draw();
            glFrontFace(GL_CCW);
            //u = glGetUniformLocation(shaderProgramID, "turn_off");
            //glUniform1i(u, 0);
        }
		make_tile.result_matrix(cam);
		make_tile.Draw();
    }

    // 미니맵에 쓰는 함수
    void DrawAll_O(Camera& cam) {
        for (auto* tile : tiles) {
            tile->result_O_matrix(cam);
            tile->Draw();
        }
    }

    void CubeEnter(glm::vec3 pos) {
        for (TileBase* t : tiles) {
            if (t->position == pos) {
                t->OnCubeEnter();
            }
        }
    }

    void CubeStay(glm::vec3 pos) {
        for (TileBase* t : tiles) {
            if (t->position == pos) {
                t->OnCubeStay();
            }
        }
    }

    void CubeExit(glm::vec3 pos) {
        for (TileBase* t : tiles) {
            if (t->position == pos) {
                t->OnCubeExit();
            }
        }
    }

    void Clear() {
        for (auto* tile : tiles) {
            delete tile;
        }
        tiles.clear();
    }

    ~TileManager() {
        Clear();
    }
};
TileManager tileManager;