#ifndef Q_M_H
#define Q_M_H

#include <iostream>
#include <bits/stdc++.h>
using namespace std;

struct Implicant {
    string bits;            // e.g. "1-0"
    vector<int> covers;     // list of terms (minterm or don't-care) covered
    bool used = false;
};

string varName(int idx);
bool combine(const Implicant &a, const Implicant &b, Implicant &result);
bool existsBits(const vector<Implicant>& list, const string &bits);
string implicantToExpr(const Implicant &p, int vars, bool isMaxterm);
string implicantToVerilogTerm(const Implicant &p, int vars, bool isMaxterm);
int literalCount(const Implicant &p);

// main minimization driver (previously main)
int qm_minimize(const string &filename);

#endif
