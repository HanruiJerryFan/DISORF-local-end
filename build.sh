sudo apt update && sudo apt upgrade -y

echo "Installing and building OpenCV ..."
sudo apt install build-essential cmake git pkg-config libgtk-3-dev \
libavcodec-dev libavformat-dev libswscale-dev libv4l-dev \
libxvidcore-dev libx264-dev libjpeg-dev libpng-dev libtiff-dev \
gfortran openexr libatlas-base-dev python3-dev python3-numpy \
libtbb2 libtbb-dev libdc1394-22-dev -y
 
cd Thirdparty
git clone https://github.com/opencv/opencv.git
git clone https://github.com/opencv/opencv_contrib.git
cd opencv
git checkout 3.4.9
cd ../opencv_contrib
git checkout 3.4.9
cd ../opencv
mkdir build && cd build
cmake -D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules ..
make -j8
sudo make install

echo "Installing eigen3 ..."
sudo apt install libeigen3-dev -y

echo "Configuring and building Thirdparty/Pangolin ..."
cd ../../Pangolin
./scripts/install_prerequisites.sh recommended
cmake -B build
cmake --build build
cd build
sudo make install

echo "Configuring and building Thirdparty/DBoW2 ..."
cd ../../../Thirdparty/DBoW2
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j

echo "Configuring and building Thirdparty/g2o ..."
cd ../../g2o
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j

cd ../../../

echo "Uncompress vocabulary ..."

cd Vocabulary
tar -xf ORBvoc.txt.tar.gz
cd ..

sudo apt-get install libboost-all-dev -y

echo "Configuring and building ORB_SLAM2 ..."
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j
