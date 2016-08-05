/*************************************************
Copyright:
Date:    2016-06-12
Description: TODO
**************************************************/
#include <iostream>
#include <mutex>
#include <thread>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <fcntl.h>

using namespace std;
using namespace cv;
enum CameraType{
	CAMERA_TYPE_USB,
	CAMERA_TYPE_RTSP
};
#define STRING_FILENAME_LENGTH 40
CascadeClassifier cascadeClassier;
mutex classierMutex;
int indexFile = 0;

void faceDetect(Mat img, CascadeClassifier& cascade,
	double scale, bool tryflip){
	char fileName[STRING_FILENAME_LENGTH];
	int i = 0;
	vector<Rect> faces, faces2;
	faces.clear();
	faces2.clear();

	const static Scalar colors[] = { CV_RGB(0, 0, 255),
		CV_RGB(0, 128, 255),
		CV_RGB(0, 255, 255),
		CV_RGB(0, 255, 0),
		CV_RGB(255, 128, 0),
		CV_RGB(255, 255, 0),
		CV_RGB(255, 0, 0),
		CV_RGB(255, 0, 255) };

	Mat gray, smallImg(cvRound(img.rows / scale), cvRound(img.cols / scale), CV_8UC1);
	cvtColor(img, gray, CV_BGR2GRAY);
	resize(gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR);
	equalizeHist(smallImg, smallImg);

	classierMutex.lock();
	cascade.detectMultiScale(smallImg, faces,
		1.1, 2, 0
		//|CV_HAAR_FIND_BIGGEST_OBJECT
		//|CV_HAAR_DO_ROUGH_SEARCH
		| CV_HAAR_SCALE_IMAGE
		,
		Size(50, 50));

	if (tryflip)
	{
		flip(smallImg, smallImg, 1);
		cascade.detectMultiScale(smallImg, faces2,
			1.1, 2, 0
			//|CV_HAAR_FIND_BIGGEST_OBJECT
			//|CV_HAAR_DO_ROUGH_SEARCH
			| CV_HAAR_SCALE_IMAGE
			,
			Size(50, 50));
		for (vector<Rect>::const_iterator r = faces2.begin(); r != faces2.end(); r++)
		{
			faces.push_back(Rect(smallImg.cols - r->x - r->width, r->y, r->width, r->height));
		}
	}
	classierMutex.unlock();
	if (faces.size() > 0){
		for (vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); r++, i++)
		{
			Mat saveImage(smallImg, Rect((smallImg.cols - r->x - r->width)*scale, r->y*scale, r->width*scale, r->height*scale));
			cout << "faces.size():" << faces.size() << "faces2.size():" << faces2.size() << endl;
			cout << "This image is a face, so save it!" << endl;
			snprintf(fileName, STRING_FILENAME_LENGTH, "./image/test%d.jpg", indexFile);
			indexFile++;
			imwrite(fileName, saveImage);

			Mat smallImgROI();
			vector<Rect> nestedObjects;
			Point center;
			Scalar color = colors[i % 8];
			int radius;
			double aspect_ratio = (double)r->width / r->height;
			if (0.75 < aspect_ratio && aspect_ratio < 1.3)
			{
				center.x = cvRound((r->x + r->width*0.5)*scale);
				center.y = cvRound((r->y + r->height*0.5)*scale);
				radius = cvRound((r->width + r->height)*0.25*scale);
				circle(img, center, radius, color, 3, 8, 0);
			}
			else
				rectangle(img, cvPoint(cvRound(r->x*scale), cvRound(r->y*scale)),
				cvPoint(cvRound((r->x + r->width - 1)*scale), cvRound((r->y + r->height - 1)*scale)),
				color, 3, 8, 0);
		}
	}
	else{
		cout << "This image is not a face, ignore it" << endl;
	}

}

void getFrameFromCamera(CameraType type){
	cout << "Entry:" << type << endl;
	Mat image;
	VideoCapture vcap;
	if (CAMERA_TYPE_RTSP == type){
		const string videoStreamAddress = "rtsp://192.168.1.10:554/user=admin&password=&channel=2&stream=1.sdp?real_stream";
		vcap.open(videoStreamAddress);
	}
	else {
		vcap.open(0);
	}
	//open the usb camera and make sure it's opened
	if (!vcap.isOpened()) {
		cout << "Error opening video stream or file" << endl;
		return;
	}

	bool loadResult = cascadeClassier.load("./resource/haarcascade_frontalface_alt.xml");
	cout << "loadResult:" << loadResult << endl;
	while (true){
		if (!vcap.read(image)) {
			cout << "No frame" << endl;
			continue;
		}
		imshow("Output Window", image); // show image for user
		waitKey(1);
		Mat faceImage = image.clone();
		thread t = thread(faceDetect, faceImage, std::ref(cascadeClassier), 1.0, true);
		t.detach();
	}
}

int main(int argc, char** argv) 
{
	getFrameFromCamera(CAMERA_TYPE_USB);
	//getFrameFromCamera(CAMERA_TYPE_RTSP);
	return 0;
}

