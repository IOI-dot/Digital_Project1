#include <iostream>
#include <bits/stdc++.h>
using namespace std;

struct Implicant {
    string bits;            // e.g. "1-0"
    vector<int> covers;     // list of terms (minterm or don't-care) covered
    bool used = false;
};

static string varName(int idx) {
    if (idx < 26) return string(1, 'A' + idx);
    return "V" + to_string(idx);
}

bool combine(const Implicant &a, const Implicant &b, Implicant &result) {
    int diff = 0;
    string newBits = a.bits;
    for (int i = 0; i < (int)a.bits.size(); ++i) {
        if (a.bits[i] != b.bits[i]) {
            if (a.bits[i] != '-' && b.bits[i] != '-') {
                diff++;
                newBits[i] = '-';
            } else {
                return false;
            }
        }
    }
    if (diff == 1) {
        result.bits = newBits;
        result.covers = a.covers;
        result.covers.insert(result.covers.end(), b.covers.begin(), b.covers.end());
        sort(result.covers.begin(), result.covers.end());
        result.covers.erase(unique(result.covers.begin(), result.covers.end()), result.covers.end());
        result.used = false;
        return true;
    }
    return false;
}

bool existsBits(const vector<Implicant>& list, const string &bits) {
    for (const auto &x : list)
        if (x.bits == bits) return true;
    return false;
}

string implicantToExpr(const Implicant &p, int vars, bool isMaxterm) {
    // Return textual boolean expression for this implicant.
    // For SOP (isMaxterm==false): 0 -> A', 1 -> A
    // For POS (isMaxterm==true): 0 -> A, 1 -> A'
    string out;
    if (isMaxterm) {
        // POS: combine with + inside parentheses
        vector<string> terms;
        for (int k = 0; k < vars; ++k) {
            if (p.bits[k] == '-') continue;
            string v = varName(k);
            if (p.bits[k] == '0') terms.push_back(v);
            else terms.push_back(v + "'");
        }
        if (terms.empty()) return "(1)"; // tautology
        out += "(";
        for (int i = 0; i < (int)terms.size(); ++i) {
            out += terms[i];
            if (i + 1 < (int)terms.size()) out += " + ";
        }
        out += ")";
        return out;
    } else {
        // SOP: combine with concatenation and ' + ' between implicants by caller
        for (int k = 0; k < vars; ++k) {
            if (p.bits[k] == '-') continue;
            string v = varName(k);
            if (p.bits[k] == '0') out += v + "'";
            else out += v;
        }
        if (out.empty()) return "1"; // constant 1
        return out;
    }
}

string implicantToVerilogTerm(const Implicant &p, int vars, bool isMaxterm) {
    // For Verilog produce (A & ~B & C) for SOP
    // For POS, produce (~A | B | ...) and combine with & between parentheses
    if (!isMaxterm) {
        vector<string> lits;
        for (int k = 0; k < vars; ++k) {
            if (p.bits[k] == '-') continue;
            string v = varName(k);
            if (p.bits[k] == '0') lits.push_back("~" + v);
            else lits.push_back(v);
        }
        if (lits.empty()) return "1'b1";
        string s = "(";
        for (int i = 0; i < (int)lits.size(); ++i) {
            s += lits[i];
            if (i + 1 < (int)lits.size()) s += " & ";
        }
        s += ")";
        return s;
    } else {
        // POS clause like (A + B' + ...) in boolean becomes (A | ~B | ...)
        vector<string> lits;
        for (int k = 0; k < vars; ++k) {
            if (p.bits[k] == '-') continue;
            string v = varName(k);
            // earlier mapping: in POS we used 0->var, 1->var'
            if (p.bits[k] == '0') lits.push_back(v);
            else lits.push_back("~" + v);
        }
        if (lits.empty()) return "1'b1";
        string s = "(";
        for (int i = 0; i < (int)lits.size(); ++i) {
            s += lits[i];
            if (i + 1 < (int)lits.size()) s += " | ";
        }
        s += ")";
        return s;
    }
}

int literalCount(const Implicant &p) {
    // number of non-dash bits used in expression — used to prefer simpler implicants
    int c = 0;
    for (char ch : p.bits) if (ch != '-') ++c;
    return c;
}

int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string filename = (argc > 1) ? argv[1] : "Test6.txt";
    ifstream fin(filename);
    if (!fin) {
        cerr << "Cannot open '" << filename << "'\n";
        return 1;
    }

    string line, token;
    int vars = 0;
    vector<int> terms_raw, dontcares_raw;
    bool isMaxterm = false;

    // Read number of variables
    if (!getline(fin, line)) { cerr << "Invalid file format (missing vars)\n"; return 1; }
    try {
        line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
        vars = stoi(line);
    } catch (...) {
        cerr << "Invalid number of variables.\n"; return 1;
    }
    if (vars <= 0 || vars > 20) { cerr << "Number of variables must be 1..20.\n"; return 1; }

    // Read minterms / maxterms line
    if (!getline(fin, line)) { cerr << "Missing minterm/maxterm line\n"; return 1; }
    {
        stringstream ss(line);
        while (getline(ss, token, ',')) {
            // trim
            token.erase(remove_if(token.begin(), token.end(), ::isspace), token.end());
            if (token.empty()) continue;
            if (token[0] == 'M') isMaxterm = true;
            if (token[0] == 'm' || token[0] == 'M') token = token.substr(1);
            try {
                int v = stoi(token);
                terms_raw.push_back(v);
            } catch (...) {
                cerr << "Invalid term: " << token << "\n"; return 1;
            }
        }
    }

    // Read don't-cares line (optional)
    if (getline(fin, line)) {
        stringstream ss(line);
        while (getline(ss, token, ',')) {
            token.erase(remove_if(token.begin(), token.end(), ::isspace), token.end());
            if (token.empty()) continue;
            if (token[0] == 'd' || token[0] == 'D') token = token.substr(1);
            try {
                int v = stoi(token);
                dontcares_raw.push_back(v);
            } catch (...) {
                cerr << "Invalid don't-care: " << token << "\n"; return 1;
            }
        }
    }

    // Basic validation: values within range
    int Nmax = (1 << vars);
    for (int v : terms_raw) {
        if (v < 0 || v >= Nmax) { cerr << "Term out of range [0.." << Nmax-1 << "]: " << v << "\n"; return 1; }
    }
    for (int v : dontcares_raw) {
        if (v < 0 || v >= Nmax) { cerr << "Don't-care out of range [0.." << Nmax-1 << "]: " << v << "\n"; return 1; }
    }

    // check for overlap between minterms and don't cares (that's invalid)
    for (int t : terms_raw) {
        if (find(dontcares_raw.begin(), dontcares_raw.end(), t) != dontcares_raw.end()) {
            cerr << "Term " << t << " listed both as term and don't-care. Remove ambiguity.\n";
            return 1;
        }
    }

    if (terms_raw.empty()) { cerr << "No terms provided.\n"; return 1; }

    // If terms were maxterms, convert to minterms
    vector<int> minterm_list;
    if (isMaxterm) {
        // all numbers 0..2^n-1 not in terms_raw are minterms
        vector<char> isExcluded(Nmax, 0);
        for (int t : terms_raw) isExcluded[t] = 1;
        for (int i = 0; i < Nmax; ++i) if (!isExcluded[i]) minterm_list.push_back(i);
    } else {
        minterm_list = terms_raw;
    }

    // Combine minterms + don't-cares into working terms for PI generation
    vector<int> all_terms = minterm_list;
    all_terms.insert(all_terms.end(), dontcares_raw.begin(), dontcares_raw.end());
    sort(all_terms.begin(), all_terms.end());
    all_terms.erase(unique(all_terms.begin(), all_terms.end()), all_terms.end());

    // Build initial implicants (binary strings)
    vector<Implicant> current;
    for (int t : all_terms) {
        Implicant imp;
        string b;
        for (int i = vars - 1; i >= 0; --i) b.push_back( ((t >> i) & 1) ? '1' : '0' );
        imp.bits = b;
        imp.covers = {t};
        imp.used = false;
        current.push_back(imp);
    }

    // Quine-McCluskey combination loop to find prime implicants
    vector<Implicant> primes;
    bool again = true;
    while (again) {
        again = false;
        vector<Implicant> next;
        for (size_t i = 0; i < current.size(); ++i) {
            for (size_t j = i + 1; j < current.size(); ++j) {
                Implicant comb;
                if (combine(current[i], current[j], comb)) {
                    current[i].used = current[j].used = true;
                    if (!existsBits(next, comb.bits)) next.push_back(comb);
                    again = true;
                }
            }
        }
        // those not used are prime implicants
        for (auto &x : current) {
            if (!x.used && !existsBits(primes, x.bits)) primes.push_back(x);
        }
        current = next;
    }

    // Print prime implicants with their covered terms
    cout << "=== Prime Implicants ===\n";
    for (size_t i = 0; i < primes.size(); ++i) {
        cout << i << ": " << primes[i].bits << " covers { ";
        for (int v : primes[i].covers) cout << v << " ";
        cout << "}\n";
    }

    // Build prime implicant table: rows = primes, cols = minterms (only actual minterms, not don't-cares)
    vector<int> minterms = minterm_list; // these must be covered
    sort(minterms.begin(), minterms.end());
    cout << "\nMinterms to cover: ";
    for (int m : minterms) cout << m << " ";
    cout << "\n";

    if (minterms.empty()) {
        cout << "No minterms to cover (all terms were don't-cares). Function = 0 or 1 depending on spec.\n";
        return 0;
    }

    int R = (int)primes.size();
    int C = (int)minterms.size();
    vector<vector<int>> table(R, vector<int>(C, 0));
    for (int i = 0; i < R; ++i) {
        for (int j = 0; j < C; ++j) {
            int m = minterms[j];
            bool ok = true;
            for (int k = 0; k < vars; ++k) {
                if (primes[i].bits[k] == '-') continue;
                int bit = (m >> (vars - 1 - k)) & 1;
                if (bit != (primes[i].bits[k] - '0')) { ok = false; break; }
            }
            if (ok) table[i][j] = 1;
        }
    }

    // Find essential prime implicants
    vector<int> essential_indices;
    vector<int> covered(C, 0);
    for (int j = 0; j < C; ++j) {
        int count = 0, row = -1;
        for (int i = 0; i < R; ++i) if (table[i][j]) { ++count; row = i; }
        if (count == 1 && row != -1) {
            if (find(essential_indices.begin(), essential_indices.end(), row) == essential_indices.end()) {
                essential_indices.push_back(row);
                // mark covered columns by this essential implicant
                for (int k = 0; k < C; ++k) if (table[row][k]) covered[k] = 1;
            }
        }
    }

    cout << "\n=== Essential Prime Implicants ===\n";
    if (essential_indices.empty()) cout << "None\n";
    for (int idx : essential_indices) {
        cout << idx << ": " << primes[idx].bits << " -> " << implicantToExpr(primes[idx], vars, isMaxterm) << "\n";
    }

    // Print minterms not covered by essentials
    vector<int> uncovered_minterm_indices; // indices into minterms list
    cout << "\nMinterms not covered by essential PIs: ";
    for (int j = 0; j < C; ++j) {
        if (!covered[j]) {
            cout << minterms[j] << " ";
            uncovered_minterm_indices.push_back(j);
        }
    }
    if (uncovered_minterm_indices.empty()) cout << "(none)";
    cout << "\n";

    // If no uncovered minterms, final solution is just essentials
    vector<vector<int>> final_solutions; // each solution: list of implicant indices (including essentials)
    if (uncovered_minterm_indices.empty()) {
        // single solution: just the essential indices
        final_solutions.push_back(essential_indices);
    } else {
        // Build Petrick's method terms:
        // For each uncovered minterm j, create list of implicant indices i that cover it
        vector<vector<int>> petrick_lists;
        for (int idx : uncovered_minterm_indices) {
            vector<int> coverers;
            for (int i = 0; i < R; ++i) if (table[i][idx]) coverers.push_back(i);
            if (coverers.empty()) {
                cerr << "ERROR: no implicant covers minterm " << minterms[idx] << " (should not happen)\n";
                return 1;
            }
            petrick_lists.push_back(coverers);
        }

        // Multiply sums: compute all combinations (sets) that pick one implicant per uncovered minterm
        // Use sets to avoid duplicates. Start with singletons from first list.
        vector< set<int> > solutions;
        // initialize
        for (int i : petrick_lists[0]) {
            solutions.push_back(set<int>({i}));
        }
        // iterate multiplication
        for (size_t p = 1; p < petrick_lists.size(); ++p) {
            vector< set<int> > newsol;
            for (auto &s : solutions) {
                for (int i : petrick_lists[p]) {
                    set<int> s2 = s;
                    s2.insert(i);
                    newsol.push_back(s2);
                }
            }
            // reduce: remove supersets (if there exists a set that is subset of another, drop the superset)
            // We'll reduce greedily: sort by size then remove supersets
            // move newsol to solutions after reduction
            // remove duplicates first
            sort(newsol.begin(), newsol.end(), [](const set<int>& a, const set<int>& b){
                if (a.size() != b.size()) return a.size() < b.size();
                return a < b;
            });
            vector< set<int> > reduced;
            for (auto &s : newsol) {
                bool keep = true;
                for (auto &r : reduced) {
                    // if r subset of s, then s is superset -> don't keep s
                    if (includes(s.begin(), s.end(), r.begin(), r.end())) { keep = false; break; }
                }
                if (keep) reduced.push_back(s);
            }
            // deduplicate identical sets
            solutions = reduced;
        }

        // solutions now contains sets of implicant indices that cover all uncovered minterms
        // Now, combine with essential_indices to produce full solutions
        // But we should prefer minimal sets by number of implicants, then by literal count
        // First, remove any solution where an implicant is already included in essentials (not necessary but ok)
        for (auto &s : solutions) {
            for (int e : essential_indices) s.insert(e);
        }

        // Now reduce solutions to minimal cardinality
        size_t bestSize = SIZE_MAX;
        for (auto &s : solutions) bestSize = min(bestSize, s.size());
        vector< set<int> > bestBySize;
        for (auto &s : solutions) if (s.size() == bestSize) bestBySize.push_back(s);

        // If multiple with same size, choose those with minimal literal count (sum of literalCount for implicants)
        long bestLit = LONG_MAX;
        vector< set<int> > bestFinal;
        for (auto &s : bestBySize) {
            long lit = 0;
            for (int idx : s) lit += literalCount(primes[idx]);
            if (lit < bestLit) {
                bestLit = lit;
                bestFinal.clear();
                bestFinal.push_back(s);
            } else if (lit == bestLit) bestFinal.push_back(s);
        }

        // Convert set<int> to vector<int> and store
        for (auto &s : bestFinal) {
            vector<int> sol;
            for (int idx : s) sol.push_back(idx);
            sort(sol.begin(), sol.end());
            final_solutions.push_back(sol);
        }
    }

    // Print final minimized expressions (all minimal solutions)
    cout << "\n=== Minimized Expression(s) (" << (isMaxterm ? "POS" : "SOP") << ") ===\n";
    for (size_t si = 0; si < final_solutions.size(); ++si) {
        cout << "Solution " << (si+1) << ": ";
        auto &sol = final_solutions[si];

        // Build expression string
        if (isMaxterm) {
            // POS: product of sum clauses (each implicant becomes a sum clause)
            for (size_t k = 0; k < sol.size(); ++k) {
                cout << implicantToExpr(primes[sol[k]], vars, true);
                if (k + 1 < sol.size()) cout << " ";
            }
            cout << "\n";
        } else {
            // SOP: sum of products
            for (size_t k = 0; k < sol.size(); ++k) {
                cout << implicantToExpr(primes[sol[k]], vars, false);
                if (k + 1 < sol.size()) cout << " + ";
            }
            cout << "\n";
        }
    }

    // If multiple solutions exist, display count
    if (final_solutions.size() > 1) {
        cout << "\nMultiple minimal solutions found: " << final_solutions.size() << "\n";
    }

    // BONUS: Generate Verilog module(s) for each solution
    // For each solution, produce module f_solutionN with inputs A.. and output F
    cout << "\n=== Verilog Generation ===\n";
    for (size_t si = 0; si < final_solutions.size(); ++si) {
        string modname = "f_solution" + to_string(si+1);
        string fname = modname + ".v";
        ofstream fout(fname);
        if (!fout) {
            cerr << "Cannot write Verilog file " << fname << "\n";
            continue;
        }
        fout << "// Auto-generated Verilog by qm_minimizer (solution " << (si+1) << ")\n";
        fout << "module " << modname << "(";
        // ports
        for (int k = 0; k < vars; ++k) {
            fout << varName(k) << ", ";
        }
        fout << "F);\n";
        fout << "  input ";
        for (int k = 0; k < vars; ++k) {
            fout << varName(k);
            if (k + 1 < vars) fout << ", ";
        }
        fout << ";\n";
        fout << "  output F;\n";
        fout << "  wire __terms[" << (int)final_solutions[si].size() - 1 << ":0];\n";

        // Build expression
        vector<string> termVerilog;
        for (int idx : final_solutions[si]) {
            termVerilog.push_back(implicantToVerilogTerm(primes[idx], vars, isMaxterm));
        }
        string assignLine;
        if (!isMaxterm) {
            // SOP: OR of termVerilog
            if (termVerilog.empty()) assignLine = "assign F = 1'b0; // no terms";
            else {
                assignLine = "assign F = ";
                for (size_t t = 0; t < termVerilog.size(); ++t) {
                    assignLine += termVerilog[t];
                    if (t + 1 < termVerilog.size()) assignLine += " | ";
                }
                assignLine += ";\n";
            }
        } else {
            // POS: AND of clauses
            if (termVerilog.empty()) assignLine = "assign F = 1'b1; // no clauses";
            else {
                assignLine = "assign F = ";
                for (size_t t = 0; t < termVerilog.size(); ++t) {
                    assignLine += termVerilog[t];
                    if (t + 1 < termVerilog.size()) assignLine += " & ";
                }
                assignLine += ";\n";
            }
        }
        fout << "  " << assignLine;
        fout << "endmodule\n";
        fout.close();
        cout << "Wrote " << fname << " (module " << modname << ")\n";
    }

    cout << "\nDone.\n";
    return 0;
}
