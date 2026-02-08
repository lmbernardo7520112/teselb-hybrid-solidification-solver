// ************************************************************************* //
//                 Temperature Equation (Effective Enthalpy Form)            //
//                     Bernardo-Consistent Implementation                    //
// ************************************************************************* //

    // 1. Mandatory Physical Parameters (Using Globals)
    // k_val (19.0), rho_val (7900) are extern.

    // 2. Define Effective Thermal Diffusivity alpha_eff
    // alpha_eff = k / (rho * Cp_eff)
    // We must handle dimensions carefully. 
    // Cp_eff has dimEnergy/dimMass/dimTemperature.
    // k, rho are raw scalars.
    
    // Create alphaEff with correct dimensions (m2/s)
    volScalarField alphaEff
    (
        IOobject
        (
            "alphaEff",
            runTime.name(),
            mesh,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh,
        dimensionedScalar("alphaZero", dimensionSet(0, 2, -1, 0, 0, 0, 0), 1.2e-5) // Liquid Alpha
    );
    
    // Update logic:
    // alpha = k / (rho * Cp)
    // We compute the raw values and assign.
    // Since k, rho are raw scalars, we divide by Cp_eff.value() if possible? 
    // No, Cp_eff is a field.
    
    // Safer way: 
    // alphaEff = (k/rho) / Cp_eff
    // But Cp_eff is dimensioned.
    // Let's strip dimensions for calculation and re-apply.
    
    const scalar alphaConst = k_val / rho_val;
    
    forAll(alphaEff, cellI)
    {
        alphaEff[cellI] = alphaConst / Cp_eff[cellI];
    }
    alphaEff.correctBoundaryConditions();
    
    // 3. Construct Equation
    // ddt(T) - laplacian(alphaEff, T) = 0
    // STRICTLY NO SOURCE TERM
    
    fvScalarMatrix TEqn
    (
        fvm::ddt(T)
      - fvm::laplacian(alphaEff, T)
    );

    TEqn.relax();
    fvOptions.constrain(TEqn);
    TEqn.solve();
    fvOptions.correct(T);
    
    rhok = 1.0 - beta*(T - TRef);
