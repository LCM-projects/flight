/*
 * Use Gtk+ to show live video from the dc1394 camera
 *
 * Written by John Stowers <john.stowers@gmail.com>
 * Modified for STEREO VISION PROCESSING BY ANDREW BARRY <abarry@csail.mit.edu>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "opencv-calibrate.hpp"

bool left_camera_mode = false;
bool right_camera_mode = false;
bool stereo_mode = true;
int force_brightness = -1, force_exposure = -1;

int camera_custom_setup(dc1394camera_t *camera);

int main(int argc, char *argv[])
{
    // parse configuration file
    // get input arguments

    OpenCvStereoConfig stereo_config;
    string config_file = "";

    ConciseArgs parser(argc, argv);
    parser.add(config_file, "c", "config", "Configuration file containing camera GUIDs, etc.", true);
    parser.add(left_camera_mode, "l", "left-camera", "Calibrate just the left camera.");
    parser.add(right_camera_mode, "r", "right-camera", "Calibrate just the right camera.");
    parser.add(force_brightness, "b", "brightness", "set brightness to this level");
    parser.add(force_exposure, "e", "exposure", "set exposure to this level");
    parser.parse();

    // parse the config file
    if (ParseConfigFile(config_file, &stereo_config) != true)
    {
        fprintf(stderr, "Failed to parse configuration file, quitting.\n");
        return -1;
    }
printf("got config\n");
    if (left_camera_mode || right_camera_mode)
    {
        stereo_mode = false;
    }

    uint64 guid = stereo_config.guidLeft;
    uint64 guid2 = stereo_config.guidRight;


    FlyCapture2::Error error;
    
    FlyCapture2::BusManager busMgr;

    //dc1394_t        *d;
    //dc1394camera_t  *camera;
    //dc1394error_t   err;

    Mat frame_array_left[MAX_FRAMES];
    Mat frame_array_right[MAX_FRAMES];

    int numFrames = 0;

    unsigned int numCameras;
    error = busMgr.GetNumOfCameras(&numCameras);
    if (error != FlyCapture2::PGRERROR_OK)
    {
        error.PrintErrorTrace();
        return -1;
    }

    if ( numCameras != 2 )
    {
       std::cout << "Did not find 2 cameras.  Found " << numCameras << " cameras." << std::endl; 
       return -1;
    }
    else
    {
       std::cout << "Found 2 cameras!" << std::endl; 
    }

    FlyCapture2::PGRGuid guidRight, guidLeft;
    error = busMgr.GetCameraFromIndex(0, &guidRight);
    error = busMgr.GetCameraFromIndex(1, &guidLeft);
    if (error != FlyCapture2::PGRERROR_OK)
    {
       error.PrintErrorTrace();
       return -1;
    }

    FlyCapture2::Camera camRight, camLeft;

    // Connect to camera Right
    error = camRight.Connect(&guidRight);
    if (error != FlyCapture2::PGRERROR_OK)
    {
        error.PrintErrorTrace();
        return -1;
    }

    // Connect to camera Left
    error = camLeft.Connect(&guidLeft);
    if (error != FlyCapture2::PGRERROR_OK)
    {
        error.PrintErrorTrace();
        return -1;
    }

    std::cout << "Successfully connected to both cameras" << std::endl;

    // Start capturing images
    std::cout << "Starting capture... " << std::endl; 
    error = camRight.StartCapture();
    if (error != FlyCapture2::PGRERROR_OK)
    {
        error.PrintErrorTrace();
        return -1;
    }

    std::cout << "Successfully started capturing from first camera" << std::endl;

    error = camLeft.StartCapture();
    if (error != FlyCapture2::PGRERROR_OK)
    {
        error.PrintErrorTrace();
        return -1;
    }

    std::cout << "Successfully started capturing from second camera" << std::endl;


    // ----- cam 2 -----
    //dc1394_t        *d2;
    //dc1394camera_t  *camera2;
    //dc1394error_t   err2;

    // d = dc1394_new ();
    // if (!d)
    //     g_critical("Could not create dc1394 context");

    // d2 = dc1394_new ();
    // if (!d2)
    //     g_critical("Could not create dc1394 context for camera 2");

    // camera = dc1394_camera_new (d, guid);
    // if (!camera)
    //     g_critical("Could not create dc1394 camera");

    // camera2 = dc1394_camera_new (d2, guid2);
    // if (!camera2)
    //     g_critical("Could not create dc1394 camera for camera 2");

    // // setup
    // err = setup_gray_capture(camera, DC1394_VIDEO_MODE_FORMAT7_1);
    // DC1394_ERR_CLN_RTN(err, cleanup_and_exit(camera), "Could not setup camera");

    // err2 = setup_gray_capture(camera2, DC1394_VIDEO_MODE_FORMAT7_1);
    // DC1394_ERR_CLN_RTN(err2, cleanup_and_exit(camera2), "Could not setup camera number 2");

    // // enable auto-exposure
    // // turn on the auto exposure feature
    // err = dc1394_feature_set_power(camera, DC1394_FEATURE_EXPOSURE, DC1394_ON);
    // DC1394_ERR_RTN(err,"Could not turn on the exposure feature");

    // err = dc1394_feature_set_mode(camera, DC1394_FEATURE_EXPOSURE, DC1394_FEATURE_MODE_ONE_PUSH_AUTO);
    // DC1394_ERR_RTN(err,"Could not turn on Auto-exposure");

    // // enable auto-exposure
    // // turn on the auto exposure feature
    // err = dc1394_feature_set_power(camera2, DC1394_FEATURE_EXPOSURE, DC1394_ON);
    // DC1394_ERR_RTN(err,"Could not turn on the exposure feature for cam2");

    // err = dc1394_feature_set_mode(camera2, DC1394_FEATURE_EXPOSURE, DC1394_FEATURE_MODE_ONE_PUSH_AUTO);
    // DC1394_ERR_RTN(err,"Could not turn on Auto-exposure for cam2");

    // // enable camera
    // err = dc1394_video_set_transmission(camera, DC1394_ON);
    // DC1394_ERR_CLN_RTN(err, cleanup_and_exit(camera), "Could not start camera iso transmission");
    // err2 = dc1394_video_set_transmission(camera2, DC1394_ON);
    // DC1394_ERR_CLN_RTN(err2, cleanup_and_exit(camera2), "Could not start camera iso transmission for camera number 2");






    if (left_camera_mode || stereo_mode)
    {
        //InitBrightnessSettings(camera, camera2);
        //MatchBrightnessSettings(camera, camera2, true, force_brightness, force_exposure);
    } else {
        // use the right camera as the master for brightness
        // since we're calibrating that one
        //InitBrightnessSettings(camera2, camera);
        //MatchBrightnessSettings(camera2, camera, true);
    }

    // make opencv windows
    if (left_camera_mode || stereo_mode)
    {
        namedWindow("Input Left", CV_WINDOW_AUTOSIZE);
        moveWindow("Input Left", 100, 100);
    }

    if (right_camera_mode || stereo_mode)
    {
        namedWindow("Input Right", CV_WINDOW_AUTOSIZE);
        moveWindow("Input Right", 478, 100);
    }


    CvSize size;

    Mat cornersL, cornersR;

    int i;

    while (numFrames < MAX_FRAMES) {

        Mat chessL, chessR;

        // each loop dump a bunch of frames to clear the buffer
        //MatchBrightnessSettings(camera, camera2, true, force_brightness, force_exposure);
        for (i=0;i<1;i++)
        {
            if (left_camera_mode || stereo_mode)
            {
                chessL = GetFrameChameleon3(&camLeft);
            }

            if (right_camera_mode || stereo_mode)
            {
                chessR = GetFrameChameleon3(&camRight);
            }
        }

        // copy the images for drawing/display
        size = chessL.size();
        Mat chessLc;
        chessLc.create(size, CV_32FC3);
        Mat chessRc;
        chessRc.create(size, CV_32FC3);

        // attempt checkerboard matching
        bool foundPattern = true; // set to true so we can do an OR
                                  // later if we're only using one
                                  // camera

        if (left_camera_mode || stereo_mode)
        {
            foundPattern = findChessboardCorners(chessL, Size(CHESS_X, CHESS_Y), cornersL);
        }

        if (right_camera_mode || stereo_mode)
        {
            foundPattern = foundPattern & findChessboardCorners(chessR, Size(CHESS_X, CHESS_Y), cornersR);
        }

        if (left_camera_mode || stereo_mode)
        {
            cvtColor( chessL, chessLc, CV_GRAY2BGR );
            drawChessboardCorners(chessLc, Size(CHESS_X, CHESS_Y), cornersL, foundPattern);
            imshow("Input Left", chessLc);
        }

        if (right_camera_mode || stereo_mode)
        {
            cvtColor(chessR, chessRc, CV_GRAY2BGR);
            drawChessboardCorners(chessRc, Size(CHESS_X, CHESS_Y), cornersR, foundPattern);

            imshow("Input Right", chessRc);
        }


        // key codes:
        // page up: ?
        // page down: 86
        // b: 98
        char key = waitKey();
        //printf("%d\n", (int)key);
        if (key == 98)
        {
            break;
        } else if (key == 86){
            if (foundPattern)
            {
                // this was a good one -- save it
                frame_array_left[numFrames] = chessL.clone();
                frame_array_right[numFrames] = chessR.clone();

                // give the user some guidence on the number
                // of frames they should be using
                if (stereo_mode)
                {
                    printf("Saved frame %d / about 10\n", numFrames);
                } else {
                    printf("Saved frame %d / about 20-30\n", numFrames);
                }

                numFrames ++;
            } else {
                printf("Not saving frame since did not find a checkboard.\n");
            }
        } else if (key == 'W') {
            force_brightness +=20;
            std::cout << "Brightness: " << force_brightness << "\n";
        } else if (key == 'w') {
            force_brightness -=20;
            std::cout << "Brightness: " << force_brightness << "\n";
        } else if (key == 'E') {
            force_exposure +=20;
            std::cout << "Exposure: " << force_exposure << "\n";
        } else if (key == 'e') {
            force_exposure -=20;
            std::cout << "Exposure: " << force_exposure << "\n";
        }
    }

    printf("\n\n");

    // clear out the calibration directory
    printf("Deleting old images...\nrm calibrationImages/*.ppm\n");
    int retval = system("rm calibrationImages/*.ppm");
    if (retval != 0) {
        printf("Warning: Deleting images may have failed.\n");
    }
    printf("done.\n");

    char filename[1000];

    for (i=0;i<numFrames;i++)
    {
        if (left_camera_mode || stereo_mode)
        {
            sprintf(filename, "calibrationImages/cam1-%05d.ppm", i+1);
            imwrite(filename, frame_array_left[i]);
        }

        if (right_camera_mode || stereo_mode)
        {
            sprintf(filename, "calibrationImages/cam2-%05d.ppm", i+1);
            imwrite(filename, frame_array_right[i]);
        }

        printf("Writing frame %d\n", i);
    }

    printf("\n\n");

    destroyWindow("Input Left");
    destroyWindow("Input Right");


    // Commented out stopping data transmission for now

    // stop data transmission
    // err = dc1394_video_set_transmission(camera, DC1394_OFF);
    // DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not stop the camera");

    // err2 = dc1394_video_set_transmission(camera2, DC1394_OFF);
    // DC1394_ERR_CLN_RTN(err2,cleanup_and_exit(camera2),"Could not stop the camera 2");

    // // close camera
    // cleanup_and_exit(camera);
    // cleanup_and_exit(camera2);
    // dc1394_free (d);
    // dc1394_free (d2);

    return 0;
}


