#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
using namespace std;

struct Implicant {
    string bits;
    vector<int> covers;
    bool used = false;
};

bool combine(const Implicant &a, const Implicant &b, Implicant &result) {
    int diff = 0;
    string newBits = a.bits;
    for (int i = 0; i < (int)a.bits.size(); i++) {
        if (a.bits[i] != b.bits[i]) {
            diff++;
            newBits[i] = '-';
        }
    }
    if (diff == 1) {
        result.bits = newBits;
        result.covers = a.covers;
        result.covers.insert(result.covers.end(), b.covers.begin(), b.covers.end());
        sort(result.covers.begin(), result.covers.end());
        result.covers.erase(unique(result.covers.begin(), result.covers.end()), result.covers.end());
        return true;
    }
    return false;
}

bool contains(vector<Implicant> &list, const Implicant &imp) {
    for (auto &x : list)
        if (x.bits == imp.bits) return true;
    return false;
}

int main(int argc, char* argv[]) {
    string filename = (argc > 1) ? argv[1] : "Test4.txt";
    ifstream fin(filename);
    if (!fin) {
        cout << "Cannot open " << filename << "\n";
        return 1;
    }

    string line, token;
    int vars = 0;
    vector<int> terms, dontcares;
    bool isMaxterm = false;

    // read file
    if (!getline(fin, line)) { cout << "Invalid file format\n"; return 1; }
    vars = stoi(line);

    if (!getline(fin, line)) { cout << "Missing minterm/maxterm line\n"; return 1; }
    {
        stringstream ss(line);
        while (getline(ss, token, ',')) {
            token.erase(remove_if(token.begin(), token.end(), ::isspace), token.end());
            if (token.empty()) continue;
            if (token[0] == 'M') isMaxterm = true;
            if (token[0] == 'm' || token[0] == 'M') token = token.substr(1);
            terms.push_back(stoi(token));
        }
    }

    if (getline(fin, line)) {
        stringstream ss(line);
        while (getline(ss, token, ',')) {
            token.erase(remove_if(token.begin(), token.end(), ::isspace), token.end());
            if (token.empty()) continue;
            if (token[0] == 'd' || token[0] == 'D') token = token.substr(1);
            dontcares.push_back(stoi(token));
        }
    }

    if (terms.empty()) {
        cout << "No terms provided.\n";
        return 1;
    }

    //Convert maxterms to minterms
    if (isMaxterm) {
        vector<int> all;
        for (int i = 0; i < (1 << vars); i++) all.push_back(i);
        vector<int> minterms;
        for (int x : all)
            if (find(terms.begin(), terms.end(), x) == terms.end())
                minterms.push_back(x);
        terms = minterms;
    }

    // combine minterms + don't cares
    for (int d : dontcares) terms.push_back(d);
    sort(terms.begin(), terms.end());

    // convert to binary implicants
    vector<Implicant> current;
    for (int t : terms) {
        Implicant imp;
        string b = "";
        for (int i = vars - 1; i >= 0; i--)
            b += ((t >> i) & 1) ? '1' : '0';
        imp.bits = b;
        imp.covers = {t};
        current.push_back(imp);
    }

    // find prime implicants
    vector<Implicant> primes;
    bool again = true;

    while (again) {
        again = false;
        vector<Implicant> next;

        for (int i = 0; i < (int)current.size(); i++) {
            for (int j = i + 1; j < (int)current.size(); j++) {
                Implicant comb;
                if (combine(current[i], current[j], comb)) {
                    current[i].used = current[j].used = true;
                    if (!contains(next, comb)) next.push_back(comb);
                    again = true;
                }
            }
        }
        for (auto &x : current)
            if (!x.used && !contains(primes, x))
                primes.push_back(x);
        current = next;
    }

    cout << "Prime Implicants:\n";
    for (auto &p : primes) {
        cout << p.bits << " covers { ";
        for (int v : p.covers) cout << v << " ";
        cout << "}\n";
    }

    // build prime implicant table
    vector<int> minterms;
    for (int t : terms)
        if (find(dontcares.begin(), dontcares.end(), t) == dontcares.end())
            minterms.push_back(t);

    vector<vector<int>> table(primes.size(), vector<int>(minterms.size(), 0));
    for (int i = 0; i < (int)primes.size(); i++) {
        for (int j = 0; j < (int)minterms.size(); j++) {
            int m = minterms[j];
            bool ok = true;
            for (int k = 0; k < vars; k++) {
                if (primes[i].bits[k] == '-') continue;
                int bit = (m >> (vars - 1 - k)) & 1;
                if (bit != (primes[i].bits[k] - '0')) { ok = false; break; }
            }
            if (ok) table[i][j] = 1;
        }
    }

    // find essential prime implicants
    vector<int> essential;
    vector<int> covered(minterms.size(), 0);
    for (int j = 0; j < (int)minterms.size(); j++) {
        int count = 0, row = -1;
        for (int i = 0; i < (int)primes.size(); i++) {
            if (table[i][j]) { count++; row = i; }
        }
        if (count == 1) {
            if (find(essential.begin(), essential.end(), row) == essential.end())
                essential.push_back(row);
            for (int k = 0; k < (int)minterms.size(); k++)
                if (table[row][k]) covered[k] = 1;
        }
    }

    cout << "\nEssential Prime Implicants:\n";
    for (int idx : essential) cout << primes[idx].bits << "\n";

    // minimized expression
    cout << "\nMinimized Expression (" << (isMaxterm ? "POS" : "SOP") << "):\n";
    for (int i = 0; i < (int)essential.size(); i++) {
        string expr = "";
        for (int k = 0; k < vars; k++) {
            char var = 'A' + k;
            if (primes[essential[i]].bits[k] == '-') continue;
            if (isMaxterm) {
                // for POS: 0 -> var, 1 -> var'
                if (primes[essential[i]].bits[k] == '0') expr += var;
                else expr += string(1, var) + "'";
                if (k != vars - 1) expr += " + ";
            } else {
                // for SOP: 0 -> var', 1 -> var
                if (primes[essential[i]].bits[k] == '0') expr += string(1, var) + "'";
                else expr += var;
            }
        }
        if (isMaxterm) cout << "(" << expr << ")";
        else cout << expr;
        if (i != (int)essential.size() - 1) cout << (isMaxterm ? " " : " + ");
    }
    cout << "\n";
}
