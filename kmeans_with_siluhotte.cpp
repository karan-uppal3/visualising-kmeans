#include <iostream>
#include <string.h>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core/core.hpp"
#include <bits/stdc++.h>
#include <math.h>
#include <time.h>

#define INF 1e9
#define lol(x) cout<<x<<endl;

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
        {127,0,0},
        {0,127,0},
        {0,0,127}
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

float distance (datapoint x,datapoint y)
{
    if ( x.group == y.group )
    {
        return hypotf(x.x-y.x,x.y-y.y);
    }
    return 0;
}

float calculateSil(Point centroids[], int k , vector <datapoint> arr)
{
    float fin = 0;
    float a[arr.size()];
    float b[arr.size()];
    float s[arr.size()];

    for ( int i = 0; i < arr.size() ;i++ )
    {
        float t = 0;
        int ctr = 0;

        for ( int j = 0 ; j < arr.size() ; j++ )
        {
            float p = distance(arr[i],arr[j]);

            if ( p > 0 )
            {
                t += p;
                ctr++;
            }
        }

        a[i] = float(t)/ctr;

        float bss[k] = {0};
        int ctr_b[k] = {0};

        for ( int j = 0 ; j < k ; j++ )
        {
            if ( j == arr[i].group )
            {
                bss[j] = INF;
                ctr_b[j] = 1;
                continue;
            }

            for( int l = 0 ; l < arr.size() ; l++ )
            {
                if ( arr[l].group == j )
                {
                    bss[j] += hypotf(arr[i].x-arr[l].x,arr[i].y-arr[l].y);
                    ctr_b[j]++;
                }
            }

            bss[j] = float(bss[j]) / ctr_b[j];
            
        }

        b[i] = *min_element(bss,bss+k);

        if ( a[i] < b[i] )
            s[i] = 1.0 - float(a[i]/b[i]);
        else if ( a[i] > b[i] )
            s[i] = float(b[i]/a[i]) - 1.0;
        else    
            s[i] = 0;

        fin += s[i];
    }

    return fin;
}

int main()
{
    Mat img = imread("kmeans2.png",1);
    namedWindow("out",0);

    imshow("out",img);
    waitKey(1000);

    vector <datapoint> arr;
    vector <datapoint> final_arr;

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
                final_arr.push_back(temp);
            } 
        }
    }

    Point final_centroids[10];
    float max_sil = -1;
    int final_k = -1;

    for ( int k = 2 ; k <= 10 ; k++ )
    {
        Point final[k];
        int groups[arr.size()];
        float error = INF;

        int itr = 5;

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

        float sil = calculateSil(final,k,arr);
    
        if ( sil > max_sil )
        {
            max_sil = sil;
            final_k = k;

            for ( int i = 0 ; i < k ; i++ )
            {
                final_centroids[i] = final[i];
            }

            for ( int i = 0 ; i < arr.size() ; i++ )
            {
                final_arr[i] = arr[i];
            }
        }
    }

    cout<<"The number of clusters is "<<final_k;

    drawPoints(img,final_centroids,final_k,final_arr);

    waitKey(0);

    return 0;
}