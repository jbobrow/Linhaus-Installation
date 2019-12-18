//
//  ConwayCam.cpp
//  CinderConwayCamera
//
//  Created by Tamar Ziv on 12/17/19.
//

#include "ConwayCam.h"

ci::gl::BatchRef ConwayCam::mBatchRenderer = nullptr;

ConwayCamRef ConwayCam::create()
{
    return ConwayCamRef(new ConwayCam());
}

// constructor
//-------------------------------------------
ConwayCam::ConwayCam()
	: mTextureSrc(nullptr),
	mTextureDst(nullptr),
	mCapture(nullptr),
	mRes( ci::vec2(1024,768) )
{
	CI_LOG_D("Creating ConwayCam");
	setup();
}

// destructor
//-------------------------------------------
ConwayCam::~ConwayCam()
{
}

//-------------------------------------------
void ConwayCam::setup()
{
	gl::enableDepthWrite();
	gl::enableDepthRead();
	
	loadShader();
	try {
		mCapture = Capture::create( mRes.x, mRes.y );
		mCapture->start();
	}
	catch( ci::Exception &exc ) {
		CI_LOG_EXCEPTION( "Failed to init capture ", exc );
	}
	
	// some values
	mMinThreshold = 1.05;
	mMaxThreshold = 3.0;
	mAgeSpeed = 0.01;
	createParams();
}

//-------------------------------------------
void ConwayCam::loadShader()
{
	ci::gl::GlslProg::Format sFormat = ci::gl::GlslProg::Format();
	ci::gl::GlslProgRef shader;

	try {
		ci::DataSourceRef vertFile = ci::app::loadAsset( "Glsl/conway_gradiant.vert" );
		ci::DataSourceRef fragFile = ci::app::loadAsset( "Glsl/conway_gradiant.frag" );
		
		// load the shader into the batch
		sFormat.vertex( vertFile ).fragment( fragFile );
		ci::gl::GlslProgRef shader = ci::gl::GlslProg::create( sFormat );
		shader->uniform( "uTexture", 0 );
		shader->uniform( "resolution", mRes );
		mBatchRenderer = ci::gl::Batch::create( ci::geom::Rect( ci::Rectf( 0, 0, 1, 1 ) ), shader );
	}
	catch( const ci::gl::GlslProgCompileExc e ) {
		ci::app::console() << "Could not compile shader:" << e.what() << std::endl;
	}
}

//-------------------------------------------
void ConwayCam::createParams(){
	// Setup some basic parameters.
	
	mParams = params::InterfaceGl::create( ci::app::getWindow(), "Parameters", ci::app::toPixels( ivec2( 200, 400 ) ) );

	mParams->addParam( "Age Speed", &mAgeSpeed ).min( 0.01f ).max( 5.f ).precision( 2 ).step( 0.02f );
	mParams->addParam( "Min Threshold", &mMinThreshold ).min( 0.05f ).max( 5.f ).precision( 2 ).step( 0.02f );
	mParams->addParam( "Max Threshold", &mMaxThreshold ).min( 0.05f ).max( 5.f ).precision( 2 ).step( 0.02f );
	mParams->addSeparator();
}


//-------------------------------------------
void ConwayCam::update()
{
	//CI_LOG_D("Updating ConwayCam");
	
	if( mCapture && mCapture->checkNewFrame() ) {
		if( ! mTextureSrc ) {
			// Capture images come back as top-down, and it's more efficient to keep them that way
			mTextureSrc = gl::Texture::create( *mCapture->getSurface(), gl::Texture::Format().loadTopDown(false) );
			mTextureDst = gl::Texture::create( *mCapture->getSurface(), gl::Texture::Format().loadTopDown(false) );
		}
		else {
			mTextureSrc->update( *mCapture->getSurface() );
			renderTexture(mTextureSrc, mTextureDst);
		}
	}
	
	mTime = ci::app::getElapsedSeconds();
}

//-------------------------------------------
void ConwayCam::draw()
{
	if( mTextureDst ) {
		gl::draw( mTextureDst );
	}
	
	if( mParams ) {
		mParams->draw();
	}
	
}
//--------------------------------------------------------

void ConwayCam::renderTexture( ci::gl::TextureRef& source, ci::gl::TextureRef& destination )
{
	if( !mBatchRenderer ) {
		loadShader();
	}

	if( !source || !destination ) {
		return;
	}
	
	// create fbos
	ci::gl::FboRef mFbo;
	ci::gl::Fbo::Format fboFormat;
	fboFormat.setColorTextureFormat( ci::gl::Texture2d::Format().internalFormat( GL_RGBA32F ) );
	fboFormat.setSamples( 4 );

	ci::gl::Texture::Format texFormat;
	texFormat.enableMipmapping(true);
	texFormat.setMagFilter(GL_NEAREST);
	texFormat.setMinFilter(GL_LINEAR_MIPMAP_NEAREST);
	fboFormat.setColorTextureFormat(texFormat);

	ci::ivec2 size = source->getSize() ;
	mFbo = ci::gl::Fbo::create( size.x,size.y, fboFormat );

	//	Set viewport
	ci::gl::ScopedViewport viewportScope( 0, 0, mFbo->getWidth(), mFbo->getHeight() );
	ci::gl::ScopedMatrices matricesScope;
	ci::gl::setMatricesWindow( mFbo->getSize(), true );
	
	
	//	Draw texture into FBO
	{
		ci::gl::ScopedFramebuffer fboScoped( mFbo );
		ci::gl::clear( ci::ColorA( 0.0, 0.0, 0.0, 0.0 ) );
		ci::gl::ScopedTextureBind texture( source, 0 );

		mBatchRenderer->getGlslProg()->uniform( "time", mTime );
		mBatchRenderer->getGlslProg()->uniform( "ageSpeed", mAgeSpeed );
		mBatchRenderer->getGlslProg()->uniform( "minThreshold", mMinThreshold );
		mBatchRenderer->getGlslProg()->uniform( "maxThreshold", mMaxThreshold );

		ci::gl::ScopedModelMatrix modelScope;
		ci::gl::scale( mFbo->getSize() );
		mBatchRenderer->draw();
	}
	
	destination = mFbo->getColorTexture();
}

//--------------------------------------------------------




