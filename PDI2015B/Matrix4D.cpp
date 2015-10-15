#include "stdafx.h"
#include"Matrix4D.h"
#include<math.h>

float Dot(VECTOR4D &v1, VECTOR4D &v2) {
	float retValue = 0;
	for (int i = 0; i < 4; i++)
		retValue += v1.v[i] * v2.v[i];
	return retValue;
}

VECTOR4D Cross3(VECTOR4D &v1, VECTOR4D &v2) {
	VECTOR4D retValue;
	retValue.x = (v1.y * v2.z) - (v1.z * v2.y);
	retValue.y = (v1.z * v2.x) - (v1.x * v2.z);
	retValue.z = (v1.x * v2.y) - (v1.y * v2.x);
	retValue.w = 0;
	return retValue;
}

VECTOR4D operator*(VECTOR4D &v1, VECTOR4D &v2) {
	return Cross3(v1, v2);
}

VECTOR4D operator*(float s1, VECTOR4D &v1) {
	VECTOR4D retValue;
	for (int i = 0; i < 4; i++)
		retValue.v[i] = s1 * v1.v[i];
	return retValue;
}

VECTOR4D operator*(VECTOR4D &v1, float s1) {
	return s1*v1;
}

VECTOR4D operator+(VECTOR4D &v1, VECTOR4D &v2) {
	VECTOR4D retValue;
	for (int i = 0; i < 4; i++)
		retValue.v[i] = v1.v[i] + v2.v[i];
	return retValue;
}

float SqMag(VECTOR4D &v1) {
	return Dot(v1, v1);
}

MATRIX4D Identity() {
	MATRIX4D retValue = Zero();
	retValue.m00 = retValue.m11 = retValue.m22 = retValue.m33 = 1;
	return retValue;
}

MATRIX4D Zero() {
	MATRIX4D retValue;
	retValue = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	return retValue;
}

MATRIX4D RotationX(float theta) {
	MATRIX4D retValue = Identity();
	float cosT = cos(theta);
	float sinT = sin(theta);
	retValue.m11 = cosT;
	retValue.m12 = -sinT;
	retValue.m21 = sinT;
	retValue.m22 = cosT;
	return retValue;
}

MATRIX4D RotationY(float theta) {
	MATRIX4D retValue = Identity();
	float cosT = cos(theta);
	float sinT = sin(theta);
	retValue.m00 = cosT;
	retValue.m02 = sinT;
	retValue.m20 = -sinT;
	retValue.m22 = cosT;
	return retValue;
}

MATRIX4D RotationZ(float theta) {
	MATRIX4D retValue = Identity();
	float cosT = cos(theta);
	float sinT = sin(theta);
	retValue.m00 = cosT;
	retValue.m01 = -sinT;
	retValue.m10 = sinT;
	retValue.m11 = cosT;
	return retValue;
}

MATRIX4D operator*(MATRIX4D &m1, MATRIX4D &m2) {
	MATRIX4D retValue = Identity();
	float sum = 0;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++) {
			sum = 0;
			for (int k = 0; k < 4; k++)
				sum += (m1.m[i][k] * m2.m[k][j]);
			retValue.m[i][j] = sum;
		}
	return retValue;
}

VECTOR4D operator*(VECTOR4D &v1, MATRIX4D &m1) {
	VECTOR4D retValue;
	for (int i = 0; i < 4; i++) {
		retValue.v[i] = 0;
		for (int j = 0; j < 4; j++) {
			retValue.v[i] += v1.v[j] * m1.m[i][j];
		}
	}
	return retValue;
}

VECTOR4D operator*(MATRIX4D &m1, VECTOR4D &v1) {
	return v1*m1;
}

MATRIX4D Transpose(MATRIX4D M)
{
	MATRIX4D retValue = Zero();
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			retValue.m[i][j] = M.m[j][i];
	return retValue;
}

float Det(MATRIX4D &m1) {
	return
		m1.m03 * m1.m12 * m1.m21 * m1.m30 - m1.m02 * m1.m13 * m1.m21 * m1.m30 -
		m1.m03 * m1.m11 * m1.m22 * m1.m30 + m1.m01 * m1.m13 * m1.m22 * m1.m30 +
		m1.m02 * m1.m11 * m1.m23 * m1.m30 - m1.m01 * m1.m12 * m1.m23 * m1.m30 -
		m1.m03 * m1.m12 * m1.m20 * m1.m31 + m1.m02 * m1.m13 * m1.m20 * m1.m31 +
		m1.m03 * m1.m10 * m1.m22 * m1.m31 - m1.m00 * m1.m13 * m1.m22 * m1.m31 -
		m1.m02 * m1.m10 * m1.m23 * m1.m31 + m1.m00 * m1.m12 * m1.m23 * m1.m31 +
		m1.m03 * m1.m11 * m1.m20 * m1.m32 - m1.m01 * m1.m13 * m1.m20 * m1.m32 -
		m1.m03 * m1.m10 * m1.m21 * m1.m32 + m1.m00 * m1.m13 * m1.m21 * m1.m32 +
		m1.m01 * m1.m10 * m1.m23 * m1.m32 - m1.m00 * m1.m11 * m1.m23 * m1.m32 -
		m1.m02 * m1.m11 * m1.m20 * m1.m33 + m1.m01 * m1.m12 * m1.m20 * m1.m33 +
		m1.m02 * m1.m10 * m1.m21 * m1.m33 - m1.m00 * m1.m12 * m1.m21 * m1.m33 -
		m1.m01 * m1.m10 * m1.m22 * m1.m33 + m1.m00 * m1.m11 * m1.m22 * m1.m33;
}

MATRIX4D Inverse(MATRIX4D &M)
{
	//Author: Saul Hernandez
	// Calculates the inverse of the M matrix. First, it calculates
	// the adjoint of the M matrix for then divide each cofactor of
	// the adjoint by the determinant of the matrix.

	// Auxiliary variables:
	float	a = M.m22 * M.m33 - M.m23 * M.m32,
		b = M.m21 * M.m33 - M.m23 * M.m31,
		c = M.m21 * M.m32 - M.m22 * M.m31,
		d = M.m20 * M.m33 - M.m23 * M.m30,
		e = M.m20 * M.m32 - M.m22 * M.m30,
		f = M.m20 * M.m31 - M.m21 * M.m30,
		g = M.m12 * M.m33 - M.m13 * M.m32,
		h = M.m11 * M.m33 - M.m13 * M.m31,
		i = M.m11 * M.m32 - M.m12 * M.m31,
		j = M.m10 * M.m33 - M.m13 * M.m30,
		k = M.m10 * M.m32 - M.m12 * M.m30,
		l = M.m10 * M.m31 - M.m11 * M.m30,
		m = M.m12 * M.m23 - M.m13 * M.m22,
		n = M.m11 * M.m23 - M.m13 * M.m21,
		o = M.m11 * M.m22 - M.m12 * M.m21,
		p = M.m10 * M.m23 - M.m13 * M.m20,
		q = M.m10 * M.m22 - M.m12 * M.m20,
		r = M.m10 * M.m21 - M.m11 * M.m20;

	// Calculate cofactors
	float	c00 = +M.m11 * a - M.m12 * b + M.m13 * c,
		c01 = -M.m10 * a + M.m12 * d - M.m13 * e,
		c02 = +M.m10 * b - M.m11 * d + M.m13 * f,
		c03 = -M.m10 * c + M.m11 * e - M.m12 * f,
		c10 = -M.m01 * a + M.m02 * b - M.m03 * c,
		c11 = +M.m00 * a - M.m02 * d + M.m03 * e,
		c12 = -M.m00 * b + M.m01 * d - M.m03 * f,
		c13 = +M.m00 * c - M.m01 * e + M.m02 * f,
		c20 = +M.m01 * g - M.m02 * h + M.m03 * i,
		c21 = -M.m00 * g + M.m02 * j - M.m03 * k,
		c22 = +M.m00 * h - M.m01 * j + M.m03 * l,
		c23 = -M.m00 * i + M.m01 * k - M.m02 * l,
		c30 = -M.m01 * m + M.m02 * n - M.m03 * o,
		c31 = +M.m00 * m - M.m02 * p + M.m03 * q,
		c32 = -M.m00 * n + M.m01 * p - M.m03 * r,
		c33 = +M.m00 * o - M.m01 * q + M.m02 * r,
		det = Det(M);

	// Divide each element of the adjoint of the matrix (i.e. each cofactor) by the determinant
	MATRIX4D mat4 =
	{
		c00 / det, c10 / det, c20 / det, c30 / det,
		c01 / det, c11 / det, c21 / det, c31 / det,
		c02 / det, c12 / det, c22 / det, c32 / det,
		c03 / det, c13 / det, c23 / det, c33 / det
	};
	return mat4;
}

MATRIX4D Translate(float dx, float dy, float dz)
{
	MATRIX4D retValue = Identity();
	retValue.m30 = dx;
	retValue.m31 = dy;
	retValue.m32 = dz;
	return retValue;
}
MATRIX4D Scale(float sx, float sy, float sz)
{
	MATRIX4D retValue = Identity();
	retValue.m00 = sx;
	retValue.m11 = sy;
	retValue.m22 = sz;
	return retValue;
}