#include "Factor.h"
#include <limits.h>
vector<pair<mpz_class,mpz_class>> tasks;

int MAX_INT = INT_MAX;

void Factor(mpz_class n){	
    if (n < 0) {
        throw "Введено отрицательное число."; //Некорректный выброс исключения
    }

    tasks.clear();     // очищаем вектор подзадач
    vector<bool> marker;
    for (int i =0; i<n+1; i++){
        marker.push_back(false);
    }

    mpz_class i;
    mpz_init(i.get_mpz_t());
    i=i+2;
    for (;i<=n; ++i){
        if (!marker[mpz_get_ui(i.get_mpz_t())]){  //если i-простое число
            mpz_class c,k;     // считаем показатель степени в разложении
	    	mpz_init(c.get_mpz_t());
	    	mpz_init(k.get_mpz_t());
	    	k= n/i;
		    while (k > 0){
		    	c+=k;
		    	k=k/i;
			}

            pair<mpz_class,mpz_class> p = make_pair(i,c); //запоминаем можитель и показатель его степени
            tasks.push_back(p);

            mpz_class j;
            mpz_init(j.get_mpz_t());
            j =j+2;
            while (j*i <=n ){   //просеиваем составные числа
                mpz_class temp;
                mpz_init(temp.get_mpz_t());
                temp = j*i;
                marker[mpz_get_ui(temp.get_mpz_t())] = true;
                ++j;
			}
		}
	}

    for (int i =0; i< tasks.size(); i++){
        cout << tasks[i].first <<"  " <<tasks[i].second <<endl;
    }
}
