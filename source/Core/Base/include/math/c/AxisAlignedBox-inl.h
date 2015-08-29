namespace nextar {

inline bool AABoxIsValid(AABoxF box) {
	for (int i = 0; i < 3; ++i)
		if (box.maxPoint.v[i] < box.minPoint.v[i])
			return false;
	return true;
}

inline AxisAlignedBox AABoxUnion(AABoxF b, Vector3A point) {
	AxisAlignedBox box = b;
	for (int i = 0; i < 3; ++i) {
		if (box.minPoint.v[i] > point.v[i])
			box.minPoint.v[i] = point.v[i];
		if (box.maxPoint.v[i] < point.v[i])
			box.maxPoint.v[i] = point.v[i];
	}
	return box;
}

}