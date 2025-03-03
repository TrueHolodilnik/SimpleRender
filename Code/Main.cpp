#include "Application.h"
#include "Configs\\KeysConfiguration.h"
#include "Utils\\Utils.h"

#define EXIT_CODE 0 // Exit code for application

// WinApi window creation and handle
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

std::unique_ptr<Application> app; // Main application
int WINAPI WinMain(HINSTANCE i_Instance, HINSTANCE i_PrevInstance, LPSTR i_CmdLine, int i_CmdShow ) {
  
	app.reset(new Application(i_Instance, WndProc)); // Create application instance updating smart pointer
    app->Run();

    return 0;
}

LRESULT CALLBACK WndProc(HWND i_hWnd, UINT i_Message, WPARAM i_wParam, LPARAM i_lParam ) {
    switch( i_Message ) {
    case WM_PAINT: {
            if (app) app->ForceRenderUpdate();
            ValidateRect( i_hWnd, nullptr );                        // Tell operating system that drawing is finished
            return 0;
        }
    case WM_SIZE: {                                                 // Size of the window has changed
            if (app) app->WindowResize( LOWORD(i_wParam), HIWORD(i_lParam) );           // Change OpenGL screen size (viewport)
            return 0;
        }
    case WM_CLOSE: {                                                // Application is closing
            PostQuitMessage(EXIT_CODE);                                   // Send message about application exit
            return 0;
        }
    case WM_KEYDOWN: {                                              // User pressed any key
            app->GetRender()->UpdateParameters(i_wParam, i_lParam);
            return 0;
        }
    case WM_KEYUP: {                                                // User released any key
            if( i_wParam == ButtonsDefinitions::QuitButton) PostQuitMessage(EXIT_CODE);         // If key released by user was escape, tell application to quit
            break;
        }
    case WM_SYSCOMMAND: {                                           // Operating system messages
            switch( i_wParam ) {
            case SC_SCREENSAVE:                                     // Turn off screensaver or monitor standby mode by capturing proper messages
            case SC_MONITORPOWER:
                return 0;
            }
        }
    default: {                                                      // Leave processing of any other messages to operating system
            return DefWindowProc( i_hWnd, i_Message, i_wParam, i_lParam );
        }
    }
    return 0;
}
