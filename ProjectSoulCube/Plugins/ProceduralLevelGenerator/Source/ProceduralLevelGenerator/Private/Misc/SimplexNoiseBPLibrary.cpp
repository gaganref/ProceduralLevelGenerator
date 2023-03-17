/*

Ported from https://github.com/devdad/SimplexNoise with few changes
SimplexNoise 1.2.0
-----
DevDad - Afan Olovcic @ www.art-and-code.com - 08/12/2015

*/


#include "Misc/SimplexNoiseBPLibrary.h"

// USimplexNoiseBPLibrary
#define FASTFLOOR(x) ( ((x)>0) ? ((int)x) : (((int)x)-1) )

USimplexNoiseBPLibrary::USimplexNoiseBPLibrary(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

unsigned char USimplexNoiseBPLibrary::Perm[512] = { 151,160,137,91,90,15,
131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
151,160,137,91,90,15,
131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

void USimplexNoiseBPLibrary::SetNoiseSeed(const int32& NewSeed)
{
	FMath::RandInit(NewSeed);
	for (uint16 Itr = 0; Itr < 256; ++Itr)
	{
		const uint8 NextNum = FMath::RandRange(0, 255);
		USimplexNoiseBPLibrary::Perm[Itr] = static_cast<unsigned char>(NextNum);
		USimplexNoiseBPLibrary::Perm[Itr + 256] = static_cast<unsigned char>(NextNum);
	}
}

void USimplexNoiseBPLibrary::SetNoiseFromStream(FRandomStream& RandStream)
{
	for (uint16 Itr = 0; Itr < 256; ++Itr)
	{
		const uint8 NextNum = RandStream.RandRange(0, 255);
		USimplexNoiseBPLibrary::Perm[Itr] = static_cast<unsigned char>(NextNum);
		USimplexNoiseBPLibrary::Perm[Itr + 256] = static_cast<unsigned char>(NextNum);
	}
}

static unsigned char Simplex[64][4] = {
	{ 0,1,2,3 },{ 0,1,3,2 },{ 0,0,0,0 },{ 0,2,3,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,2,3,0 },
	{ 0,2,1,3 },{ 0,0,0,0 },{ 0,3,1,2 },{ 0,3,2,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,3,2,0 },
	{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },
	{ 1,2,0,3 },{ 0,0,0,0 },{ 1,3,0,2 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 2,3,0,1 },{ 2,3,1,0 },
	{ 1,0,2,3 },{ 1,0,3,2 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 2,0,3,1 },{ 0,0,0,0 },{ 2,1,3,0 },
	{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },
	{ 2,0,1,3 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 3,0,1,2 },{ 3,0,2,1 },{ 0,0,0,0 },{ 3,1,2,0 },
	{ 2,1,0,3 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 3,1,0,2 },{ 0,0,0,0 },{ 3,2,0,1 },{ 3,2,1,0 } };


float USimplexNoiseBPLibrary::Grad(const int& Hash, const float& X)
{
	const int H = Hash & 15;
	float Grad = 1.0 + (H & 7);							// Gradient value 1.0, 2.0, ..., 8.0
	if (H & 8) Grad = -Grad;							// Set a random sign for the gradient
	return (Grad * X);									// Multiply the gradient with the distance
}

float USimplexNoiseBPLibrary::Grad(const int& Hash, const float& X, const float& Y)
{
	const int H = Hash & 7;									// Convert low 3 bits of hash code
	const float U = H < 4 ? X : Y;							// into 8 simple gradient directions,
	const float V = H < 4 ? Y : X;							// and compute the dot product with (x,y).
	return ((H & 1) ? -U : U) + ((H & 2) ? -2.0f*V : 2.0f*V);
}

float USimplexNoiseBPLibrary::Grad(const int& Hash, const float& X, const float& Y, const float& Z)
{
	const int H = Hash & 15;									// Convert low 4 bits of hash code into 12 simple
	const float U = H < 8 ? X : Y;							// gradient directions, and compute dot product.
	const float V = H < 4 ? Y : H == 12 || H == 14 ? X : Z;	// Fix repeats at h = 12 to 15
	return ((H & 1) ? -U : U) + ((H & 2) ? -V : V);
}

float USimplexNoiseBPLibrary::Grad(const int& Hash, const float& X, const float& Y, const float& Z, const float& T)
{
	const int H = Hash & 31;									// Convert low 5 bits of hash code into 32 simple
	const float U = H < 24 ? X : Y;							// gradient directions, and compute dot product.
	const float V = H < 16 ? Y : Z;
	const float W = H < 8 ? Z : T;
	return ((H & 1) ? -U : U) + ((H & 2) ? -V : V) + ((H & 4) ? -W : W);
}

float USimplexNoiseBPLibrary::SimplexNoise1D_Raw(const float& X)
{
	const int I0 = FASTFLOOR(X);
	const int I1 = I0 + 1;
	const float X0 = X - I0;
	const float X1 = X0 - 1.0f;

	float T0 = 1.0f - X0 * X0;
	//  if(t0 < 0.0f) t0 = 0.0f;
	T0 *= T0;
	const float N0 = T0 * T0 * Grad(Perm[I0 & 0xff], X0);

	float T1 = 1.0f - X1 * X1;
	//  if(t1 < 0.0f) t1 = 0.0f;
	T1 *= T1;
	const float N1 = T1 * T1 * Grad(Perm[I1 & 0xff], X1);
	// The maximum value of this noise is 8*(3/4)^4 = 2.53125
	// A factor of 0.395 would scale to fit exactly within [-1,1], but
	// we want to match PRMan's 1D noise, so we scale it down some more.
	return 0.25f * (N0 + N1);
}

float USimplexNoiseBPLibrary::SimplexNoise2D_Raw(const float& X, const float& Y)
{
#define F2 0.366025403f							// F2 = 0.5*(sqrt(3.0)-1.0)
#define G2 0.211324865f							// G2 = (3.0-Math.sqrt(3.0))/6.0

	float N0, N1, N2;									// Noise contributions from the three corners
	
	// Skew the input space to determine which simplex cell we're in

	const float s = (X + Y) * F2;							// Hairy factor for 2D
	const float Xs = X + s;
	const float Ys = Y + s;
	const int i = FASTFLOOR(Xs);
	const int j = FASTFLOOR(Ys);

	const float T = static_cast<float>(i + j) * G2;
	const float X0 = i - T;									// Unskew the cell origin back to (x,y) space
	const float Y0 = j - T;
	const float X1 = X - X0;									// The x,y distances from the cell origin
	const float Y1 = Y - Y0;

	// For the 2D case, the simplex shape is an equilateral triangle.
	// Determine which simplex we are in.
	int I1, J1; // Offsets for second (middle) corner of simplex in (i,j) coords
	if (X1 > Y1) { I1 = 1; J1 = 0; } // lower triangle, XY order: (0,0)->(1,0)->(1,1)
	else { I1 = 0; J1 = 1; }      // upper triangle, YX order: (0,0)->(0,1)->(1,1)

								  // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
								  // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
								  // c = (3-sqrt(3))/6

	const float X2 = X1 - I1 + G2; // Offsets for middle corner in (x,y) unskewed coords
	const float Y2 = Y1 - J1 + G2;
	const float X3 = X1 - 1.0f + 2.0f * G2; // Offsets for last corner in (x,y) unskewed coords
	const float Y3 = Y1 - 1.0f + 2.0f * G2;

	// Wrap the integer indices at 256, to avoid indexing perm[] out of bounds
	const int Ii = i & 0xff;
	const int Jj = j & 0xff;

	// Calculate the contribution from the three corners
	float T0 = 0.5f - X1 * X1 - Y1 * Y1;
	if (T0 < 0.0f) N0 = 0.0f;
	else {
		T0 *= T0;
		N0 = T0 * T0 * Grad(Perm[Ii + Perm[Jj]], X1, Y1);
	}

	float T1 = 0.5f - X2 * X2 - Y2 * Y2;
	if (T1 < 0.0) N1 = 0.0;
	else {
		T1 *= T1;
		N1 = T1 * T1 * Grad(Perm[Ii + I1 + Perm[Jj + J1]], X2, Y2);
	}

	float T2 = 0.5f - X3 * X3 - Y3 * Y3;
	if (T2 < 0.0) N2 = 0.0;
	else {
		T2 *= T2;
		N2 = T2 * T2 * Grad(Perm[Ii + 1 + Perm[Jj + 1]], X3, Y3);
	}

	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the interval [-1,1]
	return 40.0f / 0.884343445f * (N0 + N1 + N2);	//accurate to e-9 so that values scale to [-1, 1], same acc as F2 G2.
}

float USimplexNoiseBPLibrary::SimplexNoise3D_Raw(const float& X, const float& Y, const float& Z)
{

	// Simple skewing factors for the 3D case
#define F3 0.333333333f
#define G3 0.166666667f

	float N0, N1, N2, N3; // Noise contributions from the four corners

						  // Skew the input space to determine which simplex cell we're in
	const float s = (X + Y + Z) * F3; // Very nice and simple skew factor for 3D
	const float Xs = X + s;
	const float Ys = Y + s;
	const float Zs = Z + s;
	const int I = FASTFLOOR(Xs);
	const int J = FASTFLOOR(Ys);
	const int K = FASTFLOOR(Zs);

	const float T = static_cast<float>(I + J + K) * G3;
	const float X0 = I - T; // Unskew the cell origin back to (x,y,z) space
	const float Y0 = J - T;
	const float Z0 = K - T;
	const float X1 = X - X0; // The x,y,z distances from the cell origin
	const float Y1 = Y - Y0;
	const float Z1 = Z - Z0;

	// For the 3D case, the simplex shape is a slightly irregular tetrahedron.
	// Determine which simplex we are in.
	int I1, J1, K1; // Offsets for second corner of simplex in (i,j,k) coords
	int I2, J2, K2; // Offsets for third corner of simplex in (i,j,k) coords

					/* This code would benefit from a backport from the GLSL version! */
	if (X1 >= Y1) {
		if (Y1 >= Z1)
		{
			I1 = 1; J1 = 0; K1 = 0; I2 = 1; J2 = 1; K2 = 0;
		} // X Y Z order
		else if (X1 >= Z1) { I1 = 1; J1 = 0; K1 = 0; I2 = 1; J2 = 0; K2 = 1; } // X Z Y order
		else { I1 = 0; J1 = 0; K1 = 1; I2 = 1; J2 = 0; K2 = 1; } // Z X Y order
	}
	else { // x0<y0
		if (Y1 < Z1) { I1 = 0; J1 = 0; K1 = 1; I2 = 0; J2 = 1; K2 = 1; } // Z Y X order
		else if (X1 < Z1) { I1 = 0; J1 = 1; K1 = 0; I2 = 0; J2 = 1; K2 = 1; } // Y Z X order
		else { I1 = 0; J1 = 1; K1 = 0; I2 = 1; J2 = 1; K2 = 0; } // Y X Z order
	}

	// A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
	// a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
	// a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
	// c = 1/6.

	const float X2 = X1 - I1 + G3; // Offsets for second corner in (x,y,z) coords
	const float Y2 = Y1 - J1 + G3;
	const float Z2 = Z1 - K1 + G3;
	const float X3 = X1 - I2 + 2.0f * G3; // Offsets for third corner in (x,y,z) coords
	const float Y3 = Y1 - J2 + 2.0f * G3;
	const float Z3 = Z1 - K2 + 2.0f * G3;
	const float X4 = X1 - 1.0f + 3.0f * G3; // Offsets for last corner in (x,y,z) coords
	const float Y4 = Y1 - 1.0f + 3.0f * G3;
	const float Z4 = Z1 - 1.0f + 3.0f * G3;

	// Wrap the integer indices at 256, to avoid indexing perm[] out of bounds
	const int Ii = I & 0xff;
	const int Jj = J & 0xff;
	const int Kk = K & 0xff;

	// Calculate the contribution from the four corners
	float T0 = 0.6f - X1 * X1 - Y1 * Y1 - Z1 * Z1;
	if (T0 < 0.0) N0 = 0.0;
	else {
		T0 *= T0;
		N0 = T0 * T0 * Grad(Perm[Ii + Perm[Jj + Perm[Kk]]], X1, Y1, Z1);
	}

	float T1 = 0.6f - X2 * X2 - Y2 * Y2 - Z2 * Z2;
	if (T1 < 0.0) N1 = 0.0;
	else {
		T1 *= T1;
		N1 = T1 * T1 * Grad(Perm[Ii + I1 + Perm[Jj + J1 + Perm[Kk + K1]]], X2, Y2, Z2);
	}

	float T2 = 0.6f - X3 * X3 - Y3 * Y3 - Z3 * Z3;
	if (T2 < 0.0) N2 = 0.0;
	else {
		T2 *= T2;
		N2 = T2 * T2 * Grad(Perm[Ii + I2 + Perm[Jj + J2 + Perm[Kk + K2]]], X3, Y3, Z3);
	}

	float T3 = 0.6f - X4 * X4 - Y4 * Y4 - Z4 * Z4;
	if (T3 < 0.0) N3 = 0.0;
	else {
		T3 *= T3;
		N3 = T3 * T3 * Grad(Perm[Ii + 1 + Perm[Jj + 1 + Perm[Kk + 1]]], X4, Y4, Z4);
	}

	// Add contributions from each corner to get the final noise value.
	// The result is scaled to stay just inside [-1,1]
	return 32.0f * (N0 + N1 + N2 + N3); // TODO: The scale factor is preliminary!
}

float USimplexNoiseBPLibrary::SimplexNoise4D_Raw(const float& X, const float& Y, const float& Z, const float& W)
{
#define F4 0.309016994f // F4 = (Math.sqrt(5.0)-1.0)/4.0
#define G4 0.138196601f // G4 = (5.0-Math.sqrt(5.0))/20.0

	float N0, N1, N2, N3, N4; // Noise contributions from the five corners

							  // Skew the (x,y,z,w) space to determine which cell of 24 simplices we're in
	float s = (X + Y + Z + W) * F4; // Factor for 4D skewing
	float Xs = X + s;
	float Ys = Y + s;
	float Zs = Z + s;
	float Ws = W + s;
	int I = FASTFLOOR(Xs);
	int J = FASTFLOOR(Ys);
	int K = FASTFLOOR(Zs);
	int L = FASTFLOOR(Ws);

	float T = (I + J + K + L) * G4; // Factor for 4D unskewing
	float X0 = I - T; // Unskew the cell origin back to (x,y,z,w) space
	float Y0 = J - T;
	float Z0 = K - T;
	float W0 = L - T;

	float X1 = X - X0;  // The x,y,z,w distances from the cell origin
	float Y1 = Y - Y0;
	float Z1 = Z - Z0;
	float W1 = W - W0;

	// For the 4D case, the simplex is a 4D shape I won't even try to describe.
	// To find out which of the 24 possible simplices we're in, we need to
	// determine the magnitude ordering of x0, y0, z0 and w0.
	// The method below is a good way of finding the ordering of x,y,z,w and
	// then find the correct traversal order for the simplex were in.
	// First, six pair-wise comparisons are performed between each possible pair
	// of the four coordinates, and the results are used to add up binary bits
	// for an integer index.
	int C1 = (X1 > Y1) ? 32 : 0;
	int C2 = (X1 > Z1) ? 16 : 0;
	int C3 = (Y1 > Z1) ? 8 : 0;
	int C4 = (X1 > W1) ? 4 : 0;
	int C5 = (Y1 > W1) ? 2 : 0;
	int C6 = (Z1 > W1) ? 1 : 0;
	int C = C1 + C2 + C3 + C4 + C5 + C6;

	int I1, J1, K1, L1; // The integer offsets for the second simplex corner
	int I2, J2, K2, L2; // The integer offsets for the third simplex corner
	int I3, J3, K3, L3; // The integer offsets for the fourth simplex corner

						// simplex[c] is a 4-vector with the numbers 0, 1, 2 and 3 in some order.
						// Many values of c will never occur, since e.g. x>y>z>w makes x<z, y<w and x<w
						// impossible. Only the 24 indices which have non-zero entries make any sense.
						// We use a thresholding to set the coordinates in turn from the largest magnitude.
						// The number 3 in the "simplex" array is at the position of the largest coordinate.
	I1 = Simplex[C][0] >= 3 ? 1 : 0;
	J1 = Simplex[C][1] >= 3 ? 1 : 0;
	K1 = Simplex[C][2] >= 3 ? 1 : 0;
	L1 = Simplex[C][3] >= 3 ? 1 : 0;
	// The number 2 in the "simplex" array is at the second largest coordinate.
	I2 = Simplex[C][0] >= 2 ? 1 : 0;
	J2 = Simplex[C][1] >= 2 ? 1 : 0;
	K2 = Simplex[C][2] >= 2 ? 1 : 0;
	L2 = Simplex[C][3] >= 2 ? 1 : 0;
	// The number 1 in the "simplex" array is at the second smallest coordinate.
	I3 = Simplex[C][0] >= 1 ? 1 : 0;
	J3 = Simplex[C][1] >= 1 ? 1 : 0;
	K3 = Simplex[C][2] >= 1 ? 1 : 0;
	L3 = Simplex[C][3] >= 1 ? 1 : 0;
	// The fifth corner has all coordinate offsets = 1, so no need to look that up.

	float X2 = X1 - I1 + G4; // Offsets for second corner in (x,y,z,w) coords
	float Y2 = Y1 - J1 + G4;
	float Z2 = Z1 - K1 + G4;
	float W2 = W1 - L1 + G4;
	float X3 = X1 - I2 + 2.0f * G4; // Offsets for third corner in (x,y,z,w) coords
	float Y3 = Y1 - J2 + 2.0f * G4;
	float Z3 = Z1 - K2 + 2.0f * G4;
	float W3 = W1 - L2 + 2.0f * G4;
	float X4 = X1 - I3 + 3.0f * G4; // Offsets for fourth corner in (x,y,z,w) coords
	float Y4 = Y1 - J3 + 3.0f * G4;
	float Z4 = Z1 - K3 + 3.0f * G4;
	float W4 = W1 - L3 + 3.0f * G4;
	float X5 = X1 - 1.0f + 4.0f * G4; // Offsets for last corner in (x,y,z,w) coords
	float Y5 = Y1 - 1.0f + 4.0f * G4;
	float Z5 = Z1 - 1.0f + 4.0f * G4;
	float W5 = W1 - 1.0f + 4.0f * G4;

	// Wrap the integer indices at 256, to avoid indexing perm[] out of bounds
	int Ii = I & 0xff;
	int Jj = J & 0xff;
	int Kk = K & 0xff;
	int Ll = L & 0xff;

	// Calculate the contribution from the five corners
	float T0 = 0.6f - X1 * X1 - Y1 * Y1 - Z1 * Z1 - W1 * W1;
	if (T0 < 0.0) N0 = 0.0;
	else {
		T0 *= T0;
		N0 = T0 * T0 * Grad(Perm[Ii + Perm[Jj + Perm[Kk + Perm[Ll]]]], X1, Y1, Z1, W1);
	}

	float T1 = 0.6f - X2 * X2 - Y2 * Y2 - Z2 * Z2 - W2 * W2;
	if (T1 < 0.0) N1 = 0.0;
	else {
		T1 *= T1;
		N1 = T1 * T1 * Grad(Perm[Ii + I1 + Perm[Jj + J1 + Perm[Kk + K1 + Perm[Ll + L1]]]], X2, Y2, Z2, W2);
	}

	float T2 = 0.6f - X3 * X3 - Y3 * Y3 - Z3 * Z3 - W3 * W3;
	if (T2 < 0.0) N2 = 0.0;
	else {
		T2 *= T2;
		N2 = T2 * T2 * Grad(Perm[Ii + I2 + Perm[Jj + J2 + Perm[Kk + K2 + Perm[Ll + L2]]]], X3, Y3, Z3, W3);
	}

	float T3 = 0.6f - X4 * X4 - Y4 * Y4 - Z4 * Z4 - W4 * W4;
	if (T3 < 0.0) N3 = 0.0;
	else {
		T3 *= T3;
		N3 = T3 * T3 * Grad(Perm[Ii + I3 + Perm[Jj + J3 + Perm[Kk + K3 + Perm[Ll + L3]]]], X4, Y4, Z4, W4);
	}

	float T4 = 0.6f - X5 * X5 - Y5 * Y5 - Z5 * Z5 - W5 * W5;
	if (T4 < 0.0) N4 = 0.0;
	else {
		T4 *= T4;
		N4 = T4 * T4 * Grad(Perm[Ii + 1 + Perm[Jj + 1 + Perm[Kk + 1 + Perm[Ll + 1]]]], X5, Y5, Z5, W5);
	}

	// Sum up and scale the result to cover the range [-1,1]
	return 27.0f * (N0 + N1 + N2 + N3 + N4);
}

// 1D Simplex Noise
float USimplexNoiseBPLibrary::SimplexNoise1D(const float& X, const float& InFactor)
{
	return (float)SimplexNoise1D_Raw(X * InFactor);
}

// 2D Simplex Noise
float USimplexNoiseBPLibrary::SimplexNoise2D(const float& X, const float& Y, const float& InFactor)
{
	return (float)SimplexNoise2D_Raw(X * InFactor, Y * InFactor);
}

// 3D Simplex Noise
float USimplexNoiseBPLibrary::SimplexNoise3D(const float& X, const float& Y, const float& Z, const float& InFactor)
{
	return (float)SimplexNoise3D_Raw(X * InFactor, Y * InFactor, Z* InFactor);
}

// 4D Simplex Noise
float USimplexNoiseBPLibrary::SimplexNoise4D(const float& X, const float& Y, const float& Z, const float& W, const float& InFactor)
{
	return (float)SimplexNoise4D_Raw(X * InFactor, Y * InFactor, Z * InFactor, W * InFactor);
}

// Scaled by float value

float USimplexNoiseBPLibrary::SimplexNoiseScaled1D(const float& X, const float& ScaleOut, const float& InFactor)
{
	return SimplexNoise1D_Raw(X * InFactor) * ScaleOut;
}


float USimplexNoiseBPLibrary::SimplexNoiseScaled2D(const float& X, const float& Y, const float& ScaleOut, const float& InFactor)
{
	return SimplexNoise2D_Raw(X * InFactor, Y * InFactor) * ScaleOut;
}


float USimplexNoiseBPLibrary::SimplexNoiseScaled3D(const float& X, const float& Y, const float& Z, const float& ScaleOut, const float& InFactor)
{
	return SimplexNoise3D_Raw((X * InFactor), (Y * InFactor), (Z * InFactor)) * ScaleOut;
}


float USimplexNoiseBPLibrary::SimplexNoiseScaled4D(const float& X, const float& Y, const float& Z, const float& W, const float& ScaleOut, const float& InFactor)
{
	return SimplexNoise4D_Raw(X * InFactor, Y * InFactor, Z * InFactor, W * InFactor) * ScaleOut;
};

// Return value in Range between two float numbers
// Return Value is scaled by difference between rangeMin & rangeMax value

float USimplexNoiseBPLibrary::SimplexNoiseInRange1D(const float& X, const float& RangeMin, float RangeMax, const float& InFactor)
{
	if (RangeMax < RangeMin)RangeMax = RangeMin + 1.0f; // prevent negative numbers in that case we will return value between 0 - 1
	const float NoiseValue = (SimplexNoise1D(X, InFactor) + 1) * 0.5f;
	return NoiseValue * (RangeMax - RangeMin) + RangeMin;
}


float USimplexNoiseBPLibrary::SimplexNoiseInRange2D(const float& X, const float& Y, const float& RangeMin, float RangeMax, const float& InFactor)
{
	if (RangeMax < RangeMin)RangeMax = RangeMin + 1.0f; // prevent negative numbers in that case we will return value between 0 - 1
	const float NoiseValue = (SimplexNoise2D(X, Y, InFactor) + 1) * 0.5f;
	return NoiseValue * (RangeMax - RangeMin) + RangeMin;
}


float USimplexNoiseBPLibrary::SimplexNoiseInRange3D(const float& X, const float& Y, const float& Z, const float& RangeMin, float RangeMax, const float& InFactor)
{
	if (RangeMax < RangeMin)RangeMax = RangeMin + 1.0f; // prevent negative numbers in that case we will return value between 0 - 1
	const float NoiseValue = (SimplexNoise3D(X, Y, Z, InFactor) + 1) * 0.5f;
 	return NoiseValue * (RangeMax - RangeMin) + RangeMin;
}


float USimplexNoiseBPLibrary::SimplexNoiseInRange4D(const float& X, const float& Y, const float& Z, const float& W, const float& RangeMin, float RangeMax, const float&
                                                    InFactor)
{
	if (RangeMax < RangeMin)RangeMax = RangeMin + 1.0f; // prevent negative numbers in that case we will return value between 0 - 1
	const float NoiseValue = (SimplexNoise4D(X, Y, Z, W, InFactor) + 1) * 0.5f;
	return NoiseValue * (RangeMax - RangeMin) + RangeMin;
}

// Get 1D Simplex Noise ( with lacunarity, persistence, octaves )
float USimplexNoiseBPLibrary::SimplexNoise1D_Ex(const float& X, const float& Lacunarity, const float& Persistence, const int& Octaves, const float& InFactor, const
                                                   bool ZeroToOne)
{
	float Frequency = 1.0f;
	float Amplitude = 1.0f;
	float Sum = 0.0f;

	for (int Itr = 0; Itr < Octaves; Itr++) {
		Sum += SimplexNoise1D_Raw(X * InFactor * Frequency) * Amplitude;
		Frequency *= Lacunarity;
		Amplitude *= Persistence;
	}

	return ZeroToOne ? Sum * 0.5f + 0.5f : Sum;
}

// Get 2D Simplex Noise ( with lacunarity, persistence, octaves )
float USimplexNoiseBPLibrary::SimplexNoise2D_Ex(const float& X, const float& Y, const float& Lacunarity, const float& Persistence, const int& Octaves, const float&
                                                   InFactor, const bool ZeroToOne)
{
	float Frequency = 1.0f;
	float Amplitude = 1.0f;
	float Sum = 0.0f;

	for (int Itr = 0; Itr < Octaves; Itr++) {
		Sum += SimplexNoise2D_Raw(X* InFactor * Frequency, Y* InFactor * Frequency) * Amplitude;
		Frequency *= Lacunarity;
		Amplitude *= Persistence;
	}
	
	return ZeroToOne ? Sum * 0.5f + 0.5f : Sum;
}

// Get 3D Simplex Noise ( with lacunarity, persistence, octaves )
float USimplexNoiseBPLibrary::SimplexNoise3D_Ex(const float& X, const float& Y, const float& Z, const float& Lacunarity, const float& Persistence, const int& Octaves, const
                                                   float& InFactor, const bool ZeroToOne)
{
	float Frequency = 1.0f;
	float Amplitude = 1.0f;
	float Sum = 0.0f;

	for (int Itr = 0; Itr < Octaves; Itr++) {
		Sum += SimplexNoise3D_Raw(X* InFactor * Frequency, Y* InFactor * Frequency, Z* InFactor * Frequency) * Amplitude;
		Frequency *= Lacunarity;
		Amplitude *= Persistence;
	}

	return ZeroToOne ? Sum * 0.5f + 0.5f : Sum;
}

// Get Get 4D Simplex Noise ( with lacunarity, persistence, octaves )
float USimplexNoiseBPLibrary::SimplexNoise4D_Ex(const float& X, const float& Y, const float& Z, const float& W, const float& Lacunarity, const float& Persistence, const
                                                   int& Octaves, const float& InFactor, const bool ZeroToOne)
{
	float Frequency = 1.0f;
	float Amplitude = 1.0f;
	float Sum = 0.0f;

	for (int Itr = 0; Itr < Octaves; Itr++) {
		Sum += SimplexNoise4D_Raw(X* InFactor * Frequency, Y* InFactor * Frequency, Z* InFactor * Frequency, W* InFactor * Frequency) * Amplitude;
		Frequency *= Lacunarity;
		Amplitude *= Persistence;
	}

	return ZeroToOne ? Sum * 0.5f + 0.5f : Sum;
}