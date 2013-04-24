#include <iostream>

#include <Laser/ManagerFactory.h>
#include <Laser/GraphicsManager.h>
#include <Laser/Window.h>
#include <Laser/IKeyboard.h>
#include <laser/TechniqueManager.h>
#include <Laser/Technique.h>
#include <Laser/CommandFactory.h>
#include <Laser//Pass.h>
#include <TGUL/String.h>
#include <Laser/Clear.h>
#include <Laser/VertexDeclare.h>
#include <Laser/Buffer.h>
#include <Laser/SysmemBuffer.h>
#include <Laser/ResourceManager.h>
#include <Laser/IShader.h>
#include <GL/glfw.h>

class FirstPass : public Laser::User::Pass
{
	Laser::Command::Clear *mClear;

public:
	virtual unsigned int GetClassSize() const { return sizeof( *this ); }
	virtual void Render() const
	{
		mClear->Draw();
	}

	bool Create( )
	{
		Laser::Command::IBase *pClear = 0;
		if( Laser::CommandFactory::CreateCommand( "Clear", &pClear ) == false ) {
			return false;
		}
		mClear = pClear->Get<Laser::Command::Clear>( );
		mClear->SetColor( 1.0F, 0.0F, 0.0F, 1.0F );

		return true;
	}
};

class SampleClearTechnique : public Laser::User::Technique
{
	FirstPass mFirstPass;

public:
	virtual unsigned int GetClassSize() const { return sizeof( *this ); }

public:
	bool Create( ) {
		mFirstPass.Create();

		if( Regist( mFirstPass ) == false ) {
			return false;
		}

		return true;
	}
};

int main(int argc, const char * argv[])
{
    std::cout << "初期化を行います" << std::endl;

    Laser::GraphicsManager *pManager = 0;
    
    // IManagerを作成
    if( Laser::GraphicsManagerFactory::Create( TGUL::String( "OpenGL"), &pManager ) == false ) {
        return 1;
    }
    
	// Managerを作成
    if( pManager->Create() == false ) {
        return 1;
    }
	
	// Windowを作成
	Laser::Window *pWindow;
	if( pManager->CreateWindow( &pWindow ) == false ) {
		return 1;
	}
	
	// Windowを作成
	if( pWindow->Create( "Sample", 0, 0, 0, 0 ) == false ) {
		return 1;
	}
	
	// Windowを開く
	if( pWindow->Open() == false ) {
		return 1;
	}

	// Keyboardを作成
	Laser::Input::IKeyboard *pKeyboard;
	if( pManager->CreateKeyboard( &pKeyboard ) == false ) {
		return 1;
	}

	// TechniqueManagerを作成
	Laser::TechniqueManager *pTechniqueManager;
	if( pManager->CreateTechniqueManager( &pTechniqueManager ) == false ) {
		return 1;
	}

	// Techniqueを作成
	SampleClearTechnique clear;
	if( clear.Create() == false ) {
		return 1;
	}

	pTechniqueManager->Regist( clear );
	pWindow->SetTechnique(pTechniqueManager);
	
	// 頂点定義を作成
	Laser::VertexDeclare VertexP32(Laser::IVertexDeclare::TYPE_P32);
	
	// Bufferを作成
	Laser::ResourceManager &ResourceManager = pManager->GetResourceManager();

	Laser::Resource::Buffer *pSysmemBuffer = 0;
	if( ResourceManager.CreateBuffer( "SysmemBuffer", &pSysmemBuffer ) == false ) {
		return 1;
	}
	
	if( pSysmemBuffer->Allocate( sizeof( float ) * 4, 3 ) == false ) {
		return 1;
	}

	// Bufferに頂点を書き込む
	struct TriangleP32 {
		size_t operator()( void *pAddress, size_t VertexSize, size_t ArrayIndex ) {
			float *pVertex = reinterpret_cast< float * >( pAddress );
			*pVertex = 1.0F;
			return sizeof( float ) * 4;
		}
	};
	pSysmemBuffer->Write( TriangleP32() );

	// 頂点シェーダーを作成
	Laser::Shader *pVertexShader = 0;
	if( ResourceManager.CreateShader("VertexShader", &pVertexShader ) == false ) {
		return 1;
	}
	
	// フラグメントシェーダーを作成
	Laser::Shader *pFragmentShader = 0;
	if( ResourceManager.CreateShader("FragmentShader", &pFragmentShader ) == false ) {
		return 1;
	}
	
	((Laser::IShader *)pVertexShader)->Load( "../Media/Simple.vs", 10 );

	// 描画ループ
	while( pWindow->IsOpen() ) {
		
		// リソースマネージャーを処理する
		ResourceManager.Execute();

		// キー入力操作
		pKeyboard->Update( );
		// 描画を行う
		pWindow->Render();

		// バックバッファとフロントバッファを切り替える
		pWindow->Flip();
				
		// ESCAPEを押すとウィンドウを閉じる
		if( pKeyboard->IsTrigger(Laser::Input::IKeyboard::KEY_TYPE_ESCAPE) ) {
			pWindow->Close();
		}

	}
	
	// Windowを閉じます
	pWindow->Close();
    
	// Managerを破棄する
    pManager->Destroy();
    
    return 0;
}
