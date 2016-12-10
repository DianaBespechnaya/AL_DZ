#include "Acceptor.h"

Acceptor::Acceptor(io_service &ios)
	: m_ios(ios)
    , m_acceptor(ios){
		unsigned short port_num = 5059;
	    ip::address_v4 a4(ip::address_v4::any());
	    while (true) {
		    port_num += 1;
		    try {
		    	ip::tcp::endpoint ep(a4, port_num);
		        m_acceptor = ip::tcp::acceptor(m_ios, ep);
		    	m_port = ep;
		    	break;
		    }
		    catch (...) {}
	}
}

ip::tcp::endpoint Acceptor::PortNum() {
    return m_port;
}

void Acceptor::onAccept(const boost::system::error_code&ec,
	std::shared_ptr<ip::tcp::socket> sock) {
	if (ec) {
		std::cout << "OnAccept Error";
		return;
	}
    ++col_client;
    emit col_clientchanged(col_client);
	std::cout << "accepted" << std::endl;		//cout куда 

    Service *service = new Service(sock);
    connect(service,SIGNAL(acceptError()),this,SLOT(clientexit()));     //отделить от gui
    connect(service,SIGNAL(serverend()),this,SLOT(alldone()));
    connect(service,SIGNAL(ProcChanged(float)), this, SLOT(UpdateProc(float)));
	service->InitHandling();
    this->InitAccept();
}
void Acceptor::clientexit(){
    --col_client;
    emit col_clientchanged(col_client);
}

void Acceptor::InitAccept(){

	ip::tcp::endpoint endp;
	ip::tcp::socket sock(m_ios);
	auto ptr = make_shared<ip::tcp::socket>(std::move(sock));
	m_acceptor.async_accept(*ptr, endp, [=](const boost::system::error_code& ec) {
		this->onAccept(ec, ptr);
	});
}

void Acceptor::alldone(){
    emit rezultready();
}

void Acceptor::UpdateProc(float proc) {
    emit UpdateProcInWin(proc);
}
