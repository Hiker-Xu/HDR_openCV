#include <opencv2/photo.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <vector>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

void readImagesAndTimes(vector<Mat> &images, vector<float> &times)
{
    int numImages = 4;

    static const float timesArray[] = {1/30.0f,0.25,2.5,15.0};
    times.assign(timesArray, timesArray + numImages);
    static const char* filenames[] = {"img_0.033.jpg", "img_0.25.jpg", "img_2.5.jpg", "img_15.jpg"};
    for(int i=0; i < numImages; i++)
    {
    Mat im = imread(filenames[i]);
    images.push_back(im);
    }

}



int main(int argc, char** argv)
{
    //read images and exposure times
    cout << "Reading images and exposure times ..."<< endl;
    vector<Mat> images;
    vector<float> times;
    readImagesAndTimes(images, times);
    cout <<"finished" <<endl;

    //对不同曝光下的几幅图片进行配准，这里直接采用了OpenCV中基于MTB的对齐方式
    cout << "Aligning images ..." << endl;
    Ptr<AlignMTB> alignMTB = createAlignMTB();
    alignMTB->process(images, images);

    //获取相机响应函数
    cout << "Calculating Camera Response Function ..." << endl;
    Mat responseDebevec;
    Ptr<CalibrateDebevec> calibrateDebevec = createCalibrateDebevec();
    calibrateDebevec->process(images, responseDebevec, times);
    imwrite("responseDebevec.jpg",responseDebevec);
    
    //将图像融合为一张
    cout << "Merging images into one HDR image ... " << endl;
    Mat hdrDebevec;
    Ptr<MergeDebevec> mergeDebevec = createMergeDebevec();
    mergeDebevec->process(images, hdrDebevec, times, responseDebevec);
    //保存HDR图像
    imwrite("hdrDebevec.hdr", hdrDebevec);
    cout << "saved hdrDebevec.hdr" << endl;

    // Tonemap using Drago's method to obtain 24-bit color image
    cout << "Tonemaping using Drago's method ... ";
    Mat ldrDrago;
    Ptr<TonemapDrago> tonemapDrago = createTonemapDrago(1.0, 0.7);
    tonemapDrago->process(hdrDebevec, ldrDrago);
    ldrDrago = 3 * ldrDrago;
    imwrite("ldr-Drago.jpg", ldrDrago * 255);
    cout << "saved ldr-Drago.jpg"<< endl;
  
    // Tonemap using Durand's method obtain 24-bit color image
    cout << "Tonemaping using Durand's method ... ";
    Mat ldrDurand;
    Ptr<TonemapDurand> tonemapDurand = createTonemapDurand(1.5,4,1.0,1,1);
    tonemapDurand->process(hdrDebevec, ldrDurand);
    ldrDurand = 3 * ldrDurand;
    imwrite("ldr-Durand.jpg", ldrDurand * 255);
    cout << "saved ldr-Durand.jpg"<< endl;
  
    // Tonemap using Reinhard's method to obtain 24-bit color image
    cout << "Tonemaping using Reinhard's method ... ";
    Mat ldrReinhard;
    Ptr<TonemapReinhard> tonemapReinhard = createTonemapReinhard(1.5, 0,0,0);
    tonemapReinhard->process(hdrDebevec, ldrReinhard);
    imwrite("ldr-Reinhard.jpg", ldrReinhard * 255);
    cout << "saved ldr-Reinhard.jpg"<< endl;
  
    // Tonemap using Mantiuk's method to obtain 24-bit color image
    cout << "Tonemaping using Mantiuk's method ... ";
    Mat ldrMantiuk;
    Ptr<TonemapMantiuk> tonemapMantiuk = createTonemapMantiuk(2.2,0.85, 1.2);
    tonemapMantiuk->process(hdrDebevec, ldrMantiuk);
    ldrMantiuk = 3 * ldrMantiuk;
    imwrite("ldr-Mantiuk.jpg", ldrMantiuk * 255);
    cout << "saved ldr-Mantiuk.jpg"<< endl;
  
  
  
    return EXIT_SUCCESS;

}