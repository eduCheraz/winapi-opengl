#include <Windows.h>
#include "OpenGL.h"

#define TIMER1 1001

#define _DEDICATED_GRAPHICS_CARD_ //Tienes una tarjeta grafica dedicada, descomenta esto

#ifdef _DEDICATED_GRAPHICS_CARD_
	// enable optimus!
	extern "C" {
		_declspec(dllexport) DWORD NvOptimusEnablement = 1;
		_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
	}
#endif // !_DEDICATED_GRAPHICS_CARD_


bool FULL_SCREEN = false;
bool isShutdown = false;

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	HWND hWnd = NULL;
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings; // Que es un DEVMODE
	int posX, posY;
	HINSTANCE mhInstance = GetModuleHandle(NULL);//Checar que es esto ??
	int screenWidth = 0, screenHeight = 0;
	MSG msg;
	bool done, result;

	LPCWSTR applicationName = L"Hola mundo OpenGL";

	//Configurar un ventana
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);
	
	//Crear ventana fiticia para cargar funciones de opengl
	// Create a temporary window for the OpenGL extension setup.
	hWnd = CreateWindow((LPCWSTR)applicationName, (LPCWSTR)applicationName, WS_POPUP,
		0, 0, 640, 480, NULL, NULL, mhInstance, NULL);
	if (hWnd == NULL)
	{
		return false;
	}

	// Don't show the window.
	ShowWindow(hWnd, SW_HIDE);

	result = InicializarFuncionesOpenGL(hWnd);
	if (!result) {
		MessageBoxW(hWnd, L"Could not initialize the OpenGL extensions.", L"Error", MB_OK);
		return false;
	}

	// Release the temporary window now that the extensions have been initialized.
	DestroyWindow(hWnd);
	hWnd = NULL;
	
	//Resolucion de la pantalla
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (FULL_SCREEN) {
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else {
		//Resolucion por defecto
		screenWidth = 800;
		screenHeight = 600;

		// Posicionar en centro de la pantalla
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//Crear el manejador de la ventana
	hWnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName, WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, mhInstance, NULL);
	if (hWnd == NULL)
	{
		return false;
	}

	// Initialize OpenGL now that the window has been created.
	result = InicializarOpenGL(hWnd);
	if (!result)
	{
		MessageBoxW(hWnd, L"Could not initialize OpenGL, check if video card supports OpenGL 4.0.",
			L"Error", MB_OK);
		return false;
	}


	//Mostrar en primer plano y hacer focus a la pantalla
	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);

	// Hide the mouse cursor.
	ShowCursor(false);


	//Bucle de eventos
	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			//Realiza grafico y checa si no presionas ESCAPE
			/*glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			SwapBuffers(m_deviceContext);*/

			if (isShutdown)
			{
				done = true;
			}
		}

	}


	//Shutdwon apagar pantalla
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	LiberarOpenGL(hWnd);

	// Remove the window.
	DestroyWindow(hWnd);
	hWnd = NULL;

	// Remove the application instance.
	UnregisterClass(applicationName, hInstance);
	hInstance = NULL;

	return 0;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_TIMER: {
		if (wparam == TIMER1) {
			static float color = 0.0f;
			
			//Realiza un grafico
			glClearColor(color, color, color, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//glBegin(GL_TRIANGLES);
			//	glColor3f(1.0f, 1.0f, 1.0f);
			//	glVertex3f(-1.0f, 0.0f, 1.0f);
			//	glVertex3f(0.0f, 1.0f, 1.0f);
			//	glVertex3f(1.0f,0.0f, 1.0f);
			//glEnd();

;			SwapBuffers(m_deviceContext);
		}
	}
	case WM_CREATE:
	{
		SetTimer(hwnd,TIMER1,30,NULL);
		return 0;
	}
		// Check if a key has been pressed on the keyboard.
	case WM_KEYDOWN:
	{
		// If a key is pressed send it to the input object so it can record that state.
		if ((unsigned int)wparam == VK_ESCAPE) {
			isShutdown = true;
		}
		return 0;
	}

	// Check if a key has been released on the keyboard.
	case WM_KEYUP:
	{
		return 0;
	}

	// Check if the window is being closed.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// All other messages pass to the message handler in the system class.
	default:
	{
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
	}
}