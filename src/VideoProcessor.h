#pragma once

#include "ofMain.h"

class VideoRecorderPostProcessor : public ofThread {
public:
	bool isWorking;
	string scriptMp4;
	string scriptGif;
	string compressGif;
	string makeThumbnail;

	void setup(string scriptMp4, string scriptGif, string compressGif, string makeThumbnail) {
		isWorking = false;
		this->scriptMp4 = scriptMp4;
		this->scriptGif = scriptGif;		
		this->compressGif = compressGif;
		this->makeThumbnail = makeThumbnail;
	}

	void clear() {
		isWorking = false;
	}

	void threadedFunction() {
		isWorking = true;
		cout << scriptMp4 << "\n\n";
		ofSystem(scriptMp4);
		
		cout << "\n\n\n\n" << scriptGif << "\n\n";
		ofSystem(scriptGif);

		cout << "\n\n\n\n" << compressGif << "\n\n";
		ofSystem(compressGif);

		cout << "\n\n\n\n" << makeThumbnail << "\n\n";
		ofSystem(makeThumbnail);

		isWorking = false;
	}
};