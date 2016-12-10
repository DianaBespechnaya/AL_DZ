#include "binom.h"


extern vector<pair<mpz_class,mpz_class>> tasks; 

void Binom(mpz_class n, mpz_class k){
    if (n < 0 || k < 0) {
        throw "Введено отрицательное число."; //такое себе
    }

    tasks.clear();
    vector<bool> marker;

    mpz_class i;
    mpz_init(i.get_mpz_t());
    for (; i<n; i++){
        marker.push_back(false);
    }

    mpz_init(i.get_mpz_t());
    i=i+2;
    for (; i<=n; ++i){
        if (!marker[mpz_get_ui(i.get_mpz_t())]){
            mpz_class c,q;
            mpz_init(c.get_mpz_t());
            mpz_init(q.get_mpz_t());
            q= n/i;
            while (q > 0){
                c+=q;
                q=q/i;
            }
            pair<mpz_class,mpz_class> p = make_pair(i,c);
            tasks.push_back(p);

            mpz_class j;
            mpz_init(j.get_mpz_t());
            j=j+2;

            while (j*i <=n ){
                mpz_class temp;
                mpz_init(temp.get_mpz_t());
                temp=j*i;
                marker[mpz_get_ui(temp.get_mpz_t())] = true;
                j++;
            }
        }
    }

    marker.clear();
    for (; i<k; i++){
        marker.push_back(false);
    }

    for (i=2; i<=k; ++i){
        if (!marker[mpz_get_ui(i.get_mpz_t())]){
            mpz_class c,q;
            mpz_init(c.get_mpz_t());
            mpz_init(q.get_mpz_t());
            q= k/i;
            while (q > 0){
                c+=q;
                q=q/i;
            }
            vector<pair<mpz_class,mpz_class>>::iterator it;
            for (it = tasks.begin(); it <= tasks.end(); ++it){
                if (mpz_get_ui(it->first.get_mpz_t())== i){
                    mpz_sub(it->second.get_mpz_t(),it->second.get_mpz_t(),c.get_mpz_t());
                    if (mpz_get_ui(it->second.get_mpz_t())==0) tasks.erase(it);
                    break;
                 }
            }
        }
    }

    marker.clear();
    for (; i<n-k; i++){
        marker.push_back(false);
    }

    for (i=2; i<=(n-k); ++i){
        if (!marker[mpz_get_ui(i.get_mpz_t())]){
            mpz_class c,q;
            mpz_init(c.get_mpz_t());
            mpz_init(q.get_mpz_t());
            q= (n-k)/i;
            while (q > 0){
                c+=q;
                q=q/i;
            }
            vector<pair<mpz_class,mpz_class>>::iterator it;
            for (it = tasks.begin(); it <= tasks.end(); ++it){
                if (mpz_get_ui(it->first.get_mpz_t())== i){
                    mpz_sub(it->second.get_mpz_t(),it->second.get_mpz_t(),c.get_mpz_t());
                    if (mpz_get_ui(it->second.get_mpz_t())==0) tasks.erase(it);
                    break;
                 }
            }
        }
    }


    for (int i =0; i< tasks.size(); i++){
        cout << tasks[i].first <<"  " <<tasks[i].second <<endl;
    }
}
