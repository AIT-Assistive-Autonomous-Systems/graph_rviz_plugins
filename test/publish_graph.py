#!/usr/bin/python3
"""
Copyright 2022 Austrian Institute of Technology GmbH

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
"""

import sys
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Point
from graph_msgs.msg import GeometryGraph, Edges


class PublishGraph(Node):

    def __init__(self):
        super().__init__('publish_graph')
        self._pub_graph = self.create_publisher(GeometryGraph, 'graph', 1)
        self._timer = self.create_timer(0.1, self._on_timer)

    def _on_timer(self):
        msg = GeometryGraph()
        msg.header.frame_id = 'map'
        t = self.get_clock().now()
        msg.header.stamp = t.to_msg()

        # construct squared cycle
        msg.nodes = [
            # top
            Point(x=-1.0, y=2.0, z=0.5),
            Point(x=1.0, y=2.0, z=0.5),
            # right
            Point(x=2.0, y=1.0, z=0.0),
            Point(x=2.0, y=-1.0, z=0.0),
            # bottom
            Point(x=1.0, y=-2.0, z=0.5),
            Point(x=-1.0, y=-2.0, z=0.5),
            # left
            Point(x=-2.0, y=-1.0, z=0.0),
            Point(x=-2.0, y=1.0, z=0.0),]

        msg.edges = []
        n = len(msg.nodes)
        for i in range(0, n):
            msg.edges.append(Edges(node_ids=[i, (i+1) % n]))
        for i in range(0, n // 2):
            msg.edges.append(Edges(node_ids=[i*2, ((i+1)*2) % n]))
        for i in range(0, n // 2):
            msg.edges.append(Edges(node_ids=[1 + i * 2, (1 + ((i+1)*2)) % n]))
        self._pub_graph.publish(msg)


def main(args=sys.argv):
    rclpy.init(args=args)
    try:
        rclpy.spin(PublishGraph())
    except KeyboardInterrupt:
        pass


if __name__ == '__main__':
    main()
