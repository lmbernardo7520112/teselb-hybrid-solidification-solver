#ifndef quantumSolveInternal_H
#define quantumSolveInternal_H

#include "fvCFD.H"
#include "lduMatrix.H"
#include <cstdlib>

namespace Foam
{

template<class Type>
void quantumSolveInternal(fvMatrix<Type>& Eqn)
{
    Info << "\n[Quantum Interface] Intercepting Solver Call..." << endl;

    const lduMatrix& A = Eqn.lduMatrix();
    const Field<Type>& source = Eqn.source();
    const Field<Type>& internalCoeffs = Eqn.internalCoeffs();
    const Field<Type>& boundaryCoeffs = Eqn.boundaryCoeffs();
    
    Field<Type> b = source + internalCoeffs + boundaryCoeffs;

    const scalarField& diag = A.diag();
    const scalarField& upper = A.upper();
    const scalarField& lower = A.lower();
    
    const lduAddressing& addr = A.lduAddr();
    const labelUList& upperAddr = addr.upperAddr();
    const labelUList& lowerAddr = addr.lowerAddr();

    fileName casePath = Eqn.mesh().time().path();
    OFstream matFile("A_matrix.dat");
    OFstream rhsFile("b_vector.dat");

    Info << "[Quantum Interface] Exporting Matrix (Size: " << diag.size() << ")..." << endl;

    forAll(diag, i)
    {
        matFile << i << " " << i << " " << diag[i] << endl;
    }

    forAll(upper, k)
    {
        label owner = lowerAddr[k];
        label neighbour = upperAddr[k];
        matFile << owner << " " << neighbour << " " << upper[k] << endl;
        matFile << neighbour << " " << owner << " " << lower[k] << endl;
    }

    forAll(b, i)
    {
        rhsFile << i << " " << b[i] << endl;
    }

    std::string command = "python3 ../teseLB_definitive/quantum_solver.py A_matrix.dat b_vector.dat solution.dat";
    Info << "[Quantum Interface] Invoking Backend: " << command << endl;
    
    int status = std::system(command.c_str());
    
    IFstream solFile("solution.dat");
    if (status != 0 || !solFile.good())
    {
         // Fallback or warning?
         Info << "Quantum Solver failed or no solution. Continuing with old values (DANGEROUS)." << endl;
         // return;
    }

    if (solFile.good()) {
        Field<Type>& psi = Eqn.psi();
        Info << "[Quantum Interface] Importing Solution..." << endl;
        scalar val;
        forAll(psi, i)
        {
            solFile >> val;
            psi[i] = val;
        }
    }
    
    Info << "[Quantum Interface] Complete.\n" << endl;
}

} 

#endif
