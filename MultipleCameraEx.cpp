/* test what resolutions and other modes the camera connected can provide */
#include "FlyCapture2.h"

using namespace FlyCapture2;

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

int main(int, char**) {

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
    Camera* pcam[2] = {new Camera(), new Camera()};

    // now we do the formalities needed to establish a connection
    for (unsigned int i=0; i<numCameras; i++) {
      // connect to a camera 
      PGRGuid guid;
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
	Image rawImage;	// prepare the image object and keep
  	
	unsigned int numImages = 3;
	for (unsigned int j=0; j <numImages; j++ ) {

	  error = pcam[0]->RetrieveBuffer( &rawImage );
          if (error != PGRERROR_OK)
          {
              PrintError( error );
              continue;
          }
         // Create a converted image
          Image convertedImage;

          // Convert the raw image
          error = rawImage.Convert( PIXEL_FORMAT_MONO8, &convertedImage );
          if (error != PGRERROR_OK)
          {
            PrintError( error );
            return -1;
          }

          // Create a unique filename
          char filename[512];
          sprintf( filename, "./images/Firstcam-%d.pgm", j);

          // Save the image. If a file format is not passed in, then the file
          // extension is parsed to attempt to determine the file format.
          error = convertedImage.Save( filename );
          if (error != PGRERROR_OK)
          {
            PrintError( error );
            return -1;
          }

	  // camera 2 images storage...
          error = pcam[1]->RetrieveBuffer( &rawImage );
          if (error != PGRERROR_OK)
          {
              PrintError( error );
              continue;
          }

          // Convert the raw image
          error = rawImage.Convert( PIXEL_FORMAT_MONO8, &convertedImage );
          if (error != PGRERROR_OK)
          {
            PrintError( error );
            return -1;
          }

          // Create a unique filename
          sprintf( filename, "./images/Secondcam-%d.pgm", j);

          // Save the image. If a file format is not passed in, then the file
          // extension is parsed to attempt to determine the file format.
          error = convertedImage.Save( filename );
          if (error != PGRERROR_OK)
          {
            PrintError( error );
            return -1;
          }	

	}
/**/
}
