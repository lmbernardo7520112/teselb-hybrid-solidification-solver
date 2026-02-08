// ************************************************************************* //
//                 Thermodynamic State & Effective Heat Capacity             //
//                     Bernardo-Consistent Implementation                    //
// ************************************************************************* //

    // 1. Parameter Access (Globals)
    // rho_val, cp_liquid, cp_solid, Lf_val, k_val, etc.

    scalar dt = runTime.deltaT().value();

    forAll(T, cellI)
    {
        // A. Thermodynamic State Update
        scalar wVal = w[cellI];
        
        // Phase Diagram Limits
        scalar Tl_local = ml0_val + ml1_val * wVal;
        scalar Ts_local = ms0_val + ms1_val * wVal;
        
        scalar T_local = T[cellI];
        
        scalar Gs_old = Gs.oldTime()[cellI];
        scalar Gs_target = 0.0;
        
        scalar dGsdT_mag = 0.0; // Magnitude of dGs/dT (positive)
        scalar cp_base = cp_liquid; // Default to liquid

        scalar deltaT = Tl_local - Ts_local; // Width of mushy zone (positive or negative)
        
        // Strict Bounds Check (Lever Rule) - SPEC-P-04
        if (mag(deltaT) < 1e-6) // Degenerate case (pure substance or eutectic)
        {
            if (T_local < Tl_local) Gs_target = 1.0;
            else Gs_target = 0.0;
            // Impulse function for dGs/dT not handled here, assume step change
        } 
        else 
        {
             if (T_local > Tl_local) { // Liquid
                 Gs_target = 0.0;
                 dGsdT_mag = 0.0;
                 cp_base = cp_liquid;
             } 
             else if (T_local < Ts_local) { // Solid
                 Gs_target = 1.0;
                 dGsdT_mag = 0.0;
                 cp_base = cp_solid;
             } 
             else { // Mushy Zone
                 // Gs = (Tl - T) / (Tl - Ts)
                 Gs_target = (Tl_local - T_local) / deltaT;
                 
                 // Strict Clipping - SPEC-N-02
                 Gs_target = max(0.0, min(1.0, Gs_target));
                 
                 // |dGs/dT| = 1 / |Tl - Ts| inside the mushy zone
                 dGsdT_mag = 1.0 / mag(deltaT); 
                 
                 // Interpolate Cp base linearly
                 cp_base = Gs_target * cp_solid + (1.0 - Gs_target) * cp_liquid;
             }
        }
        
        // Update Gs Field (State Function) - SPEC-P-02
        Gs[cellI] = Gs_target;
        
        // Diagnostic: Rate of change (Artifact-free, just observation)
        dGsdt[cellI] = (Gs_target - Gs_old)/dt;

        // B. Calculate Effective Specific Heat - SPEC-N-01
        // Cp_eff = Cp_base + (Lf/rho) * |dGs/dT| (using magnitude for stability)
        scalar latentTerm = (Lf_val / rho_val) * dGsdT_mag;
        Cp_eff[cellI] = cp_base + latentTerm;
    }
    
    // Correct Boundary Conditions for fields
    Gs.correctBoundaryConditions();
    dGsdt.correctBoundaryConditions();
    Cp_eff.correctBoundaryConditions();
    
    // Global Diagnostic - SPEC-G-01 Check
    scalar totalVol = gSum(mesh.V());
    scalar globalsolidVolume = gSum(Gs.primitiveField() * mesh.V());
    scalar avgGs = globalsolidVolume / totalVol;
    
    Info<< "Global Volume-Averaged Solid Fraction (Gs) = " << avgGs << endl;
