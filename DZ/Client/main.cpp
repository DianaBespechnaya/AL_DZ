#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/scope_exit.hpp>
#include <gmp.h>
#include <gmpxx.h>

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
   	try
	{
		boost::asio::io_service io_service;

		tcp::resolver resolver(io_service);
		tcp::resolver::query query(tcp::v4(), argv[1], argv[2]);
		tcp::resolver::iterator iterator = resolver.resolve(query);

		tcp::socket s(io_service);
		boost::asio::connect(s, iterator);

		auto reply = std::make_unique<char[]>(max_length);

		for (;;){
			string request;
			auto buf = make_unique<boost::asio::streambuf>();
			boost::asio::read_until(s, *buf, '\n');
			istream stream(buf.get());
			getline(stream, request);
			cout << "Data recieve " << request << endl;

			if (isdigit(request[0])) {
				string ch = "";
				string st = "";
				auto found = request.find('^');
				ch = request.substr(0, found);
				st = request.substr(found + 1, request.size() - found);

				mpz_t base;
				mpz_t pow;
				mpz_t rez;
				mpz_init(rez);
				mpz_init(pow);
				mpz_init(base);
				mpz_set_str(pow, st.c_str(), 10);
				mpz_set_str(base, ch.c_str(), 10);

				mpz_pow_ui(rez, base, mpz_get_ui(pow));
				mpz_get_str(reply.get(), 10, rez);
			}
			else {
				reply[0] = '0';
			}

			string rezult = reply.get();
			int length = rezult.size();
			reply[length] = '\n';

			boost::asio::write(s, boost::asio::buffer(reply.get(), length + 1));

			if (reply[0] == '0')
				break;
		}
	}
	catch (std::exception& e)
	{
		std::cout << "Exception: " << e.what() << "\n";
	}
	return 0;
 }
