/* 
Описание структур социально обустройства

*/
#pragma once
#include "individual.h"
#include "..\math_support\probability function.h"


struct work_contract
{
	boost::uuids::uuid hirer;
	boost::uuids::uuid employee;
	unsigned char number_of_working_days; // кол-во дней в неделю
	unsigned char working_hours;          // продолжительность рабочего дня
	double daily_wage;                    // дневная заработная плата в нац. валюте
	double payment_disability;            // оплата по нетрудоспособности
	std::map <std::string, double>  fringe_benefits; // дополнительные выплаты
};
struct work_contract_service
{
	std::map<boost::uuids::uuid, work_contract> register_wcontracts;
};

typedef std::function<double(const individual&, const individual_different&)> probably;
class society
{
friend class default_generate_society;
friend class society_query;
friend class society_statistic;
friend class society_process;
public:


public:
	std::map<std::string, std::shared_ptr<probably>> probablies;
protected:
	std::vector <individual> individuals;
	
};

struct abstract_society_process
{
	abstract_society_process(society& _society) :m_society(_society){};
	virtual void process(individual& a, individual& b) = 0;
	society& m_society;
}; 

struct functor_marry :  abstract_society_process
{
	functor_marry(society& _society) :abstract_society_process(_society){};
	void process(individual& a, individual& b)
	{
		a.links.push_back(std::make_pair(b.uid, link_type_marred));
		b.links.push_back(std::make_pair(a.uid, link_type_marred));
	}
};

struct society_statistic
{
	society_statistic(society& _society) :m_society(_society){};

	size_t amount_marred()
	{
		return std::count_if(m_society.individuals.begin(), m_society.individuals.end(), [](const individual& i){return i.is_marred(); });
	}

	size_t amount_all()
	{
		return m_society.individuals.size();
	}
private:
	society& m_society;
};

struct society_query
{
	society_query(society& _society) :m_society(_society){};

	/*Функция возвращает наборы пар возможных встреч*/
	std::vector <std::pair<individual*, individual*>> meeting(
		const double& area_of_visibility = 0, /*Область видимости*/
		const unsigned int max_pair = 100/*Максимальное кол-во пар*/
		)
	{
		std::vector <std::pair<individual*, individual*>> result;
		std::vector <individual*> _atarea = at_area(area_of_visibility);
		if (_atarea.size()<2) return result;
		auto rv = random_index_vector(_atarea.size());
		for (size_t i = 0; i < (_atarea.size()) && (result.size() <= max_pair); i += 2)
			result.push_back(std::make_pair(_atarea[rv[i]], _atarea[rv[i + 1]]));
		return result;
	}

	std::vector <individual*> at_area(const double& area_of_visibility)
	{
		std::vector <individual*> result;
		result.reserve(m_society.individuals.size());
		// ToDo: Нужен быстрый алгорим по формированию возможных пар
		for (individual& it : m_society.individuals) result.push_back(&it);
		return result;
	}

private:
	society& m_society;
};

class society_process
{
public:
	society_process(society& _society) :m_society(_society){};
	
	void meet_cycle()
	{
		auto pairs = society_query(m_society).meeting();
		for (auto& it : pairs)
		{
			action(it, "probably_like", functor_marry(m_society));
			//action(it, m_society.probably_friend, functor_marry(m_society));
			//action(it, m_society.probably_crime, functor_crime(m_society));
		}
	}

	void play(const boost::posix_time::ptime& time)
	{
		//for (auto&it : m_society.individuals)
				
	}

	


private:
	void action(std::pair<individual*, individual*> pair, 
		const std::string& prob_name, 
		abstract_society_process& proc
		)
		{
			std::shared_ptr<probably>& prob =
				m_society.probablies[prob_name];
			assert(prob.get());

			auto diff = (*pair.first) - (*pair.second);
			double prob_1_2 = (*prob.get())((*pair.first), diff);
			double prob_2_1 = (*prob.get())((*pair.second), -diff);
			double prob_finale = (prob_1_2 + prob_2_1) / 2.0;
			if (boost::random::discrete_distribution<>({ 1.0 - prob_finale, prob_finale })(gl_generator))
			{
				proc.process(*pair.first, *pair.second);
			}
		}

private:
	society& m_society;
};




class default_generate_society
{

public:
	struct distibution
	{
		boost::random::discrete_distribution<>  sex; /// Соотношение мужчин и женщин
		boost::random::discrete_distribution<>  age; // Распередение возраста
		boost::random::discrete_distribution<>  criminalism; // Распередение cклонности к криминалу
		boost::random::discrete_distribution<>  force;        /// Распередение силы
		boost::random::discrete_distribution<>  health;       /// Распередение здоровье
		boost::random::discrete_distribution<>  propensity_to_learn; /// Распередение склонности к обучению
		std::map<std::string, boost::random::discrete_distribution<> > education; // Образование распределение по направлениям
		boost::random::discrete_distribution<>  happiness;    /// Распередение ощущения счастья
		boost::random::discrete_distribution<>  responsibility; /// Распередение чувства ответственности
		boost::random::discrete_distribution<>  intelligence;  /// Распередение интелекта
	};
public:
	
	std::shared_ptr<society> 
		operator()(distibution _distibution, const size_t amount, const boost::gregorian::date& now)
	{
		std::shared_ptr<society> result(new society());
		
		result->individuals.resize(amount);

		for (auto& it : result->individuals)
		{
			it.uid = boost::uuids::random_generator()();

			it.gender = _distibution.sex(gl_generator);

			it.date_of_birth =
				now -
				boost::gregorian::years(_distibution.age(gl_generator)) -
				random_day_365();

			it.characteristics.criminalism = _distibution.criminalism(gl_generator);

			it.characteristics.force = _distibution.force(gl_generator);

			it.characteristics.health = _distibution.health(gl_generator);
			if (it.gender == gender_female)it.characteristics.health / 2;

			it.characteristics.propensity_to_learn = _distibution.propensity_to_learn(gl_generator);

			for (auto& it2 : _distibution.education)
				it.characteristics.education[it2.first] = it2.second(gl_generator);

			it.characteristics.happiness = _distibution.happiness(gl_generator);

			it.characteristics.responsibility = _distibution.responsibility(gl_generator);

			it.characteristics.intelligence = _distibution.intelligence(gl_generator);
		}


		return result;
	}

};


/*
boost::uniform_real<> uni_distr(0, 1);
boost::variate_generator<boost::minstd_rand&, boost::uniform_real<>>
uni(generator, uni_distr);
double p = uni();*/