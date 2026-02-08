
// ************************************************************************* //
//                 Solute Equation (Pure Diffusion)                          //
//                     Bernardo-Consistent Implementation                    //
// ************************************************************************* //

    // 1. Mandatory Physical Parameters (Hard-coded Audit)
    // D_val = 1.0e-9 m2/s is extern.
    dimensionedScalar Di("Di", dimensionSet(0, 2, -1, 0, 0, 0, 0), D_val);

    // 2. Solute Equation
    // dw/dt - laplacian(D, w) = 0
    // STRICTLY NO CONVECTION, NO SOURCE
    
    fvScalarMatrix wEqn
    (
        fvm::ddt(w)
      - fvm::laplacian(Di, w)
    );

    wEqn.relax();
    fvOptions.constrain(wEqn);
    wEqn.solve();
    fvOptions.correct(w);

    rhok = 1.0 - beta*(T - TRef);
