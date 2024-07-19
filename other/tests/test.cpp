#include <iostream>
#include <csignal>

#include <algorithm> 
#include <iostream> 
#include <vector> 
using namespace std; 
 
int main(){ 
    vector<int> v; 
    v.push_back(3);           //Insert element 3 
    v.push_back(5);           //Insert element 5 
    v.push_back(7);           //Insert element 7 
    v.push_back(8);
    v.push_back(9);
    //vector(v) has 3 elements with size 3 
 
    v.erase(v.begin() + 1);
 
    for(int i=0;i<v.size();i++){ 
        cout << v[i] << " "; 
    } 
     
    return 0; 
}