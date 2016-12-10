#include "server.h"

Server::Server() {
acc.reset(new Acceptor(m_ios));
}

void Server::Start(){
    emit acc->PortOK(acc->PortNum().port()); //хорошо бы отделить qt от серверной части
    emit acc->col_clientchanged(0);



    auto f = [this]() {
    m_ios.run();
    };
    acc->InitAccept();
    std::thread t(f);
    m_thread.reset(new std::thread(move(t)));
}

Acceptor* Server::get_acc(){
    return acc.get();
}

void Server::Stop(){    
	m_ios.stop();
	m_thread->join();
}


