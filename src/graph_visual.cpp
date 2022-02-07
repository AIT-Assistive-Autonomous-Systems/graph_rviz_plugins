// Copyright 2022 Austrian Institute of Technology GmbH
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <rviz_common/msg_conversions.hpp>
#include <rviz_rendering/material_manager.hpp>
#include "graph_visual.hpp"
#include <utility>
#include <memory>
#include <OgreManualObject.h>
#include <OgreSceneManager.h>
#include <OgreMaterial.h>
#include <OgreTechnique.h>

namespace graph_rviz_plugins
{
using Ogre::ColourValue;

std::atomic_uint64_t GraphVisual::unique_ids_ = 0;

GraphVisual::GraphVisual(
  Ogre::SceneManager * scene_manager,
  Ogre::SceneNode * parent_scene_node)
: scene_manager_(scene_manager), parent_scene_node_(parent_scene_node)
{
  auto inc_id = unique_ids_.fetch_add(1);
  auto id = "graph_visual/" + std::to_string(inc_id);
  lines_object_ = scene_manager_->createManualObject(id);
  scene_node_ = parent_scene_node_->createChildSceneNode();
  scene_node_->attachObject(lines_object_);

  material_ = rviz_rendering::MaterialManager::createMaterialWithNoLighting(id);
  material_->setLightingEnabled(true);
  material_->setReceiveShadows(false);
  setColor(ColourValue(1.0, 0.0, 0.0, 1.0));
}

GraphVisual::~GraphVisual()
{
  if (lines_object_) {
    scene_node_->detachObject(lines_object_);
    scene_manager_->destroyManualObject(lines_object_);
  }
  if (scene_node_) {
    scene_manager_->destroySceneNode(scene_node_);
  }
}

void GraphVisual::setColor(ColourValue color)
{
  rviz_rendering::MaterialManager::enableAlphaBlending(material_, color.a);
  material_->setAmbient(color * 0.5f);
  material_->setDiffuse(color);
  material_->setSelfIllumination(color);
}

void GraphVisual::update(const graph_msgs::msg::GeometryGraph & detection)
{
  lines_object_->clear();
  lines_object_->begin(material_->getName(), Ogre::RenderOperation::OT_LINE_LIST, "rviz_rendering");
  for (const auto & point : detection.nodes) {
    lines_object_->position(point.x, point.y, point.z);
  }
  for (const auto & edge : detection.edges) {
    for (size_t i = 1; i < edge.node_ids.size(); i++) {
      auto n0 = edge.node_ids[i - 1];
      auto n1 = edge.node_ids[i];
      lines_object_->index(n0);
      lines_object_->index(n1);
    }
  }
  lines_object_->end();
}

void GraphVisual::reset()
{
  lines_object_->clear();
}

} // namespace graph_rviz_plugins
