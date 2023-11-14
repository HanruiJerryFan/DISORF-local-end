# Local End of DISORF
DISORF aims for training NeRF for 3d reconstruction in real-time. It uses distributed archeture so that the whole NeRF-SLAM system is divided into the SLAM part for collecting posed images and the NeRF training and visualization part. In this way, By designing specialized local-end for embedded devices like Jetson, we allow resource-constrained devices to leverage power of NeRF. This repository contains local-end for devices like Ubuntu PC and Jetson board.


# System requirement
We have tested the code in **Ubuntu 20.04**. A powerful CPU is necessary for real-time performance.

# Building local-end
Clone the repository:
```
git clone https://github.com/Edward11235/local_end
```
Set the IP address of the server

We provide a script `build.sh` to download and build all dependencies. Note that some of the dependencies of ORB-SLAM2 are obsolete. So we update code for packages like Pangolin.
```
cd ORB_SLAM2
chmod +x build.sh
./build.sh
```

This will create **libORB_SLAM2.so**  at *lib* folder and the executables **mono_tum**, **mono_kitti**, **rgbd_tum**, **stereo_kitti**, **mono_euroc** and **stereo_euroc** in *Examples* folder.

# Examples
## TUM Dataset
- Download a sequence from http://vision.in.tum.de/data/datasets/rgbd-dataset/download and uncompress it.
- Execute the following command. Change `TUMX.yaml` to TUM1.yaml,TUM2.yaml or TUM3.yaml for freiburg1, freiburg2 and freiburg3 sequences respectively. Change `PATH_TO_SEQUENCE_FOLDER`to the uncompressed sequence folder.
```
./Examples/Monocular/mono_tum Vocabulary/ORBvoc.txt Examples/Monocular/TUMX.yaml PATH_TO_SEQUENCE_FOLDER
```

## KITTI Dataset  
- Download the dataset (grayscale images) from http://www.cvlibs.net/datasets/kitti/eval_odometry.php 
- Execute the following command. Change `KITTIX.yaml`by KITTI00-02.yaml, KITTI03.yaml or KITTI04-12.yaml for sequence 0 to 2, 3, and 4 to 12 respectively. Change `PATH_TO_DATASET_FOLDER` to the uncompressed dataset folder. Change `SEQUENCE_NUMBER` to 00, 01, 02,.., 11. 
```
./Examples/Monocular/mono_kitti Vocabulary/ORBvoc.txt Examples/Monocular/KITTIX.yaml PATH_TO_DATASET_FOLDER/dataset/sequences/SEQUENCE_NUMBER
```

# Acknowledgement
We have utilized and modified portions of the code from [ORB-SLAM 2](https://github.com/raulmur/ORB_SLAM2). We are deeply grateful to the developers and contributors of the remarkable project.