#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "ConwayCam.h"

#define CINDER_GL_HAS_COMPUTE_SHADER

using namespace ci;
using namespace ci::app;
using namespace std;

class CinderConwayCameraApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    ConwayCamRef mConwayCam;
    
};

void CinderConwayCameraApp::setup()
{
	ci::app::setWindowSize(1024,768);
	mConwayCam = ConwayCam::create();
}

void CinderConwayCameraApp::mouseDown( MouseEvent event )
{
}

void CinderConwayCameraApp::update()
{
	if( mConwayCam ) {
		mConwayCam->update();
	}
}

void CinderConwayCameraApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	if( mConwayCam ) {
		mConwayCam->draw();
	}
}

CINDER_APP( CinderConwayCameraApp, RendererGl )
