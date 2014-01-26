#include "ros/ros.h"
#include "std_msgs/Char.h"
#include <termios.h>
#include <stdio.h>
#include <sstream>
#include <signal.h>

int kfd=0;
struct termios cooked, raw;

void quit(int sig){
	tcsetattr(kfd, TCSANOW, &cooked);
	ros::shutdown();
	exit(0);
}
int main(int argc, char **argv){
  ros::init(argc, argv, "talker");
  ros::NodeHandle n;
  ros::Publisher chatter_pub = n.advertise<std_msgs::Char>("chatter", 1000);

  signal(SIGINT, quit);
  char c;
  tcgetattr(kfd, &cooked);
  memcpy(&raw, &cooked, sizeof(struct termios));
  raw.c_lflag &=~(ICANON | ECHO);
  raw.c_cc[VEOL] = 1;
  raw.c_cc[VEOF] = 2;
  tcsetattr(kfd, TCSANOW, &raw);
  puts("Reading keyboard");
  while (ros::ok())
  {
	if(read(kfd, &c, 1) < 0){
		perror("read():");
		exit(-1);
	}
	ROS_INFO("value: 0x%02X\n",c);
	std_msgs::Char value;
	value.data= c;
	chatter_pub.publish(value);
  }

  return 0;
}
