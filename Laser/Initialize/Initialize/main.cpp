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
#include <Laser/CommandClear.h>
#include <Laser/CommandShader.h>
#include <Laser/VertexDeclare.h>
#include <Laser/Buffer.h>
#include <Laser/VertexBuffer.h>
#include <Laser/ResourceManager.h>
#include <Laser/Shader.h>
#include <GL/glfw.h>
#include <boost/foreach.hpp>

#if LASER_IS_PLATFORM(APPLE)
#define MEDIA_PATH "../../../../../../Media/"
#elif LASER_IS_PLATFORM(WINDOWS)
#define MEDIA_PATH "../../../../Media/"
#endif
class FirstPass : public Laser::User::Pass
{
	Laser::Command::Clear *mClear;
	Laser::Command::Shader *mSimpleShader;
	Laser::VertexBuffer *mVertexBuffer;
	Laser::ResourceManager *mResources;

public:
	FirstPass()
		: mClear( 0 )
		, mSimpleShader( 0 )
		, mVertexBuffer( 0 )
		, mResources( 0 )
	{}

public:
	virtual unsigned int GetClassSize() const { return sizeof( *this ); }
	virtual void Render() const
	{
		mClear->Draw();
		
		// シェーダーを作成する
		if( mSimpleShader->IsAvailable() == false ) {
			mSimpleShader->Create();
		}

		if( mSimpleShader->IsAvailable() ) {
			mSimpleShader->Draw();
		}
	}

	bool Create( Laser::GraphicsManager *pManager )
	{
		mResources = &pManager->GetResourceManager();

		Laser::Command::IBase *pClear = 0;
		if( Laser::CommandFactory::CreateCommand( "Clear", &pClear ) == false ) {
			return false;
		}
		mClear = pClear->Get<Laser::Command::Clear>( );
		mClear->SetColor( 1.0F, 0.0F, 0.0F, 1.0F );

		// シェーダーを作成する
		Laser::Shader *pVertex = 0, *pFragment = 0;
		mResources->GetShader( "SimpleVertex", &pVertex );
		mResources->GetShader( "SimpleFragment", &pFragment );

		Laser::Command::IBase *pShader = 0;
		if( Laser::CommandFactory::CreateCommand( "Shader", &pShader ) == false ) {
			return false;
		};
		mSimpleShader = pShader->Get<Laser::Command::Shader>( );
		mSimpleShader->SetShader( Laser::Command::Shader::SHADER_TYPE_VERTEX, pVertex );
		mSimpleShader->SetShader( Laser::Command::Shader::SHADER_TYPE_FRAGMENT, pFragment );
		
		// 頂点定義を作成
		Laser::VertexDeclare VertexP32C32( Laser::IVertexDeclare::TYPE_P32 | Laser::IVertexDeclare::TYPE_C32 );
		VertexP32C32.CreateVertexElement(Laser::IVertexDeclare::TYPE_P32, "inPosition", 0 );
		VertexP32C32.CreateVertexElement(Laser::IVertexDeclare::TYPE_C32, "inColor", 1 );
		mSimpleShader->BindVertexDeclare( VertexP32C32 );

		// 頂点バッファを作成
		if( mResources->CreateBuffer( "VertexBuffer", "Triangle", (Laser::Resource::Buffer**)&mVertexBuffer ) == false ) {
			return false;
		}

		if( mVertexBuffer->Create( VertexP32C32, 3 ) == false ) {
			return 1;
		}
		
		// Bufferに頂点を書き込み
		struct TriangleP32C32 {
			size_t operator()( void *pAddress, size_t VertexSize ) {
				boost::array< const float, 12 > positions = {
					-0.8f,-0.8f,0.0f,1.0f,
					0.8f,-0.8f,0.0f,1.0f,
					0.0f, 0.8f,0.0f,1.0f
				};
				boost::array< const float, 12 > colors = {
					1.0f, 0.0f, 0.0f, 1.0f,
					0.0f, 1.0f, 0.0f, 1.0f,
					0.0f, 0.0f, 1.0f, 1.0f,
				};

				size_t result = 0;
				float *pCurrent = reinterpret_cast< float * >( pAddress );

				BOOST_FOREACH( const float value, positions ) {
					*pCurrent = value; ++ pCurrent; ++ result;
				}
				BOOST_FOREACH( const float value, colors ) {
					*pCurrent = value; ++ pCurrent; ++ result;
				}

				return result * sizeof( float );
			}
		};
		mVertexBuffer->Write( TriangleP32C32() );
		

		return true;
	}
};

class SampleClearTechnique : public Laser::User::Technique
{
	FirstPass mFirstPass;

public:
	virtual unsigned int GetClassSize() const { return sizeof( *this ); }

public:
	SampleClearTechnique() {}

public:
	bool Create( Laser::GraphicsManager *pManager ) {
		mFirstPass.Create( pManager );

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

	Laser::ResourceManager &ResourceManager = pManager->GetResourceManager();

	// 頂点シェーダーを作成
	Laser::Shader *pVertexShader = 0;
	if( ResourceManager.CreateShader("VertexShader", "SimpleVertex", &pVertexShader ) == false ) {
		return 1;
	}
	
	if( pVertexShader->Load( MEDIA_PATH"Simple.vs", 10 ) == false ) {
		return 1;
	}

	// フラグメントシェーダーを作成
	Laser::Shader *pFragmentShader = 0;
	if( ResourceManager.CreateShader("FragmentShader", "SimpleFragment", &pFragmentShader ) == false ) {
		return 1;
	}
	
	if( pFragmentShader->Load( MEDIA_PATH"Simple.fs", 10 ) == false ) {
		return 1;
	}
	
	// TechniqueManagerを作成
	Laser::TechniqueManager *pTechniqueManager;
	if( pManager->CreateTechniqueManager( &pTechniqueManager ) == false ) {
		return 1;
	}

	// Techniqueを作成
	SampleClearTechnique clear;
	if( clear.Create( pManager ) == false ) {
		return 1;
	}

	pTechniqueManager->Regist( clear );
	pWindow->SetTechnique(pTechniqueManager);

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
