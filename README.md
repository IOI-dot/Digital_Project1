🧮 Quine-McCluskey Logic Minimizer
👥 By:
- Omar Abdel Motalb
- Seif eldin Ramy Mostafa
- Ali Ahmed Alkholy
Date: November 2025

------------------------------------------------------------
📘 Objective
------------------------------------------------------------
This program implements the Quine–McCluskey Logic Minimization Algorithm to simplify Boolean functions expressed as:
- Sum of Products (SOP) using minterms (m)
- Product of Sums (POS) using maxterms (M)

It supports “don’t care” terms (d) and handles Boolean functions up to 20 variables.

------------------------------------------------------------
🖥️ Features Implemented
------------------------------------------------------------
 Reads Boolean functions from a text file

 Supports minterms (m) and maxterms (M)

 Supports don’t-care terms (d)

 Generates and displays all Prime Implicants (PIs)

 Identifies and prints Essential Prime Implicants (EPIs)

 Outputs the minimized Boolean expression (SOP or POS)

 Generates Verilog modules for each minimal solution

------------------------------------------------------------
📂 Input File Format
------------------------------------------------------------
Each input file has 3 lines:
1. Number of variables
2. Minterms (m) or Maxterms (M), separated by commas
3. Don’t-care terms (d), separated by commas

Examples:

For SOP:
3
m1,m3,m6,m7
d0,d5

For POS:
3
M0,M2,M4,M5
d1,d3

------------------------------------------------------------
⚙️ How to Build
------------------------------------------------------------
1. Open the project folder in CLion, Code::Blocks, or any C++ IDE.
2. Make sure your source file (e.g. main.cpp) is added to the project.
3. Build the program (it will generate `qm.exe` or similar executable).

Or using terminal:
After downloading the zip file from the git repository, compile the program with g++ main.cpp q_m.cpp -o qm_minimizer on the terminal and prepare a text file specifying the number of variables, minterms/maxterms, and optional don’t-cares in three lines separate or use the built in test cases. Run with ./qm_minimizer (uses default file) or ./qm_minimizer <filename>. The program outputs minimized expressions and generates corresponding Verilog modules.

------------------------------------------------------------
🚀 How to Run
------------------------------------------------------------
Option 1: From your IDE
- Run the program; it will read `Test6.txt` by default.

Option 2: From the terminal or PowerShell
- Place your input file in the same directory as qm.exe
- Run:
  .\qm.exe (YOUR TEST FILE).txt

If no filename is provided, the program defaults to:
  .\qm.exe
  (and looks for “Test1.txt”)

------------------------------------------------------------
📜 Example Input
------------------------------------------------------------
3
m1,m3,m6,m7
d0,d5

------------------------------------------------------------
📤 Example Output
------------------------------------------------------------
Prime Implicants:
0: 00- covers { 0 1 }
1: 11- covers { 6 7 }
2: --1 covers { 1 3 5 7 }

Minterms to cover: 1 3 6 7

Essential Prime Implicants:
2: --1 -> C
1: 11- -> AB

Minterms not covered by essential PIs: (none)

Minimized Expression(s) (SOP):
Solution 1: C + AB

Verilog Generation:
Generated f_solution1.v

Process finished with exit code 0

------------------------------------------------------------
ℹ️ Notes
------------------------------------------------------------
- For maxterms (POS), the program automatically inverts logic.
- Don’t-care terms are included during grouping but ignored in the final expression.
- Make sure the input follows the exact format.
- Works for Boolean functions with up to 20 variables.
- Generates ready to be used Verilog files for Hardware Implementation.
