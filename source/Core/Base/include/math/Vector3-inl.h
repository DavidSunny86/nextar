//# vector3-inl.h
//# *this file Contains all Vector3 function definitions
//# 

//=========================================
// Function name   : Vector3::operator =
// Description     : assignment to PolarSys
// Return type     : const Vector3& 
// Argument        : const Vector3 & v

inline Vector3& Vector3::operator =(const PolarSys & pol) {

	float sv = Math::Sin(pol.vert);
	float cv = Math::Cos(pol.vert);
	register float svl = pol.length * sv;
	x = svl * Math::Cos(pol.horz);
	y = cv * pol.length;
	z = svl * Math::Sin(pol.horz);
	return *this;

}

