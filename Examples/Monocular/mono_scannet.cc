/**
* This file is modified from ORB-SLAM2.
*
* Copyright (C) 2014-2016 Raúl Mur-Artal <raulmur at unizar dot es> (University of Zaragoza)
* For more information see <https://github.com/raulmur/ORB_SLAM2>
*
* ORB-SLAM2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ORB-SLAM2 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ORB-SLAM2. If not, see <http://www.gnu.org/licenses/>.
*/

#include <unistd.h>

#include<iostream>
#include<algorithm>
#include<fstream>
#include<chrono>
#include <sys/stat.h>
#include <dirent.h>

#include<opencv2/core/core.hpp>

#include<System.h>

using namespace std;

void LoadImages(const string &path, vector<string> &vstrImageFilenames, vector<double> &vTimestamps);

bool customCompare(const std::string &a, const std::string &b) {
    int numA = std::stoi(a.substr(0, a.find(".jpg")));
    int numB = std::stoi(b.substr(0, b.find(".jpg")));
    
    return numA < numB;
}

int main(int argc, char **argv)
{
    bool bUseViewer;
    if (argc != 5)
    {
        cerr << endl << "Usage: ./mono_scannet path_to_vocabulary path_to_settings path_to_sequence bUseViewer" << endl;
        return 1;
    }
    stringstream ss1(argv[4]);
    ss1 >> boolalpha >> bUseViewer;

    // Retrieve paths to images
    vector<string> vstrImageFilenames;
    vector<double> vTimestamps;
    string path = string(argv[3])+"/color/";
    LoadImages(path, vstrImageFilenames, vTimestamps);

    int nImages = vstrImageFilenames.size();

    // Create SLAM system with UI Disable
    ORB_SLAM2::System SLAM(argv[1],argv[2],ORB_SLAM2::System::MONOCULAR,bUseViewer);
    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    //ORB_SLAM2::System SLAM(argv[1],argv[2],ORB_SLAM2::System::MONOCULAR,true);

    // Vector for tracking time statistics
    vector<float> vTimesTrack;
    vTimesTrack.resize(nImages);

    cout << endl << "-------" << endl;
    cout << "Start processing sequence ..." << endl;
    cout << "Images in the sequence: " << nImages << endl << endl;

    // Main loop
    cv::Mat im;
    for(int ni=0; ni<nImages; ni++)
    {
        // Read image from file
        im = cv::imread(path+vstrImageFilenames[ni],-1);
        cv::resize(im, im, cv::Size(640, 480));
        double tframe = ni;

        if(im.empty())
        {
            cerr << endl << "Failed to load image at: "
                 << path << vstrImageFilenames[ni] << endl;
            return 1;
        }

#ifdef COMPILEDWITHC11
        std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
#else
        std::chrono::monotonic_clock::time_point t1 = std::chrono::monotonic_clock::now();
#endif

        // Pass the image to the SLAM system
        SLAM.TrackMonocular(im,tframe);

#ifdef COMPILEDWITHC11
        std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
#else
        std::chrono::monotonic_clock::time_point t2 = std::chrono::monotonic_clock::now();
#endif

        double ttrack= std::chrono::duration_cast<std::chrono::duration<double> >(t2 - t1).count();

        vTimesTrack[ni]=ttrack;

        // Wait to load the next frame
        double T=0;
        if(ni<nImages-1)
            T = (vTimestamps[ni+1]-tframe) / 30;
        else if(ni>0)
            T = (tframe-vTimestamps[ni-1]) / 30;

        if(ttrack<T)
            usleep((T-ttrack)*1e6);
    }

    // Stop all threads
    SLAM.Shutdown();

    // Tracking time statistics
    sort(vTimesTrack.begin(),vTimesTrack.end());
    float totaltime = 0;
    for(int ni=0; ni<nImages; ni++)
    {
        totaltime+=vTimesTrack[ni];
    }
    cout << "-------" << endl << endl;
    cout << "median tracking time: " << vTimesTrack[nImages/2] << endl;
    cout << "mean tracking time: " << totaltime/nImages << endl;

    // Save camera trajectory
    SLAM.SaveKeyFrameTrajectoryTUM("KeyFrameTrajectory.txt");

    return 0;
}

void LoadImages(const string &path, vector<string> &vstrImageFilenames, vector<double> &vTimestamps)
{
    DIR *imgdir;
    struct dirent *imgfile;
    struct stat imgstat;

    imgdir = opendir(path.c_str());
    if (!imgdir) {
        cerr << "Unable to open " << path << endl;
        exit(1);
    }
    int index = 0;
    while((imgfile = readdir(imgdir)) != NULL)
    {
        stat(imgfile->d_name, &imgstat); 
        std::string filename = std::string(imgfile->d_name);
        if (filename != "." && filename != "..")
            vstrImageFilenames.push_back(filename);
            vTimestamps.push_back(index);
            index += 1;
    }
    closedir(imgdir);
    sort(vstrImageFilenames.begin(), vstrImageFilenames.end(), customCompare);
}