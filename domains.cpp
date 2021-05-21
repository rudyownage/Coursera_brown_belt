#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "test_runner.h"
#include <unordered_set>
using namespace std;

vector<string> ReadDomeins(istream& input = cin) {
    int num_domens;
    input >> num_domens;
    vector<string> result;
    result.reserve(num_domens);
    for (int i = 0; i < num_domens; ++i) {
        string current_domen;
        input >> current_domen;
        result.push_back(move(current_domen));
    }
    return result;
}

vector<string> SplitOnSubDomains(string domains) {
    vector<string> result;
    for (auto pos = domains.find('.'); pos != domains.npos; pos = domains.find('.')) {
        result.push_back(domains);
        domains = domains.substr(pos + 1);      
    }
    if (domains.size() != 0) {
        result.push_back(domains);
    }
    return result;
}


bool IsGoodDomein(const unordered_set<string>& restricted_domains, const string& current_domein) {
    vector<string> sub_domains = SplitOnSubDomains(current_domein);

    for (auto& dom : sub_domains) {
        if (restricted_domains.count(dom)) {
            return false;
        }
    }
    return true;
}


void CheckAllDomensAndPrint(const unordered_set<string>& restricted_domeins, const vector<string>& all_domeins, ostream& out = cout) {
    for (const auto& current_domein : all_domeins) {
        if (IsGoodDomein(restricted_domeins, current_domein)) {
            out << "Good\n";
        }
        else {
            out << "Bad\n";
        }
    }
}

int main() {
    auto tmp = ReadDomeins();
    unordered_set<string> restricted_domeins(tmp.begin(), tmp.end());
    vector<string> all_domeins = ReadDomeins();
    CheckAllDomensAndPrint(restricted_domeins, all_domeins);
   
    return 0;
}