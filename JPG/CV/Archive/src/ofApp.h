#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxGui.h"

using namespace ofxCv;
using namespace cv;

class CustomParticle : public ofxBox2dCircle {
    public:
        CustomParticle() {
        }
        ofColor color;
        ofImage theCat;
        void draw() {
            float radius = getRadius();
            theCat.load("cat.png");
            
            glPushMatrix();
            glTranslatef(getPosition().x-30, getPosition().y-30, 0);

            ofSetColor(color.r, color.g, color.b);
            theCat.draw(0,0);
            
            glPopMatrix();
        }
};

class ofApp : public ofBaseApp {
    
    public:
        void setup();
        void update();
        void draw();

        void keyPressed(int key);
        void keyReleased(int key);
        void mouseMoved(int x, int y);
        void mouseDragged(int x, int y, int button);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
        void mouseEntered(int x, int y);
        void mouseExited(int x, int y);
        void windowResized(int w, int h);
        void dragEvent(ofDragInfo dragInfo);
        void gotMessage(ofMessage msg);
    
        bool boolMouseF;
        
        ofxBox2d box2d; //box2d setup
        ofPolyline drawLine; //Adding/modifying vertexes to image
        ofxBox2dEdge edgeLine; //edge/line of box2d shape
        vector<shared_ptr<ofxBox2dCircle>> circle; //default box2d circles
        vector<shared_ptr<ofxBox2dRect>> box; //defalut box2d box
        vector<shared_ptr<CustomParticle>> customCatParticle;  //custom particle currently set as a cat
    
        float scale;
        ofxCvColorImage colorImg;
        ofxCvGrayscaleImage grayImg, grayBg, grayDiff; // depth image for contour
        ofxCvContourFinder contourFinder;
         
        bool boolThresholdOpenCV;
     
        ofVideoGrabber vidGrabber;
        ofxPanel gui;
};
