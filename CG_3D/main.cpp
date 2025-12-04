//이민제 커밋확인용
#include "player.h"
#include "tilemanager.h"

// 기존 filetobuf, TimerFunction 등은 그대로 사용
char* filetobuf(const char* file)
{
    FILE* fptr;
    long length;
    char* buf;
    fptr = fopen(file, "rb"); // Open file for reading
    if (!fptr) // Return NULL on failure
        return NULL;
    fseek(fptr, 0, SEEK_END); // Seek to the end of the file
    length = ftell(fptr); // Find out how many bytes into the file we are
    buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator
    fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file
    fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer
    fclose(fptr); // Close the file
    buf[length] = 0; // Null terminator
    return buf; // Return the buffer
}
void trace_player() {
    camera.target.x = player.position.x;
    camera.target.z = player.position.z;
    camera.position.x = player.position.x + camera.between_player_or_camera;
    camera.position.z = player.position.z + camera.between_player_or_camera;
	mini_camera.target = player.position;
	mini_camera.position.x = player.position.x;
    mini_camera.position.z = player.position.z;
}
void TimerFunction(int value) {
    line.xyz = tileManager.make_tile.position;
    float dt = 1.5f / 60.0f; // 60 FPS 기준 deltaTime



    // 플레이어 업데이트 먼저
    player.Update(dt);
    
    if (tileManager.switching) {
		player.position = tileManager.current_switch_tile->switch_position;
        tileManager.switching = false;
        light.light[1].position = player.position;
        light.player_position_update();
    }

    // 타일 매니저 업데이트 (한 번만 호출)
    if (!tileManager.editing_mode) {
        tileManager.UpdateALl(dt);
        
        // 플레이어가 움직이지 않을 때만 MoveTile과 함께 이동
        if (!player.Rolling && !player.Falling) {
            TileBase* currentTile = tileManager.GetTileUnderPlayer(player.position);
            if (currentTile && currentTile->type == "movetile") {
                MoveTile* moveTile = dynamic_cast<MoveTile*>(currentTile);
                if (moveTile) {
                    // 플레이어를 타일 중심의 정확한 위치로 스냅
                    player.position.x = moveTile->position.x;
                    player.position.z = moveTile->position.z;
                    player.position.y = moveTile->position.y + 2.0f; // 타일 표면(+1.0f) + 플레이어 반경(+1.0f)
                    
                    // MoveTile의 이동량도 적용
                    if (glm::length(moveTile->movementDelta) > 0.0001f) {
                        player.position += moveTile->movementDelta;
                    }
                }
            }
        }
    }
    
    fireworkmanager.update(dt);
    
    // 카메라 업데이트
    trace_player();
    
    glutPostRedisplay();  // 화면 다시 그리기
    glutTimerFunc(16, TimerFunction, 1);  // 다음 타이머 설정
}

// 입력 콜백들은 ImGuiManager의 Handle*를 우선 호출하도록 변경
void onKey(unsigned char key, int x, int y) {
    tileManager.make_tile.switching_make_tile(key);
    switch (key)
    {
    case 'e': {
        // 조명 활성화 여부를 셰이더에 전달
        light_off = !light_off;
        GLuint lightEnabledLoc = glGetUniformLocation(shaderProgramID, "turn_off");
        glUniform1i(lightEnabledLoc, light_off ? 0 : 1);
        cout << light_off << endl;
        break;
    }
    case 'l':
        camera.between_player_or_camera *= -1.0f;
        break;
    case 'q':
        exit(1);
        break;
    case 'a':
    case 'A':
        player.Rolling_in_the_deep(glm::vec3(-1.0f, 0.0f, 0.0f));
		break;
	case 'd':
	case 'D':
        player.Rolling_in_the_deep(glm::vec3(1.0f, 0.0f, 0.0f));
		break;
	case 'w':
    case 'W':
        player.Rolling_in_the_deep(glm::vec3(0.0f, 0.0f, -1.0f));
		break;
	case 's':
    case 'S':
        player.Rolling_in_the_deep(glm::vec3(0.0f, 0.0f,  1.0f));
        break;
	case '\r': 
        if (tileManager.making_move_tile) {
            tileManager.making_move_tile = false;
			tileManager.selected_tile = nullptr;
        }
        else tileManager.tile_make(); // 타일 만들기
        break; // 엔터키
    case 'r':
		tileManager.delete_tile(); // 타일 지우기
        break;
    case 'p':
		tileManager.move_tile_add_command(); // 타일 이동 명령 추가
        break;
    case 'o':
		tileManager.setting_switch_position(); // 스위치 타일 설정
        break;
	case ' ':
		tileManager.make_tile.position.y += 2.0f; // 높이 조절
        break; // 스페이스바
	case 'c': 
		tileManager.make_tile.position.y -= 2.0f; // 높이 조절
        break; // 'c' 키
    case 'm':
		tileManager.editing_mode = !tileManager.editing_mode;
    }
}

void onSpecialKey(int key, int x, int y) {
    tileManager.make_tile.switching_make_tile(key);
	
    switch (key)
    {
        case GLUT_KEY_UP:
			tileManager.make_tile.position.z -= 2.0f;
			break;
		case GLUT_KEY_DOWN:
            tileManager.make_tile.position.z += 2.0f;
			break;
		case GLUT_KEY_LEFT:
            tileManager.make_tile.position.x -= 2.0f;
            break;
		case GLUT_KEY_RIGHT:
            tileManager.make_tile.position.x += 2.0f;
			break;
		case GLUT_KEY_F5:
			tileManager.SaveToJSON("json/Mainmenu.json");
            break;
        case GLUT_KEY_F10:
            tileManager.SaveToJSON("json/stage.json");
            break;
        case GLUT_KEY_F6:
            tileManager.LoadFromJSON("json/stage.json");
            player.position = tileManager.playerPos;
            light.light[1].position = player.position;
            light.player_position_update();
			break;
        case GLUT_KEY_F7:
            tileManager.SaveToJSON("json/stage2.json");
			break;
        case GLUT_KEY_F8:
            tileManager.LoadFromJSON("json/stage2.json");
            player.position = tileManager.playerPos;
            light.light[1].position = player.position;
            light.player_position_update();
            break;
        case GLUT_KEY_F9:
            tileManager.LoadFromJSON("json/Mainmenu.json");
            player.position = tileManager.playerPos;
            light.light[1].position = player.position;
            light.player_position_update();
            break;
    }
}

void onSpecialKeyUp(int key, int x, int y) {}

void onMouse(int button, int state, int x, int y) {}

void onMouseMotion(int x, int y) {
}
void onMousePassiveMotion(int x, int y) {
}
void RoadTexture() {
    player_cube_texture.Load("resource/player_of_space.png");
    player2_cube_texture.Load("resource/player/player1.png");
    player3_cube_texture.Load("resource/player/player2.png");
    ground_cube_texture.Load("resource/tile/silver.png");
    One_cube_texture.Load("resource/tile/stage_tile.png");
	Two_cube_texture.Load("resource/tile/stage_tile.png");
    moving_cube_texture.Load("resource/tile/scaffolding.png");
    rotate_cube_texture.Load("resource/tile/silver.png");
    switch_cube_texture.Load("resource/tile/silver.png");
	quit_texture.Load("resource/tile/stage_tile.png");

    stage_cube.Init();
    public_cube.Init(); // 전역 변수로 선언된 큐브 모델 초기화
	harf_cube.Init();
	BackGround_cube_texture.Load("resource/space.png");
	BackGround_cube.Init();

	mini_camera.between_player_or_camera = 50.0f;
}

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
    srand(time(NULL));
    width = 1200; 
    height = 900;
    //--- 윈도우 생성하기
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("Example1");
    //--- GLEW 초기화하기
    glewExperimental = GL_TRUE;
    glewInit();
    //--- 세이더 읽어와서 세이더 프로그램 만들기: 사용자 정의함수 호출
    make_vertexShaders(); //--- 버텍스 세이더 만들기
    make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
    shaderProgramID = make_shaderProgram(); //--- 세이더 프로그램 만들기

	RoadTexture(); // 텍스쳐 로드 함수
	line.Init();
    move_cube_line.Init();
    player.InitializeRendering(&public_cube, &player_cube_texture);

	// 타일 매니저 초기화
    //tileManager.GenerateGrid();
    //tileManager.GenerateBackground();

	tileManager.LoadFromJSON("json/Mainmenu.json");

    light.Init();

    glutDisplayFunc(drawScene); //--- 출력 콜백 함수
    glutTimerFunc(16, TimerFunction, 1);  // 60 FPS
    glutKeyboardFunc(onKey); // 키보드
    glutSpecialFunc(onSpecialKey); // 특수키
    glutSpecialUpFunc(onSpecialKeyUp); // 떼기
    glutMouseFunc(onMouse); // 마우스 콜백 등록
    glutMotionFunc(onMouseMotion);
    glutPassiveMotionFunc(onMousePassiveMotion);
    glutReshapeFunc(Reshape);
    glutMainLoop();
}

// drawScene: ImGui 프레임/에디터 UI 추가
GLvoid drawScene() {
    // 기본 생성
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);
    // 뒤면 처리
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
	// 블렌딩 처리
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, width, height);
    player.result_matrix(camera);
    player.Draw();

	result_line_matrix(camera,line);
	line.Draw();

    // making_move_tile일 때만 그리기
    if (tileManager.making_move_tile && move_cube_line.vertices.size() >= 2) {
        move_cube_line.Update();
        result_line_matrix(camera, move_cube_line);
        move_cube_line.DDraw();
    }

	tileManager.DrawAll(camera);

    fireworkmanager.Draw(camera);

    
    glViewport(width * 4/5, height- width * 1 / 5, width * 1 / 5, width * 1 / 5);
    
    player.result_O_matrix(mini_camera);
    player.Draw();
    
    tileManager.DrawAll_O(mini_camera);
    
    glutSwapBuffers();
}

//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
    glViewport(0, 0, w, h);
}


void make_vertexShaders()
{
    GLchar* vertexSource;
    //--- 버텍스 세이더 읽어 저장하고 컴파일 하기
    //--- filetobuf: 사용자정의 함수로 텍스트를 읽어서 문자열에 저장하는 함수
    vertexSource = filetobuf("vertex.glsl");
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
        return;
    }
}
void make_fragmentShaders()
{
    GLchar* fragmentSource;
    //--- 프래그먼트 세이더 읽어 저장하고 컴파일하기
    fragmentSource = filetobuf("fragment.glsl"); // 프래그세이더 읽어오기
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << std::endl;
        return;
    }
}
void make_geometryShaders()
{
    geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

    // geometry.glsl 읽기
    std::ifstream shaderFile("geometry.glsl");
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    std::string shaderSource = shaderStream.str();
    const char* shaderCode = shaderSource.c_str();

    glShaderSource(geometryShader, 1, &shaderCode, NULL);
    glCompileShader(geometryShader);

    // 오류 체크
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
        std::cerr << "ERROR: Geometry Shader Compile Failed\n" << infoLog << std::endl;
    }
}

GLuint make_shaderProgram()
{
    GLint result;
    GLchar* errorLog = NULL;
    GLuint shaderID;
    shaderID = glCreateProgram(); //--- 세이더 프로그램 만들기
    glAttachShader(shaderID, vertexShader); //--- 세이더 프로그램에 버텍스 세이더 붙이기
    glAttachShader(shaderID, fragmentShader); //--- 세이더 프로그램에 프래그먼트 세이더 붙이기
    glAttachShader(shaderID, geometryShader); //--- 세이더 프로그램에 기하 쉐이더 붙이기
    glLinkProgram(shaderID); //--- 세이더 프로그램 링크하기
    
    glDeleteShader(vertexShader); //--- 세이더 객체를 세이더 프로그램에 링크했음으로, 세이더 객체 자체는 삭제 가능
    glDeleteShader(fragmentShader);
    glDeleteShader(geometryShader);

    glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---세이더가 잘 연결되었는지 체크하기
    if (!result) {
        glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
        std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
        return false;
    }
    glUseProgram(shaderID); //--- 만들어진 세이더 프로그램 사용하기
    //--- 여러 개의 세이더프로그램 만들 수 있고, 그 중 한개의 프로그램을 사용하려면
    //--- glUseProgram 함수를 호출하여 사용 할 특정 프로그램을 지정한다.
    //--- 사용하기 직전에 호출할 수 있다.
    return shaderID;
}
