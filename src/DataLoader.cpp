
#include "DataLoader.h"

#include <filesystem>
#include <iostream>
#include <fstream>

using namespace std;

// It is worth noting that this code will only load the data correctly on a big endian machine
// (i.e. all machines that I care about)

#define swapint(x) bswap32(x)
#define swapshort(x) bswap16(x)
#define swap64(x) bswap64(x)

inline unsigned int bswap32(unsigned int in) {
#ifndef _MSC_VER
    __asm__ volatile("bswap %0" : "=r" (in) : "r" (in) );
    return in;
#else
	unsigned int r = (in << 24) + (in << 8 & 0x00ff0000) + (in >> 8 & 0x0000ff00) + (in >> 24);
	return r;
#endif
}

inline unsigned short bswap16(unsigned short in) {
	short r = in;
	r <<= 8;
	r |= (in >> 8 & 0x00ff);
	return r;
}

// beware, C style casts!
inline unsigned long long bswap64(unsigned long long in) {
    unsigned int upper = bswap32((unsigned int)(in << 32 >> 32));
    unsigned int lower = bswap32((unsigned int)(in >> 32));
	unsigned long long r = ((unsigned long long)(upper) << 32) + (unsigned long long)(lower);
	return r;
}

bool IsBigEndian() {
	union {
		unsigned int i;
		char c[4];
	} endiannes = {0x01000000};
	return endiannes.c[0] == 1;
}

double SwapDoubleEndian(double WrongEndian) {
    // the classic union swap trick
	union {
		long long i;
		double  d;
	} conv;
	conv.d = WrongEndian;
	conv.i = bswap64(conv.i);
	return conv.d;
}

bool ReadNodes(const string& Filename, NodeMap& NodesContainer) {
	ifstream NodeFile(Filename, std::ios::in | ios::binary);
	if (!NodeFile.is_open()) {
		cerr << "Node file (" << Filename << ") not found!" << endl;
		return false;
	}
	// read size (number of nodes)
	size_t Size;
	NodeFile.read(reinterpret_cast<char*>(&Size), 4);
	Size = swapint(Size);
	
	// read nodes one by one (could do it batch)
	// each node is 24 bytes
	char Buffer [24];
	long long Id;
	double Lat, Lon;
	for (int i = 0; i < Size; ++i) {
		NodeFile.read(Buffer, 24);
		Id = swap64(*reinterpret_cast<long long*>(Buffer));
		Lat = SwapDoubleEndian(*reinterpret_cast<double*>(Buffer + 8));
		Lon = SwapDoubleEndian(*reinterpret_cast<double*>(Buffer + 16));
		NodesContainer.insert(make_pair(Id, new GraphNode(Id, GPSLocation(Lat, Lon))));
	}
	cout << NodesContainer.size() << " nodes loaded." << endl;
	if (NodesContainer.size() != Size) {
		cout << "Node file is inconsistent!" << endl;
		return false;
	}
	return true;
}

bool ReadEdges(const string& Filename, EdgeMap& Edges) {
	ifstream EdgeFile(Filename, std::ios::in | ios::binary);
	if (!EdgeFile.is_open()) {
		cerr << "Edges file (" << Filename << ") not found!" << endl;
		return false;
	}
	//read size
	size_t Size;
	EdgeFile.read(reinterpret_cast<char*>(&Size), 4);
	Size = swapint(Size);

	long long FromId, ToId;
	double MaxAllowedSpeed, LengthInMetres;
	char Buffer [32];

	// Each edge si 32 bytes
	for (int i = 0; i < Size; ++i) {
		EdgeFile.read(Buffer, 32);
		FromId = swap64(*reinterpret_cast<long long*>(Buffer));
		ToId = swap64(*reinterpret_cast<long long*>(Buffer + 8));
		MaxAllowedSpeed = SwapDoubleEndian(*reinterpret_cast<double*>(Buffer + 16));
		LengthInMetres = SwapDoubleEndian(*reinterpret_cast<double*>(Buffer + 24));
		Edges.insert(make_pair(new EdgeId(FromId,ToId), new GraphEdge(FromId, ToId, LengthInMetres, MaxAllowedSpeed)));
	}

	cout << Edges.size() << " edges loaded." << endl;
	if (Edges.size() != (unsigned int)Size) {
		cout << "Edges file is inconsistent!" << endl;
		return false;
	}
	return true;
}

bool ReadOutcomingEdges(const string& Filename, NodeEdgeMap& OutcomingEdges, EdgeMap& Edges) {
	ifstream OutcomingEdgesFile(Filename, std::ios::in | std::ios::binary);
	if (!OutcomingEdgesFile.is_open()) {
		cerr << "Outgoing Edges file (" << Filename << ") not found!" << endl;
		return false;
	}

	int Size;
	OutcomingEdgesFile.read(reinterpret_cast<char*>(&Size), 4);
	Size = swapint(Size);

	long long FromId, ToId;
	short NumberOfSuccessors;
	char Buffer [10];
	vector<char> SuccessorBuffer;
	// each edge is 10 bytes (FromId (8) + NumberOfSuccessors(2)) + 8 bytes for each successor
	for (int i = 0; i < Size; ++i) {

		OutcomingEdgesFile.read(Buffer, 10);
		FromId = swap64(*reinterpret_cast<long long*>(Buffer));
		NumberOfSuccessors = swapshort(*reinterpret_cast<short*>(Buffer + 8));

		EdgeSet* Successors = new EdgeSet(NumberOfSuccessors);
		if (SuccessorBuffer.capacity() < NumberOfSuccessors * 8) {
			SuccessorBuffer.reserve(NumberOfSuccessors * 8);
		}
		OutcomingEdgesFile.read(SuccessorBuffer.data(), NumberOfSuccessors*8);

		for (int j = 0; j < NumberOfSuccessors; ++j) {
			ToId = swap64(*reinterpret_cast<long long*>(SuccessorBuffer.data() + 8*j));
			EdgeId EdgeIdentification(FromId, ToId);
			//cache the find
			auto EdgeIterator = Edges.find(&EdgeIdentification);
			if (EdgeIterator == Edges.end()) {
				cout << "Edge not previously inserted to Edges." << endl;
				return false;
			} else {
				Successors->at(j) = (*EdgeIterator).second;	
			}
		}
		OutcomingEdges.insert(make_pair(FromId, Successors));
	}

	cout << OutcomingEdges.size() << " outgoing edges loaded." << endl;
	if (OutcomingEdges.size() != (unsigned int)Size) {
		cout << "Outcoming edges file is inconsistent!" << endl;
		return false;
	}
	return true;
}

bool ReadIncomingEdges(const string& Filename, NodeEdgeMap& IncomingEdges, EdgeMap& Edges) {
	ifstream IncomingEdgesFile(Filename, std::ios::in | std::ios::binary);
	if (!IncomingEdgesFile.is_open()) {
		cerr << "Incoming Edges file (" << Filename << ") not found!" << endl;
		return false;
	}

	int Size;
	IncomingEdgesFile.read(reinterpret_cast<char*>(&Size), 4);
	Size = swapint(Size);

	long long FromId, ToId;
	short NumberOfPredecessors;
	char Buffer [10];
	vector<char> PredecessorBuffer;

	for (int i = 0; i < Size; ++i) {

		IncomingEdgesFile.read(Buffer, 10);
		ToId = swap64(*reinterpret_cast<long long*>(Buffer));
		NumberOfPredecessors = swapshort(*reinterpret_cast<long long*>(Buffer + 8));

		EdgeSet* Precedessors = new EdgeSet(NumberOfPredecessors);
		if (PredecessorBuffer.capacity() < NumberOfPredecessors * 8) {
			PredecessorBuffer.reserve(NumberOfPredecessors * 8);
		}
		IncomingEdgesFile.read(PredecessorBuffer.data(), NumberOfPredecessors*8);

		for (int j = 0; j < NumberOfPredecessors; ++j) {
			FromId = swap64(*reinterpret_cast<long long*>(PredecessorBuffer.data() + 8*j));
			EdgeId EdgeIdentification(FromId, ToId);
			auto EdgeIterator = Edges.find(&EdgeIdentification);
			if (EdgeIterator == Edges.end())
			{
				cout << "Edge not previously inserted to Edges." << endl;
			} else {
				Precedessors->at(j) = (*EdgeIterator).second;	
			}
		}
		IncomingEdges.insert(make_pair(ToId, Precedessors));
	}

	cout << IncomingEdges.size() << " incoming edges loaded." << endl;
	if (IncomingEdges.size() != (unsigned int)Size) {
		cout << "Incoming edges file is inconsistent!" << endl;
		return false;
	}
	return true;
}

// All of the datafiles are in Little Endian because they were serialized from Java using the standard 
// library serialization..
bool ReadData(const string& Directory, NodeMap& NodesByNodeId, EdgeMap& EdgesByFromToNodeIds, NodeEdgeMap& NodeOutcomingEdges, NodeEdgeMap& NodeIncomingEdges) {
	clock_t begin = clock();
	bool success = true;
	auto nodefile = (experimental::filesystem::path(Directory) / "Nodes_binary.dat").string();
	auto edgefile = (experimental::filesystem::path(Directory) / "Edges_binary.dat").string();
	auto outedgefile = (experimental::filesystem::path(Directory) / "OutcomingEdges_binary.dat").string();
	auto inedgefile = (experimental::filesystem::path(Directory) / "IncomingEdges_binary.dat").string();

	success &= ReadNodes(nodefile, NodesByNodeId); //"../Nodes_binary.dat"
	success &= ReadEdges(edgefile, EdgesByFromToNodeIds); // "../Edges_binary.dat"
	success &= ReadOutcomingEdges(outedgefile, NodeOutcomingEdges, EdgesByFromToNodeIds); // "../OutcomingEdges_binary.dat"
	success &= ReadIncomingEdges(inedgefile, NodeIncomingEdges, EdgesByFromToNodeIds); // "../IncomingEdges_binary.dat"
	if (!success) {
		return false;
	}
	cout << "Graph data successfully loaded in " << (double)(clock()-begin)/CLOCKS_PER_SEC << "s." << endl;
	cout << "------------------------------" << endl;
	return true;
}

std::unique_ptr<Graph> DataLoader::LoadFromFiles() {
	// check datadir exists
	if (!std::experimental::filesystem::is_directory(Directory)) {
		std::cerr << "Indicated directory '" << Directory << "' does not exist" << std::endl;
		return std::unique_ptr<Graph>(nullptr);
	}

	// rough estimate based on the values that i already know
	const int Buckets = 200000;
	NodeMap NodesById(Buckets);
	EdgeMap EdgesById(Buckets+90000);
	NodeEdgeMap NodesIncomingEdges(Buckets+50000);
	NodeEdgeMap NodesOutcomingEdges(Buckets+50000);

	//Read the graph data from binary files
	auto success = ReadData(Directory, NodesById, EdgesById, NodesOutcomingEdges, NodesIncomingEdges);
	if (!success) {
		return std::unique_ptr<Graph>(nullptr);
	}
	return std::make_unique<Graph>(std::move(NodesById), std::move(EdgesById), std::move(NodesOutcomingEdges), std::move(NodesIncomingEdges));
}
