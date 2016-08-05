##MakeFile


LOCAL_PATH:= $(shell pwd)

CXX := g++ -std=gnu++11
#LOCAL_STATIC_MACRO := -D CPU_ONLY=1 -D DLIB_JPEG_SUPPORT

LOCAL_C_INCLUDES := \
                    -I $(LOCAL_PATH)/include/ \



LOCAL_C_LIBRARIES := \
                    -L $(LOCAL_PATH)/lib/ \


LOCAL_SHARE_LIB := \
			-lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_flann -lopencv_objdetect -lopencv_video\
			-lpthread

LOCAL_SOURCE_FILE := \
		    src/faceDetect.cpp \


faceDetect : faceDetect.o
	$(CXX) -o faceDetect faceDetect.o  $(LOCAL_C_LIBRARIES) $(LOCAL_SHARE_LIB) -Wl,--rpath=$(LOCAL_PATH)/lib/  

faceDetect.o : $(LOCAL_SOURCE_FILE)
	$(CXX) -c $(LOCAL_SOURCE_FILE) $(LOCAL_C_INCLUDES) $(LOCAL_STATIC_MACRO)

.PHONY : clean
clean :
	-rm *.o faceDetect
