#include "CSceneManager.h"

// Construtor
	
CSceneManager::CSceneManager()
{
	this->Scene = NULL;

	bWireframeMode = false; 
	bVsyncEnabled = true;

	// Inicializa ponteiros das cenas	
	// Cria a cena principal
	uiCurrentScene = 1;
	iMenuCurrentScene = uiCurrentScene;
	ChangeScene(uiCurrentScene);
}


// Destrutor
CSceneManager::~CSceneManager(void)
{
	if (this->Scene)
	{
		delete this->Scene;
		this->Scene = NULL;
	}
}

void CSceneManager::DrawScene(float deltaTime)
{
	this->Scene->DrawScene(deltaTime);
}

void CSceneManager::ProcessSceneInput(GLFWwindow* window, float deltaTime)
{
	InputManager& input = InputManager::GetInstance();

	// Close window on ESC
	if (input.IsKeyJustPressed(GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, true);

	


	// Toggle cursor mode with TAB or VSync
	if (input.IsKeyJustPressed(GLFW_KEY_V))
	{
		bVsyncEnabled = !bVsyncEnabled;
		glfwSwapInterval(bVsyncEnabled);

		std::cout << "Vsync: '" << bVsyncEnabled << "'" << std::endl;
	}

	if (input.IsKeyJustPressed(GLFW_KEY_CAPS_LOCK))
	{
		// Toggle wireframe mode
		bWireframeMode = !bWireframeMode;

		std::cout << "Wireframe: '" << bWireframeMode << "'" << std::endl;
	}
	if (bWireframeMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Fill mode
	
	// Seleciona a cena 0
	if (input.IsKeyJustPressed(GLFW_KEY_0))
		ChangeScene(0);

	// Seleciona a cena 1
	if (input.IsKeyJustPressed(GLFW_KEY_1))
		ChangeScene(1);
	
	// Seleciona a cena 5
	if (input.IsKeyJustPressed(GLFW_KEY_5))
		ChangeScene(5);

	// Seleciona a cena 6
	if (input.IsKeyJustPressed(GLFW_KEY_6))
		ChangeScene(6);

	// Seleciona a cena 7
	if (input.IsKeyJustPressed(GLFW_KEY_7))
		ChangeScene(7);


	// Processa input da cena atual
	for (int key = GLFW_KEY_2; key <= GLFW_KEY_4; ++key)
        if (input.IsKeyJustPressed(key)) ChangeScene(key - GLFW_KEY_0);

    Scene->ProcessSceneInput(window, deltaTime);
}


void CSceneManager::ChangeScene(unsigned int _uiCurrentScene)
{
	std::system("cls"); // Clear Console on Windows

	// Disable Cursor for all scenes
	InputManager& input = InputManager::GetInstance();
	input.SetCursorMode(GLFW_CURSOR_DISABLED);

	this->uiCurrentScene = _uiCurrentScene;

	if (this->Scene)
	{
		delete this->Scene;
		this->Scene = NULL;
	}


	switch (uiCurrentScene)
	{
	case 0:
		this->Scene = new Scene0();
		break;

	case 1:
		this->Scene = new SceneTDE(1);
		break;

	case 2:
        this->Scene = new SceneTDE(2);
        break;
    case 3:
        this->Scene = new SceneTDE(3);
        break;
    case 4:
        this->Scene = new SceneTDE(4);
        break;

    case 5:
		this->Scene = new Scene5();
		break;

	case 6:
		this->Scene = new Scene6();
		break;

	case 7:
		this->Scene = new Scene7();
		break;

	}
}

