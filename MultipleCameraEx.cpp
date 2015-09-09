/* test what resolutions and other modes the camera connected can provide */
#include "FlyCapture2.h"

using namespace FlyCapture2;

void PrintCameraInfo( CameraInfo* pCamInfo )
{
    printf(
        "\n*** totally total CAMERA INFORMATION ***\n"
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
	// PrintCameraInfo(&camInfo);
      }
	// the following line proved that we can retrieve caminfo from each cmaera after this process
	// pcam[1]->GetCameraInfo(&camInfo);

	// now we'll try to capture an image from each
	Image rawImage;
	// Camera cam1, cam2;
	// cam1 = Camera(*pcam[0]);
  	  // Start capturing images from camera 0
    	  error = pcam[0]->StartCapture();
    	  if (error != PGRERROR_OK)
    	  {
       	 	PrintError( error );
       	 	return -1;
    	  }
	// capture images from camera 1
         error = pcam[1]->StartCapture();
          if (error != PGRERROR_OK)
          {
                PrintError( error );
                return -1;
          }


/**/
	unsigned int numImages = 3;
	for (unsigned int j=0; j <numImages; j++ ) {

	  // cmaera 0 ka kaam....
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
