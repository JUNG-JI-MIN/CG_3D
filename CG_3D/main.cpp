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

void TimerFunction(int value) {
    line.xyz = tileManager.make_tile.position;
    float dt = 1.5f / 60.0f; // 60 FPS 기준 deltaTime

    // 플레이어 업데이트
    player.Update(dt);
	// 타일 매니저 업데이트
    if (!tileManager.editing_mode) {
        tileManager.UpdateALl(dt);
    }

    glutPostRedisplay();  // 화면 다시 그리기
    glutTimerFunc(16, TimerFunction, 1);  // 다음 타이머 설정
}

// 입력 콜백들은 ImGuiManager의 Handle*를 우선 호출하도록 변경
void onKey(unsigned char key, int x, int y) {

    switch (key)
    {
    case 'q':
        exit(1);
        break;
    case 'a':
        player.Rolling_in_the_deep(glm::vec3(-1.0f, 0.0f, 0.0f));
		break;
	case 'd':
        player.Rolling_in_the_deep(glm::vec3(1.0f, 0.0f, 0.0f));
		break;
	case 'w':
        player.Rolling_in_the_deep(glm::vec3(0.0f, 0.0f, -1.0f));
		break;
	case 's':
        player.Rolling_in_the_deep(glm::vec3(0.0f, 0.0f,  1.0f));
        break;
	case '\r': 
        if (tileManager.making_move_tile) tileManager.making_move_tile = false;
        else tileManager.tile_make(); // 타일 만들기

        break; // 엔터키
    case 'r':
		tileManager.delete_tile(); // 타일 지우기
        break;
    case 'p':
		tileManager.move_tile_add_command(); // 타일 이동 명령 추가
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
			tileManager.SaveToJSON("json/stage.json");
            break;
        case GLUT_KEY_F6:
            tileManager.LoadFromJSON("json/stage.json");
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
    spring_cube_texture.Load("resource/tile/slime.png");
    moving_cube_texture.Load("resource/tile/scaffolding.png");
    rotate_cube_texture.Load("resource/tile/silver.png");
    switch_cube_texture.Load("resource/tile/silver.png");

    public_cube.Init(); // 전역 변수로 선언된 큐브 모델 초기화
	harf_cube.Init();
	BackGround_cube_texture.Load("resource/space.png");
	BackGround_cube.Init();
}

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
    srand(time(NULL));
    width = 1600;
    height = 1200;
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
    tileManager.GenerateGrid();
    tileManager.GenerateBackground();

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
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


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
GLuint make_shaderProgram()
{
    GLint result;
    GLchar* errorLog = NULL;
    GLuint shaderID;
    shaderID = glCreateProgram(); //--- 세이더 프로그램 만들기
    glAttachShader(shaderID, vertexShader); //--- 세이더 프로그램에 버텍스 세이더 붙이기
    glAttachShader(shaderID, fragmentShader); //--- 세이더 프로그램에 프래그먼트 세이더 붙이기
    glLinkProgram(shaderID); //--- 세이더 프로그램 링크하기
    glDeleteShader(vertexShader); //--- 세이더 객체를 세이더 프로그램에 링크했음으로, 세이더 객체 자체는 삭제 가능
    glDeleteShader(fragmentShader);
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
