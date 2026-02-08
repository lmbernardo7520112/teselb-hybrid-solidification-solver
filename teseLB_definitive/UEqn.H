    // Solve the momentum equation

    // MRF.correctBoundaryVelocity(U); // Deprecated API

    fvVectorMatrix UEqn
    (
        fvm::ddt(U) + fvm::div(phi, U)
      // + MRF.DDt(U)
      + turbulence->divDevTau(U)
     ==
        fvOptions(U)
    );

    UEqn.relax();

    fvOptions.constrain(UEqn);

    if (pimple.momentumPredictor())
    {
        solve
        (
            UEqn
         ==
            fvc::reconstruct
            (
                (
                  - ghf*fvc::snGrad(rhok)
                  - fvc::snGrad(p_rgh)
                )*mesh.magSf()
            )
        );

        fvOptions.correct(U);
    }
