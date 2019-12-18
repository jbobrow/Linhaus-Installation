//
//  ConwayCam.hpp
//  CinderConwayCamera
//
//  Created by Tamar Ziv on 12/17/19.
//

#pragma once
#include <stdio.h>
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/Capture.h"
#include "cinder/params/Params.h"
#include "cinder/app/App.h"

using namespace ci;
using namespace std;

class ConwayCam;
typedef std::shared_ptr<ConwayCam> ConwayCamRef;

class ConwayCam {
public:
    static ConwayCamRef create();
	ConwayCam();
	~ConwayCam();
	
	void setup();
    void update();
    void draw();

	void loadShader();
	void createParams();
	void renderTexture( ci::gl::TextureRef& source, ci::gl::TextureRef& destination );

	// member vars
private:
	CaptureRef		mCapture;
	gl::TextureRef	mTextureSrc;
	gl::TextureRef	mTextureDst;
	ci::vec2 mRes;
	static ci::gl::BatchRef mBatchRenderer;
	
	// params
	params::InterfaceGlRef	mParams;
	float					mTime;
	float					mAgeSpeed;
	float					mMinThreshold;
	float					mMaxThreshold;
};
