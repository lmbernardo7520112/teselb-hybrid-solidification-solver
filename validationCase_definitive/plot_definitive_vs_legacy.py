#!/usr/bin/env python3
"""
Definitive Model Comparison: SDD Invariant Core vs Legacy/Experiments
"""

import numpy as np
import matplotlib.pyplot as plt
import re

# ============================================================================
# 1. Reference Data (Extracted from Project Archives)
# ============================================================================

# Bernardo Reference (Thesis - Diffusive Kinetic Model)
BERNARDO_TIME = np.array([0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 
                          55, 60, 65, 70, 75, 80, 85, 90, 95, 100])
BERNARDO_GS = np.array([0.000, 0.006, 0.018, 0.042, 0.070, 0.095, 0.110, 0.120, 0.128, 0.134, 
                        0.139, 0.143, 0.147, 0.150, 0.153, 0.156, 0.158, 0.161, 0.163, 0.166, 0.170])

# Gibbs Experimental Data (Digitized)
GIBBS_TIME = np.array([0.000, 7.142, 14.796, 22.448, 29.851, 37.253, 45.410, 52.803, 
                       60.448, 67.841, 74.734, 82.400, 89.311, 97.219, 104.126, 111.787])
GIBBS_GS = np.array([0.000, 0.015, 0.029, 0.051, 0.085, 0.102, 0.104, 0.108, 
                     0.110, 0.119, 0.122, 0.120, 0.124, 0.124, 0.124, 0.125])

# ============================================================================
# 2. Read Simulation Data (Definitive Run 8)
# ============================================================================

def read_log_gs(log_file):
    """Extracts Time and Global Gs from OpenFOAM log."""
    times = []
    gs_values = []
    
    current_time = 0
    
    with open(log_file, 'r') as f:
        for line in f:
            # Extract Time
            if 'Time =' in line and 'ExecutionTime' not in line:
                try:
                    parts = line.split()
                    t_str = parts[2].replace('s', '')
                    current_time = float(t_str)
                except:
                    pass
            
            # Extract Gs
            if 'Global Volume-Averaged Solid Fraction (Gs)' in line:
                try:
                    val = float(line.split('=')[-1])
                    times.append(current_time)
                    gs_values.append(val)
                except:
                    pass
                    
    return np.array(times), np.array(gs_values)

# Parse Run 8 (Mesh Independent Case)
t_run8, gs_run8 = read_log_gs('log.teseLB_definitive_run8')

# ============================================================================
# 3. Plotting
# ============================================================================

plt.figure(figsize=(10, 7))

# Legacy/Reference
plt.plot(BERNARDO_TIME, BERNARDO_GS, 'k--', linewidth=1.5, marker='o', 
         markerfacecolor='white', label='Bernardo (Original Model)', zorder=2)

plt.scatter(GIBBS_TIME, GIBBS_GS, s=80, marker='^', c='cyan', edgecolors='black', 
            label='Gibbs (Experiment)', zorder=3)

# Definitive Model
# Only plot up to 120s to match experimental range
mask = t_run8 <= 120
plt.plot(t_run8[mask], gs_run8[mask], 'r-', linewidth=3, 
         label='Definitive Model (Invariant Core)', zorder=4)

# Formatting
plt.title('Solid Fraction Evolution: Definitive Model vs Legacy', fontsize=14)
plt.xlabel('Time (s)', fontsize=12)
plt.ylabel('Solid Fraction (Gs)', fontsize=12)
plt.grid(True, alpha=0.3)
plt.legend(loc='upper left', fontsize=12)
plt.xlim(0, 120)
plt.ylim(0, 0.20)

plt.savefig('gs_comparison_definitive.png', dpi=150)
print("Plot saved to gs_comparison_definitive.png")
