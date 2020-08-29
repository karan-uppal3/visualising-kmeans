#include <iostream>
#include <string.h>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core/core.hpp"
#include <bits/stdc++.h>
#include <math.h>
#include <time.h>

#define INF 1e9

using namespace std;
using namespace cv;

struct datapoint{
    int x;
    int y;
    int group;
};

int findCluster( Point centroids[] , int n , int x , int y )
{
    float min_dist = INF ;
    int group = -1;

    for ( int  i = 0 ; i < n ; i++ )
    {
        float temp = hypotf(x-centroids[i].x, y - centroids[i].y);

        if ( temp < min_dist )
        {
            min_dist = temp;
            group = i;
        }
    }

    return group;
}

Point findMean ( vector<Point> arr )
{
    int x = 0 , y = 0;

    for ( int i = 0 ; i < arr.size() ; i++ )
    {
        x = x + arr[i].x;
        y = y + arr[i].y;
    }

    x = x/arr.size();
    y = y/arr.size();

    return Point(x,y);
}

int giveColor (int k , int c)
{
    int arr[10][3] = {
        {255,0,0},
        {0,255,0},
        {0,0,255},
        {255,255,255},
        {255,0,255},
        {0,255,255},
        {255,0,255},
        {127,127,127},
    };

    return arr[k][c];
}

void drawPoints (Mat img,Point centroids[] , int k , vector <datapoint> arr)
{
    Mat temp = img.clone();

    for ( int i = 0 ; i < k ; i++ )
    {
        circle(temp,Point(centroids[i].y,centroids[i].x),3,Scalar(giveColor(i,0),giveColor(i,1),giveColor(i,2)),-1);
    }

    for ( int i = 0 ; i < arr.size() ; i++ )
    {
        temp.at<Vec3b>(arr[i].x,arr[i].y)[0] = giveColor(arr[i].group,0);
        temp.at<Vec3b>(arr[i].x,arr[i].y)[1] = giveColor(arr[i].group,1);
        temp.at<Vec3b>(arr[i].x,arr[i].y)[2] = giveColor(arr[i].group,2);
    }

    imshow("out",temp);
    waitKey(1000);
}

float findError(Point centroids[],vector <datapoint> arr)
{
    float temp = 0;

    for ( int i = 0 ; i < arr.size() ; i++ )
    {
        temp += hypotf(arr[i].x-centroids[arr[i].group].x, arr[i].y - centroids[arr[i].group].y);
    }

    return temp;
}

int main()
{
    int itr = 5;

    Mat img = imread("/home/zoh/Downloads/kmeans1.png",1);
    namedWindow("out",0);

    imshow("out",img);
    waitKey(1000);

    vector <datapoint> arr;

    for ( int i = 0 ; i < img.rows ; i++ )
    {
        for ( int j = 0 ; j < img.cols ; j++ )
        {
            if ( img.at<Vec3b>(i,j)[0] + img.at<Vec3b>(i,j)[1] + img.at<Vec3b>(i,j)[2] == 255*3  )
            {
                datapoint temp;
                temp.x = i;
                temp.y = j;
                temp.group = -1;
                arr.push_back(temp);
            } 
        }
    }

    int groups[arr.size()];

    int k = 4;
    Point final[k];

    float error = INF;

    while(itr--)
    {
        Mat temp = img.clone();
        Point centroids[k];

        srand (time(NULL));

        for ( int i = 0 ; i < k ; i++ )
        {
            centroids[i].x = rand()%img.rows;
            centroids[i].y = rand()%img.cols;
            circle(temp,Point(centroids[i].y,centroids[i].x),3,Scalar(giveColor(i,0),giveColor(i,1),giveColor(i,2)),-1);
            imshow("out",temp);
            waitKey(1000);
        }
        
        bool ifChange = true;
        
        while ( ifChange == true )
        {
            vector<Point> sortByGroup[k];
            int changeCount = 0;

            for ( int i = 0 ; i < arr.size() ; i++ )
            {
                int prev = arr[i].group;
                int curr = findCluster(centroids,k,arr[i].x,arr[i].y);

                if ( prev == curr )
                    changeCount++;

                sortByGroup[curr].push_back(Point(arr[i].x,arr[i].y));

                arr[i].group = curr;
            }

            drawPoints(img,centroids,k,arr);
            
            for ( int i = 0 ; i < k ; i++ )
            {
                if ( sortByGroup[i].size() > 0 )
                    centroids[i] = findMean(sortByGroup[i]);
            }

            drawPoints(img,centroids,k,arr);

            if ( changeCount == arr.size() )
                ifChange = false;
        }

        float t = findError(centroids,arr);

        if ( t < error )
        {
            error = t;

            for ( int i = 0 ; i < k ; i++ )
            {
                final[i] = centroids[i];
            }

            for ( int i = 0 ; i < arr.size() ; i++ )
            {
                groups[i] = arr[i].group;
            }
        }
    }

    for ( int i = 0 ; i < arr.size() ; i++ )
    {
        arr[i].group = groups[i]; 
    }

    drawPoints(img,final,k,arr);

    waitKey(0);

    return 0;
}