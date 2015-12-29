/*****************************************************************
  POINT GREY CAMERA DRIVER FOR ANTERIOR SEGMENT IMAGING PROJECT

  AUTHORS: Dhruv Joshi, Darpan Sanghavi

  compile using 'make'.
  This code projects a moving slit (white or green) from a laser projector onto an object of interest. This is captured by two Point Grey Blackfly cameras and saved as TIFF images.
  
*****************************************************************/

#include "FlyCapture2.h"
#include <vector>
#include <opencv2/opencv.hpp>
#include <cstring>
#include <cstdlib>

using namespace FlyCapture2;
using namespace std;

void PrintCameraInfo( CameraInfo* pCamInfo )
{
    printf(
        "\n*** CAMERA INFORMATION ***\n"
        "Serial number - %u\n"
        "Camera model - %s\n"
        "Camera vendor - %s\n"
        "Sensor - %s\n"
        "Resolution - %s\n"
        "Firmware version - %s\n"
        "Firmware build time - %s\n\n",
        pCamInfo->serialNumber,
        pCamInfo->modelName,
        pCamInfo->vendorName,
        pCamInfo->sensorInfo,
        pCamInfo->sensorResolution,
        pCamInfo->firmwareVersion,
        pCamInfo->firmwareBuildTime );
}

void PrintError( Error error )
{
    error.PrintErrorTrace();
}

int main(int argc, char* argv[]) {

    // instructions on how to use this software
    cout << "Welcome to the ASI software.\n There are two modes - calibration and data mode. The calibration mode enables you to take pictures from each camera one at a time while changing the orientation of the checkerboard pattern with each 'run'. The Scanning mode is where a moving slit is projected onto the object and  images taken by both cameras are synchronized with it." << endl;
    cout << "The general syntax of the command is \n\n" << endl;
    cout << "./out -mode -count -int -color\n\n" << endl;

    Error error;
    CameraInfo camInfo;

    BusManager busMgr;
    unsigned int numCameras;

    error = busMgr.GetNumOfCameras(&numCameras);
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }
    printf("cameras: %u\n", numCameras);

    // create a new array of cameras     
    Camera* pcam[2] ;

    // now we do the formalities needed to establish a connection
    for (unsigned int i=0; i<numCameras; i++) {
      // connect to a camera 
      PGRGuid guid;
      pcam[i] = new Camera();

      error = busMgr.GetCameraFromIndex( i, &guid );
      if (error != PGRERROR_OK)
        {
            PrintError( error );
            return -1;
        }

      error = pcam[i]->Connect(&guid);
      if (error != PGRERROR_OK)
        {
            PrintError( error );
            return -1;
        }

      // Get the camera information
      error = pcam[i]->GetCameraInfo( &camInfo );
      if (error != PGRERROR_OK)
        {
            PrintError( error );
            return -1;
        }
      // uncomment the following line if you really care about the camera info
      // PrintCameraInfo(&camInfo);

      // Next we turn isochronous images capture ON for both cameras
      error = pcam[i]->StartCapture();
      if (error != PGRERROR_OK)
    	  {
       	 	PrintError( error );
       	 	return -1;
    	  }

     }

	Image rawImage, convertedImage;	// prepare the image object and keep


	// checking command line parameters
	bool mode_specified = false, count_specified = false, int_specified = false, color_specified = false;
	
	// setting defaults. mode is slit, no of images is 50, intensity is midway and color is white.
	int mode = 0, numImages = 50, intensity = 255, color = 0;

	// parse command line arguments
	for (int cmd = 1; cmd < argc - 1; cmd += 2) {
	  if (!strcmp(argv[cmd],"-mode")) {
	    if (!strcmp(argv[cmd + 1], "slit")) {
	        mode_specified = true;
		cout << "mode is slitscan" << endl;
	        mode = 0;
	    } else if (!strcmp(argv[cmd + 1], "calib")) {
 	        mode_specified = true;
		cout << "mode is calibration" << endl;
		mode = 1;
	    } 
	  } else if (!strcmp(argv[cmd],"-count")) {
	    count_specified = true;
	    cout << "no of images is " << atoi(argv[cmd + 1]) << endl;
	    numImages = atoi(argv[cmd + 1]);
	  } else if (!strcmp(argv[cmd],"-int")) {
	    int_specified = true;
            cout << "brightness of illumination is " << atoi(argv[cmd + 1]) << endl;
	    intensity = atoi(argv[cmd + 1]);
          } else if (!strcmp(argv[cmd],"-color")) {
	    if (mode == 0){
 	      if (!strcmp(argv[cmd + 1], "white")) {
	        color_specified = true;
                cout << "color is white" << endl;
		color = 0;
              } else if (!strcmp(argv[cmd + 1], "green")) {
	        color_specified = true;
                cout << "color is green" << endl;
		color = 1;
              }
	    } else if (mode == 1) {
		cout << "Mode is calibration, will use only white." << endl;
	    }
          }
	}

	std::vector<Image> vecImages1;
        vecImages1.resize(numImages);
	std::vector<Image> vecImages2;
        vecImages2.resize(numImages);
	int slitRow = 1200, slitCol = 1600, slitStart = 0.3*slitRow, slitMove = 0.6*slitRow/50;
	cv::Mat projectedSlit(slitRow, slitCol, CV_8UC1);
	cv::cvtColor(projectedSlit, projectedSlit, CV_GRAY2RGB);
	    
	// we'll create a namedWindow which can be closed by us
	cvNamedWindow("Image1", CV_WINDOW_NORMAL);
	cvSetWindowProperty("Image1", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	cv::imshow("Image1", projectedSlit);
	cv::waitKey(1000);

	cv::Vec3b black, green, white, slit_color;
	black.val[0] = 0; black.val[1] = 0; black.val[2] = 0;
	green.val[0] = 0; green.val[1] = intensity; green.val[2] = 0;
	white.val[0] = intensity; white.val[1] = intensity; white.val[2] = intensity;

	// set the color of the slit based on cmdline parameters
	if (color == 0) {
	  slit_color = white;
	} else if (color == 1) {
	  slit_color = green;
	}


	// if the mode is calibration, we just show a white screen
	if (mode == 1) {
	  projectedSlit = cv::Scalar(intensity, intensity, intensity);
	  cv::imshow("Image1", projectedSlit);
	  cv::waitKey(100);
	}

	for (int j=0; j < numImages; j++ ) {
	    // first display the window with the slit
	    // We will update the Mat object and update the slit position

	  // if the mode is slitscan, prepare the slit
	  if (mode == 0) {
	    for (int a = 400; a < 800; a++) {
		if (j > 0) {
		     projectedSlit.at<cv::Vec3b>(cv::Point(a, slitStart + (j - 1)*slitMove)) = black;
		}
		projectedSlit.at<cv::Vec3b>(cv::Point(a, slitStart + j*slitMove)) = slit_color;
	    }

	    cv::imshow("Image1", projectedSlit);
	    cv::waitKey(1);
	  }


	    // then we capture the image from both cameras
	    for (unsigned int cam=0; cam < numCameras; cam++) {
		error = pcam[cam]->RetrieveBuffer( &rawImage );
		if (error != PGRERROR_OK)
		{
		  PrintError( error );
		  continue;
		}

		if(cam==0) {
			vecImages1[j].DeepCopy(&rawImage);
		} else {
			vecImages2[j].DeepCopy(&rawImage);
		}
	    }
	}

	// then destroy the window
	cvDestroyWindow("Image1"); 


	//Process and store the images captured
	if (numCameras > 0) {
  	printf("Saving images.. please wait\n");
  	for (int j=0; j < numImages; j++) {
  		error = vecImages1[j].Convert( PIXEL_FORMAT_RGB, &convertedImage );
                  if (error != PGRERROR_OK)
                  {
                    PrintError( error );
                    return -1;
                  }

                  // Create a unique filename
                  char filename[512];
                  sprintf( filename, "./images/cam--%d-%d.tiff", 0, j);

                  // Save the image. If a file format is not passed in, then the file
                  // extension is parsed to attempt to determine the file format.
                  error = convertedImage.Save( filename );
                  if (error != PGRERROR_OK)
                  {
                    PrintError( error );
                    return -1;
                  }
  		            //Do the same for the second camera
  		            error = vecImages2[j].Convert( PIXEL_FORMAT_RGB, &convertedImage );
                  if (error != PGRERROR_OK)
                  {
                    PrintError( error );
                    return -1;
                  }

                  // Create a unique filename
                  char filename2[512];
                  sprintf( filename2, "./images/cam--%d-%d.tiff", 1, j);

                  // Save the image. If a file format is not passed in, then the file
                  // extension is parsed to attempt to determine the file format.
                  error = convertedImage.Save( filename2 );
                  if (error != PGRERROR_OK)
                  {
                    PrintError( error );
                    return -1;
                  }
  	}
    	for ( unsigned int i = 0; i < numCameras; i++ )
    	{
        	pcam[i]->StopCapture();
        	pcam[i]->Disconnect();
	        delete pcam[i];
      		}
	}

    // delete [] pcam;

    printf( "Done! Press Enter to exit...\n" );
    getchar();

        return 0;
}
