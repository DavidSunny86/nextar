namespace nextar {

inline bool AABoxIsValid(AABoxF box) {
	return _mm_movemask_epi8(_mm_castps_si128(_mm_cmplt_ps(box.maxPoint, box.minPoint))) == 0;
}

inline AxisAlignedBox AABoxUnion(AABoxF b, Vector3A point) {
	return  AxisAlignedBox(_mm_min_ps(b.minPoint, point), 
		_mm_max_ps(b.maxPoint, point));
}

}