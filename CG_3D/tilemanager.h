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
};

// 움직이는 땅 타일
class MoveTile : public TileBase {
public:
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
    void movement() {
		// 움직임 로직 구현
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
};

// TileManager 클래스 추가 이건 json 저장 및 불러오기 기능 포함
class TileManager {
public:
    vector<TileBase*> tiles;

    int gridWidth = 20;
    int gridHeight = 20;
    float tileSize = 2.0f; // 타일 하나의 크기

    void GenerateGrid() {
        tiles.clear();
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
    }
    void GenerateGrid_harf() {
        tiles.clear();
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

        for (size_t i = 0; i < tiles.size(); ++i) {
            file << "    {\n";
            file << "      \"x\": " << tiles[i]->position.x << ",\n";
            file << "      \"y\": " << tiles[i]->position.y << ",\n";
            file << "      \"z\": " << tiles[i]->position.z << ",\n";
            file << "      \"type\": " << tiles[i]->type << "\n";
            file << "    }" << (i < tiles.size() - 1 ? "," : "") << "\n";
        }

        file << "  ]\n";
        file << "}\n";
        file.close();
    }
    void LoadFromJSON(const char* filepath) {
        tiles.clear();
        ifstream file(filepath);
        if (!file.is_open()) {
            cout << "Failed to open: " << filepath << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            if (line.find("\"x\":") != string::npos) {
                float x, y, z;
                x = stof(line.substr(line.find(":") + 1));
                getline(file, line); // y 값 읽기
                y = stof(line.substr(line.find(":") + 1));
                getline(file, line); // z 값 읽기
                z = stof(line.substr(line.find(":") + 1));

                GroundTile* tile = new GroundTile(glm::vec3(x, y, z));
                tile->InitializeRendering(&public_cube, &ground_cube_texture);
                tiles.push_back(tile);
            }
        }
        file.close();
    }

    void DrawAll(Camera& cam) {
        for (auto* tile : tiles) {
            if (tile -> type == "background") glFrontFace(GL_CW);
            tile->result_matrix(cam);
            tile->Draw();
            glFrontFace(GL_CCW);
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
