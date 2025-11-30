#pragma once
#include "gameobject.h"

// 전방 선언
class PlayerCube;

// 타일의 기본 클래스
class TileBase : public GameObject {
public:
    TileBase(glm::vec3 pos)
        : GameObject(pos) {
    }
    string type;
    // 플레이어 큐브가 타일에 들어갔을 때 호출되는 함수
    virtual void OnCubeEnter(PlayerCube* cube) {}
    // 플레이어 큐브가 타일위에 머무를 때 호출되는 함수    
    virtual void OnCubeStay(PlayerCube* cube) {}
    // 플레이어 큐브가 타일에서 나갔을 때 호출되는 함수
    virtual void OnCubeExit(PlayerCube* cube) {}
};

// 기본 땅 타일
class GroundTile : public TileBase {
public:
    GroundTile(glm::vec3 pos)
		: TileBase(pos) {
		type = "groundtile";
    }
    void OnCubeEnter(PlayerCube* cube) override {
    }
    void OnCubeStay(PlayerCube* cube) override {
    }
    void OnCubeExit(PlayerCube* cube) override {
    }
    void Update(float dt) override {
    }
};

// 스프링 땅 타일
class SpringTile : public TileBase {
public:
    SpringTile(glm::vec3 pos)
        : TileBase(pos) {
        type = "springtile";
    }
    void OnCubeEnter(PlayerCube* cube) override {
    }
    void OnCubeStay(PlayerCube* cube) override {
    }
    void OnCubeExit(PlayerCube* cube) override {
    }
    void Update(float dt) override {
    }
};

// 스프링 땅 타일
class SwitchTile : public TileBase {
public:
    SwitchTile(glm::vec3 pos)
        : TileBase(pos) {
        type = "switchtile";
    }
    void OnCubeEnter(PlayerCube* cube) override {
    }
    void OnCubeStay(PlayerCube* cube) override {
    }
    void OnCubeExit(PlayerCube* cube) override {
    }
    void Update(float dt) override {
    }
};

// 움직이는 땅 타일
class MoveTile : public TileBase {
public:
    vector<glm::vec3> moveCommend; // 움직임에 대한 명령어들을 모아놓은 벡터
	int currentMoveIndex = 0; // 현재 수행 중인 명령어 인덱스
	float speed = 1.0f; // 이동 속도
	bool moving = true; // 현재 움직이고 있는지 여부
    MoveTile(glm::vec3 pos)
        : TileBase(pos) {
        type = "movetile";
    }
    void OnCubeEnter(PlayerCube* cube) override {
    }
    void OnCubeStay(PlayerCube* cube) override {
        //player.position = position;
    }
    void OnCubeExit(PlayerCube* cube) override {
    }
    void add_movement(glm::vec3 moving) {
		moveCommend.push_back(moving);
    }
	// 구조를 설명하자면 moveCommend 벡터에 이동할 위치들을 순서대로 넣어놓고 
    // Update 함수에서 그 위치들로 순차적으로 이동하는 방식임
    void Update(float dt) override{
		if (moveCommend.empty()) return;

		glm::vec3 targetPos = moveCommend[currentMoveIndex];
		position += glm::normalize(targetPos - position) * speed;
        
		// 목표 위치에 도달했는지 확인
        if (glm::length(position - targetPos) < 0.1f) {
			currentMoveIndex = (currentMoveIndex + 1) % moveCommend.size();
        }
    }
};

// 회전하는 땅 타일
class RotateTile : public TileBase {
public:
    RotateTile(glm::vec3 pos)
        : TileBase(pos) {
        type = "movetile";
    }
    void OnCubeEnter(PlayerCube* cube) override {
    }
    void OnCubeStay(PlayerCube* cube) override {
    }
    void OnCubeExit(PlayerCube* cube) override {
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
    void OnCubeEnter(PlayerCube* cube) override {
    }
    void OnCubeStay(PlayerCube* cube) override {
    }
    void OnCubeExit(PlayerCube* cube) override {
    }
    void Update(float dt) override {
	}
};

class MakeTile : public TileBase {
public:
    MakeTile()
        : TileBase(glm::vec3(0.0f, 6.0f, 0.0f)) {}
    void OnCubeEnter(PlayerCube* cube) override {
    }
    void OnCubeStay(PlayerCube* cube) override {
    }
    void OnCubeExit(PlayerCube* cube) override {
    }
    void switching_make_tile(int key) {
        switch (key)
        {
        case GLUT_KEY_F1: 
            type = "groundtile";  
			texture = &ground_cube_texture;
            break;
		case GLUT_KEY_F2: 
            type = "springtile";  
			texture = &spring_cube_texture;
            break;
		case GLUT_KEY_F3: 
            type = "switchtile"; 
			texture = &switch_cube_texture;
            break;
		case GLUT_KEY_F4: 
            type = "movetile";    
			texture = &moving_cube_texture;
            break;
        }
        cout << "현재 선택된 타일: " << type << endl;
    }
};

// TileManager 클래스 추가 이건 json 저장 및 불러오기 기능 포함
class TileManager {
public:
    vector<TileBase*> tiles;
	MakeTile make_tile;
    int gridWidth = 20;
    int gridHeight = 20;
    float tileSize = 2.0f; // 타일 하나의 크기
	bool editing_mode = true;
    bool making_move_tile = false;
	TileBase* selected_move_tile = nullptr;

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
            tiles.push_back(tile);
        }
        else if (make_tile.type == "springtile") {
            SpringTile* tile = new SpringTile(pos);
            tile->InitializeRendering(&public_cube, &spring_cube_texture);
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
					move_cube_line.vertices.push_back({ moveTile->position, glm::vec4{0.0f, 0.0f, 0.0f, 1.0f} });
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
        file << "  \"gridWidth\": " << gridWidth << ",\n";
        file << "  \"gridHeight\": " << gridHeight << ",\n";
        file << "  \"tileSize\": " << tileSize << ",\n";
        file << "  \"tiles\": [\n";

        for (int i = 0; i < tiles.size(); ++i) {
            file << "    {\n";
            file << "      \"x\": " << tiles[i]->position.x << ",\n";
            file << "      \"y\": " << tiles[i]->position.y << ",\n";
            file << "      \"z\": " << tiles[i]->position.z << ",\n";
            file << "      \"type\": \"" << tiles[i]->type << "\"\n";
        }

        file << "  ]\n";
        file << "}\n";
        file.close();
		cout << "Stage saved to " << filepath << endl;
    }
    void LoadFromJSON(const char* filepath) {
        Clear();  // 기존 타일 모두 삭제

        ifstream file(filepath);
        if (!file.is_open()) {
            cout << "파일 열기 실패: " << filepath << endl;
            return;
        }

        string line;
        float x = 0, y = 0, z = 0;
        string tileType;
        bool readingTiles = false;

        while (getline(file, line)) {
            // gridWidth, gridHeight, tileSize 파싱 (필요시)
            if (line.find("\"gridWidth\":") != string::npos) {
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

            // 타일 배열 시작
            if (line.find("\"tiles\":") != string::npos) {
                readingTiles = true;
                continue;
            }

            if (!readingTiles) continue;

            // x, y, z, type 파싱
            if (line.find("\"x\":") != string::npos) {
                size_t pos = line.find(":");
                x = stof(line.substr(pos + 1));
            }
            else if (line.find("\"y\":") != string::npos) {
                size_t pos = line.find(":");
                y = stof(line.substr(pos + 1));
            }
            else if (line.find("\"z\":") != string::npos) {
                size_t pos = line.find(":");
                z = stof(line.substr(pos + 1));
            }
            else if (line.find("\"type\":") != string::npos) {
                // "type": "groundtile" 형태에서 타입 추출
                size_t first = line.find("\"", line.find(":") + 1);
                size_t second = line.find("\"", first + 1);
                tileType = line.substr(first + 1, second - first - 1);

                // 타일 생성 (type 읽은 직후)
                glm::vec3 pos(x, y, z);
                TileBase* tile = nullptr;

                if (tileType == "groundtile") {
                    tile = new GroundTile(pos);
                }
                else if (tileType == "springtile") {
                    tile = new SpringTile(pos);
                }
                else if (tileType == "switchtile") {
                    tile = new SwitchTile(pos);
                }
                else if (tileType == "movetile") {
                    tile = new MoveTile(pos);
                }
                else if (tileType == "background") {
                    tile = new BackgroundTile(pos);
                }

                if (tile) {
                    // 타일 타입에 따라 적절한 렌더링 설정
                    if (tileType == "background") {
                        tile->InitializeRendering(&BackGround_cube, &BackGround_cube_texture);
                    }
                    else if (tileType == "groundtile"){
                        tile->InitializeRendering(&public_cube, &ground_cube_texture);
                    }
                    else if (tileType == "springtile") {
                        tile->InitializeRendering(&public_cube, &spring_cube_texture);
                    }
                    else if (tileType == "movetile") {
                        tile->InitializeRendering(&public_cube, &moving_cube_texture);
                    }
                    else if (tileType == "rotatetile") {
                        tile->InitializeRendering(&public_cube, &rotate_cube_texture);
                    }
                    else {
                        tile->InitializeRendering(&public_cube, &switch_cube_texture);
					}
                    tiles.push_back(tile);
                }
            }
        }

        file.close();
        make_init();  // make_tile 초기화
        cout << "로드 완료: " << tiles.size() << "개 타일" << endl;
    }

    void UpdateALl(float dt) {
        for (auto* tile : tiles) {
            tile->Update(dt);
		}
    }
    void DrawAll(Camera& cam) {
        for (auto* tile : tiles) {
            if (tile -> type == "background") glFrontFace(GL_CW);
            tile->result_matrix(cam);
            tile->Draw();
            glFrontFace(GL_CCW);
        }
		make_tile.result_matrix(cam);
		make_tile.Draw();
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