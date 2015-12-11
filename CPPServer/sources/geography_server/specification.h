/* 


*/
#pragma once

#include <vector>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_serialize.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"

struct geo_postion
{
 double latitude; /*������*/
 double longitude; /*�������*/
 double altitude;  /*������*/

};

struct geo_distance
{
	double delta_latitude; /*������*/
	double delta_longitude; /*�������*/
	double delta_altitude;  /*������*/
};

geo_distance operator-(const geo_postion& a, const geo_postion& b)
{
	geo_distance result;
	result.delta_latitude = a.latitude - b.latitude;
	result.delta_longitude = a.longitude - b.longitude;
	result.delta_altitude = a.altitude - b.altitude;
	return result;
}