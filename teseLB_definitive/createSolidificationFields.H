// Densidade de área (REMOVED - Kinetic Artifact)

    volScalarField Cp_eff
    (
        IOobject
        (
            "Cp_eff",
            runTime.name(),
            mesh,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        mesh,
        dimensionedScalar("Cp_eff", dimEnergy/dimMass/dimTemperature, cp_liquid)
    );

    // Initial Gs calculation
    // interfaceDensity REMOVED
    
    // totalNearInterfaceVolume REMOVED

    // Liquidus Temperature (Diagnostic/Phase Diagram)
    volScalarField liquidusT 
    (
        IOobject
        (
            "liquidusT",
            runTime.name(),
            mesh,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        mesh,
        dimensionSet(0, 0, 0, 1, 0, 0, 0)
    );
    liquidusT = linearTf + liquidusSlope * w;
    liquidusT.write();


    // Undercooling (Diagnostic only)
    volScalarField undercooling 
    (
        IOobject
        (
            "undercooling",
            runTime.name(),
            mesh,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        mesh,
        dimensionSet(0, 0, 0, 1, 0, 0, 0)
    );
    undercooling = liquidusT - T;
    undercooling.write();


    // solidificationVelocity REMOVED (Kinetic Artifact)


    // soluteSource REMOVED (Kinetic/Source Artifact)


    // heatSource REMOVED (Handled by Effective Capacity)


    // Taxa de solidificação (Diagnostic dGs/dt)
    volScalarField dGsdt 
    (
        IOobject
        (
            "dGsdt",
            runTime.name(),
            mesh,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        mesh,
        dimensionSet(0, 0, -1, 0, 0, 0, 0)
    );
    
    dGsdt = 0. /runTime.deltaT(); // Init
    dGsdt.write();

    // Fração de sólido
    volScalarField Gs
    (
        IOobject
        (
            "Gs",
            runTime.name(),
            mesh,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        mesh,
        dimensionSet(0, 0, 0, 0, 0, 0, 0)
    );
    
    Gs = 0. * dGsdt * runTime.deltaT(); // Init
    Gs.write();
