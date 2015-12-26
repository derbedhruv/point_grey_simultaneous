/*****************************************************************
  POINT GREY CAMERA DRIVER FOR ANTERIOR SEGMENT IMAGING PROJECT

  AUTHORS: Dhruv Joshi, Darpan Sanghavi

  
*****************************************************************/

#include "FlyCapture2.h"
#include <vector>
#include <opencv2/opencv.hpp>


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

	// now we'll try to capture an image from each
	Image rawImage, convertedImage;	// prepare the image object and keep

	// testing the display of a Mat object which is under our control
	cv::Mat image(600, 1024, CV_8UC1);
	cvNamedWindow("Image1", CV_WINDOW_NORMAL);
	cvSetWindowProperty("Image1", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	cv::imshow("Image1", image);
	cv::waitKey(0);

	// no of images to capture is given as a numerical argument to the program
	// check if this has actuallybeen given, or else use default..
	unsigned int numImages = 50;
	if (argc < 2) {
	  printf("No parameter entered for number of images, going with default 50.\n");
	} else {
  	  // numImages = (int)*argv[1];
	  numImages = 1;
	}
	std::vector<Image> vecImages1;
        vecImages1.resize(numImages);
	std::vector<Image> vecImages2;
        vecImages2.resize(numImages);
	for (unsigned int j=0; j < numImages; j++ ) {
	    for (unsigned int cam=0; cam < numCameras; cam++) {
		error = pcam[cam]->RetrieveBuffer( &rawImage );
		if (error != PGRERROR_OK)
		{
		  PrintError( error );
		  continue;
		}

	/*
		// We'll also print out a timestamp
	        TimeStamp timestamp = rawImage.GetTimeStamp();
                printf(
                  "Cam %d - Frame %d - TimeStamp [%d %d]\n",
                  cam,
                  j,
                  timestamp.cycleSeconds,
                  timestamp.cycleCount
		);
	*/

		if(cam==0) {
			vecImages1[j].DeepCopy(&rawImage);
		} else {
			vecImages2[j].DeepCopy(&rawImage);
		}
	    }
	}
	//Process and store the images captured
	if (numCameras > 0) {
  	printf("Saving images.. please wait\n");
  	for (unsigned int j=0; j < numImages; j++) {
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
