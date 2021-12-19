#pragma once

#include "ofMain.h"
#include "VideoProcessor.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:
		ofVideoPlayer* playerLeft;		
		ofVideoPlayer* playerRight;
		VideoRecorderPostProcessor videoProcessor;

		ofxPanel guiPanel;
		ofParameter<string> filePath;		

		string videoPath;
		float leftMarker, rightMarker;
		bool shouldLoadRight;

		void setRightVideo(string path);
		void setVideo(string path);
		void setVideo2(string path);

		void setup();
		void update();
		void draw();

		bool isExporting = false;
		void exportSelection();
		void startExporting(string scriptPath, string script2, string script3, string script4);
		void stopExporting();

		string formatTime(float s, float fps);

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
		
};
