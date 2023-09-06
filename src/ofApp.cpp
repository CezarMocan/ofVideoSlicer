#include "ofApp.h"

#define PADDING 20
#define PLAY_BAR_HEIGHT 10

//--------------------------------------------------------------
void ofApp::setup() {

	guiPanel.setup("Settings");
	guiPanel.add(filePath.set("filePath", "seq-test.mp4"));
	guiPanel.loadFromFile("settings.xml");

	playerLeft = new ofVideoPlayer();
	playerRight = new ofVideoPlayer();
	
	ofEnableAlphaBlending();

	shouldLoadRight = false;
	setVideo(filePath.get());
	//setVideo("C:/Users/mocan/Videos/2023-02-20 13-48-59.mp4");
}

void ofApp::setVideo2(string path) {

}

void ofApp::setVideo(string path) {
	leftMarker = rightMarker = -1;

	videoPath = path;

	playerLeft->load(videoPath);
	playerLeft->play();

	playerLeft->setPosition(0);
	playerLeft->setPaused(true);

	playerRight->load(videoPath);
	playerRight->play();
	playerRight->setPosition(0);
	playerRight->setPaused(true);
	//playerRight->setVolume(1);


	cout << "Load 1 done" << "\n";
}

//--------------------------------------------------------------
void ofApp::update(){	
	//cout << "update\n";
	if (playerLeft != NULL)
		playerLeft->update();
	if (playerRight != NULL)
		playerRight->update();

	if (leftMarker >= 0 && rightMarker >= 0 && playerRight != NULL) {
		if (playerRight->getPosition() > rightMarker) {
			playerRight->setPosition(leftMarker);
		}
	}

	if (isExporting && !videoProcessor.isThreadRunning()) {
		stopExporting();
	}

}

//--------------------------------------------------------------
void ofApp::draw() {	
	if (playerLeft == NULL) return;
	//cout << "draw\n";
	ofPushStyle();
	// Draw video player
	float width = (ofGetWidth() - 3 * PADDING) / 2.0;
	float height = playerLeft->getHeight() / playerLeft->getWidth() * width;
	if (playerLeft != NULL)
		playerLeft->draw(PADDING, (ofGetHeight() - height) / 2, width, height);
	if (playerRight != NULL)
		playerRight->getTexture().draw(width + 2 * PADDING, (ofGetHeight() - height) / 2, width, height);

	// Draw play bar
	float playbarWidth = ofGetWidth() - 2 * PADDING;
	ofSetColor(198, 198, 198);
	ofDrawRectangle(PADDING, ofGetHeight() - PADDING - PLAY_BAR_HEIGHT - 20, playbarWidth, PLAY_BAR_HEIGHT);

	float playhead = playerLeft->getPosition();
	ofSetColor(98, 98, 248);
	ofDrawRectangle(PADDING, ofGetHeight() - PADDING - PLAY_BAR_HEIGHT - 20, ofMap(playhead, 0, 1, 0, playbarWidth), PLAY_BAR_HEIGHT);

	float duration = playerLeft->getDuration();
	float currSeconds = duration * playhead;
	float fps = playerLeft->getTotalNumFrames() / duration;
	
	ofSetColor(198, 198, 198);
	string s1 = formatTime(currSeconds, fps);
	string s2 = formatTime(duration, fps);
	ofDrawBitmapString(s1 + " / " + s2, PADDING, ofGetHeight() - PADDING);

	// Draw markers
	ofSetColor(98, 98, 248);
	if (leftMarker >= 0) {
		float leftPos = PADDING + ofMap(leftMarker, 0, 1, 0, playbarWidth);
		ofDrawTriangle(ofVec2f(leftPos, ofGetHeight() - PADDING - PLAY_BAR_HEIGHT - 25), ofVec2f(leftPos - 5, ofGetHeight() - PADDING - PLAY_BAR_HEIGHT - 40), ofVec2f(leftPos + 5, ofGetHeight() - PADDING - PLAY_BAR_HEIGHT - 40));
	}	

	if (rightMarker >= 0) {
		float rightPos = PADDING + ofMap(rightMarker, 0, 1, 0, playbarWidth);
		ofDrawTriangle(ofVec2f(rightPos, ofGetHeight() - PADDING - PLAY_BAR_HEIGHT - 25), ofVec2f(rightPos - 5, ofGetHeight() - PADDING - PLAY_BAR_HEIGHT - 40), ofVec2f(rightPos + 5, ofGetHeight() - PADDING - PLAY_BAR_HEIGHT - 40));
	}

	stringstream selectDuration; 
	selectDuration << "Selection: " << (rightMarker - leftMarker) * playerLeft->getDuration() << " seconds";
	if (leftMarker >= 0 && rightMarker >= 0) {
		ofSetColor(198, 198, 198);
		ofDrawBitmapString(selectDuration.str(), width + 2 * PADDING, (ofGetHeight() + height) / 2 + 15);
	}

	if (isExporting) {
		ofSetColor(128, 0, 0, 100);
		ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
	}

	// Draw legend of keys
	ofSetColor(198, 198, 198);
	ofDrawBitmapString("Controls:", 20, 20);
	ofDrawBitmapString("space -> play / pause (left side)", 20, 50);
	ofDrawBitmapString("right arrow -> next frame", 20, 70);
	ofDrawBitmapString("left arrow -> previous frame", 20, 90);
	ofDrawBitmapString(". -> skip ahead by 10 frames", 20, 110);
	ofDrawBitmapString(", -> go behind by 10 frames", 20, 130);

	ofDrawBitmapString("i -> set start marker of selected sequence", 20, 155);
	ofDrawBitmapString("o -> set end marker of selected sequence", 20, 175);
	ofDrawBitmapString("p -> play / pause selected sequence (right side)", 20, 195);
	ofDrawBitmapString("[ -> export selected sequence", 20, 215);

	ofDrawBitmapString("z -> open file", 20, 240);

	ofPopStyle();
}

string ofApp::formatTime(float s, float fps = 24) {
	float hours = floor(s/ 3600);
	s-= 3600 * hours;
	float minutes = floor(s/ 60);
	s-= 60 * minutes;
	float seconds = s;
	
	stringstream ss;
	ss << (hours < 10 ? "0" : "") << (int)hours << ":" << (minutes < 10 ? "0" : "") << (int)minutes << ":" << (seconds < 10 ? "0" : "") << std::fixed << std::setprecision(3) << seconds;

	return ss.str();
}

void ofApp::exportSelection() {
	// 		postProcessingScriptPath = ofToDataPath("img-to-video.bat", true);
	string startTime = formatTime(leftMarker * playerLeft->getDuration());
	string stopTime = formatTime(rightMarker * playerLeft->getDuration());
	
	string timestamp = ofGetTimestampString();
	string inputFilename = ofFilePath::getFileName(videoPath);
	string output = inputFilename.substr(0, inputFilename.find("_"));

	string startTimeSanitized = startTime;
	std::replace(startTimeSanitized.begin(), startTimeSanitized.end(), ':', '_');

	string stopTimeSanitized = stopTime;
	std::replace(stopTimeSanitized.begin(), stopTimeSanitized.end(), ':', '_');

	string timestampedOutput = output + "-" + startTimeSanitized + "-" + stopTimeSanitized;

	string outputMp4 = ofToDataPath(timestampedOutput + "-video.mp4", true);
	string outputGif = ofToDataPath(timestampedOutput + "-hq-gif.gif", true);
	string outputGifCompressed = ofToDataPath(timestampedOutput + "-compressed-gif.gif", true);
	string outputThumbnail = ofToDataPath(timestampedOutput + "-thumbnail.jpg", true);

	stringstream command;
	command << ofToDataPath("ffmpeg.exe", true) << " -i " << ofToDataPath(videoPath, true) << " -ss " << startTime << " -to " << stopTime << " -b:v 8M " << outputMp4;
	std::cout << command.str() << "\n\n";

	stringstream command2;
	command2 << ofToDataPath("ffmpeg.exe", true) << " -i " << outputMp4 << " -filter_complex \"fps=12,scale=960:-1:flags=lanczos,split [o1] [o2];[o1] palettegen [p]; [o2] fifo [o3];[o3] [p] paletteuse\" " << outputGif;

	stringstream command3;
	command3 << ofToDataPath("gifsicle.exe", true) << " -O3 --lossy=15 " << outputGif << " -o " << outputGifCompressed;

	stringstream command4;
	command4 << ofToDataPath("ffmpeg.exe", true) << " -i " << outputMp4 << " -vf \"select = eq\(n\\, 0\)\" -q:v 3 " << outputThumbnail;


	stringstream textFile;
	textFile << "Processing video: " << ofToDataPath(videoPath, true) << endl;
	textFile << "Start time: " << startTime << endl;
	textFile << "End time: " << stopTime << endl << endl;
	textFile << command.str() << endl;
	textFile << command2.str() << endl;
	textFile << command3.str() << endl;
	textFile << command4.str() << endl;
	
	ofFile textFileD;
	textFileD.open(ofToDataPath(timestampedOutput + "-stats.txt"), ofFile::WriteOnly);
	textFileD << textFile.str();
	textFileD.close();

	startExporting(command.str(), command2.str(), command3.str(), command4.str());
}

void ofApp::startExporting(string scriptMp4, string scriptGif, string scriptCompressGif, string scriptThumbnail) {
	isExporting = true;
	videoProcessor.setup(scriptMp4, scriptGif, scriptCompressGif, scriptThumbnail);
	videoProcessor.startThread();
}

void ofApp::stopExporting() {
	isExporting = false;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	//if (isExporting) return;

	if (key == 'z') {
		ofFileDialogResult result = ofSystemLoadDialog("Select Video", false, ofToDataPath(""));
		if (result.bSuccess) {
			filePath.set(result.getPath());
			guiPanel.saveToFile("settings.xml");

			stringstream restartCommand;
			restartCommand << "start " << ofFilePath::getAbsolutePath(ofFilePath::getCurrentExePath());
			_popen(restartCommand.str().c_str(), "w");
			//ofSystem(restartCommand.str());
			ofExit();
			//setVideo(result.getPath());
		}
	}

	if (key == 32) {
		if (playerLeft->isPlaying()) {
			playerLeft->setPaused(true);
		}
		else {
			playerLeft->setPaused(false);
		}
	}

	if (key == 'i') {
		cout << "In\n";
		leftMarker = playerLeft->getPosition();
		playerRight->setPaused(true);
		playerRight->setPosition(leftMarker);
	}

	if (key == 'o') {
		cout << "Out\n";
		rightMarker = playerLeft->getPosition();
		playerRight->setPaused(true);
		playerRight->setPosition(rightMarker);
	}

	if (key == 'p') {
		playerRight->setPaused(!playerRight->isPaused());
	}

	if (key == OF_KEY_RIGHT) {		
		float newPosition = (playerLeft->getPosition() + 1.0 / playerLeft->getTotalNumFrames());
		playerLeft->setPosition(newPosition);
	}

	if (key == OF_KEY_LEFT) {
		float newPosition = (playerLeft->getPosition() - 1.0 / playerLeft->getTotalNumFrames());
		playerLeft->setPosition(newPosition);
	}

	if (key == '.') {
		float newPosition = (playerLeft->getPosition() + 24.0 / playerLeft->getTotalNumFrames());
		playerLeft->setPosition(newPosition);
	}

	if (key == ',') {
		float newPosition = (playerLeft->getPosition() - 24.0 / playerLeft->getTotalNumFrames());
		playerLeft->setPosition(newPosition);
	}

	if (key == '[') {
		exportSelection();
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
