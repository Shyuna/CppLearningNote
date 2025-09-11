#include <thread>
#include <iostream>

int main() {
    std::jthread t([]{});
std::cout<<"1"<<std::endl;
    return 0;
}
//#include <iostream>
//#include <thread>
//#include <string>
//#include <vector>
//
//std::string a="1";
//
//void t1(){
//    a="123";
//
//}
//
//void t2(){
//    std::cout<<a<<std::endl;
//}
//
//int main() {
//    std::vector<std::jthread> pool;
//    pool.push_back(std::thread(t1));
//    pool.push_back(std::thread(t2));
//    pool[0].join();
//    pool[1].join();
//    return 0;
//}
//
