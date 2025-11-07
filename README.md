🧮 Quine-McCluskey Logic Minimizer
Milestone 3 – Semi-Functional Implementation

👥 Authors:
- [Your Name]
- [Partner Name(s)]
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
✅ Reads Boolean functions from a text file
✅ Supports minterms (m) and maxterms (M)
✅ Supports don’t-care terms (d)
✅ Generates and displays all Prime Implicants (PIs)
✅ Identifies and prints Essential Prime Implicants (EPIs)
✅ Outputs the minimized Boolean expression (SOP or POS)

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
g++ main.cpp -o qm.exe

------------------------------------------------------------
🚀 How to Run
------------------------------------------------------------
Option 1: From your IDE
- Run the program; it will read `Test1.txt` by default.

Option 2: From the terminal or PowerShell
- Place your input file (e.g., Test1.txt) in the same directory as qm.exe
- Run:
  .\qm.exe Test1.txt

If no filename is provided, the program defaults to:
  .\qm.exe
  (and looks for “Test1.txt”)

------------------------------------------------------------
📜 Example Input
------------------------------------------------------------
5
m0,m1,m2,m3,m8,m9,m10,m11,m24,m25,m26,m27
d4,d12,d20,d28

------------------------------------------------------------
📤 Example Output
------------------------------------------------------------
Prime Implicants:
00- covers { 0 1 }
11- covers { 6 7 }
--1 covers { 1 3 5 7 }

Essential Prime Implicants:
--1

Minimized Expression (SOP):
C + AB

------------------------------------------------------------
ℹ️ Notes
------------------------------------------------------------
- For maxterms (POS), the program automatically inverts logic.
- Don’t-care terms are included during grouping but ignored in the final expression.
- Make sure the input follows the exact format.
- Works for Boolean functions with up to 20 variables.
