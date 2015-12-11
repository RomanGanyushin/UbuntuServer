// vcproject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//#define BOOST_ALL_DYN_LINK

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>


using boost::asio::ip::tcp;

class session
{
public:
	session(boost::asio::io_service& io_service)
		: socket_(io_service)
	{
	}

	tcp::socket& socket()
	{
		return socket_;
	}

	void start()
	{
		char p[] = "World Virtual Bank Of X-Lag\n\r)";
	
		boost::asio::async_write(socket_,
			boost::asio::buffer(p, sizeof(p)),
			boost::bind(&session::handle_write, this,
			boost::asio::placeholders::error));

		boost::asio::async_read_until(socket_, b, '\n', boost::bind(&session::handle_read, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));

	}

	boost::asio::streambuf b;
private:
	void handle_read(const boost::system::error_code& error,
		size_t bytes_transferred)
	{
		if (!error)
		{
		/*	boost::asio::async_write(socket_,
				boost::asio::buffer(data_, bytes_transferred),
				boost::bind(&session::handle_write, this,
				boost::asio::placeholders::error));*/
			std::istream is(&b);
			std::string line;
			std::getline(is, line);
			std::cout << line << "\n";

			

			boost::asio::async_read_until(socket_, b, "end", boost::bind(&session::handle_read, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
			
		}
		else
		{
			delete this;
		}
	}

	void handle_write(const boost::system::error_code& error)
	{
		if (!error)
		{
		/*	socket_.async_read_some(boost::asio::buffer(data_, max_length),
				boost::bind(&session::handle_read, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));*/
		}
		else
		{
			delete this;
		}
	}

	tcp::socket socket_;
	enum { max_length = 1024 };
	char data_[max_length];
};

class server
{
public:
	server(boost::asio::io_service& io_service, short port)
		: io_service_(io_service),
		acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
	{
		start_accept();
	}

private:
	void start_accept()
	{
		session* new_session = new session(io_service_);
		acceptor_.async_accept(new_session->socket(),
			boost::bind(&server::handle_accept, this, new_session,
			boost::asio::placeholders::error));
	}

	void handle_accept(session* new_session,
		const boost::system::error_code& error)
	{
		if (!error)
		{
			new_session->start();
		}
		else
		{
			delete new_session;
		}

		start_accept();
	}

	boost::asio::io_service& io_service_;
	tcp::acceptor acceptor_;
};

#include "social_server\society.h"

int main(int argc, char* argv[])
{
	auto a = probabilit(100,probabilit::gauss(25,5));
	auto b = probabilit(100, probabilit::sigma(15,35));
	auto c = (a*b);  
	c.normalize();

	default_generate_society::distibution distr;

	/* 70% - мужчин, 30% - женщин*/
	distr.sex = boost::random::discrete_distribution<>({ 0.5, 0.5 });

	/* Возраст нормально распределен по гауссу  в 15-35 лет, с максимумом в 25*/
	distr.age = boost::random::discrete_distribution<>(symmetric_gauss_probabilit(100, 25, 5));
	/* Сколнность к криминалу, распределение гаусса: 0-100, по шкале 0-255, c маскимуму в 50*/
	distr.criminalism = boost::random::discrete_distribution<>(symmetric_gauss_probabilit(255, 50, 50));
	/* Сила, распределение гаусса: 10-30, по шкале 0-255, c маскимуму в 20*/
	/* Для женщин, делиться на 2*/
	distr.force = boost::random::discrete_distribution<>(symmetric_gauss_probabilit(255, 20, 10));
	/* Ожущение счастья, распределение гаусса: 5-15, по шкале 0-255, c маскимуму в 10*/
	distr.happiness = distr.force = boost::random::discrete_distribution<>(symmetric_gauss_probabilit(255, 10, 5));
	/* Здоровье, распределение гаусса: 50-150, по шкале 0-255, c маскимуму в 100*/
	distr.health = boost::random::discrete_distribution<>(symmetric_gauss_probabilit(255, 100, 50));
	/* Склоноость к обучению, распределение гаусса: 10-30, по шкале 0-255, c маскимуму в 20*/
	distr.propensity_to_learn = boost::random::discrete_distribution<>(symmetric_gauss_probabilit(255, 20, 10));
	/* Склоноость к ответственности, распределение гаусса: 10-30, по шкале 0-255, c маскимуму в 20*/
	distr.responsibility = boost::random::discrete_distribution<>(symmetric_gauss_probabilit(255, 20, 10));
	/* Интелект, распределение гаусса: 10-30, по шкале 0-255, c маскимуму в 20*/
	distr.intelligence = boost::random::discrete_distribution<>(symmetric_gauss_probabilit(255, 20, 10));

	/*Создаем социум из 100 персонажей*/
	auto o = default_generate_society()(distr, 100, boost::posix_time::microsec_clock::local_time().date());
	
	/*Лямда - функция вероятности понравится*/
	o->probablies["probably_like"].reset(
		new probably(
		([](const individual& i, const individual_different& d)->double{
			// Запрещаем браки между родственниками и одноплые 
			if (d.is_relatives || d.same_gender) return 0;
		//Пока не обращаем внимание на детали, просто дадим вероятность 0.2
		//TODO: Написать полноценный механизм
		return 0.02;
	})));


	society_process(*o.get()).meet_cycle();

//	int all = o->amount_all();
//	int m = o->amount_marred();
//	std::cout << " " << m << std::endl;
	for (int i = 1; i < 100; i++)
	{
	society_process(*o.get()).meet_cycle();
//	 m = o->amount_marred();
//	 std::cout << i << " " << m << std::endl;
	}
	//try
	//{
	//	if (argc != 2)
	//	{
	//		std::cerr << "Usage: async_tcp_echo_server <port>\n";
	//		return 1;
	//	}

	//	boost::asio::io_service io_service;

	//	using namespace std; // For atoi.
	//	server s(io_service, atoi(argv[1]));

	//	io_service.run();
	//}
	//catch (std::exception& e)
	//{
	//	std::cerr << "Exception: " << e.what() << "\n";
	//}

	return 0;
}
