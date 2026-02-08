# 🧭 teseLB — Hybrid Quantum-Classical Solver for Solidification

![OpenFOAM](https://img.shields.io/badge/OpenFOAM-v11-orange?style=for-the-badge&logo=cplusplus)
![Python](https://img.shields.io/badge/Python-3.x-blue?style=for-the-badge&logo=python)
![Qiskit](https://img.shields.io/badge/Qiskit-SDK-purple?style=for-the-badge&logo=qiskit)
![NumPy](https://img.shields.io/badge/NumPy-Array-lightblue?style=for-the-badge&logo=numpy)
![SciPy](https://img.shields.io/badge/SciPy-Solver-blue?style=for-the-badge&logo=scipy)
![C++](https://img.shields.io/badge/C++-Core-darkblue?style=for-the-badge&logo=cplusplus)

> [!WARNING]
> **Work In Progress (WIP)**: This project is currently under active development. Features, solvers, and quantum implementations are subject to change. Some physical models are undergoing strict validation, and potential errors are currently being tracked and addressed by the author.

---

## 📘 General Description

**teseLB** is an advanced computational solver for simulating **metal alloy solidification processes**, integrating the robustness of classical **Computational Fluid Dynamics (CFD)** with the emerging potential of **Quantum Computing**.

The project extends OpenFOAM (Finite Volume Method) capabilities by introducing a hybrid architecture where complex linear systems can be exported and solved via quantum algorithms (VQLS/HHL) or classical simulators, enabling cutting-edge research in **Quantum CFD**.

The focus is to investigate **solute diffusion**, **heat transfer with phase change** (effective enthalpy), and the **feasibility of quantum accelerators** for materials engineering problems.

---

## 🧩 Hybrid Architecture

```text
┌───────────────────────┐
│     OpenFOAM (C++)    │  →  FVM Discretization & Continuum Physics
│ (teseLB_definitive)   │     (Navier-Stokes, Energy, Species)
└──────────┬────────────┘
           │  (Matrix A and Vector b Export)
           ▼
┌───────────────────────┐
│    Bridge (File I/O)  │  →  Data Exchange (.dat)
│ (A_matrix / b_vector) │
└──────────┬────────────┘
           │
           ▼
┌───────────────────────┐
│    Quantum Backend    │  →  Python + Qiskit + SciPy
│  (quantum_solver.py)  │     (VQLS, HHL, Classical Iterative)
└───────────────────────┘
```

---

## 🎓 Physics Journey (Solidification Model)

The solver implements a purely diffusive solidification model with thermal and solutal coupling.

## ✳️ Main Features

**Effective Enthalpy Method**: Treatment of latent heat without explicit source terms, ensuring numerical stability.

**Solute Transport**: Species conservation equation coupled to the solid fraction ($G_s$).

**Darcy Source Term**: Modeling the mushy zone as a porous medium (Carman-Kozeny) in the momentum equation.

**Boussinesq Density Correction**: Natural convection induced by thermal and solutal gradients.

___

## 🗂️ Computational Journey (Hybrid Solver)

The differential core is solved in C++, but the algebraic linear system ($Ax=b$) can be intercepted.

## 🧠 Quantum Solver (`quantum_solver.py`)

**Classical Mode (Baseline)**: Uses `scipy.sparse.linalg.spsolve` or `BiCGSTAB` for fast validation.

**Simulated Quantum Mode**: Uses Qiskit's `AerSimulator` to emulate quantum hardware.

**Dynamic Configuration**: Control via `solver_settings.json` without recompiling C++ code.

```json
{
    "mode": "quantum",
    "backend": "aer_simulator",
    "use_qiskit": true
}
```

___

## ⚙️ Technical Structure and Decisions

| Topic | Strategy | Benefit |
| :--- | :--- | :--- |
| **C++/Python Integration** | Text Files (`.dat`) | Total decoupling and ease of debugging |
| **Sparse Matrices** | COO/LDU Format | Efficiency in storing large meshes |
| **Hard-coded Physics** | Fixed thermophysical properties | Focus on numerical validation of the method |
| **Cross Validation** | `classic` vs `definitive` cases | Guarantee that the new solver reproduces benchmarks |
| **Performance Log** | CSV Timestamping | Quantitative analysis of "Quantum Overhead" |

___

## 🧪 Validation Cases

- ✅ **validationCase_classic**: Base case using legacy models from Bernardo/Gibbs.
- ✅ **validationCase_definitive**: Main case validating the Effective Enthalpy model.
- ✅ **validationCase_definitive_qc**: Validation of the quantum workflow (OpenFOAM -> Python -> OpenFOAM).
- ✅ **teseLB_solute**: Tests focused purely on solute segregation.

---

## 🧩 Folder Structure

```bash
teseLB/
├── teseLB_definitive/          # Main solver C++ source code
│   ├── quantumSolve.H          # Matrix interception interface
│   ├── quantum_solver.py       # Python Backend
│   ├── TEqn.H                  # Energy Equation
│   └── wEqn.H                  # Species Transport Equation
├── validationCase_definitive/  # Standard test case (Classical)
├── validationCase_definitive_qc/ # Test case configured for Quantum
│   ├── solver_settings.json    # Backend configuration
│   ├── README_quantum.md       # Specific QC workflow documentation
│   └── solver_performance.csv  # Execution time logs
├── classic_baseline/           # Experimental and legacy data for comparison
└── README.md                   # This file
```

___

## 🕒 Development History (Human Commit Log)

### 🧩 Phase 1 — Foundation and Classical Baseline
**Period:** January 2026
**Summary:**
- Establishment of `classic_baseline` retrieving data from Bernardo and Gibbs.
- Initial implementation of the simple diffusive model.
- Comparison of results ($G_s$ evolution) to ensure initial physical consistency.

### 🎓 Phase 2 — Effective Enthalpy Implementation
**Period:** Mid-January 2026
**Summary:**
- Creation of `teseLB_diffusive_enthalpy`.
- Removal of explicit latent heat source terms to increase stability.
- Adoption of $C_p^{eff}$ and $\alpha_{eff}$ based on phase change thermodynamics.
- Validation of solid fraction growth monotonicity.

### 🧠 Phase 3 — Definitive Solver and Refactoring
**Period:** Late January 2026
**Summary:**
- Consolidation into `teseLB_definitive`.
- Code cleanup (`TEqn.H`, `solidification.H`).
- Robust integration of solutal transport equations (`wEqn.H`).
- Generation of automated comparative plots (`plot_gs_average.py`).

### 🚀 Phase 4 — The Quantum Frontier
**Period:** February 2026
**Summary:**
- Development of the `quantumSolve.H` interface for LDU matrix extraction.
- Creation of `quantum_solver.py` with Qiskit support.
- Implementation of performance logging and VQLS/HHL support.
- Creation of the validation environment `validationCase_definitive_qc`.
- Documentation of the hybrid flow and computational overhead analysis.

---

> 💬 *"This project is not just a CFD solver, it is a bridge between classical materials engineering and the next generation of high-performance computing."*
> — **Leonardo Maximino Bernardo**, 2026
