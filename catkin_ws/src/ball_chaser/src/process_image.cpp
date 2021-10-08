#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z= ang_z;

    client.call(srv);
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{
    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera

    int white_pixel = 255;
    bool white_ball_seen = false;
    int column_number = 0;

    for (int i =0; i < img.height * img.step;i++){
        if (img.data[i] == white_pixel){
            white_ball_seen = true;
            column_number = i % img.width;
        }
    }

    if(white_ball_seen){
        if (column_number <= int(img.width/3)){
            drive_robot(0,0.1);
            ROS_INFO("Left");
        }
        else if (int(img.width/3) < column_number <= 2*int(img.width/3)){
            drive_robot(0.5,0);
            ROS_INFO("Mid");
        }
        else{
            drive_robot(0,-0.1);
            ROS_INFO("Right");
        }
    }
    else{
        drive_robot(0,0);
        ROS_INFO("Ball not seen");
    }

    
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
