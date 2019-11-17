#include "Graph.h"

GraphNode& Graph::GetNodeByNodeId(long long NodeId) {
	NodeMap::iterator it = NodesByNodeId.find(NodeId);
	if (it == NodesByNodeId.end())
		throw NodeNotFoundException();
	return *(*(it)).second;
}

EdgeSet& Graph::GetNodeOutcomingEdges(long long NodeId) {
	NodeEdgeMap::iterator it = NodeOutcomingEdges.find(NodeId);
	if (it != NodeOutcomingEdges.end()) {
		return *(*(it)).second;
	}
	return EmptySet;
}

EdgeSet& Graph::GetNodeIncomingEdges(long long NodeId) {
	NodeEdgeMap::iterator it = NodeIncomingEdges.find(NodeId);
	if (it != NodeIncomingEdges.end()) {
		return *(*(it)).second;
	}
	return EmptySet;
}

GraphEdge& Graph::GetEdge(long long FromNodeId, long long ToNodeId) {
	EdgeId Tmp(FromNodeId, ToNodeId);
	GraphEdge& Out = *(*(EdgesByFromToNodeIds.find(&Tmp))).second;
	return Out;
}
