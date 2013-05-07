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
#include <Laser/CommandVertexBuffer.h>
#include <Laser/CommandMaterial.h>
#include <Laser/CommandRenderTarget.h>
#include <Laser/CommandViewport.h>
#include <Laser/CommandPrimitive.h>
#include <Laser/VertexDeclare.h>
#include <Laser/Buffer.h>
#include <Laser/VertexBuffer.h>
#include <Laser/IndexBuffer.h>
#include <Laser/Texture.h>
#include <Laser/RenderTarget.h>
#include <Laser/ResourceManager.h>
#include <Laser/Shader.h>
#include <Laser/ShaderUniformBuffer.h>
#include <Laser/DrawStatus.h>
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
	Laser::Command::Viewport *mViewport;
	Laser::Command::Viewport *mRenderTargetViewport;
	Laser::Command::VertexBuffer *mTriangleVertexColor;
	Laser::Command::VertexBuffer *mTriangleVertexTex;
	Laser::Command::Primitive *mTriangle;
	Laser::Command::Material *mMaterial;
	Laser::Command::Material *mMaterialTex;
	Laser::Command::RenderTarget *mRenderTarget;
	Laser::Command::RenderTargetReset *mRenderTargetReset;
	Laser::ResourceManager *mResources;
	int mDefaultWindowWidth, mDefaultWindowHeight;

	struct TransformUniformBlock
	{
		float MVPMatrix;
	};
	Laser::ShaderUniformBufferClass< TransformUniformBlock > mTransformBlock;

public:
	FirstPass()
		: mClear( 0 )
		, mViewport(0)
		, mRenderTargetViewport(0)
		, mTriangleVertexColor( 0 )
		, mTriangleVertexTex( 0 )
		, mTriangle( 0 )
		, mMaterial( 0 )
		, mMaterialTex( 0 )
		, mRenderTarget( 0 )
		, mRenderTargetReset( 0 )
		, mResources( 0 )
	{}

public:
	void SetDefaultWidowWidth( int width ) { mDefaultWindowWidth = width; }
	void SetDefaultWidowHeight( int height ) { mDefaultWindowHeight = height; }

	virtual unsigned int GetClassSize() const { return sizeof( *this ); }
	virtual void Render() const
	{
		Laser::DrawStatus status;

		mRenderTarget->Draw( status );

		// クリア
		mClear->SetColor(0.0F, 1.0F, 0.0F, 1.0F);
		mClear->Draw( status );
		
		mRenderTargetViewport->Draw( status );

		// 頂点を描画
		mMaterial->Draw(status);
		mTriangleVertexColor->Draw(status);
		mTriangle->Draw(status);
		
		mRenderTargetReset->Draw( status );
		
		mClear->SetColor(0.0F, 0.0F, 1.0F, 1.0F);
		mClear->Draw( status );

		// 頂点を描画
		mViewport->Draw( status );
		mMaterialTex->Draw(status);
		mTriangleVertexTex->Draw(status);

	}

	bool Create( Laser::GraphicsManager *pManager )
	{
		mResources = &pManager->GetResourceManager();

		Laser::Command::IBase *pClear = 0;
		if( Laser::CommandFactory::CreateCommand( "Clear", &pClear ) == false ) {
			return false;
		}
		mClear = pClear->Get<Laser::Command::Clear>( );
		mClear->SetColor( 0.8F, 0.8F, 0.8F, 1.0F );

		// シェーダーを作成する
		Laser::Shader *pVertexShader = 0, *pFragmentShader = 0;
		Laser::Shader *pVertexShaderTex = 0, *pFragmentShaderTex = 0;
		mResources->GetShader( "VertexColorVertex", &pVertexShader );
		mResources->GetShader( "VertexColorFragment", &pFragmentShader );
		mResources->GetShader( "SimpleVertex", &pVertexShaderTex );
		mResources->GetShader( "SimpleFragment", &pFragmentShaderTex );

		// 頂点定義を作成
		Laser::VertexDeclare VertexP32C32T16;
		VertexP32C32T16.CreateVertexElement(Laser::IVertexDeclare::TYPE_P32, "inPosition", 0 );
		VertexP32C32T16.CreateVertexElement(Laser::IVertexDeclare::TYPE_C32, "inColor", 1 );
		VertexP32C32T16.CreateVertexElement(Laser::IVertexDeclare::TYPE_T16, "inTexCoord", 2 );

		// 頂点バッファを作成
		Laser::VertexBuffer *pVertexBuffer;
		if( mResources->CreateVertexBuffer( "VertexBuffer", "Triangle", &pVertexBuffer ) == false ) {
			return 1;
		}

		if( pVertexBuffer->Create( VertexP32C32T16, 4 ) == false ) {
			return 1;
		}
		
		// Bufferに頂点を書き込み
		struct TriangleP32C32T16 {
			size_t operator()( void *pAddress, size_t VertexSize ) {
				boost::array< const float, 16 > positions = {
					-0.8f,-0.8f, 0.0f, 1.0f,
					-0.8f, 0.8f, 0.0f, 1.0f,
					 0.8f,-0.8f, 0.0f, 1.0f,
					 0.8f, 0.8f, 0.0f, 1.0f
				};
				boost::array< const float, 16 > colors = {
					1.0f, 0.0f, 0.0f, 1.0f,
					1.0f, 0.0f, 0.0f, 1.0f,
					0.0f, 0.0f, 1.0f, 1.0f,
					0.0f, 0.0f, 1.0f, 1.0f,
				};
				boost::array< const float, 8 > texcoords = {
					0.0f, 3.0f,
					0.0f, 0.0f,
					3.0f, 3.0f,
					3.0f, 0.0f,
				};

				size_t result = 0;
				float *pCurrent = reinterpret_cast< float * >( pAddress );

				BOOST_FOREACH( const float value, positions ) {
					*pCurrent = value; ++ pCurrent; ++ result;
				}
				BOOST_FOREACH( const float value, colors ) {
					*pCurrent = value; ++ pCurrent; ++ result;
				}
				BOOST_FOREACH( const float value, texcoords ) {
					*pCurrent = value; ++ pCurrent; ++ result;
				}

				return result * sizeof( float );
			}
		};
		pVertexBuffer->Write( TriangleP32C32T16() );
		
		Laser::Command::IBase *pTriangle = 0;
		if( Laser::CommandFactory::CreateCommand( "VertexBuffer", &pTriangle ) == false ) {
			return false;
		};
		mTriangleVertexColor = pTriangle->Get<Laser::Command::VertexBuffer>();
		mTriangleVertexColor->Create( pVertexBuffer, Laser::Command::VertexBuffer::TOPOLOGY_TRIANGLE_STRIP );
		
		if( Laser::CommandFactory::CreateCommand( "VertexBuffer", &pTriangle ) == false ) {
			return false;
		};
		mTriangleVertexTex = pTriangle->Get<Laser::Command::VertexBuffer>();
		mTriangleVertexTex->Create( pVertexBuffer, Laser::Command::VertexBuffer::TOPOLOGY_TRIANGLE_STRIP );

		// IndexBufferを作成
		Laser::IndexBuffer *pIndexBuffer;
		if( mResources->CreateIndexBuffer( "IndexBuffer", "Triangle", &pIndexBuffer ) == false ) {
			return 1;
		}

		// Primitiveを作成
		if( Laser::CommandFactory::CreateCommand( "Primitive", &pTriangle ) == false ) {
			return false;
		};
		mTriangle = pTriangle->Get<Laser::Command::Primitive>();
		mTriangle->Create( pIndexBuffer, pVertexBuffer, Laser::Command::Primitive::TOPOLOGY_TRIANGLE_STRIP );

		// Materialを作成
		Laser::Command::IBase *pMaterial = 0;
		if( Laser::CommandFactory::CreateCommand( "Material", &pMaterial ) == false ) {
			return false;
		};
		mMaterial = pMaterial->Get<Laser::Command::Material>( );

		if( Laser::CommandFactory::CreateCommand( "Material", &pMaterial ) == false ) {
			return false;
		};
		mMaterialTex = pMaterial->Get<Laser::Command::Material>( );

		// Texture
		Laser::Texture *pTexture = 0;
		if( mResources->GetTexture( "SimpleTexture", &pTexture ) == false ) {
			return false;
		}

		// Uniform Buffer
		Laser::ShaderUniformBuffer *pUniformBuffer = 0;
		if( mResources->GetUniformBuffer( "Transform", &pUniformBuffer ) == false ) {
			return false;
		}
		void *pTransformBufferTmp;
		if( pUniformBuffer->QueryInterface(Laser::UUIDS::SHADER_UNIFORM_BUFFER, &pTransformBufferTmp ) == false ) {
			return false;
		}
		Laser::ShaderUniformBuffer *pTransformBuffer = static_cast< Laser::ShaderUniformBuffer *>( pTransformBufferTmp );
		mMaterial->Create( pVertexShader, pFragmentShader, pTransformBuffer );
		mMaterialTex->Create( pVertexShaderTex, pFragmentShaderTex, pTransformBuffer );

		mTransformBlock.GetBuffer().MVPMatrix = 1.0F;
		mMaterialTex->UpdateShaderUniformBuffer(mTransformBlock, 0, "Transform" );
		
		mMaterialTex->SetTexture( 0, "DecalMap", pTexture );
		pTexture->SetWrap( Laser::Texture::WRAP_CLAMP_TO_EDGE, Laser::Texture::WRAP_REPREAT, Laser::Texture::WRAP_REPREAT );

		// RenderTargetを作成する
		Laser::RenderTarget *pRenderTarget;
		if( mResources->CreateRenderTarget( "RenderTarget", "SimpleRenderTarget", &pRenderTarget ) == false ) {
			return false;
		}
		if( pRenderTarget->Create( 512,512 ) == false ) {
			return false;
		}
		
		Laser::Command::IBase *pRenderTargetBase = 0;
		if( Laser::CommandFactory::CreateCommand( "RenderTarget", &pRenderTargetBase ) == false ) {
			return false;
		}
		mRenderTarget = pRenderTargetBase->Get< Laser::Command::RenderTarget >( );
		if( mRenderTarget->Create( pRenderTarget ) == false ) {
			return false;
		}

		Laser::Command::IBase *pRenderTargetReset = 0;
		if( Laser::CommandFactory::CreateCommand( "RenderTargetReset", &pRenderTargetReset) == false ) {
			return false;
		}
		mRenderTargetReset = pRenderTargetReset->Get< Laser::Command::RenderTargetReset >( );

		mMaterialTex->SetRenderTarget( "DecalMap", pRenderTarget );

		Laser::Command::IBase *pViewport = 0;
		if( Laser::CommandFactory::CreateCommand( "Viewport", &pViewport ) == false ) {
			return false;
		}
		mViewport = pViewport->Get<Laser::Command::Viewport>( );
		mViewport->SetViewport( 0, 0, mDefaultWindowWidth, mDefaultWindowHeight );

		if( Laser::CommandFactory::CreateCommand( "Viewport", &pViewport ) == false ) {
			return false;
		}
		mRenderTargetViewport = pViewport->Get<Laser::Command::Viewport>( );
		mRenderTargetViewport->SetViewport( 0, 0, 512, 512 );

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

	void SetDefaultWidowWidth( int width ) { mFirstPass.SetDefaultWidowWidth( width ); }
	void SetDefaultWidowHeight( int height ) { mFirstPass.SetDefaultWidowHeight(height ); }

public:
	bool Create( Laser::GraphicsManager *pManager ) {
		if( mFirstPass.Create( pManager ) == false ) {
			return false;
		}

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

	if( ResourceManager.CreateShader("VertexShader", "VertexColorVertex", &pVertexShader ) == false ) {
		return 1;
	}
	
	if( pVertexShader->Load( MEDIA_PATH"VertexColor.vs", 10 ) == false ) {
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

	if( ResourceManager.CreateShader("FragmentShader", "VertexColorFragment", &pFragmentShader ) == false ) {
		return 1;
	}
	
	if( pFragmentShader->Load( MEDIA_PATH"VertexColor.fs", 10 ) == false ) {
		return 1;
	}

	// テクスチャを作成
	Laser::Texture *pTexture = 0;
	if( ResourceManager.CreateTexture("Texture", "SimpleTexture", &pTexture ) == false ) {
		return 1;
	}
	
	if( pTexture->Load(MEDIA_PATH"Sample.tga", 1024 ) == false ) {
		return 1;
	}
	
	// UniformBufferを作成
	Laser::ShaderUniformBuffer *pVertexUniformBuffer = 0;
	if( ResourceManager.CreateUniformBuffer( "UniformBuffer", "Transform", &pVertexUniformBuffer ) == false ) {
		return 1;
	}
	if( pVertexUniformBuffer->Create() == false ) {
		return 1;
	}
	
	// TechniqueManagerを作成
	Laser::TechniqueManager *pTechniqueManager;
	if( pManager->CreateTechniqueManager( &pTechniqueManager ) == false ) {
		return 1;
	}

	// Techniqueを作成
	SampleClearTechnique clear;
	clear.SetDefaultWidowWidth( pWindow->GetWidth() );
	clear.SetDefaultWidowHeight( pWindow->GetHeight() );
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
