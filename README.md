# 🧭 teseLB — Solver Híbrido Quântico-Clássico para Solidificação

![OpenFOAM](https://img.shields.io/badge/OpenFOAM-v11-orange?style=for-the-badge&logo=cplusplus)
![Python](https://img.shields.io/badge/Python-3.x-blue?style=for-the-badge&logo=python)
![Qiskit](https://img.shields.io/badge/Qiskit-SDK-purple?style=for-the-badge&logo=qiskit)
![NumPy](https://img.shields.io/badge/NumPy-Array-lightblue?style=for-the-badge&logo=numpy)
![SciPy](https://img.shields.io/badge/SciPy-Solver-blue?style=for-the-badge&logo=scipy)
![C++](https://img.shields.io/badge/C++-Core-darkblue?style=for-the-badge&logo=cplusplus)

> [!WARNING]
> **Work In Progress (WIP)**: This project is currently under active development. Features, solvers, and quantum implementations are subject to change. Some physical models are undergoing strict validation, and potential errors are currently being tracked and addressed by the author.

---

## 📘 Descrição Geral

O **teseLB** é um solver computacional avançado para simulação de processos de **solidificação de ligas metálicas**, integrando a robustez da **Dinâmica dos Fluidos Computacional (CFD)** clássica com o potencial emergente da **Computação Quântica**.

O projeto expande as capacidades do OpenFOAM (Finite Volume Method), introduzindo uma arquitetura híbrida onde sistemas lineares complexos podem ser exportados e resolvidos via algoritmos quânticos (VQLS/HHL) ou simuladores clássicos, permitindo pesquisa de ponta em **Quantum CFD**.

O foco é investigar a **difusão de soluto**, **transferência de calor com mudança de fase** (entalpia efetiva) e a **viabilidade de aceleradores quânticos** para problemas de engenharia de materiais.

---

## 🧩 Arquitetura Híbrida

```text
┌───────────────────────┐
│     OpenFOAM (C++)    │  →  Discretização FVM & Física do Contínuo
│ (teseLB_definitive)   │     (Navier-Stokes, Energia, Espécies)
└──────────┬────────────┘
           │  (Exportação de Matriz A e Vetor b)
           ▼
┌───────────────────────┐
│    Bridge (File I/O)  │  →  Intercâmbio de Dados (.dat)
│ (A_matrix / b_vector) │
└──────────┬────────────┘
           │
           ▼
┌───────────────────────┐
│    Quantum Backend    │  →  Python + Qiskit + SciPy
│  (quantum_solver.py)  │     (VQLS, HHL, Clássico Iterativo)
└───────────────────────┘
```

---

## 🎓 Jornada da Física (Modelo de Solidificação)

O solver implementa um modelo de solidificação puramente difusivo com acoplamento térmico e solutal.

## ✳️ Funcionalidades Principais

**Método da Entalpia Efetiva**: Tratamento do calor latente sem termos de fonte explícitos, garantindo estabilidade numérica.

**Transporte de Soluto**: Equação de conservação de espécies acoplada à fração sólida ($G_s$).

**Termo de Fonte de Darcy**: Modelagem da zona pastosa como meio poroso (Carman-Kozeny) no momento.

**Correção de Densidade Boussinesq**: Convecção natural induzida por gradientes térmicos e solutais.

___

## 🗂️ Jornada Computacional (Solver Híbrido)

O núcleo diferencial é solver em C++, mas o sistema linear algébrico ($Ax=b$) pode ser interceptado.

## 🧠 Solucionador Quântico (`quantum_solver.py`)

**Modo Clássico (Baseline)**: Uso de `scipy.sparse.linalg.spsolve` ou `BiCGSTAB` para validação rápida.

**Modo Quântico Simulado**: Uso do `AerSimulator` do Qiskit para emular hardware quântico.

**Configuração Dinâmica**: Controle via `solver_settings.json` sem recompilar o código C++.

```json
{
    "mode": "quantum",
    "backend": "aer_simulator",
    "use_qiskit": true
}
```

___

## ⚙️ Estrutura Técnica e Decisões

| Tema | Estratégia | Benefício |
| :--- | :--- | :--- |
| **Integração C++/Python** | Arquivos de Texto (`.dat`) | Desacoplamento total e facilidade de debug |
| **Matrizes Esparsas** | Formato COO/LDU | Eficiência no armazenamento de malhas grandes |
| **Hard-coded Physics** | Propriedades termofísicas fixas | Foco na validação numérica do método |
| **Validação Cruzada** | Casos `classic` vs `definitive` | Garantia de que o novo solver reproduz benchmarks |
| **Performance Log** | CSV Timestamping | Análise quantitativa do "Quantum Overhead" |

___

## 🧪 Casos de Validação

- ✅ **validationCase_classic**: Caso base usando modelos legados de Bernardo/Gibbs.
- ✅ **validationCase_definitive**: Caso principal validando o modelo de Entalpia Efetiva.
- ✅ **validationCase_definitive_qc**: Validação do fluxo de trabalho quântico (OpenFOAM -> Python -> OpenFOAM).
- ✅ **teseLB_solute**: Testes focados puramente na segregação de soluto.

---

## 🧩 Estrutura de Pastas

```bash
teseLB/
├── teseLB_definitive/          # Código fonte C++ do solver principal
│   ├── quantumSolve.H          # Interface de interceptação da matriz
│   ├── quantum_solver.py       # Backend Python (Link Simbólico)
│   ├── TEqn.H                  # Equação da Energia
│   └── wEqn.H                  # Equação de Transporte de Espécies
├── validationCase_definitive/  # Caso de teste padrão (Clássico)
├── validationCase_definitive_qc/ # Caso de teste configurado para Quântico
│   ├── solver_settings.json    # Configuração do backend
│   ├── README_quantum.md       # Documentação específica do fluxo QC
│   └── solver_performance.csv  # Logs de tempo de execução
├── classic_baseline/           # Dados experimentais e legados para comparação
└── README.md                   # Este arquivo
```

___

## 🕒 Histórico de Desenvolvimento (Commit Log Humano)

### 🧩 Fase 1 — Fundação e Baseline Clássico
**Período:** Janeiro 2026
**Resumo:**
- Estabelecimento do `classic_baseline` recuperando dados de Bernardo e Gibbs.
- Implementação inicial do modelo difusivo simples.
- Comparação de resultados ($G_s$ evolution) para garantir consistência física inicial.

### 🎓 Fase 2 — Implementação da Entalpia Efetiva
**Período:** Meio de Janeiro 2026
**Resumo:**
- Criação do `teseLB_diffusive_enthalpy`.
- Remoção de termos de fonte explícitos de calor latente para aumentar estabilidade.
- Adoção de $C_p^{eff}$ e $\alpha_{eff}$ baseados na termodinâmica da mudança de fase.
- Validação da monotonicidade do crescimento da fração sólida.

### 🧠 Fase 3 — Solver Definitivo e Refatoração
**Período:** Final de Janeiro 2026
**Resumo:**
- Consolidação no `teseLB_definitive`.
- Limpeza de código (`TEqn.H`, `solidification.H`).
- Integração robusta das equações de transporte solutal (`wEqn.H`).
- Geração de gráficos comparativos automatizados (`plot_gs_average.py`).

### 🚀 Fase 4 — A Fronteira Quântica
**Período:** Fevereiro 2026
**Resumo:**
- Desenvolvimento da interface `quantumSolve.H` para extração de matrizes LDU.
- Criação do `quantum_solver.py` com suporte a Qiskit.
- Implementação de log de performance e suporte a VQLS/HHL.
- Criação do ambiente de validação `validationCase_definitive_qc`.
- Documentação do fluxo híbrido e análise de overhead computacional.

---

> 💬 *"Este projeto não é apenas um solver CFD, é uma ponte entre a engenharia de materiais clássica e a próxima geração de computação de alto desempenho."*
> — **Leonardo Maximino Bernardo**, 2026
