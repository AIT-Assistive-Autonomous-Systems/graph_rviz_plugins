/* Copyright 2022 Austrian Institute of Technology GmbH

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#pragma once

#include <memory>
#include <list>
#include <OgreSceneNode.h>
#include "graph_msgs/msg/geometry_graph.hpp"

namespace graph_rviz_plugins {

class GraphVisual
{
private:
  Ogre::SceneManager * scene_manager_;
  Ogre::SceneNode * parent_scene_node_;
  Ogre::SceneNode * scene_node_;

  Ogre::ManualObject * lines_object_;
  Ogre::MaterialPtr material_;

  static std::atomic_uint64_t unique_ids_;

public:
  GraphVisual(Ogre::SceneManager *, Ogre::SceneNode * parent_scene_node);

  GraphVisual(GraphVisual&&) = delete;

  virtual ~GraphVisual();

  void setColor(Ogre::ColourValue);

  void update(const graph_msgs::msg::GeometryGraph &);

  void reset();
};

} // namespace graph_rviz_plugins
