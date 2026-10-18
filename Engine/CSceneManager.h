#pragma once
#include "CSceneBase.h"
#include "Scenes/TDE/SceneTDE.h"
#include "Scenes/Scene0/Scene0.h"
#include "Scenes/Scene1/Scene1.h"
#include "Scenes/Scene5/Scene5.h"
#include "Scenes/Scene6/Scene6.h"
#include "Scenes/Scene7/Scene7.h"

class CSceneManager : public CSceneBase
{
public:
	CSceneManager();
	~CSceneManager(void);
	void ChangeScene(unsigned int _uiCurrentScene);

	virtual void DrawScene(float deltaTime);
	virtual void ProcessSceneInput(class GLFWwindow* window, float deltaTime);


private:
	// Ponteiro para cenas do jogo
	CSceneBase* Scene;

	// ID da cena
	unsigned int uiCurrentScene;
	int iMenuCurrentScene;

	bool bWireframeMode;
	bool bVsyncEnabled;
};

