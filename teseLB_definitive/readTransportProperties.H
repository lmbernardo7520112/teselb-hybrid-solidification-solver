
    // singlePhaseTransportModel laminarTransport(U, phi); // REMOVED

    IOdictionary laminarTransport
    (
        IOobject
        (
            "transportProperties",
            runTime.constant(),
            mesh,
            IOobject::MUST_READ_IF_MODIFIED,
            IOobject::NO_WRITE
        )
    );

    // Thermal expansion coefficient [1/K]
    dimensionedScalar beta("beta", dimless/dimTemperature, laminarTransport);

    // Species concentration coefficient [1/K]
    dimensionedScalar betaW("betaW", dimless, laminarTransport);

    // Reference temperature [K]
    dimensionedScalar TRef("TRef", dimTemperature, laminarTransport);

    // Reference Species concentration [dimensionless]
    dimensionedScalar wRef("wRef", dimless, laminarTransport);

    // Laminar Prandtl number
    dimensionedScalar Pr("Pr", dimless, laminarTransport);
   
    // Turbulent Prandtl number
    dimensionedScalar Prt("Prt", dimless, laminarTransport);

    // Laminar Schmidt number
    dimensionedScalar Sc("Sc", dimless, laminarTransport);

    // Turbulent Schmidt number
    dimensionedScalar Sct("Sct", dimless, laminarTransport);

    // Diffusion Coeff.
    dimensionedScalar D(laminarTransport.lookup("D"));

// taxa de variacao temporal da temperatura
dimensionedScalar DTDtime("DTDtime", dimTemperature/dimTime, laminarTransport);

// Gradiente da temperatura
dimensionedVector gradT("gradT", dimTemperature/dimLength, laminarTransport);

// Temperatura de inoculacao [K]
//dimensionedScalar TNucleacao("TNucleacao", dimTemperature, laminarTransport);

// linearTf
dimensionedScalar linearTf(laminarTransport.lookup("linearTf"));

// liquidusSlope
dimensionedScalar liquidusSlope(laminarTransport.lookup("liquidusSlope"));

//Pre exponential constant for tip velocity
dimensionedScalar velModelCteA (laminarTransport.lookup("velModelCteA"));

//Exponential for tip velocity
dimensionedScalar velModelCteN  (laminarTransport.lookup("velModelCteN"));
//scalar velModelCteN  (laminarTransport.lookup("velModelCteN"));

// Entalpia de fusão
dimensionedScalar enthalpyOfFusion  (laminarTransport.lookup("enthalpyOfFusion"));


// Calor específico
dimensionedScalar specificHeat  (laminarTransport.lookup("specificHeat"));

// Rho real
dimensionedScalar physicalRho  (laminarTransport.lookup("physicalRho"));

// Temperatura eutética
dimensionedScalar eutecticT (laminarTransport.lookup("eutecticT"));

// Concentração eutética
dimensionedScalar eutecticW (laminarTransport.lookup("eutecticW"));

// Superresfriamento inicial
dimensionedScalar initialUndercooling (laminarTransport.lookup("initialUndercooling"));

// Coeficiente de partição
dimensionedScalar partitionK (laminarTransport.lookup("partitionK"));

// Superresfriamento eutético
dimensionedScalar eutecticUndercooling (laminarTransport.lookup("eutecticUndercooling"));

// True Thermodynamic Model Coefficients (Gibbs et al.)
dimensionedScalar ml0(laminarTransport.lookup("ml0"));
dimensionedScalar ml1(laminarTransport.lookup("ml1"));
dimensionedScalar ms0(laminarTransport.lookup("ms0"));
dimensionedScalar ms1(laminarTransport.lookup("ms1"));
