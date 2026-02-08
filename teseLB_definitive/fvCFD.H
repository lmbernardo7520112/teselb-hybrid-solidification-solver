#ifndef fvCFD_H
#define fvCFD_H

#include "parRun.H"
#include "Time.H"
#include "fvMesh.H"
#include "fvc.H"
#include "fvm.H"
#include "fvMatrices.H"
#include "volFields.H"
#include "surfaceFields.H"
#include "argList.H"
#include "OSspecific.H"
#include "mathematicalConstants.H"

#include "constrainHbyA.H"
#include "constrainPressure.H"
#include "findRefCell.H"
#include "adjustPhi.H"
#include "uniformDimensionedFields.H"
#include "IOstreams.H"
#include "OFstream.H"
#include "IFstream.H"

using namespace Foam;

#endif
