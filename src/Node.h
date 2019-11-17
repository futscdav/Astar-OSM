#ifndef _NODE_GUARD
#define _NODE_GUARD

#include "GPSLocation.h"

class Node {
	public:
		Node(long long Id, GPSLocation GPSLoc) : Id(Id), GPSLoc(GPSLoc) { }

		const long long			GetId() const						{ return Id; }
		bool					operator==(const Node& other) const	{ return Id == other.GetId(); }
		const double			GetLatitude() const					{ return GPSLoc.GetLatitude(); }
		const double			GetLongitude() const				{ return GPSLoc.GetLongitude(); }
		const GPSLocation&		GetGPSLocation() const				{ return GPSLoc; }

	private:
		const long long Id;
		const GPSLocation GPSLoc;
		// description is thrown away to make loading faster
		// const string& Description;
};

#endif