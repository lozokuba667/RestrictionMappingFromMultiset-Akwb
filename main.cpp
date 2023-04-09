#include <iostream>
#include <vector>
#include <iterator>
#include <fstream>
#include <tuple>
#include <chrono>
#include <set>
#include <numeric>
#include <optional>
#include <cmath>
#include <iomanip>
#include <map>

using namespace std;
using namespace std::chrono;

void showSet(multiset<int> container) {
    for (auto elem: container) {
        cout << elem << ",";
    }
    cout << "\n";
}


multiset<int> readFile() {
    ifstream file("multiset.txt");
    multiset<int> instance{istream_iterator<int>(file), istream_iterator<int>()};
    file.close();
    cout << "\nRead Multiset:\n";
    showSet(instance);
    return instance;
}

int estimateCutAmount(int size) {
    vector<int> predefined_cuts = {3,4,5,6,7,8,9,10,11,12,13,14};
    auto ca = (-3 + sqrt(8 * size + 1)) / 2;
    if(find(predefined_cuts.begin(),predefined_cuts.end(),ca) == predefined_cuts.end()){
        cout << "\nGiven instance has inappropirate size (" << size << ")!\nEnding program\n";
        exit(0);
    }
    cout << "\nInstance size: " << size << "\n";
    return ca+1;
}

pair<int, int> estimateMaximumValues(multiset<int> set) {
    int max_one = *set.rbegin();
    int max_two = *++set.rbegin();
    return make_pair(max_one, max_two);
}

void deleteFromSetBeforeHand(multiset<int> &set, pair<int, int> max_values, int elem) {
    vector<int> to_remove = {elem, max_values.first, max_values.second};
    for (auto target: to_remove) {
        auto place = set.find(target);
        set.erase(place);
    }
}

void showInfoBeforeHand(int first_elem, int out_size) {
    cout << "\n";
    cout << "BEFORE MAPPING INFO (based on instance itself)\n";
    cout << "----------------------------------------------\n";
    cout << "Amount of elements in map: " << out_size << "\n";
    cout << "Predicted first length/element: " << first_elem << "\n";
    cout << "----------------------------------------------\n";
}

tuple<vector<int>, int> establishBeginning(multiset<int> &instance){
    auto output_size = estimateCutAmount(instance.size());
    pair<int, int> maxims = estimateMaximumValues(instance);
    int first_elem = maxims.first - maxims.second;
    if(instance.find(first_elem) == instance.end()){
        cout << "\nGiven instance couldn't have created the map -- Invalid difference between two biggest elements\n";
        exit(0);
    }
    deleteFromSetBeforeHand(instance,maxims,first_elem);
    vector<int> solution = {first_elem};
    showInfoBeforeHand(first_elem,output_size);
    return {solution,maxims.second};
}


vector<int> pullPossibilities(vector<int> &map, int interval){
    vector<int> possible_values;
    int to_right = interval - map.back();
    possible_values.push_back(to_right);
    for(auto point=map.begin(); point != map.end(); point++){
        int from_left = accumulate(point, map.end(), 0);
        possible_values.push_back(from_left);
    }
    return possible_values;
}

bool validateInsertion(multiset<int> &current_instance, vector<int> current_map, int interval){
    vector<int>to_check = pullPossibilities(current_map, interval);
    for(auto elem:to_check){
        if(current_instance.find(elem) == current_instance.end()){
            return false;
        }
        current_instance.erase(current_instance.find(elem));
    }
    return true;
}

void DEBUGINFO(vector<int> output, vector<int> new_output, int old_maximum, int new_maximum){
    cout << "I had " << output.size() << " elems And my last element was: "<< output.back() << " and also maximum: " << old_maximum << "\n";
    cout << "NOW IM GOING WITH " << new_output.size() << " elems And my last element is: "<< new_output.back() << " and also maximum: " << new_maximum << "\n\n";
}

optional<vector<int>> search(multiset<int>& instance, vector<int> &output, int interval,int ind, map<int,vector<int>> &usedValues){
    if(instance.empty()) {
        output.push_back(interval);
        return output;
    }
    for(const auto& element: instance){
        if(element > interval){break;}
        else if (find(usedValues[ind].begin(),usedValues[ind].end(),element) != usedValues[ind].end()){continue;}
        vector<int> potential_output = output;
        auto copy_instance = instance;
        potential_output.push_back(element);
        if(validateInsertion(copy_instance, potential_output, interval)){
            usedValues[ind].push_back(element);
            //DEBUGINFO(output,potential_output,interval,interval - *potential_output.rbegin());
            if(auto res = search(copy_instance, potential_output, interval - *potential_output.rbegin(), ind+1, usedValues)){
                return res;
            }
        }
    }
    usedValues.erase(ind);
    return {};
}

int main() {
    multiset<int> multiset = readFile();
    auto[solution,length] = establishBeginning(multiset);
    cout << "\nTime start!\n";
    auto start = high_resolution_clock::now();
    map<int,vector<int>> used;
    if(auto result = search(multiset,solution,length,1,used)){
        cout << "\nResult found!\n\nSolution: {";
        for (const auto& each : result.value()) {cout << each << ",";}
        cout << "}\n";
        auto end = high_resolution_clock::now();
        auto elapsed = duration_cast<duration<float>>(end - start);
        cout << "\nTime taken: " << setprecision(4) << elapsed.count() << "[s]\n";
    } else {
        cout << "Solution not found";
    }

    return 0;
}
