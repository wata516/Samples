#include <iostream>

#include <Laser/Common/System/ManagerFactory.h>
#include <Laser/Common/System/IManager.h>
#include <Laser/Common/System/Window.h>
#include <Laser/Common/Input/IKeyboard.h>
#include <laser/Common/System/TechniqueManager.h>
#include <Laser/Common/User/Technique.h>
#include <Laser/Common/User/Pass.h>
#include <TGUL/String.h>

#include <GL/glfw.h>

class FirstPass : public Laser::User::Pass
{
public:
	virtual unsigned int GetClassSize() const { return sizeof( *this ); }
	virtual void Render() const
	{
		glClearColor(1.0F,0.0F,0.0F,1.0F);
		glClear( GL_COLOR_BUFFER_BIT );
	}
};

class SampleClearTechnique : public Laser::User::Technique
{
	FirstPass mFirstPass;

public:
	virtual unsigned int GetClassSize() const { return sizeof( *this ); }

public:
	bool Create( ) {

		if( Regist( mFirstPass ) == false ) {
			return false;
		}

		return true;
	}
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
	if( pWindow->Create( "Sample", 0, 0, 0, 0 ) == false ) {
		return 1;
	}
	
	// Windowを開く
	if( pWindow->Open() == false ) {
		return 1;
	}

	// Keyboardを作成する
	Laser::Input::IKeyboard *pKeyboard;
	if( pManager->CreateKeyboard( &pKeyboard ) == false ) {
		return 1;
	}

	// TechniqueManagerを作成する
	Laser::System::TechniqueManager *pTechniqueManager;
	if( pManager->CreateTechniqueManager( &pTechniqueManager ) == false ) {
		return 1;
	}

	// Techniqueを作成する
	SampleClearTechnique clear;
	if( clear.Create() == false ) {
		return 1;
	}

	pTechniqueManager->Regist( clear );
	pWindow->SetTechnique(pTechniqueManager);
	
	// 描画ループ
	while( pWindow->IsOpen() ) {

		// キー入力操作
		pKeyboard->Update( );

		// ESCAPEを押すとウィンドウを閉じる
		if( pKeyboard->IsTrigger(Laser::Input::IKeyboard::KEY_TYPE_ESCAPE) ) {
			pWindow->Close();
		}

		// 描画を行う
		pWindow->Render();

		// バックバッファとフロントバッファを切り替える
		pWindow->Flip();
	}
	
	// Windowを閉じます
	pWindow->Close();
    
	// Managerを破棄する
    pManager->Destroy();
    
    return 0;
}
