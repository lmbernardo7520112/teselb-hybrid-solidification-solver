#ifndef fvOptions_H
#define fvOptions_H

#include "fvMatrices.H"

namespace Foam
{
    class fvOptions_shim
    {
    public:
        // Constructor
        fvOptions_shim(const fvMesh& mesh) {}

        // Source term operator
        template<class Type>
        tmp<fvMatrix<Type>> operator()(VolField<Type>& field)
        {
            // Return zero matrix/source
            return tmp<fvMatrix<Type>>(new fvMatrix<Type>(field, field.dimensions()*dimVolume/dimTime));
        }

        template<class Type>
        tmp<fvMatrix<Type>> operator()(const VolField<Type>& field)
        {
             return tmp<fvMatrix<Type>>(new fvMatrix<Type>(const_cast<VolField<Type>&>(field), field.dimensions()*dimVolume/dimTime));
        }

        // Constrain
        template<class Type>
        void constrain(fvMatrix<Type>& eqn)
        {
            // Do nothing
        }

        // Correct
        template<class Type>
        void correct(VolField<Type>& field)
        {
            // Do nothing
        }
    };
}

#endif
