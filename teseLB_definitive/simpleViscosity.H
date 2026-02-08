#ifndef simpleViscosity_H
#define simpleViscosity_H

#include "viscosity.H"
#include "dimensionedScalar.H"
#include "volFields.H"

namespace Foam
{

class simpleViscosity
:
    public viscosity
{
    dimensionedScalar nu_;
    volScalarField nuField_;

public:

    simpleViscosity
    (
        const volVectorField& U,
        const dictionary& dict
    )
    :
        nu_("nu", dimArea/dimTime, dict),
        nuField_
        (
            IOobject
            (
                "nu",
                U.time().userTimeName(),
                U.mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            U.mesh(),
            nu_
        )
    {}

    virtual ~simpleViscosity() {}

    virtual tmp<volScalarField> nu() const
    {
        return nuField_;
    }

    virtual tmp<scalarField> nu(const label patchi) const
    {
        return nuField_.boundaryField()[patchi];
    }
};

}

#endif
