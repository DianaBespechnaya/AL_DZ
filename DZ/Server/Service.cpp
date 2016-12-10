#include "Service.h"
#include <mutex>
using namespace std;

mutex mtx;
vector<int> control;
vector<mpz_class> rezult;
Service::Service() {}

Service::Service(std::shared_ptr<ip::tcp::socket> _sock) : m_sock(_sock) {}

void Service::InitHandling() {
	string str ="";
    int NumOfTask=-1;
	mtx.lock();
	for (int i =0; i< control.size(); ++i){
		if (control[i] == 0){
            string row(mpz_get_str(NULL,10,tasks[i].second.get_mpz_t()));
            string ch(mpz_get_str(NULL,10,tasks[i].first.get_mpz_t()));
            str = ch +"^"+ row+ "\n";
			control[i] = 2; //в процессе выполнения
			NumOfTask = i;
			break;
		}
	}

	mtx.unlock();

	if (str == ""){
		for (int i =0; i< control.size(); ++i){
			if (control[i]!= 1)
                str = "Все задачи на этапе решения или решены \n"; //gui в сервисной части 
		}
    }
    if (str ==""){
        str = "Все задачи решены\n";
    }
	async_write(*m_sock, buffer(str), [=](const boost::system::error_code& ec, size_t bytesTransf) {
		this->OnSend(ec, bytesTransf,NumOfTask);
    });
}

void Service::OnSend(const boost::system::error_code& ec, size_t bytesTransf,int NumOfTask) {
	if (ec) {
        //std::cout << "OnSend Error" <<endl;
        cout << NumOfTask <<endl;	//куда cout?
		if (NumOfTask != -1)
			control[NumOfTask] = 0;
		stopHandling();
		return;
	}
	StartRecieving(NumOfTask);
}

void Service::StartRecieving(int NumOfTask) {
	auto buf = make_shared<boost::asio::streambuf>();
	async_read_until(*m_sock, *buf, '\n', [=](const boost::system::error_code& ec,
		size_t bytesTransf) {
		this->OnRecieved(ec, bytesTransf, buf, NumOfTask);
	});
}

void Service::OnRecieved(const boost::system::error_code& ec, size_t bytesTransf,
	std::shared_ptr<boost::asio::streambuf> buf, int NumOfTask) {
    if (ec) {
         //   cout << "onRecievedError" <<endl;
            if (NumOfTask != -1)
                control[NumOfTask] = 0;
            stopHandling();
            return;
        }
        istream stream(buf.get());
        string answer;
        stream >> answer;
        cout << "Data recieve  " << answer << std::endl;
        mpz_class rez;
        if (answer!="0") {
            mpz_init(rez.get_mpz_t());
            mpz_set_str(rez.get_mpz_t(),answer.c_str(),10);
            rezult[NumOfTask]=rez;
            control[NumOfTask] =1;

            float Process = (float) (NumOfTask+1) / control.size(); //Процесс выполнения в %
            emit ProcChanged(Process*100);
        }
        InitHandling();
}

void Service::stopHandling() {
    int temp =0;
    for (int i =0;i<control.size();i++){
        if (control[i] != 1){
            temp =1;
        }
    }
    if(temp){
        emit acceptError();
        m_sock->close();
    }
    else{
        emit acceptError();
        m_sock->close();
        emit serverend();
    }
}


