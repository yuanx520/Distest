#include <iostream> 
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/legacy/legacy.hpp>
#include "matcher.hpp"
int main(int argc, char *argv[])
{
    if(argc != 3)
	{
		fprintf(stderr, "distest [image1] [image2]\n");
                fprintf(stderr, "for example: \"distest 1.jpg 2.jpg\"\n");
		exit(-1);
	}
	cv::Mat image1 = cv::imread(argv[1], 0); //Load as gray 
    cv::Mat image2 = cv::imread(argv[2], 0);
	if (!image1.data || !image2.data)
		return 0; 

    // Display the images
	cv::namedWindow("Right Image",CV_WINDOW_NORMAL);
	cv::imshow("Right Image",image1);
	cv::namedWindow("Left Image",CV_WINDOW_NORMAL);
	cv::imshow("Left Image",image2);

	// Prepare the matcher
	RobustMatcher rmatcher;
	rmatcher.setConfidenceLevel(0.98);
	rmatcher.setMinDistanceToEpipolar(1.0);
	rmatcher.setRatio(0.65f);
	cv::Ptr<cv::FeatureDetector> pfd= new cv::SurfFeatureDetector(10); 
	rmatcher.setFeatureDetector(pfd);

	// Match the two images
	std::vector<cv::DMatch> matches;
	std::vector<cv::KeyPoint> keypoints1, keypoints2;
	cv::Mat fundemental= rmatcher.match(image1,image2,matches, keypoints1, keypoints2);
    cv::Mat featureimage1,featureimage2;
    cv::drawKeypoints(image1,
            keypoints1,
            featureimage1,
            cv::Scalar(255,0,255),
            cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    cv:: namedWindow("featureimage1", CV_WINDOW_NORMAL);// Create a window for display.
    cv ::      imshow( "featureimage1", featureimage1);                   // Show our image inside it.
    cv ::drawKeypoints(image2,
            keypoints2,
            featureimage2,
        cv::    Scalar(255,0,0),
           cv:: DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    cv::namedWindow("featureimage2", CV_WINDOW_NORMAL);// Create a window for display.
    cv::imshow( "featureimage2", featureimage2);                   // Show our image inside it.
	// draw the matches
	cv::Mat imageMatches;
	cv::drawMatches(image1,keypoints1,  // 1st image and its keypoints
		            image2,keypoints2,  // 2nd image and its keypoints
					matches,			// the matches
					imageMatches,		// the image produced
					cv::Scalar(255,0,0)); // color of the lines
	cv::namedWindow("Matches",CV_WINDOW_NORMAL);
	cv::imshow("Matches",imageMatches);
	
	// Convert keypoints into Point2f and cal the difference
	std::vector<cv::Point2f> points1, points2;
	
    int sum_score = 0;
	for (std::vector<cv::DMatch>::const_iterator it= matches.begin();
			 it!= matches.end(); ++it) {

			 // Get the position of left keypoints
			 float x= keypoints1[it->queryIdx].pt.x;
			 float y= keypoints1[it->queryIdx].pt.y;
			 points1.push_back(cv::Point2f(x,y));
			 cv::circle(image1,cv::Point(x,y),3,cv::Scalar(255,0,0),3);
			 // Get the position of right keypoints
			 x= keypoints2[it->trainIdx].pt.x;
			 y= keypoints2[it->trainIdx].pt.y;
			 cv::circle(image2,cv::Point(x,y),3,cv::Scalar(255,0,0),3);
			 points2.push_back(cv::Point2f(x,y));

             // Get the differnce between image1 and image2
             int score;
             score = keypoints1[it->queryIdx].size -keypoints2[it->trainIdx].size; 
             sum_score += score;
	}
    std ::cout << "The size difference is "<< sum_score << std::endl;
    if (sum_score > 0){
    std::cout << "image1 is closer than image2" << std ::endl;
    }else if (sum_score < 0){
    std::cout << "image2 is closer than image1" << std::endl;
    }else{
    std::cout << "This two pictures the same close" <<std ::endl;    
    }
	// Draw the epipolar lines
	std::vector<cv::Vec3f> lines1; 
	cv::computeCorrespondEpilines(cv::Mat(points1),1,fundemental,lines1);
		
	for (std::vector<cv::Vec3f>::const_iterator it= lines1.begin();
			 it!=lines1.end(); ++it) {

			 cv::line(image2,cv::Point(0,-(*it)[2]/(*it)[1]),
				             cv::Point(image2.cols,-((*it)[2]+(*it)[0]*image2.cols)/(*it)[1]),
							 cv::Scalar(255,0,0));
	}

	std::vector<cv::Vec3f> lines2; 
	cv::computeCorrespondEpilines(cv::Mat(points2),2,fundemental,lines2);
	
	for (std::vector<cv::Vec3f>::const_iterator it= lines2.begin();
		     it!=lines2.end(); ++it) {

			 cv::line(image1,cv::Point(0,-(*it)[2]/(*it)[1]),
				             cv::Point(image1.cols,-((*it)[2]+(*it)[0]*image1.cols)/(*it)[1]),
							 cv::Scalar(255,0,0));
	}

    // Display the images with epipolar lines
	cv::namedWindow("Right Image Epilines (RANSAC)",CV_WINDOW_NORMAL);
	cv::imshow("Right Image Epilines (RANSAC)",image1);
	cv::namedWindow("Left Image Epilines (RANSAC)",CV_WINDOW_NORMAL);
	cv::imshow("Left Image Epilines (RANSAC)",image2);
    std :: cout <<"any key to end" << std:: endl;
	cv::waitKey(0);
	return 0;
}
