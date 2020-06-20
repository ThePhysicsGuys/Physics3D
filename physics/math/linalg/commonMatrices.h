#pragma once

#include "mat.h"
#include "quat.h"
#include "../constants.h"

#define QROT_X_90(Type) Quaternion<Type> {\
SQ2_2, SQ2_2, 0, 0}

#define QROT_X_180(Type) Quaternion<Type> {\
0, 1, 0, 0}

#define QROT_X_270(Type) Quaternion<Type> {\
-SQ2_2, SQ2_2, 0, 0}

#define QROT_Y_90(Type) Quaternion<Type> {\
SQ2_2, 0, SQ2_2, 0}

#define QROT_Y_180(Type) Quaternion<Type> {\
0, 0, 1, 0}

#define QROT_Y_270(Type) Quaternion<Type> {\
SQ2_2, 0, -SQ2_2, 0}

#define QROT_Z_90(Type) Quaternion<Type> {\
SQ2_2, 0, 0, SQ2_2}

#define QROT_Z_180(Type) Quaternion<Type> {\
0, 0, 0, 1}

#define QROT_Z_270(Type) Quaternion<Type> {\
-SQ2_2, 0, 0, SQ2_2}

#define ROT_X_90(Type) Matrix<Type, 3, 3> {\
1, 0, 0,\
0, 0, -1,\
0, 1, 0}

#define ROT_X_180(Type) Matrix<Type, 3, 3> {\
1, 0, 0,\
0, -1, 0,\
0, 0, -1}

#define ROT_X_270(Type) Matrix<Type, 3, 3> {\
1, 0, 0, \
0, 0, 1, \
0, -1, 0}


#define ROT_Y_90(Type) Matrix<Type, 3, 3> {\
0, 0, 1,\
0, 1, 0,\
-1, 0, 0}

#define ROT_Y_180(Type) Matrix<Type, 3, 3> {\
-1, 0, 0,\
0, 1, 0,\
0, 0,-1}

#define ROT_Y_270(Type) Matrix<Type, 3, 3> {\
0, 0,-1,\
0, 1, 0,\
1, 0, 0}

#define ROT_Z_90(Type) Matrix<Type, 3, 3> {\
0, -1, 0,\
1, 0, 0,\
0, 0, 1}

#define ROT_Z_180(Type) Matrix<Type, 3, 3> {\
-1, 0, 0,\
0, -1, 0,\
0, 0, 1}

#define ROT_Z_270(Type) Matrix<Type, 3, 3> {\
0, 1, 0,\
-1, 0, 0,\
0, 0, 1}
