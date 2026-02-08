# Quantum Solver Workflow Validation

This folder (`validationCase_definitive_qc`) is configured to test the "True" Quantum Mode of the `teseLB_definitive` solver.

## How it works

The `teseLB_definitive` solver uses a **Hybrid Quantum-Classical** approach. The heavy implementation details are offloaded from C++ (OpenFOAM) to Python (Qiskit/SciPy).

### 1. Data Export (C++ Side)
In `teseLB_definitive/quantumSolve.H`:
- The solver intercepts the pressure equation (or other linear systems).
- Instead of solving it directly with OpenFOAM's generic solvers (PCG, PBiCG), it:
  1. Extracts the sparse matrix **A** (diagonals, upper, lower coefficients).
  2. Extracts the source vector **b**.
  3. Writes them to `A_matrix.dat` and `b_vector.dat` in the case directory.
  4. Calls the python script: `python3 ../teseLB/quantum_solver.py ...`

### 2. Quantum/Classical Solving (Python Side)
In `teseLB_definitive/quantum_solver.py`:
- **Reading**: It loads the matrix **A** and vector **b** from the text files.
- **Configuration**: It looks for `solver_settings.json` in the run directory.
  - If `"mode": "quantum"` is set, it activates the Qiskit branch.
  - If `"mode": "direct"` (or default), it uses classical SciPy solvers.
- **Quantum Execution (`USE_QISKIT = True`)**:
  - It constructs a Quantum Circuit or uses a Quantum Algorithm (Data Encoding).
  - **VQLS (Variational Quantum Linear Solver)**: Optimizes a parameterized quantum circuit to approximate the solution $|x\rangle$ such that $A|x\rangle = |b\rangle$.
  - **HHL (Harrow-Hassidim-Lloyd)**: (Experimental) Uses quantum phase estimation.
  - **NumPyLinearSolver**: A Qiskit class that validates the logic by calculating the *exact* quantum state vector classically (useful for debugging without quantum noise).
- **Fallback**: If Qiskit is missing or fails, it falls back to `scipy.sparse.linalg.spsolve`.

### 3. Data Import (C++ Side)
- The Python script writes the result to `solution.dat`.
- The C++ code reads this file and updates the field (e.g., Pressure, Temperature).
- The simulation continues to the next time step.

## How to Run This Case

1. **Install Prerequisites**:
   You need `qiskit` and `qiskit-algorithms` installed in your Python environment.
   ```bash
   pip install qiskit qiskit-algorithms scipy numpy
   ```

2. **Run the Simulation**:
   ```bash
   # Ensure you are in this directory
   cd validationCase_definitive_qc
   
   # Run the solver (ensure teseLB_definitive is compiled and in path)
   teseLB_definitive
   ```

3. **Verify Quantum Mode**:
   Check the output log. You should see messages like:
   ```
   [Quantum Interface] Intercepting Solver Call...
   ...
   Loaded config: Mode=QUANTUM, Backend=aer_simulator
   -> ATTEMPTING QUANTUM/HYBRID EXECUTION
   ```

## Configuration (`solver_settings.json`)

You can control the solver behavior without recompiling:

```json
{
    "mode": "quantum",               // Options: "quantum", "iterative", "direct"
    "backend": "aer_simulator",      // "aer_simulator", "vqls", or specific backend
    "use_qiskit": true               // Force enable Qiskit
}
```

## Performance Analysis

When deciding between Classical (`direct`) and Quantum (`quantum`) modes, execution time is critical.

**Important Note**: Running **Quantum Algorithms (VQLS/HHL)** on a classical computer (via Simulator) is **exponentially slower** than running classical algorithms (like LU Decomposition or CG).
- **Classical Mode**: Uses highly optimized matrix libraries (LAPACK/BLAS). Fast.
- **Quantum Mode (Simulated)**: Computes the full state vector of a quantum system ($2^N$ complex numbers). Extremely slow for large matrices.
- **Quantum Mode (Real Hardware)**: Limited by noise and access time, but theoretically faster for specific immense problems (not yet realized for CFD scales).

### Measuring Performance
The solver now automatically logs execution time for every linear system solved.
Check the file `solver_performance.csv` generated in your case directory:

```csv
Timestamp,Mode,MatrixSize,TimeSeconds
2026-02-08T16:00:01,Classic,500,0.002
2026-02-08T16:00:05,QuantumSim,500,4.521
```

To decide if it's "better", compare the `TimeSeconds` column. You will likely see that Quantum Mode is significantly slower for now, which is expected for research/validation.
