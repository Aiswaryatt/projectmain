#ifndef UPDATEPATCHES_H
#define UPDATEPATCHES_H
#include <opencv2/opencv.hpp>
#include <iostream>

struct TemplatePatches
{
   cv::Mat Image;
   cv::Mat InitialPatch;
   std:: vector<cv::Rect> RoiOfPatches;
   std:: vector<cv::Rect> candidateRois;
   std:: vector<float> DistanceBwRois;
   std:: vector<cv::Mat> Patches;
   std:: vector<cv::Mat> candidatePatches;
   int PatchNumber;
};

class UpdatePatches
{
public:
    UpdatePatches();
    bool TemplateMatching(TemplatePatches &Input);
    bool RemoveOverlapedRois(std:: vector<cv::Rect> &RoiArray,float ScaleFactor);

private:
    bool IsPatchMatched(cv::Mat CandidatePatch,cv::Mat InitialPatch);
    int BlueAvg=0,GreenAvg=0,RedAvg=0;
    cv::Mat FillPatches(cv::Mat Image);
};


#endif // UPDATEPATCHES_H
