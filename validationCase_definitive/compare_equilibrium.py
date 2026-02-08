#!/usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt
import re

# Ref Data
BERNARDO_TIME = np.array([0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 
                          55, 60, 65, 70, 75, 80, 85, 90, 95, 100])
BERNARDO_GS = np.array([0.000, 0.006, 0.018, 0.042, 0.070, 0.095, 0.110, 0.120, 0.128, 0.134, 
                        0.139, 0.143, 0.147, 0.150, 0.153, 0.156, 0.158, 0.161, 0.163, 0.166, 0.170])

def read_log_gs(log_file):
    times = []
    gs_values = []
    current_time = 0.0
    time_pattern = re.compile(r'^Time = ([\d\.]+) s')
    gs_pattern = re.compile(r'Global Volume-Averaged Solid Fraction \(Gs\) = ([\d\.eE\-\+]+)')
    
    try:
        with open(log_file, 'r') as f:
            for line in f:
                m_time = time_pattern.match(line)
                if m_time:
                    try: current_time = float(m_time.group(1))
                    except: pass
                    continue
                
                m_gs = gs_pattern.search(line)
                if m_gs:
                    try:
                        gs = float(m_gs.group(1))
                        # Only append if time advanced (logs can be verbose)
                        if len(times) == 0 or abs(current_time - times[-1]) > 1e-6:
                            times.append(current_time)
                            gs_values.append(gs)
                    except: pass
    except: return [],[]
    return np.array(times), np.array(gs_values)

t_eq, gs_eq = read_log_gs('log.teseLB_diffusive_enthalpy')

plt.figure(figsize=(10,6))
plt.plot(BERNARDO_TIME, BERNARDO_GS, 'mo-', label='Bernardo (Kinetic Reference)', markersize=8)

if len(t_eq)>0: 
    plt.plot(t_eq, gs_eq, 'b--', linewidth=2, label='Diffusive Enthalpy (Equilibrium)')

plt.xlabel('Time [s]'); plt.ylabel('Gs')
plt.title('Physics Comparison: Equilibrium vs Kinetics')
plt.legend(); plt.grid(True)
plt.xlim(0, 100); plt.ylim(0, 0.20)

plt.text(40, 0.05, "Equilibrium (Lever Rule)\ntracks cooling rate linearly\nNo undercooling lag", 
         color='blue', fontsize=10)

plt.savefig('equilibrium_vs_kinetic.png')
