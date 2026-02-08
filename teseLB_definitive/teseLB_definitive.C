/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2016 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

Application
    teseLB_definitive

Description
    Bernardo's Definitive Diffusive Model Reconstruction.
    Strictly Enthalpy/Effective Capacity Method without Kinetics.

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "incompressibleMomentumTransportModel.H"
#include "radiationModel.H"
#include "fvOptions.H"
#include "pimpleControl.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
#include "wallFvPatch.H" 
#include "simpleViscosity.H"

#define solidificationEPSILON 1.e-10

// ************************************************************************* //
//                 Hardcoded Physical Parameters (Bernardo Phase 7)          //
// ************************************************************************* //
// Mandatory Audit-Proof Constants

const Foam::scalar k_val = 19.0;         // W/mK
const Foam::scalar rho_val = 7900.0;     // kg/m3
const Foam::scalar cp_liquid = 201.0;    // J/kgK
const Foam::scalar cp_solid = 167.0;     // J/kgK
const Foam::scalar Lf_val = 4.3106e8;    // J/m3
const Foam::scalar D_val = 1.0e-9;       // m2/s

// Phase Diagram (Bi-Sn specific, linear fit)
const Foam::scalar ml0_val = 503.02;
const Foam::scalar ml1_val = -1.5372;
const Foam::scalar ms0_val = 503.15;
const Foam::scalar ms1_val = -4.439;

int main(int argc, char *argv[])
{
    #include "setRootCase.H"
    #include "createTime.H"
    #include "createMesh.H"
    #include "createControl.H"
    #include "createFields.H"
    #include "createSolidificationFields.H"
    #include "createFvOptions.H"
    #include "createTimeControls.H"
    #include "CourantNo.H"
    #include "setInitialDeltaT.H"
    #include "initContinuityErrs.H"

    turbulence->validate();

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
    Info<< "\n Starting time loop\n" << endl;

    while (runTime.run())
    {
        #include "readTimeControls.H"
        #include "CourantNo.H"

        runTime++;

        Info<< "Time = " << runTime.userTimeName() << nl << endl;

        // --- Pressure-velocity PIMPLE corrector loop
        while (pimple.loop())
        {
            #include "UEqn.H"

            // --- Solidification parameters loop
            #include "wEqn.H"
            #include "solidification.H"
            #include "TEqn.H"

            // --- Pressure corrector loop
            while (pimple.correct())
            {
                #include "pEqn.H"
            }

            if (true) 
            {
                turbulence->correct();
            }
        }

        runTime.write();

        Info<< "ExecutionTime = " << runTime.elapsedCpuTime() << " s"
            << "  ClockTime = " << runTime.elapsedClockTime() << " s"
            << nl << endl;
    }

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
