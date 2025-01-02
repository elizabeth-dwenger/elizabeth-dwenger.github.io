#include "ofApp.h"
static int pts[] = {256,219,256,258,259,274,263,325,266,345,266,352,269,369,276,387,286,415,291,425,302,451,308,463,316,473,321,480,328,488,333,495,339,501,345,505,350,507,365,515,370,519,377,522,382,525,388,527,405,534,426,539,439,539,452,539,468,540,485,540,496,541,607,541,618,539,625,537,641,530,666,512,682,500,710,476,723,463,727,457,739,453,732,450,734,447,738,440,746,423,756,404,772,361,779,343,781,339,784,327,789,301,792,278,784,266,794,257,795,250,795,232,796,222,796,197,797,195,797,188,796,188};

//--------------------------------------------------------------
void ofApp::setup() {
    gui.setup();
    vidGrabber.initGrabber(340,240);
    ofSetFrameRate(60);

    ofSetVerticalSync(true);
    
    boolMouseF = false;
    
    // Set up box2d
    box2d.init();
    box2d.setGravity(0, 15);
    box2d.createGround();
    box2d.setFPS(60.0);
    box2d.registerGrabbing();
    
    // Create the shape
    edgeLine.setPhysics(0.0, 0.5, 0.5);
    edgeLine.create(box2d.getWorld());
    
    //allocate memory for img used for contour finder
    colorImg.allocate(vidGrabber.getWidth(), vidGrabber.getHeight());
    grayImg.allocate(vidGrabber.getWidth(), vidGrabber.getHeight());
    grayBg.allocate(vidGrabber.getWidth(), vidGrabber.getHeight());
    grayDiff.allocate(vidGrabber.getWidth(), vidGrabber.getHeight());

    // Compare background on open
    boolThresholdOpenCV = true;
    
    scale=2;

}

//--------------------------------------------------------------
void ofApp::update() {
    //Refresh the video data
    vidGrabber.update();
    
    // Code for finding contour of image
    if (vidGrabber.isFrameNew()){
        colorImg.setFromPixels(vidGrabber.getPixels());
        grayImg = colorImg; // Convert video to grayscale
        if (boolThresholdOpenCV == true) {
            grayBg = grayImg; // Update the background image
            boolThresholdOpenCV = false;
        }
        grayDiff.absDiff(grayBg, grayImg);
        grayDiff.threshold(17); // Threshold for contour detection
        contourFinder.findContours(grayDiff, 1000, (vidGrabber.getWidth()*vidGrabber.getHeight())/4, 4, false, true); // Find holes is set as true, so these will be found
    }

    
    box2d.update(); // Update the environment of box2D
    
  // Changing the mouse attraction force
    if(boolMouseF) {
        float strength = 8.0f;
        for(int i=0; i<circle.size(); i++) {
            circle[i].get()->addAttractionPoint(mouseX, mouseY, strength);
        }
        for(int i=0; i<customCatParticle.size(); i++) {
            customCatParticle[i].get()->addAttractionPoint(mouseX, mouseY, strength);
        }
        
    }
    
    // Delete the objects which are not displayed on screen
    ofRemove(box, ofxBox2dBaseShape::shouldRemoveOffScreen);
    ofRemove(circle, ofxBox2dBaseShape::shouldRemoveOffScreen);
    ofRemove(customCatParticle, ofxBox2dBaseShape::shouldRemoveOffScreen);
    
    for(int i=contourFinder.blobs.size()-1; i>=0; i--) {
        auto pts = contourFinder.blobs.at(i).pts;
        drawLine.clear();
        for(int i = 0; i<pts.size(); i++) {
            ofDefaultVec3 p = pts[i];
            p*=scale;
            drawLine.addVertex(p);
        }
    }
    
    if((int)ofRandom(0, 30) == 0) {
        ofxBox2dCircle c;
        c.setPhysics(0.35, 0.55, 0.15);
        c.setup(box2d.getWorld(), (ofGetWidth()/2)+ofRandom(-25, 25), -25, ofRandom(15, 25));
    }


}

//--------------------------------------------------------------
void ofApp::draw() {
    ofBackground(255, 255, 255);
    ofSetHexColor(0xffffff);
    grayImg.draw(0,0,340,240);
    
    contourFinder.draw(0,0,340,240);
    
    // Color of the circle
    for(int i=0; i<circle.size(); i++) {
        ofFill();
        ofSetHexColor(0x90cbe3);
        circle[i].get()->draw();
    }
    
    // Color of the box
    for(int i=0; i<box.size(); i++) {
        ofFill();
        ofSetHexColor(0xe63b6b);
        box[i].get()->draw();
    }
    
    // Code for custom particle
    for(int i=0; i<customCatParticle.size(); i++) {
        customCatParticle[i].get()->draw();
    }
    
    // Code to add a vertex
    ofNoFill();
    ofSetHexColor(0x363635);
    if(drawLine.size()==0) {
        edgeLine.updateShape();
        edgeLine.draw();
    }
    else drawLine.draw();
    
    // Info at top left to instruct user of what options they have
    string info = "";
    info += "Press [l] to draw a line\n";
    info += "Press [f] to toggle Mouse Force ["+ofToString(boolMouseF)+"]\n";
    info += "Press [b] to drop a ball\n";
    info += "Press [s] to drop a square\n";
    info += "Press [c] to drop a custom particle\n";
    info += "Press [spacebar] to reset background for detection\n";
    info += "Total Objects: "+ofToString(box2d.getBodyCount())+"\n";
    info += "FPS: "+ofToString(ofGetFrameRate())+"\n";

    ofSetHexColor(0x302f2f);
    ofDrawBitmapString(info, 30, 30);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    // Add ball if b is pressed
    if(key == 'b') {
        float r = ofRandom(5, 15); // a random radius 5px - 15px
        circle.push_back(shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle));
        circle.back().get()->setPhysics(3.0, 0.53, 0.1);
        circle.back().get()->setup(box2d.getWorld(), mouseX, mouseY, r);
    }

    // Add square if s is pressed
    if(key == 's') {
        float w = ofRandom(5, 25); // a random width between 5px - 25px
        float h = ofRandom(5, 25);  // a height width between 5px - 25px
        box.push_back(shared_ptr<ofxBox2dRect>(new ofxBox2dRect));
        box.back().get()->setPhysics(3.0, 0.53, 0.1);
        box.back().get()->setup(box2d.getWorld(), mouseX, mouseY, w, h);
    }

    // Add custom cat particle if c is pressed
    if(key == 'c') {
        customCatParticle.push_back(shared_ptr<CustomParticle>(new CustomParticle));
        CustomParticle * p = customCatParticle.back().get();
        float r = 20;
        p->setPhysics(0.4, 0.53, 0.31);
        p->setup(box2d.getWorld(), mouseX, mouseY, r);
        p->color.r = 255;
        p->color.g = 255;
        p->color.b = 255;
    }
    
    // Add line if l is pressed
    if(key == 'l') {
        drawLine.addVertex(mouseX, mouseY);
    }

    // toggle mouse force
    if(key == 'f') boolMouseF = !boolMouseF;

    // Toggle for setting background
    switch (key) {
        case ' ':
            boolThresholdOpenCV = !boolThresholdOpenCV;
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
    // Add line drawing
    drawLine.addVertex(x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    // Resets whether the objects interact with the contour
    if(edgeLine.isBody()) {
        drawLine.clear();
        edgeLine.destroy();
    }
    drawLine.addVertex(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
    // Close the polyline so it can be traversed
    drawLine.setClosed(false);
    drawLine.simplify();
    
    // Reset world
    edgeLine.addVertexes(drawLine);
    edgeLine.setPhysics(0.0, 0.5, 0.5);
    edgeLine.create(box2d.getWorld());
    
    // Reset drawing
    drawLine.clear();
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
