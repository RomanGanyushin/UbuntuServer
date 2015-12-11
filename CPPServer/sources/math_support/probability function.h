#pragma once
#include <algorithm>
#include <functional> 

#include <boost\random\uniform_real.hpp>
#include <boost\random\uniform_int.hpp>
#include <boost\random\linear_congruential.hpp>
#include <boost\random\variate_generator.hpp>
#include <boost\random\discrete_distribution.hpp>
#include <boost/random/normal_distribution.hpp>

#include "boost\date_time\date.hpp"

boost::minstd_rand gl_generator(time(0));

struct probabilit : public std::vector<double>
{
	struct gauss { 
		gauss(double _a, double _b, double _c) :a(_a), b(_b), c(_c) {};
		gauss(double _b, double _c) :a(1), b(_b), c(_c) {};
	double a, b, c; };
	struct sigma { sigma(size_t _a, size_t _b) :a(_a), b(_b) {}; size_t a, b; };

	probabilit(size_t _size) :std::vector<double>(_size){};
	probabilit(size_t _size, double _fill) :std::vector<double>(_size, _fill){};

	probabilit(size_t _size, const gauss& g) :std::vector<double>(_size)
	{
		double x = 0;
		std::generate(begin(), end(), [&]{
			double r = g.a*exp(-1 * (x - g.b)*(x - g.b) / (2 * g.c*g.c));
			x +=1; return r;
		});
	}
	probabilit(size_t _size, const sigma& s) :std::vector<double>(_size, 0)
	{
		std::fill(begin() + s.a, begin() + s.b, 1);
	}

	probabilit operator*(const probabilit& p)
	{
		assert(size() == p.size());
		probabilit result(size());
		std::transform(begin(), end(), p.begin(), result.begin(), std::multiplies<double>());
		return result;
	}

	const probabilit&  normalize()
	{
		double sum = 0; for_each(begin(), end(), [&sum](double& it){sum += it; });
		std::transform(begin(), end(), begin(), std::bind2nd(std::divides<double>(), sum));
		return *this;
	}

};

inline probabilit symmetric_gauss_probabilit(const size_t& size, const unsigned int& centre, const unsigned int& width)
{
	return probabilit(size, probabilit::gauss(centre, width))*
		probabilit(size, probabilit::sigma(centre - width, centre + width)).normalize();
}

inline boost::gregorian::days random_day_365()
{
	return boost::gregorian::days(
		boost::variate_generator<boost::minstd_rand&, boost::uniform_int<>>
		(gl_generator, boost::uniform_int<>(0, 365))());
}

/*‘ормирует случайный вектор индексов без повтора*/
std::vector<size_t> random_index_vector(size_t size)
{
	

	std::vector<size_t> result(size);
	boost::uniform_int<> uni_distr(0, size*size);
	boost::variate_generator<boost::minstd_rand&, boost::uniform_int<>>
		uni(boost::minstd_rand(gl_generator), uni_distr);

	std::map<size_t, size_t> rnd_map;
	for (size_t i = 0; rnd_map.size() < size; i++) rnd_map[uni()] = rnd_map.size();

	size_t index = 0;
	for (auto& it : rnd_map) result[index++] = it.second;

	return result;
}

