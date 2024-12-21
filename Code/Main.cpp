#include "Application.h"
#include "Configs//KeysConfiguration.h"

// WinApi window creation and handle
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

static Application* app;

int WINAPI WinMain(	HINSTANCE   i_Instance,
                    HINSTANCE   i_PrevInstance,
                    LPSTR       i_CmdLine,
                    int         i_CmdShow ) {

   
    app = new Application(i_Instance, WndProc);
    app->Run();

    delete(app);

    return 0;
}

LRESULT CALLBACK WndProc( HWND      i_hWnd,
                          UINT      i_Message,
                          WPARAM    i_wParam,
                          LPARAM    i_lParam ) {
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
            PostQuitMessage( 0 );                                   // Send message about application exit
            return 0;
        }
    case WM_KEYDOWN: {                                              // User pressed any key
             app->GetRender()->UpdateParameters(i_wParam, i_lParam);
            return 0;
        }
    case WM_KEYUP: {                                                // User released any key
            if( i_wParam == VK_ESCAPE ) PostQuitMessage(0);         // If key released by user was escape, tell application to quit
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
