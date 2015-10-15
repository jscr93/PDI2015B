#pragma once

struct VECTOR4D
{
	union
	{
		struct
		{
			float x, y, z, w;
		};
		float v[4];
	};
};
struct MATRIX4D
{
	union
	{
		struct
		{
			float m00, m01, m02, m03;
			float m10, m11, m12, m13;
			float m20, m21, m22, m23;
			float m30, m31, m32, m33;
		};
		float m[4][4];
		float v[16];
	};

};

//operaciones aritmeticas

float Dot(VECTOR4D &v1, VECTOR4D &v2);
VECTOR4D Cross3(VECTOR4D &v1, VECTOR4D &v2);
VECTOR4D operator*(VECTOR4D &v1, VECTOR4D &v2);
VECTOR4D operator*(float s1, VECTOR4D &v1);
VECTOR4D operator*(VECTOR4D &v1, float s1);
VECTOR4D operator+(VECTOR4D &v1, VECTOR4D &v2);
float SqMag(VECTOR4D &v1);//Dot(A,A);

						  //operaciones con matrices y vectores a implementar
MATRIX4D Identity();
MATRIX4D Zero();

MATRIX4D RotationX(float theta);//left handled
MATRIX4D RotationY(float theta);//left handled
MATRIX4D RotationZ(float theta);//left handled
MATRIX4D operator*(MATRIX4D &m1, MATRIX4D &m2);
VECTOR4D operator*(VECTOR4D &v1, MATRIX4D &m1);
VECTOR4D operator*(MATRIX4D &m1, VECTOR4D &v1);
MATRIX4D Transpose(MATRIX4D m1);
float Det(MATRIX4D &m1);
MATRIX4D Inverse(MATRIX4D &m1);

MATRIX4D Translate(float dx, float dy, float dz);
MATRIX4D Scale(float sx, float sy, float sz);