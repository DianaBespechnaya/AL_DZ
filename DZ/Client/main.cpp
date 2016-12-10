#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/scope_exit.hpp>
#include <gmp.h>
#include <gmpxx.h>

using boost::asio::ip::tcp;

enum { max_length = 1024 };

// https://github.com/google/protobuf здесь сильно помог бы для упаковки данных в сообщения

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
            char reply[max_length];
            auto buf = make_shared<boost::asio::streambuf>(); // зачем?
            boost::asio::read_until(s, *buf, '\n');
            std::istream stream(buf.get());
            stream.getline(reply, max_length); // получают _запрос_; отправляют -- ответ
            std::cout << "Data recieved " << reply << std::endl; // cerr?
            string str= reply;
            char *request = new char[max_length](); // странные ()
            // есть ли причина reply размещать на стеке, а request в куче?

            BOOST_SCOPE_EXIT((request)){ // классно, но есть unique_ptr, а 1к можно на стеке разместить
                delete[] request;
           } BOOST_SCOPE_EXIT_END

            if (reply[0]<=57 && reply[0] > 48) { // люто, непрозрачно, это C, не C++
                // это же split!
                string ch = "";
                string st = "";
                auto found = str.find('^');
                ch = str.substr(0, found);
                st = str.substr(found + 1, str.size() - found);

                // это работа клиента, она может быть другой.
                // это должен делать отдельный класс/функция, иначе нерасширяемо
                mpz_class ch1;
                mpz_class st1;
                mpz_class rez;
                mpz_init(rez.get_mpz_t());
                mpz_init(st1.get_mpz_t());
                mpz_init(ch1.get_mpz_t());
                mpz_set_str(st1.get_mpz_t(), st.c_str(), 10);
                mpz_set_str(ch1.get_mpz_t(), ch.c_str(), 10);

                rez = 1;
                for (int i = 1; i <= st1; i++) {
                    rez = rez * ch1;
                }

                mpz_get_str(request, 10, rez.get_mpz_t());
            }
            else {
                request[0] = '0'; // протокол слишком прост, не хватает сигнализации об ошибках
            }

            std::stringstream ss(request);
            while(ss){
                char* temp= new char[100]; // зачем на каждой итерации это выделять?

                // вынести подготовку данных к отправке в отдельную функцию?
                // что здесь происходит?
                ss.read(temp,100);
                string temp_str=temp; // зачем объект нужен?
                int length =temp_str.size();
                if (length < 100) {
                    temp[length] = '\n';
                    boost::asio::write(s, boost::asio::buffer(temp,length+1));
                }
                else
                    boost::asio::write(s, boost::asio::buffer(temp,100));
                // тут не бросаются исключения?
                delete[] temp;
            }

            if (request[0]=='0')
                break; // а сервер ждет ответа
        }
    }
    catch (std::exception& e) // любое исключение загасит клиента?
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
 }

 // Резюме: немасштабируемо, трудная поддержка, неустойчиво к ошибкам в обмене данными