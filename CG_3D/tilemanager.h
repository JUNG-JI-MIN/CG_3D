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

// 빈(비어있는) 타일: 렌더링 하지 않음
class EmptyTile : public TileBase {
public:
    EmptyTile(glm::vec3 pos) : TileBase(pos) {
        type = "empty";
        model = nullptr; // 렌더링 건너뜀
    }
    void OnCubeEnter(PlayerCube* cube) override {}
    void OnCubeStay(PlayerCube* cube) override {}
    void OnCubeExit(PlayerCube* cube) override {}
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

	// 마우스 월드 좌표를 그리드 인덱스로 변환
    bool WorldToGrid(const glm::vec3& world, int& outX, int& outZ) {
        float originX = -(gridWidth * tileSize / 2.0f);
        float originZ = -(gridHeight * tileSize / 2.0f);
        float localX = (world.x - originX);
        float localZ = (world.z - originZ);
        int gx = (int)floor(localX / tileSize);
        int gz = (int)floor(localZ / tileSize);
        if (gx < 0 || gx >= gridWidth || gz < 0 || gz >= gridHeight) return false;
        outX = gx;
        outZ = gz;
        return true;
    }
	// 그리드 인덱스를 월드 좌표로 변환
    glm::vec3 GridToWorld(int gx, int gz) {
        float originX = -(gridWidth * tileSize / 2.0f);
        float originZ = -(gridHeight * tileSize / 2.0f);
        float wx = originX + gx * tileSize + tileSize * 0.5f;
        float wz = originZ + gz * tileSize + tileSize * 0.5f;
        return glm::vec3(wx, 0.0f, wz);
    }
    bool IsValidGrid(int gx, int gz) {
        return gx >= 0 && gx < gridWidth && gz >= 0 && gz < gridHeight;
    }

    // 타입 설정 함수
    void SetTileTypeAt(int gx, int gz, const string& type) {
        if (!IsValidGrid(gx, gz)) return;
        int idx = gz * gridWidth + gx;
        if (idx < 0 || idx >= (int)tiles.size()) return;

        // remove existing tile at that grid index and replace with new one of requested type
        TileBase* old = tiles[idx];
        glm::vec3 pos = old->position;
        // delete only grid tiles (avoid deleting background if out of expected range)
        delete old;

        TileBase* newTile = nullptr;
        if (type == "groundtile") {
            newTile = new GroundTile(pos);
        }
        else if (type == "springtile") {
            newTile = new SpringTile(pos);
        }
        else if (type == "switchtile") {
            newTile = new SwitchTile(pos);
        }
        else if (type == "movetile") {
            newTile = new MoveTile(pos);
        }
        else {
            // default to ground
            newTile = new GroundTile(pos);
        }

		// empty 타일은 렌더링 하지 않음
        if (newTile->type != "empty") {
            newTile->InitializeRendering(&public_cube, &ground_cube_texture);
        }
        tiles[idx] = newTile;
    }

    void RemoveTileAt(int gx, int gz) {
        SetTileTypeAt(gx, gz, "empty");
    }

    void SaveToJSON(const char* filepath) {
        ofstream file(filepath);
        file << "{\n";
        file << "  \"gridWidth\": " << gridWidth << ",\n";
        file << "  \"gridHeight\": " << gridHeight << ",\n";
        file << "  \"tileSize\": " << tileSize << ",\n";
        file << "  \"tiles\": [\n";

        int count = gridWidth * gridHeight;
        for (int i = 0; i < count; ++i) {
            file << "    {\n";
            file << "      \"x\": " << tiles[i]->position.x << ",\n";
            file << "      \"y\": " << tiles[i]->position.y << ",\n";
            file << "      \"z\": " << tiles[i]->position.z << ",\n";
            file << "      \"type\": \"" << tiles[i]->type << "\"\n";
            file << "    }" << (i < count - 1 ? "," : "") << "\n";
        }

        file << "  ]\n";
        file << "}\n";
        file.close();
    }
    void LoadFromJSON(const char* filepath) {
        Clear();
        ifstream file(filepath);
        if (!file.is_open()) {
            cout << "Failed to open: " << filepath << endl;
            return;
        }
        GenerateGrid();
        file.clear();
        file.seekg(0, ios::beg);
        string line;
        int tileIndex = 0;
        while (getline(file, line)) {
            if (line.find("\"type\":") != string::npos) {
                size_t first = line.find('"', line.find(':'));
                size_t second = line.find('"', first + 1);
                if (first != string::npos && second != string::npos && second > first) {
                    string t = line.substr(first + 1, second - first - 1);
                    if (tileIndex < gridWidth * gridHeight) {
                        int gx = tileIndex % gridWidth;
                        int gz = tileIndex / gridWidth;
                        SetTileTypeAt(gx, gz, t);
                    }
                }
                ++tileIndex;
            }
        }
        file.close();
    }

    void DrawAll(Camera& cam) {
        for (auto* tile : tiles) {
            if (tile->type == "empty") continue;
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
