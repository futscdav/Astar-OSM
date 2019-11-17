#ifndef _GPSLOCATION_GUARD
#define _GPSLOCATION_GUARD

// struct encompassing a location on Earth
class GPSLocation {
	public:
		GPSLocation(double Latitude, double Longitude) : Latitude(Latitude), Longitude(Longitude) {}
		
		const double		GetLatitude() const		{ return Latitude; }
		const double		GetLongitude() const	{ return Longitude; }
		const long long		GetId() const			{ return -1L; }

	private:
		double Latitude;
		double Longitude;
};

#endif