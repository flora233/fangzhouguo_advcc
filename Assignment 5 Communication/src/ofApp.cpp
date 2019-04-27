#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    // OSC
    
    receiver.setup(PORT);
    
    
    // png for drawing
    
    bgPNG.load("bodies_png/bg.png");
    headPNG.load("bodies_png/bodies-11.png");
    bodyPNG.load("bodies_png/bodies-17.png");
    ruaPNG.load("bodies_png/bodies-12.png");
    rlaPNG.load("bodies_png/bodies-10.png");
    luaPNG.load("bodies_png/bodies-19.png");
    llaPNG.load("bodies_png/bodies-18.png");
    rulPNG.load("bodies_png/bodies-13.png");
    rllPNG.load("bodies_png/bodies-15.png");
    lulPNG.load("bodies_png/bodies-14.png");
    lllPNG.load("bodies_png/bodies-16.png");
    
    
    
    centerX = ofGetWidth()/2;
    centerY = ofGetHeight()/3+50;
    
    // sizes
    bodys = ofVec2f(150, 250);
    heads = ofVec2f(120, 110);
    rUAs = ofVec2f(100, 45);
    rLAs = ofVec2f(150, 50);
    lUAs = ofVec2f(100, 45);
    lLAs = ofVec2f(150, 50);
    rULs = ofVec2f(60, 167);
    rLLs = ofVec2f(65, 180);
    lULs = ofVec2f(55, 167);
    lLLs = ofVec2f(95, 180);
    
    
    // coordinates
    
    // primiry joints
    
    center = ofVec2f(centerX, centerY);
    neck = ofVec2f( centerX, centerY-150 );
    rShoulder = ofVec2f( centerX-75, centerY-125 );
    lShoulder = ofVec2f( centerX+75, centerY-125 );
    rHip = ofVec2f( centerX-bodys.x/3, centerY+bodys.y/2 );
    lHip = ofVec2f( centerX+bodys.x/3, centerY+bodys.y/2 );

    // secondary joints
    
    rElbow = ofVec2f( rShoulder.x-rUAs.x, rShoulder.y );
    lElbow = ofVec2f( lShoulder.x+lUAs.x, lShoulder.y );
    rKnee = ofVec2f( rHip.x, rHip.y+rULs.y );
    lKnee = ofVec2f( lHip.x, lHip.y+rULs.y );
    
    
    // PNG img pos
    
    // right arm
    ruaPos = ofVec2f(rElbow.x+40, rElbow.y+30);
    rlaPos = ofVec2f(rElbow.x-90, rElbow.y+20);
    
    // left arm
    luaPos = ofVec2f(lShoulder.x-40, lShoulder.y+25);
    llaPos = ofVec2f(lElbow.x-60,lElbow.y+25);
    
    // right leg
    rulPos = ofVec2f(rHip.x-10, rHip.y-rULs.y/3-10);
    rllPos = ofVec2f(rKnee.x-20, rKnee.y-100);
    
    // left leg
    lulPos = ofVec2f(lHip.x-lULs.x, lHip.y-rULs.y/3-10);
    lllPos = ofVec2f(lKnee.x-60, lKnee.y-100);
    
    
    // positions vec
    positions.push_back(&ruaPos); positions.push_back(&rlaPos);
    positions.push_back(&luaPos); positions.push_back(&llaPos);
    positions.push_back(&rulPos); positions.push_back(&rllPos);
    positions.push_back(&lulPos); positions.push_back(&lllPos);
    
    
    // initial speeds
    RUAspeed=0; RLAspeed=0;
    LUAspeed=0; LLAspeed=0;
    RULspeed=0; RLLspeed=0;
    LULspeed=0; LLLspeed=0;
    
    
    // speeds
    speeds.push_back(&RUAspeed); speeds.push_back(&RLAspeed);
    speeds.push_back(&LUAspeed); speeds.push_back(&LLAspeed);
    speeds.push_back(&RULspeed); speeds.push_back(&RLLspeed);
    speeds.push_back(&LULspeed); speeds.push_back(&LLLspeed);
    
    

}

//--------------------------------------------------------------
void ofApp::update(){
    
    while(receiver.hasWaitingMessages()){
        ofxOscMessage m;
        
        ofLogNotice() << "has new message" << endl;
        
        if(receiver.getNextMessage(m)){
            if(m.getAddress() == "/2/sendlevel/1"){
                auto s = m.getArgAsFloat(0);
                float sp = ofClamp(s, 0, 10);
                
                RUAspeed = sp*10;
//                ruaPos.x += sp*10;
//                ofLogNotice() << "ruaPos.x " << ruaPos.x << endl;
                
            } else if (m.getAddress() == "/2/sendlevel/2"){
                auto s = m.getArgAsFloat(0);
                float sp = ofClamp(s, 0, 10);
                
                //ofLogNotice() << "RLAspeed push_back " << sp*10 << endl;
                RLAspeed = sp*10;
                
            } else if (m.getAddress() == "/2/sendlevel/3"){
                auto s = m.getArgAsFloat(0);
                //ofLogNotice() << x << endl;
                float sp = ofClamp(s, 0, 10);
                LUAspeed = sp*10;
                
            } else if (m.getAddress() == "/2/sendlevel/4"){
                auto s = m.getArgAsFloat(0);
                //ofLogNotice() << x << endl;
                float sp = ofClamp(s, 0, 10);
                LLAspeed = sp*10;
                
            } else if (m.getAddress() == "/2/fxpar1"){
                auto s = m.getArgAsFloat(0);
                //ofLogNotice() << x << endl;
                float sp = ofClamp(s, 0, 10);
                RULspeed = sp*10;
                
            } else if (m.getAddress() == "/2/fxpar2"){
                auto s = m.getArgAsFloat(0);
                //ofLogNotice() << x << endl;
                float sp = ofClamp(s, 0, 10);
                RLLspeed = sp*10;
                
            } else if (m.getAddress() == "/2/fxpar3"){
                auto s = m.getArgAsFloat(0);
                //ofLogNotice() << x << endl;
                float sp = ofClamp(s, 0, 10);
                LULspeed = sp*10;
                
            } else if (m.getAddress() == "/2/fxpar4"){
                auto s = m.getArgAsFloat(0);
                //ofLogNotice() << x << endl;
                float sp = ofClamp(s, 0, 10);
                LLLspeed = sp*10;
            } else if (m.getAddress() == "/2/slot+"){
                auto s = m.getArgAsFloat(0);
                
                if(s==1){
                    bExport = true;
                } else if(s==0){
                    ofEndSaveScreenAsPDF();
                    bExport = false;
                }
            }
        }
    }
    
    
    // move imgs around
    for(int i=0; i<8; i++){
        
        positions[i]->x += ofRandom((*speeds[i])*-1, *speeds[i]);
        positions[i]->y += ofRandom((*speeds[i])*-1, *speeds[i]);
        
        
        if(i==0){
            ofLogNotice() << i << " s1: " << *speeds[i] << endl;
            //ofLogNotice() << i << " x: " << positions[i].x << endl;
        }
        
        // constrain
        
        if( positions[i]->x < 0 ){
            positions[i]->x = 700;
        } else if ( positions[i]->x > 700 ){
            positions[i]->x = 0;
        }
        
        if( positions[i]->y< 0 ){
            positions[i]->y = 700;
        } else if ( positions[i]->y > 700 ){
            positions[i]->y = 0;
        }
    }
    
    //ofLogNotice() << "RUA x: " << positions[0].x << " | RUA y: " << positions[0].y << endl;
    
    //ruaPos.x += RUAspeed;
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    // bg
    bgPNG.draw(20, -20, ofGetWidth()-40, ofGetHeight()-40);
    
    // body & head
    bodyPNG.draw(center.x-bodys.x/2, center.y-bodys.y/2, bodys.x, bodys.y);
    headPNG.draw(neck.x-heads.x/2, neck.y-heads.y/3, heads.x, heads.y);
    
    // right arm
    ruaPNG.draw(ruaPos, rUAs.x, rUAs.y);
    rlaPNG.draw(rlaPos, rLAs.x, rLAs.y);
    
    // left arm
    luaPNG.draw(luaPos, lUAs.x, lUAs.y);
    llaPNG.draw(llaPos, lLAs.x, lLAs.y);
    
    // right leg
    rulPNG.draw(rulPos, rULs.x, rULs.y);
    rllPNG.draw(rllPos, rLLs.x, rLLs.y);
    
    // left leg
    lulPNG.draw(lulPos, lULs.x, lULs.y);
    lllPNG.draw(lllPos, lLLs.x, lLLs.y);
    
    if(bExport){
        ofBeginSaveScreenAsPDF("body"+ofToString(ofGetElapsedTimef())+ ".pdf");
    }
}

//--------------------------------------------------------------
void ofApp::rotateRUA(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if(key == 'a'){
        bExport = true;
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key == 'a'){
        ofEndSaveScreenAsPDF();
        bExport = false;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

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
