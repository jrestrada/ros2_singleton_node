#include "Ros.h"
#include <thread>
#include <functional>
#include <future>
#include <signal.h>

using namespace std::placeholders;

void kill(int /*sig*/) {
}

Ros *Ros::s_self = nullptr;

Ros::Ros(int argc, char *argv[], const std::string &node_name) {
    // Initilize ROS
    rclcpp::init(argc, argv);
    if (s_self) {
        LOG("Ops, only one instance of 'Ros' can be created!");
    }
    else {
        s_self = this; 
        LOG("Ros created...");
    }
    // Create ROS executer and node
    m_executor = rclcpp::executors::StaticSingleThreadedExecutor::make_shared();
    try {
      m_node = rclcpp::Node::make_shared(node_name);
    } catch (const std::exception &ex){
      LOG("Failed to start node %s: %s", node_name.c_str(), ex.what());
    }
    m_executor->add_node(m_node);
    // Add ROS publisher and subscribers
    m_crawl_client = rclcpp_action::create_client<CrawlerAction>(m_node,"activate_crawler"); 
    m_reel_client = rclcpp_action::create_client<ReelAction>(m_node,"turn_reel"); 
    signal(SIGINT, kill);
}

Ros::~Ros() {
    rclcpp::shutdown(); 
    LOG("ROS shutdown!");
}

void Ros::spin(void) {
    m_executor->spin();
}

void Ros::spinOnBackground(void) {
    std::thread thread(std::bind(&rclcpp::executors::StaticSingleThreadedExecutor::spin, m_executor));
    thread.detach();
}

void Ros::shutdown(void) {
    m_executor->cancel();
}

void Ros::wait_servers(){
    if (!this->m_crawl_client->wait_for_action_server(timeout)) {
    LOG("Crawler server not available after waiting");
    // DO SOMETHING
  }

  if (!this->m_reel_client->wait_for_action_server(timeout)) {
    LOG("Reel server not available after waiting");
    // DO SOMETHING
  }
}

void Ros::goal_response_callback(std::shared_future<CrawlGoalHandle::SharedPtr> future)  {
  auto goal_handle = future.get();
  if (!goal_handle) {
    LOG( "Goal was rejected by server");
  } else {
    LOG( "Goal accepted by server, waiting for result");
  }
}
