#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxOsc.h"

#define PORT 8000

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        void rotateRUA();

        ofImage rUA_img;
        ofImage rLA_img;
    
        int centerX;
        int centerY;
    
    
        // sizes
        ofVec2f bodys;
        ofVec2f heads;
        ofVec2f rUAs; ofVec2f rLAs;
        ofVec2f lUAs; ofVec2f lLAs;
        ofVec2f rULs; ofVec2f rLLs;
        ofVec2f lULs; ofVec2f lLLs;
    
    
        // cooridinates
        ofVec2f center;
        ofVec2f neck;
        ofVec2f rShoulder;
        ofVec2f lShoulder;
        ofVec2f rElbow;
        ofVec2f lElbow;
        ofVec2f rHip;
        ofVec2f lHip;
        ofVec2f rKnee;
        ofVec2f lKnee;
    
        ofVec2f ruaPos;
        ofVec2f rlaPos;
        ofVec2f luaPos;
        ofVec2f llaPos;
        ofVec2f rulPos;
        ofVec2f rllPos;
        ofVec2f lulPos;
        ofVec2f lllPos;
    
        vector<ofVec2f*> positions;
    
    
        // png imgs
    
        ofImage headPNG;
        ofImage bodyPNG;
        ofImage ruaPNG;
        ofImage rlaPNG;
        ofImage luaPNG;
        ofImage llaPNG;
        ofImage rulPNG;
        ofImage rllPNG;
        ofImage lulPNG;
        ofImage lllPNG;
    
        ofImage bgPNG;
    
        ofxCvColorImage rUA;
        ofxCvColorImage rLA;
    
    
        // osc
    
        ofxOscReceiver receiver;
    
        float RUAspeed;
        float RLAspeed;
        float LUAspeed;
        float LLAspeed;
        float RULspeed;
        float RLLspeed;
        float LULspeed;
        float LLLspeed;
    
        vector<float*> speeds;
        bool bExport = false;
    
};
