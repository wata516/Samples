#include <iostream>

#include <Laser/Common/System/ManagerFactory.h>
#include <Laser/Common/System/IManager.h>
#include <laser/Common/System/IWindow.h>
#include <TGUL/String.h>

#include <GL/glfw.h>
int main(int argc, const char * argv[])
{
    std::cout << "初期化を行います" << std::endl;

    Laser::System::IManager *pManager = 0;
    
    // IManagerを作成します
    if( Laser::System::ManagerFactory::Create( TGUL::String( "OpenGL"), &pManager ) == false ) {
        return 1;
    }
    
	// Managerを作成する
    if( pManager->Create() == false ) {
        return 1;
    }
	
	// IWindowを作成します
	Laser::System::IWindow *pWindow;
	if( pManager->CreateWindow( &pWindow ) == false ) {
		return 1;
	}
	
	// Windowを作成する
	if( pWindow->Create( "初期化サンプル", 0, 0, 0, 0 ) == false ) {
		return 1;
	}
	
	// Windowを開く
	if( pWindow->Open() == false ) {
		return 1;
	}
	
	// 描画ループ
	while( pWindow->IsOpen() ) {
		glClearColor(1.0F,0.0F,0.0F,1.0F);

		// OpenGL rendering goes here...
		glClear( GL_COLOR_BUFFER_BIT );

		// Swap front and back rendering buffers
		glfwSwapBuffers();
		
		if( glfwGetKey( GLFW_KEY_ESC ) ) {
			pWindow->Close();
		}
	}
	
	// Windowを閉じます
	pWindow->Close();
    
	// Managerを破棄する
    pManager->Destroy();
    
    return 0;
}