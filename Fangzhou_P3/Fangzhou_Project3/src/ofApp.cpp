#include "ofApp.h"
using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
void ofApp::setup(){

    camWidth = 640;
    camHeight = 480;
    
    cam.setup(camWidth, camHeight);
    contourFinder.setMinAreaRadius(10);
    contourFinder.setMaxAreaRadius(150);
    //contourFinder.setInvert(true); // find black instead of white
    
    gui.setup();
    gui.add(threshold.set("Threshold", 30, 0, 255));
    gui.add(trackHs.set("Track Hue/Saturation", false));
    
    
    bgMusic.load("bgmusic.mp3")
    crash.load("crash.mp3");
    bass.load("kick-bass.mp3");
    snare.load("snare.mp3");
    tom1.load("tom-1.mp3");
    tom2.load("tom-2.mp3");
    tom3.load("tom-3.mp3");
    tom4.load("tom-4.mp3");
}

//--------------------------------------------------------------
void ofApp::update(){
    cam.update();
    ofBackground(0, 0, 0);
    
    if(cam.isFrameNew()) {
        flipCam.setFromPixels(cam.getPixels());
        flipCam.mirror(false, true);
        //camTracker.update(toCv(flipCam));
        contourFinder.setTargetColor(targetColor, trackHs ? TRACK_COLOR_HS : TRACK_COLOR_RGB);
        contourFinder.setThreshold(threshold);
        contourFinder.findContours(cam);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255);
    cam.draw(0, 0);
    //cam.draw( cam.getWidth(),0,-cam.getWidth(),cam.getHeight());
    bgMusic.play();
    
    ofSetLineWidth(3);
    contourFinder.draw();
    
    gui.draw();
    
    ofTranslate(8, 75);
    ofFill();
    ofSetColor(0);
    ofDrawRectangle(-3, -3, 64+6, 64+6);
    ofSetColor(targetColor);
    ofDrawRectangle(0, 0, 64, 64);
    
    //draw user interfaces
    ofSetColor(220);
    ofDrawEllipse(camWidth / 6, camHeight / 2, 80, 80);
    
    ofSetColor(0,255,255,200);
    ofDrawEllipse(camWidth / 6 * 2, camHeight / 2, 80, 80);

    ofSetColor(0,200,255,100);
    ofDrawEllipse(camWidth / 6 * 3, camHeight / 2, 80, 80);
    
    ofSetColor(255,255,255,150);
    ofDrawEllipse(camWidth / 6 * 4, camHeight / 2, 80, 80);
    
    ofSetColor(0,200,255,150);
    ofDrawEllipse(camWidth / 6 * 5, camHeight / 2, 80, 80);
    
    
    int n = contourFinder.size();
    for(int i = 0; i < n; i++) {
        cv::RotatedRect ellipse = contourFinder.getFitEllipse(i);
        ofPushMatrix();
        ofVec2f ellipseCenter = toOf(ellipse.center);
        ofTranslate(ellipseCenter.x, ellipseCenter.y);
        ofSetColor(255, 255, 255);
        //ofDrawEllipse(0, 0, 20, 20);
        ofPopMatrix();
        
        if ( ellipseCenter.x > camWidth / 6 - 40 && ellipseCenter.x < camWidth / 6 + 40 &&  ellipseCenter.y > camHeight / 2 - 40 &&  ellipseCenter.y < camHeight / 2 + 40) {
            crash.play();
            ofSetColor(200);
            ofDrawEllipse(camWidth / 6, camHeight / 2, 150, 90);
        }
        
        if ( ellipseCenter.x > camWidth / 6 * 2 - 40 && ellipseCenter.x < camWidth / 6 * 2 + 40 &&  ellipseCenter.y > camHeight / 2 - 40 &&  ellipseCenter.y < camHeight / 2 + 40) {
            bass.play();
            ofSetColor(0,200,200);
            ofDrawEllipse(camWidth / 6 * 2, camHeight / 2, 150, 90);
        }
        
        if ( ellipseCenter.x > camWidth / 6 * 3 - 40 && ellipseCenter.x < camWidth / 6 * 3 + 40 &&  ellipseCenter.y > camHeight / 2 - 40 &&  ellipseCenter.y < camHeight / 2 + 40) {
            snare.play();
            ofSetColor(0,200,255);
            ofDrawEllipse(camWidth / 6 * 3, camHeight / 2, 120, 90);
        }
        
        if ( ellipseCenter.x > camWidth / 6 * 4 - 40 && ellipseCenter.x < camWidth / 6 * 4 + 40 &&  ellipseCenter.y > camHeight / 2 - 40 &&  ellipseCenter.y < camHeight / 2 + 40) {
            tom1.play();
            ofSetColor(255,255,255,200);
            ofDrawEllipse(camWidth / 6 * 4, camHeight / 2, 120, 90);
        }
        
        if ( ellipseCenter.x > camWidth / 6 * 5 - 40 && ellipseCenter.x < camWidth / 6 * 5 + 40 &&  ellipseCenter.y > camHeight / 2 - 40 &&  ellipseCenter.y < camHeight / 2 + 40) {
            tom2.play();
            ofSetColor(255,255,255,200);
            ofDrawEllipse(camWidth / 6 * 5, camHeight / 2, 120, 90);
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    targetColor = cam.getPixels().getColor(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
