/*********************************************************************
 *
 *  Copyright (c) 2019, 2030, Pibot Technology (Suzhou) Co., Ltd.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Pibot Technology (Suzhou) Co., Ltd. nor the
 *     names of its contributors may be used to endorse or promote 
 *     products derived from this software without specific prior 
 *     written permission.
 *
 * Author: Danny Zhu @Pibot
 *         
 *********************************************************************/

#include <gtt_planner/global_trajectory.h>
#include <ros/ros.h>
#include <yaml-cpp/yaml.h>
#include <ros/package.h>

namespace gtt_planner {

GlobalTrajectory::GlobalTrajectory(){
}

GlobalTrajectory::~GlobalTrajectory(){
};

bool GlobalTrajectory::loadPath(const std::string& file){
    std::string path_dir = ros::package::getPath("gtt_planner") + std::string("/path/");
    std::string f_ = file + ".yaml";
    ROS_INFO("gtt_planner::GlobalTrajectory loading global trajectory from %s ...", f_.c_str());
    YAML::Node node = YAML::LoadFile(path_dir+f_);
    std::string path_name = node["global_path"]["name"].as<std::string>();
    std::vector<std::pair<int, int> > xy_list = node["global_path"]["waypoints"].as<std::vector<std::pair<int, int> > >();

    int num_waypoints = xy_list.size();
    gt_.clear();
    for (int i=0; i<num_waypoints; i++){
        geometry_msgs::PoseStamped waypoint;
        waypoint.pose.position.x = xy_list[i].first;
        waypoint.pose.position.y = xy_list[i].second;
        gt_.push_back(waypoint);
    }
    ROS_INFO("Global path <%s> loaded successfully, containing %d waypoints.", path_name.c_str(), num_waypoints);

    return true;
}

void GlobalTrajectory::getFullPath(std::vector<geometry_msgs::PoseStamped>& path){
    path = gt_;
}

void GlobalTrajectory::getTrimmedPath(const geometry_msgs::PoseStamped& start, std::vector<geometry_msgs::PoseStamped>& path){
    path.clear();
    int nearest = getNearestPoint(start.pose.position.x, start.pose.position.y);
    for(int i=nearest; i<gt_.size(); i++){
        path.push_back(gt_[i]);        
    }
}

int GlobalTrajectory::getNearestPoint(const double& x, const double& y){
    int index = 0;
    double x_ = gt_[0].pose.position.x;
    double y_ = gt_[0].pose.position.y;
    double minDistance = sqrt(pow(x_ - x, 2.0) + pow(y_ - y, 2.0));

    for(int i=0; i<gt_.size(); i++){
        x_ = gt_[i].pose.position.x;
        y_ = gt_[i].pose.position.y;

        double distance = sqrt(pow(x_ - x, 2.0) + pow(y_ - y, 2.0));

        if(minDistance > distance){
            minDistance = distance;
            index = i;
        }
    }
    return index;
}

} // end namespace gtt_planner

