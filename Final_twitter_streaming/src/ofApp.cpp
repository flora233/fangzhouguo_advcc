#include "ofApp.h"

//===============functions for face tracker================
// Used to extract eye rectangles. from ProCamToolkit
GLdouble modelviewMatrix[16], projectionMatrix[16];
GLint viewport[4];

void updateProjectionState() {
    glGetDoublev(GL_MODELVIEW_MATRIX, modelviewMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
    glGetIntegerv(GL_VIEWPORT, viewport);
}

glm::vec3 ofWorldToScreen(glm::vec3 world) {
    updateProjectionState();
    glm::vec3 screen = glm::project(glm::dvec3(world),
                                    glm::make_mat4(modelviewMatrix),
                                    glm::make_mat4(projectionMatrix),
                                    glm::make_vec4(viewport));
    screen.y = ofGetHeight() - screen.y;
    return screen;
}

ofMesh getProjectedMesh(const ofMesh& mesh) {
    ofMesh projected = mesh;
    for(std::size_t i = 0; i < mesh.getNumVertices(); i++) {
        glm::vec3 cur = ofWorldToScreen(mesh.getVerticesPointer()[i]);
        cur.z = 0;
        projected.setVertex(i, cur);
    }
    return projected;
}

template <class T>
void addTexCoords(ofMesh& to, const std::vector<T>& from) {
    for(std::size_t i = 0; i < from.size(); i++) {
        to.addTexCoord(glm::vec2(from[i].x, from[i].y));
    }
}
//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFullscreen(true);
    w = ofGetWindowWidth();
    h = ofGetWindowHeight();
    
    bgSound.load("sound/bgsound.wav");
    bgSound.setLoop(true);
    bgSound.play();
    
    clickSound.load("sound/clicksound.wav");
    clickSound.setLoop(false);
    
    hitSound.load("sound/hitsound.wav");
    hitSound.setLoop(false);
    
//    receiver.setup(PORT);
    //twitter.setup();
    
    //-----------------------------face tracking--------------------------
    ofSetFrameRate(60);
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
    cam.initGrabber(w, h);
    ofBackgroundHex(0);
    // Set up images we'll use for frame-differencing
    camImage.allocate(w, h, OF_IMAGE_COLOR);
    eyeImageColor.allocate(w/5, h/10);
    eyeImageGray.allocate(w/5, h/10);
    eyeImageGrayPrev.allocate(w/5, h/10);
    eyeImageGrayDif.allocate(w/5, h/10);
    eyeFbo.allocate(w/5, h/10, GL_RGB);
    
    // Initialize our images to black.
    eyeImageColor.set(0);
    eyeImageGray.set(0);
    eyeImageGrayPrev.set(0);
    eyeImageGrayDif.set(0);
    
    // Set up other objects.
    tracker.setup();
    osc.setup("localhost", 8338);
    
    // This GUI slider adjusts the sensitivity of the blink detector.
    gui.setup();
    gui.add(percentileThreshold.setup("Threshold", 0.92, 0.75, 1.0));
    //gui
    gui.add(ui_button.setup("search", 80, 20));
    ui_button.addListener(this, &ofApp::UIButtonPressed);
    
    //---------------------box2d--------------------------
    ofDisableAntiAliasing();
    ofSetLogLevel(OF_LOG_NOTICE);
    ofSetVerticalSync(true);
    
    // Box2d
    box2d.init();
    box2d.setGravity(0, 10);
    box2d.createGround(0,h,w,h);
    box2d.setFPS(30.0);
    
    //load strings as shapes and convert to vertex
    vector < string > tagsText;
    ofBuffer buffer = ofBufferFromFile("tagsText.txt");
    for (auto line : buffer.getLines()){
        tagsText.push_back(line);
    }
    
    auto poly = std::make_shared<ofxBox2dPolygon>();
    for (int i = 0; i < tagsText.size(); i++) {
        ofLogNotice() << string(tagsText[i]) << endl;
        font.load("cmb10.ttf", 150, true, true, true);
        bool vflip=true;
        bool filled=false;
        contour = font.getStringAsPoints(tagsText[i], vflip, filled);
        for (int a = 0; a < contour.size(); a++) {
            for (int b = 0; b < (int)contour[a].getOutline().size();b++){
                ofLogNotice() << contour[a].getOutline().size() << endl;
                if( b!= 0)ofNextContour(true) ;
                contour[a].getOutline()[b].getResampledBySpacing(3);
                for (int c = 0; c < (int)contour[a].getOutline()[b].size(); c++) {
                    ofVertex(w / 3 + contour[a].getOutline()[b].getVertices()[c].x,   contour[a].getOutline()[b].getVertices()[c].y, 0);
                    ofDefaultVertexType p = ofDefaultVertexType(w / 3 + contour[a].getOutline()[b].getVertices()[c].x,   contour[a].getOutline()[b].getVertices()[c].y, 0);
                        contourData.push_back(p);
                        //ofLogNotice() << contour[a].getOutline()[b].getVertices()[c].y << endl;
                }
            }
//            poly->addVertices(contourData);
//            poly->setPhysics(1.0, 0.3, 0.3);
//            poly->triangulatePoly();
//            poly->create(box2d.getWorld());
//            polyShapes.push_back(poly);
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
//    if((int)ofRandom(0, 10) == 0) {
//        auto circle = std::make_shared<ofxBox2dCircle>();
//        circle->setPhysics(0.3, 0.5, 0.1);
//        circle->setup(box2d.getWorld(), (ofGetWidth()/2)+ofRandom(-20, 20), -20, ofRandom(10, 20));
//        circles.push_back(circle);
//    }
    
    // remove shapes offscreen
    //ofRemove(circles, shouldRemove);
    box2d.update();
//    //-----------------------recieve message from sender-------
//    while (receiver.hasWaitingMessages()) {
//        ofxOscMessage m;
//        receiver.getNextMessage(&m);
//
//        if(m.getAddress() == "/count" ) {
//            int count = m.getArgAsInt(0);
//            ofLogNotice() << count << endl;
//        }
//
//        if(m.getAddress() == "/searchContent") {
//            searchContent = m.getArgAsString(0);
//            ofLogNotice() << searchContent << endl;
//        }
//    }
    
    //----------------------face track--------------------
    cam.update();
    if(cam.isFrameNew()) {
        camImage.setFromPixels(cam.getPixels());
        
        tracker.update(ofxCv::toCv(cam));
        position = tracker.getPosition();
        scale = tracker.getScale();
        rotationMatrix = tracker.getRotationMatrix();
        
        if(tracker.getFound()) {
            glm::vec2
            leftOuter = tracker.getImagePoint(36),
            leftInner = tracker.getImagePoint(39),
            rightInner = tracker.getImagePoint(42),
            rightOuter = tracker.getImagePoint(45);
            
            ofPolyline leftEye = tracker.getImageFeature(ofxFaceTracker::LEFT_EYE);
            ofPolyline rightEye = tracker.getImageFeature(ofxFaceTracker::RIGHT_EYE);
            
            glm::vec2 leftCenter = leftEye.getBoundingBox().getCenter();
            glm::vec2 rightCenter = rightEye.getBoundingBox().getCenter();
            
            float leftRadius = (glm::distance(leftCenter, leftInner) + glm::distance(leftCenter, leftOuter)) / 2;
            float rightRadius = (glm::distance(rightCenter, rightInner) + glm::distance(rightCenter, rightOuter)) / 2;
            
            glm::vec3
            leftOuterObj = tracker.getObjectPoint(36),
            leftInnerObj = tracker.getObjectPoint(39),
            rightInnerObj = tracker.getObjectPoint(42),
            rightOuterObj = tracker.getObjectPoint(45);
            
            glm::vec3 upperBorder(0, -3.5, 0), lowerBorder(0, 2.5, 0);
            glm::vec3 leftDirection(-1, 0, 0), rightDirection(+1, 0, 0);
            float innerBorder = 1.5, outerBorder = 2.5;
            
            ofMesh leftRect, rightRect;
            leftRect.setMode(OF_PRIMITIVE_LINE_LOOP);
            leftRect.addVertex(leftOuterObj + upperBorder + leftDirection * outerBorder);
            leftRect.addVertex(leftInnerObj + upperBorder + rightDirection * innerBorder);
            leftRect.addVertex(leftInnerObj + lowerBorder + rightDirection * innerBorder);
            leftRect.addVertex(leftOuterObj + lowerBorder + leftDirection * outerBorder);
            rightRect.setMode(OF_PRIMITIVE_LINE_LOOP);
            rightRect.addVertex(rightInnerObj+ upperBorder + leftDirection * innerBorder);
            rightRect.addVertex(rightOuterObj + upperBorder + rightDirection * outerBorder);
            rightRect.addVertex(rightOuterObj + lowerBorder + rightDirection * outerBorder);
            rightRect.addVertex(rightInnerObj + lowerBorder + leftDirection * innerBorder);
            
            ofPushMatrix();
            ofSetupScreenOrtho(640, 480, -1000, 1000);
            ofScale(1, 1, -1);
            ofTranslate(position);
            ofxCv::applyMatrix(rotationMatrix);
            ofScale(scale, scale, scale);
            leftRectImg = getProjectedMesh(leftRect);
            rightRectImg = getProjectedMesh(rightRect);
            ofPopMatrix();
            
            // more effective than using object space points would be to use image space
            // but translate to the center of the eye and orient the rectangle in the
            // direction the face is facing.
            
             ofPushMatrix();
             ofTranslate(tracker.getImageFeature(ofxFaceTracker::LEFT_EYE).getCentroid2D());
             ofxCv::applyMatrix(rotationMatrix);
             //ofDrawRectangle(-50, -40, 2*50, 2*40);
             ofPopMatrix();
             
             ofPushMatrix();
             ofTranslate(tracker.getImageFeature(ofxFaceTracker::RIGHT_EYE).getCentroid2D());
             ofxCv::applyMatrix(rotationMatrix);
             ofDrawRectangle(-50, -40, 2*50, 2*40);
             ofPopMatrix();
            
            
            ofMesh normRect, normLeft, normRight;
            normRect.addVertex(glm::vec3(0, 0, 0));
            normRect.addVertex(glm::vec3(64, 0, 0));
            normRect.addVertex(glm::vec3(64, 48, 0));
            normRect.addVertex(glm::vec3(0, 48, 0));
            normLeft.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
            normRight.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
            normLeft.addVertices(normRect.getVertices());
            normRight.addVertices(normRect.getVertices());
            addTexCoords(normLeft, leftRectImg.getVertices());
            addTexCoords(normRight, rightRectImg.getVertices());
            
            // Copy the extracted quadrilaterals into the eyeFbo
            eyeFbo.begin();
            ofSetColor(255);
            ofFill();
            cam.getTexture().bind();
            normLeft.draw();
            ofTranslate(64, 0);
            normRight.draw();
            cam.getTexture().unbind();
            eyeFbo.end();
            eyeFbo.readToPixels(eyePixels);
            
            
            // Convert the combined eye-image to grayscale,
            // and put its data into a frame-differencer.
            eyeImageColor.setFromPixels(eyePixels);
            eyeImageGrayPrev.setFromPixels(eyeImageGray.getPixels());
            eyeImageGray.setFromColorImage(eyeImageColor);
            eyeImageGray.contrastStretch();
            eyeImageGrayDif.absDiff(eyeImageGrayPrev, eyeImageGray);
            
            // Compute the average brightness of the difference image.
            unsigned char* difPixels = eyeImageGrayDif.getPixels().getData();
            int nPixels = 128*48;
            float sum = 0;
            for (int i=0; i<nPixels; i++){
                if (difPixels[i] > 4){ // don't consider diffs less than 4 gray levels;
                    sum += difPixels[i]; // reduces noise
                }
            }
            // Store the current average in the row graph
            float avg = sum / (float) nPixels;
            rowGraph.addSample(avg, percentileThreshold);
            
            // Send out an OSC message,
            // With the value 1 if the current average is above threshold
            ofxOscMessage msg;
            msg.setAddress("/gesture/blink");
            int oscMsgInt = (rowGraph.getState() ? 1 : 0);
            msg.addIntArg(oscMsgInt);
            osc.sendMessage(msg);
            
            // Print out a message to the console if there was a blink.
            if (oscMsgInt > 0){
                isBlink = true;
                printf("Blink happened at frame #:    %d\n", (int)ofGetFrameNum());
            }
            
        }
    }
    
    //----------------------interavtions------------------
    if(isBlink == true) {
        hitSound.play();
        auto p = std::make_shared<CustomParticle>();
        p.get()->setPhysics(1.0, 0.5, 0.3);
        p.get()->setup(box2d.getWorld(), w/2, 100, ofRandom(30, 50));
        p.get()->setVelocity(ofRandom(-30, 30), ofRandom(-30, 30));
        p.get()->setupTheCustomData();
        particles.push_back(p);
        isBlink = false;
    }
    
//    for (int i = 0; i < particles.size(); i++) {
//        ofVec2f pPos = particles[i].get()->getWorldPosition();
//        if (pPos.x > w || pPos.x < 0 || pPos.y > h) {
//            particles[i].get()->~CustomParticle();
//        }
//    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    //twitter
    //twitter.draw();
    
    //gui
    ofSetColor(255);
    int width = 150;
    int height = 30;
    
    ofPushStyle();
    //monoLineTextInput.draw();
    ofPopStyle();
    
    
    //draw the ground grind
    ofPushMatrix();
    ofTranslate(0,h);
    ofRotateXDeg(-95);
    ofNoFill();
    gdMovingSpeed += 0.1;
    for (int a = -1000; a < 2000; a +=60) {
        for (int b = -1000; b < 2000; b+=60) {
            ofSetLineWidth(1);
            ofSetColor(255, 255, 255, 30);
            ofDrawLine(a, -1000, 10*sinf(gdMovingSpeed), a, 2000, 10*sinf(gdMovingSpeed));
            ofDrawLine(-1000, b + gdMovingSpeed, 50*cosf(gdMovingSpeed), 2000, b+ gdMovingSpeed, 50*cosf(gdMovingSpeed));
        }
    }
    ofPopMatrix();
    
    // draw the text shape
    ofSetColor(255);
    ofNoFill();
    for(int i=0; i<polyShapes.size(); i++) {
        polyShapes[i]->draw();
        ofDrawCircle(polyShapes[i]->getPosition(), 3);
    }
    ofFill();
    ofSetColor(0, 255, 255);

    for(int i=0; i<particles.size(); i++) {
        particles[i].get()->draw();
    }
        
    for(int i=0; i<circles.size(); i++) {
        ofFill();
        ofSetColor(0,255,255);
        ofPushMatrix();
        ofTranslate(tracker.getImageFeature(ofxFaceTracker::INNER_MOUTH).getCentroid2D().x - w/2, tracker.getImageFeature(ofxFaceTracker::INNER_MOUTH).getCentroid2D().y);
        circles[i]->draw();
        ofPopMatrix();
    }
    
    //----------------------face tracking-----------------------
    ofSetColor(255);
    
    //camImage.draw(0, 0);
    tracker.draw();
    //leftRectImg.draw();
    //rightRectImg.draw();
    
    float y = 80;
    gui.draw();
    //eyeImageGray.draw(10, y);    y+=58;
    //eyeImageGrayDif.draw(10, y); y+=58;
    rowGraph.draw(10, y, 48);
    ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, ofGetHeight() - 20);
    
    // draw eye lasers
    if(particles.size() > 3) {
        int particlesSize = particles.size() - 2;
        ofVec2f pPos = particles[particlesSize].get()->getWorldPosition();
        //left eye
        ofPushMatrix();
        ofVec2f leftEyePos = tracker.getImageFeature(ofxFaceTracker::LEFT_EYE).getCentroid2D();
        ofTranslate(leftEyePos);
        ofSetColor(0, 255, 255);
        ofSetLineWidth(2);
        ofDrawLine(0,0,-leftEyePos.x + pPos.x, h -leftEyePos.y + pPos.y);
        ofPopMatrix();
        //righteye
        ofVec2f pPos2 = particles[particlesSize - 1].get()->getWorldPosition();
        ofVec2f rightEyePos = tracker.getImageFeature(ofxFaceTracker::RIGHT_EYE).getCentroid2D();
        ofTranslate(rightEyePos);
        ofSetColor(0, 255, 255);
        ofSetLineWidth(3);
        ofDrawLine(0,0,-leftEyePos.x + pPos2.x, h -leftEyePos.y + pPos2.y);
        ofPopMatrix();
    }
    
    ofSetLineWidth(1);
    
//    ofPushMatrix();
//    ofTranslate(w/2, h/2);
//    vector<ofPolyline> x = contour[0].getOutline();
//    x[0].draw();
//    ofPopMatrix();
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    //face tracking
    if(key == 'r') {
        tracker.reset();
    }
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

void ofApp::UIButtonPressed(){
    clickSound.play();
}

