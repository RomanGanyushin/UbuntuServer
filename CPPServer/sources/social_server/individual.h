#pragma once

#include <vector>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_serialize.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost\date_time\date.hpp"
#include "boost\shared_ptr.hpp"


#include "constants.h"
#include "..\geography_server\specification.h"


template<typename T>
struct base_characteristics
{
	T intelligence; /// ��������
	T force;        /// ����
	T health;       /// ��������
	T propensity_to_learn; /// ���������� � ��������
	std::map<std::string, T> education; // ����������� ������������� �� ������������
	T criminalism;  /// ��������������
	T happiness;    /// �������� �������
	T responsibility; /// ���������������
};

base_characteristics<int> operator-(const base_characteristics<unsigned char>& a,
	const base_characteristics<unsigned char>& b)
{
	base_characteristics<int> result;
	result.intelligence = (int)a.intelligence - b.intelligence;
	result.force = (int)a.force - b.force;
	result.health = (int)a.force - b.health;
	result.propensity_to_learn = (int)a.propensity_to_learn - b.propensity_to_learn;
	result.criminalism = (int)a.criminalism - b.criminalism;
	result.health = (int)a.happiness - b.happiness;
	result.propensity_to_learn = (int)a.responsibility - b.responsibility;

	for (auto& it : a.education)
	{
		if (b.education.find(it.first) == b.education.end())
		{
			result.education.insert(it);
		}
		else
		{
			result.education[it.first] = (int)a.education.at(it.first) - b.education.at(it.first);
		}

	}

	return result;
}



struct individual
{
	boost::uuids::uuid uid;		/*�������������*/
	boost::uuids::uuid nationality;  /*��������������*/
	unsigned char gender;		/*���*/
	boost::gregorian::date  date_of_birth; /*���� ��������*/
	base_characteristics<unsigned char> characteristics; /*������� ��������������*/
	std::map<std::string, std::string> additional_characteristics; /*�������������� ��������������*/
	std::vector<std::pair<boost::uuids::uuid, unsigned char>> links; /*����������� � ��������� �����*/
	boost::uuids::uuid work_contract; /*����� ��������� ��������*/
	boost::uuids::uuid housing_registration; /*��������*/
	std::vector<std::pair<boost::uuids::uuid, boost::uuids::uuid>> bank_accounts; /*���������� ����� ����/����*/
	std::vector<boost::uuids::uuid> real_estate; /*������������ � �������������*/
	std::vector<boost::uuids::uuid> transport;   /*��������� � �������������*/
	std::vector<std::pair<boost::uuids::uuid, double>> cash; /*���������� ��� ������/����������*/
	std::vector<boost::uuids::uuid> things; /*���� � �������������*/
	std::map <std::string, std::string> additional; /*�������������� ����������*/
	geo_postion position; /*�������*/

	const bool is_marred() const
	{
		for (auto& it : links) if (it.second == link_type_marred){ return true; }
		return false;
	}
};


struct individual_different
{
	bool same_nationality;
	bool same_gender;
	bool is_relatives, is_friends;
	int age_difference;
	base_characteristics<int> bc_difference;
	geo_distance distance;
};

individual_different operator-(const individual&a, const individual&b)
{
	individual_different result;
	result.same_nationality = a.nationality == b.nationality;
	result.same_gender = a.gender == b.gender;
	result.is_relatives = false;
	result.is_friends = false;

	for (auto& it : a.links)
		if (it.first == b.uid)
		{
			if (it.second >= link_type_childer && it.second <= link_type_marred)
				result.is_relatives = true;
			else if (it.second == link_type_friend)
				result.is_friends = true;
		}



	result.age_difference = (a.date_of_birth - b.date_of_birth).days();
	result.bc_difference = a.characteristics - b.characteristics;
	result.distance = a.position - b.position;

	return result;
}

individual_different operator*(const double &a, const individual_different& b)
{
	individual_different result = b;
	result.age_difference *= a;
	result.bc_difference.criminalism *= a;
	result.bc_difference.force *= a;
	result.bc_difference.happiness *= a;
	result.bc_difference.health *= a;
	result.bc_difference.intelligence *= a;
	result.bc_difference.propensity_to_learn *= a;
	result.bc_difference.responsibility *= a;
	for (auto& it : result.bc_difference.education) it.second *= a;

	return result;
}

individual_different operator*(const individual_different& b, const double &a)
{
	return operator*(a, b);
}

individual_different operator-(const individual_different& b)
{
	return operator*(-1, b);
}

