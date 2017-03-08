#pragma once

#include <cstdint>
#include <cassert>
#include <cmath>
#include <algorithm>

namespace srend
{

template<typename T, int d> struct Vector {
	static constexpr const int size = d;

	inline Vector() {}

	inline Vector(const Vector<T, d>& v) {
		for (int i = 0; i < d; i++) {
			data[i] = v.data[i];
		}
	}

	explicit inline Vector(const T& s) {
		for (int i = 0; i < d; i++) {
			data[i] = s;
		}
	}

	explicit inline Vector(const T* a) {
		for (int i = 0; i < d; i++) {
			data[i] = a[i];
		}
	}

	inline Vector(const T& s0, const T& s1) {
		static_assert(d == 2, "size needs to be at least 2");
		data[0] = s0;
		data[1] = s1;
	}

	inline Vector(const T& s0, const T& s1, const T& s2) {
		static_assert(d == 3, "size needs to be at least 3");
		data[0] = s0;
		data[1] = s1;
		data[2] = s2;
	}

	inline Vector(const T& s0, const T& s1, const T& s2, const T& s3) {
		static_assert(d == 4, "size needs to be equal 4");
		data[0] = s0;
		data[1] = s1;
		data[2] = s2;
		data[3] = s3;
	}

	inline operator T* () { return data; }

	inline operator const T* () const { return data; }

	inline T& operator[] (const int i) {
		return data[i];
	}

	inline const T& operator[] (const int i) const {
		return data[i];
	}

	inline T& x() {
		static_assert(d > 0, "invalid size");
		return data[0];
	}

	inline T& y() {
		static_assert(d > 1, "invalid size");
		return data[1];
	}

	inline T& z() {
		static_assert(d > 2, "invalid size");
		return data[2];
	}

	inline T& w() {
		static_assert(d > 3, "invalid size");
		return data[3];
	}

	inline const T& x() const {
		static_assert(d > 0, "invalid size");
		return data[0];
	}

	inline const T& y() const {
		static_assert(d > 1, "invalid size");
		return data[1];
	}

	inline const T& z() const {
		static_assert(d > 2, "invalid size");
		return data[2];
	}

	inline const T& w() const {
		static_assert(d > 3, "invalid size");
		return data[3];
	}

	inline Vector<T, 4> xyzw() {
		static_assert(d == 4, "invalid size");
		return Vector<T, 4>(x(), y(), z(), w());
	}

	inline const Vector<T, 4> xyzw() const {
		static_assert(d == 4, "invalid size");
		return Vector<T, 4>(x(), y(), z());
	}

	inline Vector<T, 3> xyz() {
		static_assert(d > 3, "invalid size");
		return Vector<T, 3>(x(), y(), z());
	}

	inline const Vector<T, 3> xyz() const {
		static_assert(d > 3, "invalid size");
		return Vector<T, 3>(x(), y(), z());
	}

	inline Vector<T, 2> xy() {
		static_assert(d > 2, "invalid size");
		return Vector<T, 2>(x(), y());
	}

	inline const Vector<T, 2> xy() const {
		static_assert(d > 2, "invalid size");
		return Vector<T, 2>(x(), y());
	}

	inline T& r() {
		static_assert(d > 0, "invalid size");
		return data[0];
	}

	inline T& g() {
		static_assert(d > 1, "invalid size");
		return data[1];
	}

	inline T& b() {
		static_assert(d > 2, "invalid size");
		return data[2];
	}

	inline T& a() {
		static_assert(d > 3, "invalid size");
		return data[3];
	}

	inline const T& r() const {
		static_assert(d > 0, "invalid size");
		return data[0];
	}

	inline const T& g() const {
		static_assert(d > 1, "invalid size");
		return data[1];
	}

	inline const T& b() const {
		static_assert(d > 2, "invalid size");
		return data[2];
	}

	inline const T& a() const {
		static_assert(d > 3, "invalid size");
		return data[3];
	}

	inline Vector<T, 4> rgba() {
		static_assert(d == 4, "invalid size");
		return Vector<T, 4>(x(), y(), z(), w());
	}

	inline const Vector<T, 4> rgba() const {
		static_assert(d == 4, "invalid size");
		return Vector<T, 4>(x(), y(), z(), w());
	}

	inline Vector<T, 4> argb() {
		static_assert(d == 4, "invalid size");
		return Vector<T, 4>(w(), x(), y(), z());
	}

	inline const Vector<T, 4> argb() const {
		static_assert(d == 4, "invalid size");
		return Vector<T, 3>(w(), x(), y(), z());
	}

	inline Vector<T, 4> bgra() {
		static_assert(d == 4, "invalid size");
		return Vector<T, 4>(z(), y(), x(), w());
	}

	inline const Vector<T, 4> bgra() const {
		static_assert(d == 4, "invalid size");
		return Vector<T, 4>(z(), y(), x(), w());
	}

	inline Vector<T, 3> rgb() {
		static_assert(d > 3, "invalid size");
		return Vector<T, 3>(x(), y(), z());
	}

	inline const Vector<T, 3> rgb() const {
		static_assert(d > 3, "invalid size");
		return Vector<T, 3>(x(), y(), z());
	}

	inline bool operator==(const Vector<T, d>& v) const {
		for (int i = 0; i < d; ++i) {
			if ((*this)[i] != v[i]) return false;
		}
		return true;
	}

	inline bool operator!=(const Vector<T, d>& v) const { return !operator==(v); }

	inline Vector<T, d> operator+(const Vector<T, d>& v) const {
		Vector<T, d> result;
		for (int i = 0; i < d; ++i) {
			result.data[i] = data[i] + v.data[i];
		}
		return result;
	}

	inline Vector<T, d> operator-(const Vector<T, d>& v) const {
		Vector<T, d> result;
		for (int i = 0; i < d; ++i) {
			result[i] = data[i] - v[i];
		}
		return result;
	}

	inline Vector<T, d> operator*(const Vector<T, d>& v) const {
		Vector<T, d> result;
		for (int i = 0; i < d; ++i) {
			result[i] = data[i] * v[i];
		}
		return result;
	}

	inline Vector<T, d> operator/(const Vector<T, d>& v) const {
		Vector<T, d> result;
		for (int i = 0; i < d; ++i) {
			result[i] = data[i] / v[i];
		}
		return result;
	}

	inline Vector<T, d> operator+(const T& s) const {
		Vector<T, d> result;
		for (int i = 0; i < d; ++i) {
			result[i] = data[i] + s;
		}
		return result;
	}

	inline Vector<T, d> operator-(const T& s) const {
		Vector<T, d> result;
		for (int i = 0; i < d; ++i) {
			result[i] = data[i] - s;
		}
		return result;
	}

	inline Vector<T, d> operator*(const T& s) const {
		Vector<T, d> result;
		for (int i = 0; i < d; ++i) {
			result[i] = data[i] * s;
		}
		return result;
	}

	inline Vector<T, d> operator/(const T& s) const {
		Vector<T, d> result;
		for (int i = 0; i < d; ++i) {
			result[i] = data[i] / s;
		}
		return result;
	}

	inline Vector<T, d> operator+=(const Vector<T, d>& v) const {
		for (int i = 0; i < d; ++i) {
			data[i] += v.data[i];
		}
		return *this;
	}

	inline Vector<T, d> operator-=(const Vector<T, d>& v) const {
		for (int i = 0; i < d; ++i) {
			data[i] += v.data[i];
		}
		return *this;
	}

	inline Vector<T, d> operator*=(const Vector<T, d>& v) const {
		for (int i = 0; i < d; ++i) {
			data[i] += v.data[i];
		}
		return *this;
	}

	inline Vector<T, d> operator/=(const Vector<T, d>& v) const {
		for (int i = 0; i < d; ++i) {
			data[i] += v.data[i];
		}
		return *this;
	}



	inline Vector<T, d> operator+=(const T& s) const {
		for (int i = 0; i < d; ++i) {
			data[i] += s;
		}
		return *this;
	}

	inline Vector<T, d> operator-=(const T& s) const {
		for (int i = 0; i < d; ++i) {
			data[i] -= s;
		}
		return *this;
	}

	inline Vector<T, d> operator*=(const T& s) const {
		for (int i = 0; i < d; ++i) {
			data[i] *= s;
		}
		return *this;
	}

	inline Vector<T, d> operator/=(const T& s) const {
		for (int i = 0; i < d; ++i) {
			data[i] /= s;
		}
		return *this;
	}

	static inline T DotProduct(const Vector<T, d>& v1, const Vector<T, d>& v2) {
		T result = 0;
		for (int i = 0; i < d; ++i) {
			result[i] += v1[i] * v2[i];
		}
		return result;
	}

	static inline Vector<T, 3> CrossProduct(const Vector<T, 3>& v1, const Vector<T, 3>& v2) {
		return Vector<T, 3>(v1[1] * v2[2] - v1[2] * v2[1],
							v1[2] * v2[0] - v1[0] * v2[2],
							v1[0] * v2[1] - v1[1] * v2[0]);
	}

protected:
	T data[d];
};

typedef Vector<uint8_t, 4> color_rgba8;
typedef Vector<float, 3> color_rgbf;
typedef Vector<float, 4> color_rgbaf;

typedef Vector<int32_t, 2> vec2i;
typedef Vector<int32_t, 3> vec3i;
typedef Vector<float, 2> vec2f;
typedef Vector<float, 3> vec3f;
typedef Vector<float, 4> vec4f;

} // namespace srend