#include "Ros.h"
#include <thread>

volatile sig_atomic_t g_sigint_flag = 0;

void sigint_handler(int sig){
  std::cout << sig;
  g_sigint_flag = 1;
}

int main(int argc, char ** argv)
{
  Ros ros(argc, argv, "my_node");
  auto bind = [&] (std::string msg){

  };
  auto sub = ros.node()->create_subscription<std_msgs::msg::String>("mysubscription", 10, bind);
  ros.spinOnBackground();
  signal(SIGINT, sigint_handler);
  while (!g_sigint_flag){
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  return 0;
}
