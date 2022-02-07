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

#include <memory>
#include "geometry_graph_display.hpp"
#include "rviz_common/properties/color_property.hpp"
#include "rviz_common/properties/float_property.hpp"
#include "rviz_common/validate_floats.hpp"

namespace graph_rviz_plugins
{
using graph_msgs::msg::GeometryGraph;
using rviz_common::properties::StatusProperty;

GeometryGraphDisplay::GeometryGraphDisplay()
{
  color_property_ = new rviz_common::properties::ColorProperty(
    "Color", QColor(255, 25, 0), "Color to draw the edges.",
    this, SLOT(updateColorAndAlpha()));

  alpha_property_ = new rviz_common::properties::FloatProperty(
    "Alpha", 1, "Amount of transparency to apply to the edges.",
    this, SLOT(updateColorAndAlpha()));
  alpha_property_->setMin(0);
  alpha_property_->setMax(1);
}

GeometryGraphDisplay::~GeometryGraphDisplay() = default;

void GeometryGraphDisplay::onInitialize()
{
  MFDClass::onInitialize();
  graph_visual_.reset(new GraphVisual(scene_manager_, scene_node_));
}

void GeometryGraphDisplay::updateColorAndAlpha()
{
  Ogre::ColourValue color = color_property_->getOgreColor();
  color.a = alpha_property_->getFloat();
  if (graph_visual_) {
    graph_visual_->setColor(color);
  }
  context_->queueRender();
}

void GeometryGraphDisplay::processMessage(GeometryGraph::ConstSharedPtr message)
{
  for (const auto & node : message->nodes) {
    if (!rviz_common::validateFloats(node)) {
      setStatus(
        StatusProperty::Error, "Topic",
        "Message contained invalid floating point values (nans or infs) in graph nodes");
      return;
    }
  }

  Ogre::Vector3 position;
  Ogre::Quaternion orientation;
  if (!context_->getFrameManager()->getTransform(message->header, position, orientation)) {
    setMissingTransformToFixedFrame(message->header.frame_id);
    return;
  }
  setTransformOk();

  scene_node_->setPosition(position);
  scene_node_->setOrientation(orientation);

  graph_visual_->update(*message);
  updateColorAndAlpha();
}

void GeometryGraphDisplay::reset()
{
  MFDClass::reset();
  graph_visual_->reset();
}

}  // namespace graph_rviz_plugins

#include <pluginlib/class_list_macros.hpp> // NOLINT
PLUGINLIB_EXPORT_CLASS(graph_rviz_plugins::GeometryGraphDisplay, rviz_common::Display)
