#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <signal.h>

sig_atomic_t volatile g_request_shutdown = 0;

void request_shutdown(int signal)
{
  g_request_shutdown = 1;
}

void vel_publish(ros::Publisher& vel_pub,
                 geometry_msgs::Twist vel_msg,
                 double pub_secs)
{
  ros::WallTime start_time = ros::WallTime::now();
  ros::WallDuration pub_duration = ros::WallDuration(pub_secs);
  ros::WallTime end_time = start_time + pub_duration;

  ROS_INFO_STREAM("## Publishing velocity. ##");
  while (ros::WallTime::now() < end_time)
  {
    vel_pub.publish(vel_msg);
    ros::WallDuration(0.02).sleep();
  }

  ROS_INFO_STREAM("## Stopping. ##");
  geometry_msgs::Twist stop_msg;
  vel_pub.publish(stop_msg);
}

void move_twist(ros::Publisher& vel_pub, double vel_x, double vel_z, double pub_secs)
{
    geometry_msgs::Twist twist_msg;
    twist_msg.linear.x = vel_x;
    twist_msg.angular.z = vel_z;

    vel_publish(vel_pub, twist_msg, pub_secs);
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "figure_eight");


    ros::NodeHandle n;
    signal(SIGINT, request_shutdown);

    ros::Rate loop_rate(10);

    ros::Publisher vel_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1000);

    while (! g_request_shutdown)
    {
        // commands
        move_twist(vel_pub, 0.5, 0.5, 2.0);
    }

    ROS_INFO_STREAM("## Shutting down. ##");
    geometry_msgs::Twist stop_msg;
    vel_publish(vel_pub, stop_msg, 1.0);
    ros::shutdown();
    return 0;
}