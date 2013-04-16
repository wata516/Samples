#include <iostream>

#include <Laser/Common/System/ManagerFactory.h>
#include <Laser/Common/System/IManager.h>
#include <Laser/Common/System/Window.h>
#include <Laser/Common/Input/IKeyboard.h>
#include <TGUL/String.h>

#include <GL/glfw.h>

#include <Laser/Common/System/TechniqueManager.h>
#include <Laser/Common/User/Technique.h>
class ClearSample : public Laser::User::Technique
{
public:
	ClearSample() { }
	
	ClearSample( const ClearSample &sample ) {
		
	}
public:
	virtual void Draw() {}
	virtual uint GetClassSize( ) const { return sizeof( *this ); }
};

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
	Laser::System::Window *pWindow;
	if( pManager->CreateWindow( &pWindow ) == false ) {
		return 1;
	}
	
	// Windowを作成する
	if( pWindow->Create( "sample", 0, 0, 0, 0 ) == false ) {
		return 1;
	}
	
	// Windowを開く
	if( pWindow->Open() == false ) {
		return 1;
	}

	// Keyboardを作成する
	Laser::Input::IKeyboard *pKeyboard;
	if( pManager->CreateKeyboard( &pKeyboard ) == false ) {
		return false;
	}
	
	// TechniqueManagerを作成する
	Laser::System::TechniqueManager *pTechniqueManager;
	if( pManager->CreateTechniqueManager(&pTechniqueManager) == false ) {
		return false;
	}
	
	ClearSample ClearTechnique;

	pTechniqueManager->Regist( ClearTechnique );
	
	pWindow->SetTechnique( pTechniqueManager );
	
	// 描画ループ
	while( pWindow->IsOpen() ) {
		pKeyboard->Update( );

		if( pKeyboard->IsTrigger(Laser::Input::IKeyboard::KEY_TYPE_ESCAPE) ) {
			pWindow->Close();
		}
		
		glClearColor(1.0F,0.0F,0.0F,1.0F);
		
		// OpenGL rendering goes here...
		glClear( GL_COLOR_BUFFER_BIT );
		
		pWindow->Render();

		pWindow->Flip();
	}
	
	// Windowを閉じます
	pWindow->Close();
    
	// Managerを破棄する
    pManager->Destroy();
    
    return 0;
}
