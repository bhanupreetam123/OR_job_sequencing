#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <numeric>
#include <memory>
#include <chrono>


template<typename T> 
void write(T vect){
    for(auto i:vect) std::cout << i << ' ';
    std::cout << std::endl;
} 


class Machine{
    public:
    int n , delay_time;
    std::vector<int> T_in , T_out , Idle , processing_time;
    Machine(int n) : n(n) , T_in(n) , T_out(n) , Idle(n) , processing_time(n) , delay_time(0) {}
    
    void set_processing_time(std::vector<int> processing_time , std::vector<int> job_sequence){
        for(int i=0;i<n;i++){
            int job = job_sequence[i];
            this->processing_time[i] = processing_time[job];
        }
    }

    friend std::ostream& operator<<(std::ostream& os , Machine& m){
        os << "T_in : ";
        write(m.T_in);
        os << "Processing Time : ";
        write(m.processing_time);
        os << "T_out : ";
        write(m.T_out);
        os << "Idle : ";
        write(m.Idle);
        return os;
    }
};



bool initial_check(std::vector<std::vector<int> > vect , int m){
    int min1 = *min_element(cbegin(vect[0]) , cend(vect[0]));
    int min3 = *min_element(cbegin(vect[m-1]) , cend(vect[m-1]));
    int max2 = INT_MIN;
    for(int i=1;i<m-1;i++){
        max2 = std::max(max2 , *max_element(cbegin(vect[i]) , cend(vect[i])));
    }

    return ((min1 >= max2) || (min3 >= max2));
}


bool myComp(std::pair<std::pair<int,int> , std::pair<int,int> >p1  ,std::pair<std::pair<int,int> , std::pair<int,int> >p2){
    if(p1.first.first == p2.first.first) return p1.first.second < p2.first.second;
    return p1.first.first < p2.first.first;
}


std::vector<std::pair<int,int> > get_ordered_pairs(std::vector<std::pair<std::pair<int,int> , std::pair<int,int> > > G_H){
    std::vector<std::pair<int,int> > ordered_pairs;
    for(auto i:G_H){
        ordered_pairs.push_back(i.second);
    }
    return ordered_pairs;
}

std::vector<int> get_job_sequence(std::vector<std::pair<int,int> > ordered_pairs , std::map<std::pair<int,int> , int> mp , int n){
    std::vector<int> job_sequence(n,-1);
    int front_ptr = 0;
    int back_ptr = n-1;

    for(int i=0;i<n;i++){
        std::pair<int,int> current_pair = ordered_pairs[i];
        if(current_pair.first <= current_pair.second){
            job_sequence[front_ptr++] = mp[current_pair];
        }
        else{
            job_sequence[back_ptr--] = mp[current_pair];
        }
    }
    return job_sequence;
}


void set_Time_in_Time_out(std::vector<Machine>& machines , int m){
    for(int i=0;i<m;i++){
        if(i==0){
            int current_Time_in = 0;
            for(int j=0;j<machines[i].n;j++){
                machines[i].T_in[j] = current_Time_in;
                current_Time_in += machines[i].processing_time[j];
                machines[i].T_out[j] = current_Time_in;
            }
        }
        else{
            int current_Time_in = 0;
            for(int j=0;j<machines[i].n;j++){
                current_Time_in = std::max(current_Time_in , machines[i-1].T_out[j]);
                machines[i].T_in[j] = current_Time_in;
                current_Time_in += machines[i].processing_time[j];
                machines[i].T_out[j] = current_Time_in;
            }
        }
    }
}


void set_idle(std::vector<Machine>& machines , int m){
    for(int i=0;i<m;i++){
        if(i==0){
            for(int j=0;j<machines[i].n;j++){
                machines[i].Idle[j] = 0;
            }
        }
        else{
            for(int j=0;j<machines[i].n;j++){
                if(j==0){
                    machines[i].Idle[j] = machines[i].T_in[j];
                }
                else{
                    machines[i].Idle[j] = machines[i].T_in[j] - machines[i].T_out[j-1];
                }
            }
        }
    }

}


void calculate_delay_time(std::vector<Machine>& machines , int m){
    for(int i=0;i<m-1;i++){
        machines[i].delay_time = machines[m-1].T_out[machines[m-1].n-1] - machines[i].T_out[machines[i].n-1];
    }
}

void print_evaluations(std::vector<Machine>& machines , int m){
    std::vector<int> idle_times;
    for(int i=0;i<m;i++){
        int idle_time = std::accumulate(cbegin(machines[i].Idle) , cend(machines[i].Idle) , 0);
        idle_times.push_back(idle_time);
    }
    
    calculate_delay_time(machines , m);

    for(int i=0;i<m;i++){
        std::cout << "Machine-" << i+1 << " :: Total Idle time = " << idle_times[i] << " , Delay time = " << machines[i].delay_time << std::endl;
    }

    int total_idle_time = std::accumulate(cbegin(idle_times) , cend(idle_times) , 0);
    int total_delay_time = 0;
    for(int i=0;i<m;i++) total_delay_time += machines[i].delay_time;
    std::cout << "Total Idle Time and delay of all Machines : " << total_idle_time + total_delay_time << std::endl;
    std::cout << "Total Elapsed time : " << machines[m-1].T_out[machines[m-1].n-1] << std::endl;
}

int main(){
    int n , m;
    std::cout << "number of machines ? ";
    std::cin >> m;
    std::cout << std::endl;
    std::cout << "number of jobs ? ";
    std::cin >> n;
    std::vector<std::vector<int> >vec(m,std::vector<int> (n,0));
    for(int i=1;i<=m;i++){
        std::cout << std::endl << "Enter Processing time of " << n <<" jobs by Machine-"<<i<<" : ";
        for(int j=0;j<n;j++) std::cin >> vec[i-1][j];
    }

    auto start = std::chrono::high_resolution_clock::now();
    bool initial_condition_check = initial_check(vec , m);
    if(!initial_condition_check){
        std::cout << "No initial conditions were satisfied" << std::endl;
        return 0;
    }

    std::vector<int> G , H;
    for(int i=0;i<n;i++){
        int sum = 0;
        static int row_ptr = 0;
        for(int j=0;j<m;j++) sum += vec[j][i];
        G.push_back(sum - vec[m-1][row_ptr]);
        H.push_back(sum - vec[0][row_ptr]);
        row_ptr++;
    }


    std::map<std::pair<int,int> , int > mp;
    for(int i=0;i<n;i++) mp[{G[i] , H[i]}] = i;

    std::vector<std::pair<std::pair<int,int> , std::pair<int,int> > > G_H;
    for(int i=0;i<n;i++){
        std::pair<int,int> inner_pair2{G[i] , H[i]};
        int minn = (G[i] < H[i]) ? G[i] : H[i];
        int maxx = (minn == H[i]) ? G[i] : H[i];
        std::pair<int,int> inner_pair1{minn , maxx};
        std::pair<std::pair<int,int> , std::pair<int,int> > outer_pair{inner_pair1 , inner_pair2};
        G_H.push_back(outer_pair);
    }
    
    sort(begin(G_H) , end(G_H) , myComp);

    std::vector<std::pair<int,int> > ordered_pairs = get_ordered_pairs(G_H);

    std::vector<int> job_sequence = get_job_sequence(ordered_pairs , mp , n);

    std::unique_ptr<Machine> machine = std::make_unique<Machine>(n);
    
    std::vector<Machine> machines(m , *machine);

    for(int i=0;i<m;i++) machines[i].set_processing_time(vec[i] , job_sequence);

    set_Time_in_Time_out(machines , m);
    set_idle(machines , m);

    for(int i=0;i<m;i++) {
        std::cout << "Machine-" << i+1 << " Status : " << std::endl;
        std::cout << machines[i] << std::endl;
    }

    print_evaluations(machines , m);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    std::cout << "Time taken : " << duration << " milliseconds" << std::endl;
    return 0;
}