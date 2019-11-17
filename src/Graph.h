#ifndef _GRAPH_GUARD
#define _GRAPH_GUARD

#include "Node.h"
#include "Edge.h"
#include <map>
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <iostream>

using GraphNode = Node;
using GraphEdge = Edge;

using EdgeMap = std::unordered_map<EdgeId*, GraphEdge*, std::hash<EdgeId*>, EdgeIdComparator>;
using NodeMap = std::unordered_map<long long, GraphNode*>;
using EdgeSet = std::vector<GraphEdge*>;
using NodeSet = std::vector<long long>;
using EdgeList = EdgeSet;
using NodeEdgeMap = std::unordered_map<long long, EdgeSet*>;

class Graph {
	public:
		Graph(NodeMap NodesByNodeId,
			  EdgeMap EdgesByFromToNodeIds,
			  NodeEdgeMap NodeOutcomingEdges,
			  NodeEdgeMap NodeIncomingEdges) 
			: NodesByNodeId(std::move(NodesByNodeId)), EdgesByFromToNodeIds(std::move(EdgesByFromToNodeIds)),
			  NodeOutcomingEdges(std::move(NodeOutcomingEdges)), NodeIncomingEdges(std::move(NodeIncomingEdges)), EmptySet(0) { }
		~Graph() { 
			// it's not exactly clear what the ownership schema should look like here, so there is none, the only given is that when the graph
			// is destroyed, it's destroyed in its entirety
			for (auto node : NodesByNodeId) {
				delete node.second;
			}
			for (auto edge : EdgesByFromToNodeIds) {
				delete edge.first;
				delete edge.second;
			}
			for (auto outedges : NodeOutcomingEdges) {
				delete outedges.second;
			}
			for (auto inedges : NodeIncomingEdges) {
				delete inedges.second;
			}
		};

		GraphNode& GetNodeByNodeId(long long NodeId);
		GraphEdge& GetEdge(long long FromNodeId, long long ToNodeId);
		NodeMap& GetAllNodes() { return NodesByNodeId; }
		EdgeSet& GetNodeIncomingEdges(long long NodeId);
		EdgeSet& GetNodeOutcomingEdges(long long NodeId);
		EdgeMap& GetAllEdges() { return EdgesByFromToNodeIds; }

		class NodeNotFoundException : public std::exception {

		};

	private:
		NodeMap NodesByNodeId;
		EdgeMap EdgesByFromToNodeIds;
		NodeEdgeMap NodeOutcomingEdges;
		NodeEdgeMap NodeIncomingEdges;
		// a quick reference to an empty set
		EdgeSet EmptySet;

};

#endif
