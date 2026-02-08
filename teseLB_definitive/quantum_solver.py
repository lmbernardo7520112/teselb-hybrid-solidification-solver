
import numpy as np
import scipy.sparse as sp
import sys
import os

# Configuration
USE_QISKIT = False # Set to True to enable actual Quantum/Hybrid execution

def read_openfoam_matrix(matrix_path, source_path):
    """
    Reads OpenFOAM lduMatrix dump and source vector.
    Format is custom COO-like text files.
    """
    print(f"Reading matrix from {matrix_path}...")
    
    # Read Matrix info
    # Format expected: row col value
    rows = []
    cols = []
    vals = []
    
    with open(matrix_path, 'r') as f:
        lines = f.readlines()
        
    for line in lines:
        parts = line.strip().split()
        if len(parts) != 3: continue
        r, c, v = int(parts[0]), int(parts[1]), float(parts[2])
        rows.append(r)
        cols.append(c)
        vals.append(v)
        
    if not rows:
        raise ValueError("Matrix file is empty!")

    n_unknowns = max(max(rows), max(cols)) + 1
    A = sp.coo_matrix((vals, (rows, cols)), shape=(n_unknowns, n_unknowns))
    
    print(f"Matrix shape: {A.shape}, Non-zeros: {A.nnz}")
    
    # Read Source vector b
    b = []
    idx_b = []
    with open(source_path, 'r') as f:
        lines = f.readlines()
        
    for line in lines:
        parts = line.strip().split()
        if len(parts) != 2: continue
        idx_b.append(int(parts[0]))
        b.append(float(parts[1]))
        
    # Sort b by index just in case
    b_arr = np.zeros(n_unknowns)
    for i, val in zip(idx_b, b):
        b_arr[i] = val
        
    return A, b_arr

def solve_quantum(A, b):
    """
    Solves Ax = b. Can switch between Classical (Direct/Iterative) and Quantum (Simulated/Real).
    """
    print("Initializing Solver Interface...")
    
    # Normalization (Quantum Requirement)
    norm_b = np.linalg.norm(b)
    if norm_b > 1e-12:
        b_norm = b / norm_b
    else:
        b_norm = b
        
    # Check for config file in current directory
    config_file = "solver_settings.json"
    use_iterative = False
    use_quantum = False
    tolerance = 1e-8
    quantum_backend = "aer_simulator"
    
    import json
    if os.path.exists(config_file):
        try:
            with open(config_file, 'r') as f:
                config = json.load(f)
                mode = config.get("mode", "direct").lower()
                
                if mode == "iterative":
                    use_iterative = True
                    tolerance = config.get("tolerance", 1e-8)
                    print(f"Loaded config: Mode=ITERATIVE, Tol={tolerance}")
                elif mode == "quantum" or config.get("use_qiskit", False):
                    use_quantum = True
                    quantum_backend = config.get("backend", "aer_simulator")
                    print(f"Loaded config: Mode=QUANTUM, Backend={quantum_backend}")
                else:
                    print(f"Loaded config: Mode=DIRECT (default)")
                    
                    
        except Exception as e:
            print(f"Error reading config: {e}")

    # Logic: Global Flag OR Config Flag
    ACTIVATE_QUANTUM = USE_QISKIT or use_quantum

    if ACTIVATE_QUANTUM:
        print(f"-> ATTEMPTING QUANTUM/HYBRID EXECUTION (Backend: {quantum_backend})")
        try:
            # Import Qiskit components only if needed
            # This is wrapped in try-except to avoid crashing if not installed
            from qiskit import Aer
            # Note: For newer Qiskit versions, algorithms moved to qiskit_algorithms
            # We try both for compatibility
            try:
                from qiskit.algorithms.linear_solvers import VQLS, NumPyLinearSolver
                from qiskit.algorithms.optimizers import COBYLA
            except ImportError:
                 from qiskit_algorithms.linear_solvers import VQLS, NumPyLinearSolver
                 from qiskit_algorithms.optimizers import COBYLA
                 
            try:
                 from qiskit.primitives import Estimator
            except ImportError:
                 # Older qiskit
                 from qiskit.primitives import Estimator

            
            if "vqls" in quantum_backend.lower():
                print("   -> Using VQLS (Variational Quantum Linear Solver)")
                # Minimal VQLS Setup
                optimizer = COBYLA(maxiter=100, disp=True)
                estimator = Estimator()
                # VQLS setup might differ by version, using generic call
                solver = VQLS(
                    estimator=estimator,
                    optimizer=optimizer
                )
                solution = solver.solve(A, b_norm)
                x = solution.state
            else:
                # Default to classical simulation of quantum linear solver
                # This verifies the matrix/circuit mapping logic
                print("   -> Using NumPyLinearSolver (Classical Sim of Quantum)")
                solver = NumPyLinearSolver() 
                solution = solver.solve(A, b_norm)
                x = solution.state
            
        except ImportError as ie:
            print(f"!!! Qiskit ImportError: {ie}")
            print("!!! To run in Quantum mode, install qiskit: pip install qiskit qiskit-algorithms")
            print("!!! Fallback to Classical Direct Solver.")
            x = sp.linalg.spsolve(A.tocsr(), b_norm)
        except Exception as e:
            print(f"!!! QUANTUM EXECUTION FAILED: {e}")
            # import traceback
            # traceback.print_exc()
            print("!!! Fallback to Classical Direct Solver.")
            x = sp.linalg.spsolve(A.tocsr(), b_norm)
    else:
        if use_iterative:
            print(f"Running in Classical ITERATIVE Mode (BiCGSTAB, tol={tolerance})")
            # Use Conjugate Gradient or BiCGSTAB
            x, exit_code = sp.linalg.bicgstab(A, b_norm, tol=tolerance, atol=tolerance)
            if exit_code != 0:
                 print(f"Warning: Iterative solver did not converge (Exit {exit_code})")
        else:
            print("Running in Classical DIRECT Mode (LU / Spsolve)")
            # Use SciPy sparse solver to mimic the "black box" solver
            x = sp.linalg.spsolve(A.tocsr(), b_norm)
        
    # Denormalize
    if norm_b > 1e-12:
        x = x * norm_b
        
    return x

def main():
    import time
    import datetime

    if len(sys.argv) < 3:
        print("Usage: python quantum_solver.py <A_file> <b_file> [out_file]")
        sys.exit(1)
        
    mat_file = sys.argv[1]
    rhs_file = sys.argv[2]
    out_file = "solution.dat"
    if len(sys.argv) > 3:
        out_file = sys.argv[3]
        
    try:
        A, b = read_openfoam_matrix(mat_file, rhs_file)
        
        start_time = time.time()
        x = solve_quantum(A, b)
        end_time = time.time()
        
        elapsed = end_time - start_time
        
        print(f"Solution calculated. Norm: {np.linalg.norm(x)}")
        print(f"--> SOLVER TIME: {elapsed:.6f} seconds")
        
        # Log Performance
        log_file = "solver_performance.csv"
        timestamp = datetime.datetime.now().isoformat()
        
        # Determine mode roughly
        mode_guess = "Classic"
        if os.path.exists("solver_settings.json"):
             with open("solver_settings.json") as f:
                 content = f.read().lower()
                 if "quantum" in content and "true" in content: 
                     mode_guess = "QuantumSim"
                 elif "iterative" in content:
                     mode_guess = "Iterative"
        
        if not os.path.exists(log_file):
            with open(log_file, 'w') as f:
                f.write("Timestamp,Mode,MatrixSize,TimeSeconds\n")
        
        with open(log_file, 'a') as f:
            f.write(f"{timestamp},{mode_guess},{A.shape[0]},{elapsed:.6f}\n")
            
        print(f"Performance logged to {log_file}")
        
        # Write solution
        np.savetxt(out_file, x, fmt='%.18e')
        print(f"Solution written to {out_file}")
        
    except Exception as e:
        print(f"FATAL ERROR in Python Backend: {e}")
        # traceback.print_exc()
        sys.exit(2)

if __name__ == "__main__":
    main()
