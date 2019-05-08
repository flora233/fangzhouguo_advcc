#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"

#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxOsc.h"
#include "ofxGui.h"
#include "ofxFaceTracker.h"

#include "ofMain.h"
//#include "ofxTwitter.h"
//#include "ofxOsc.h"

#define PORT 8000

//
class Data {
public:
    ofColor color;
    string  name;
    int     id;
};

// A Custom Particle extending the box2d circle
class CustomParticle : public ofxBox2dCircle {
    
public:
    
    void setupTheCustomData() {
        
        static int colors[] = {0x000000, 0x000000, 0x000000};
        //static string abc   = "####################";
        
        vector < string > tagsText;
        ofBuffer buffer = ofBufferFromFile("tagsText.txt");
        for (auto line : buffer.getLines()){
            tagsText.push_back(line);
        }
        
        // we are using a Data pointer because
        // box2d needs to have a pointer not
        // a referance
        setData(new Data());
        Data * theData = (Data*)getData();
        
        theData->id = ofRandom(0, 100);
        theData->name += tagsText[(int)ofRandom(0, tagsText.size())];
        theData->color.setHex(colors[(int)ofRandom(0, 3)]);
        
        printf("setting the custom data!\n");
    }
    
    void draw() {
        Data* theData = (Data*)getData();
        if(theData) {
            
            // Even though we know the data object, let's just
            // see how we can get the data out from box2d.
            // You would use this when using a contact listener
            // or tapping into box2d's solver.
            
            float radius = getRadius();
            ofPushMatrix();
            ofTranslate(getPosition());
            ofRotateZDeg(getRotation());
            ofSetColor(theData->color);
            ofFill();
            ofDrawCircle(0, 0, radius);
            
            float textSize = radius / 10;
            ofPushMatrix();
            ofScale(textSize, textSize);
            ofSetColor(0,255,255);
            ofDrawBitmapString(theData->name, -textSize/2, textSize);
            ofPopMatrix();
            
            ofPopMatrix();
        }
    }
};

//---------------class for ofxtwitter--------------------------
//class Twitter {
//public:
//    Twitter();
//
//    ofxTwitter::SearchClient client;
//    uint64_t count = 0;
//    uint64_t countMissed = 0;
//
//    void setup() {
//        // Easily register for all search events.
//        client.registerSearchEvents(this);
//        //client.setCredentialsFromFile("credentials.json");
//        ofxHTTP::OAuth10Credentials credentials("9OpsZyOaUQ4R7p6nos9Q30G03",
//                                                "MkQ9KkdCTLbCypOpZbMyYktuQsV1xLRvvoj6uogh6ftp92bErP",
//                                                "793590069609193473-AKoouvbL1mThx7SUe6OCpPW31FnAHni",
//                                                "G62VpHraQne1mJAeByoNqXq8e3sgXp38O7MJpllD2TGLP");
//        client.setCredentials(credentials);
//        client.setPollingInterval(6000);
//
//        // This starts a simple search for an emoticon.
//        //client.search("love");
//
//        // To design more complex searches, see the the API documentation here:
//        // https://dev.twitter.com/rest/public/search and use the
//        ofxTwitter::SearchQuery query(":)");
//        query.setGeoCode(41.8781, -87.6298, 10, ofxTwitter::SearchQuery::UNITS_MILES);
//        query.setLanguage("en");
//        client.search(query);
//        //
//    }
//
//    void draw(){
//        auto total = count + countMissed;
//
//        std::stringstream ss;
//        ss << "  Received: " << count << std::endl;
//        ss << "    Missed: " << countMissed << std::endl;
//        ss << "     Total: " << total << std::endl;
//
//        std::string received = "...";
//
//        if (total > 0)
//        {
//            received = ofToString(static_cast<double>(count) / total * 100, 2);
//        }
//        ss << "% Received: " << received;
//        ofDrawBitmapStringHighlight(ss.str(), 14, 14);
//    }
//
//    void onStatus(const ofxTwitter::Status& status)
//    {
//        count++;
//        ofLogNotice("ofApp::onStatus") << "Text: " << status.text();
//        ofLogNotice("ofApp::onStatus") << "\tCoordinates: " << (status.coordinates() ? ofToString(status.coordinates()) : "NONE");
//        ofLogNotice("ofApp::onStatus") << "\tPlace: " << (status.place() ? ofToString(status.place()->fullName()) : "NONE");
//    }
//
//
//    void onError(const ofxTwitter::Error& error)
//    {
//        ofLogError("ofApp::onError") << "Error: " << error.code() << " " << error.message();
//    }
//
//
//    void onException(const std::exception& notice)
//    {
//        ofLogError("ofApp::onException") << "Exception: " << notice.what();
//    }
//
//
//    void onMessage(const ofJson& json)
//    {
//        // This is the raw message json and is ignored here.
//    }
//
//};

//---------------class for face recognition--------------------
class Graph {
public:
    Graph()
    :maxSize(128)
    ,threshold(0)
    ,state(false) {
    }
    void setup(int maxSize) {
        this->maxSize = maxSize;
    }
    void addSample(float sample) {
        buffer.push_back(sample);
        if(buffer.size() > maxSize) {
            buffer.pop_front();
        }
        
        // get the median at the 92nd percentile
        vector<float> all;
        all.assign(buffer.begin(), buffer.end());
        ofSort(all);
        float percentile = 0.92;
        threshold = all[(int) (all.size() * percentile)];
    }
    
    void addSample(float sample, float percentile) {
        buffer.push_back(sample);
        if(buffer.size() > maxSize) {
            buffer.pop_front();
        }
        
        // get the median at the provided percentile
        vector<float> all;
        all.assign(buffer.begin(), buffer.end());
        ofSort(all);
        threshold = all[(int) (all.size() * percentile)];
    }
    
    void glMapX(float minInput, float maxInput, float minOutput, float maxOutput) {
        float inputRange = maxInput - minInput;
        float outputRange = maxOutput - minOutput;
        ofTranslate(minOutput, 0);
        ofScale(outputRange, 1);
        ofScale(1. / inputRange, 1);
        ofTranslate(-minInput, 0);
    }
    void glMapY(float minInput, float maxInput, float minOutput, float maxOutput) {
        float inputRange = maxInput - minInput;
        float outputRange = maxOutput - minOutput;
        ofTranslate(0, minOutput);
        ofScale(1, outputRange);
        ofScale(1, 1. / inputRange);
        ofTranslate(0, -minInput);
    }
    ofRectangle getBounds() {
        return line.getBoundingBox();
    }
    void draw(int x, int y, int height) {
        line.clear();
        for(int i = 0; i < buffer.size(); i++) {
            line.addVertex(glm::vec3(i, buffer[i], 0));
        }
        ofPushMatrix();
        ofPushStyle();
        
        ofRectangle box = getBounds();
        
        ofTranslate(x, y);
        ofFill();
        bool bright = !buffer.empty() && threshold != 0 && buffer.back() > threshold;
        ofSetColor(bright ? 160 : 0);
        ofDrawRectangle(0, 0, maxSize, height);
        ofNoFill();
        ofSetColor(255);
        ofDrawRectangle(0, 0, maxSize, height);
        
        if(!buffer.empty()) {
            glMapX(box.x, box.x + box.width, 0, maxSize);
            glMapY(box.y, box.y + box.height, height, 0);
            line.draw();
            ofDrawLine(0, threshold, buffer.size(), threshold);
            state = buffer.back() > threshold;
        }
        ofPopStyle();
        ofPopMatrix();
    }
    bool getState() {
        return state;
    }
private:
    ofPolyline line;
    int maxSize;
    deque<float> buffer;
    float threshold;
    bool state;
};

//------------------------------------------------
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
        void UIButtonPressed();
    
    int w;
    int h;
    float gdMovingSpeed = 0;
    ofSoundPlayer bgSound;
    ofSoundPlayer clickSound;
    ofSoundPlayer hitSound;
    ofxOscReceiver receiver;
    string searchContent;
    //Twitter twitter;
    //interactions
    ofxButton ui_button;
    
    //box2d
    ofxBox2d box2d;
    vector <ofPolyline>                   shape;
    vector <shared_ptr<ofxBox2dPolygon>>  polyShapes;
    vector <shared_ptr<ofxBox2dCircle>>   circles;
    vector <shared_ptr<CustomParticle> > particles;
    
    ofTrueTypeFont font;
    std::vector<ofDefaultVertexType> contourData;
    std::vector<ofPath> contour;
    ofPath xx;
    
    //------------------fece track-----------------
    ofVideoGrabber cam;
    ofxFaceTracker tracker;
    ofImage camImage;
    
    ofFbo eyeFbo;
    ofPixels eyePixels;
    ofxCvColorImage        eyeImageColor;
    ofxCvGrayscaleImage    eyeImageGray;
    ofxCvGrayscaleImage    eyeImageGrayPrev;
    ofxCvGrayscaleImage    eyeImageGrayDif;
    
    ofMesh leftRectImg, rightRectImg;
    Graph rowGraph;
    
    glm::vec2 position;
    ofxFloatSlider percentileThreshold;
    ofxPanel gui;
    
    float scale;
    ofMatrix4x4 rotationMatrix;
    
    ofxOscSender osc;
    //-----------------interactions--------------
    bool isBlink = false;
		
};
