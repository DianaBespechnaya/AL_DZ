#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/scope_exit.hpp>
#include <gmp.h>
#include <gmpxx.h>

using boost::asio::ip::tcp;

enum { max_length = 1024 };

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
            return 1;
        }

        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(tcp::v4(), argv[1], argv[2]);
        tcp::resolver::iterator iterator = resolver.resolve(query);

        tcp::socket s(io_service);
        boost::asio::connect(s, iterator);

        using namespace std;
        for(;;){
            char reply[max_length];		// поменять бы имя переменной
            auto buf = make_shared<boost::asio::streambuf>();
            boost::asio::read_until(s, *buf, '\n');
            std::istream stream(buf.get());
             stream.getline(reply, max_length);
            std::cout << "Data recieve " << reply << std::endl;
            string str= reply;		//можно сразу писать в str
            char *request = new char[max_length]();

            BOOST_SCOPE_EXIT((request)){  // зачем это нужно? unique_ptr в помощь 
                delete[] request;
           } BOOST_SCOPE_EXIT_END

            if (reply[0]<=57 && reply[0] > 48) {	//isdigit(reply[0]) ?
                string ch = "";
                string st = "";	//лучше назвать попонятнее ch -> number, st -> degree
                auto found = str.find('^');
                ch = str.substr(0, found);
                st = str.substr(found + 1, str.size() - found);

                mpz_class ch1;	//по именам переменных ничего не понятно со стороны
                mpz_class st1;
                mpz_class rez;
                mpz_init(rez.get_mpz_t());
                mpz_init(st1.get_mpz_t());
                mpz_init(ch1.get_mpz_t());
                mpz_set_str(st1.get_mpz_t(), st.c_str(), 10); //тоесть изначально мы переводили из
                mpz_set_str(ch1.get_mpz_t(), ch.c_str(), 10); //char* в string а теперь обратно, только для
															  //того чтобы распарсить?
                rez = 1;
                for (int i = 1; i <= st1; i++) {
                    rez = rez * ch1;	//в библиотеке есть возведение в степень, наверное оно более эффективно
                }

                mpz_get_str(request, 10, rez.get_mpz_t());
            }
            else {
                request[0] = '0';
            }

	           std::stringstream ss(request);
            while(ss){
                char* temp= new char[100]();	//unique_ptr

                ss.read(temp,100);
                string temp_str=temp;
                int length =temp_str.size();
                if (length < 100) {
                    temp[length] = '\n';
                    boost::asio::write(s, boost::asio::buffer(temp,length+1));
                }
                else
                    boost::asio::write(s, boost::asio::buffer(temp,100));
                delete[] temp;
            }
           

            if (request[0]=='0')
                break;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
     return 0;
 }
