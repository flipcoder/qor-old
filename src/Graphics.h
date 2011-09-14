#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include "GfxAPI.h"
#include "Util.h"
#include "math/common.h"

class Color
{
	float saturate(float f){
		if(f>1.0f)
			f=1.0f;
		else if(f<0.0f)
			f=0.0f;
		return f;
	}
	
public:

	union {
		struct { float r, g, b, a; };
		float c[4];
	};

	Color():
		r(1.0f),
		g(1.0f),
		b(1.0f),
		a(1.0f) {}

	Color(const Color& c){
		r = c.r;
		g = c.g;
		b = c.b;
		a = c.a;
	}
	explicit Color(float s, float _a = 1.0f):
		r(s),
		g(s),
		b(s),
		a(_a) {}
	Color(float _r, float _g, float _b, float _a = 1.0f):
		r(_r),
		g(_g),
		b(_b),
		a(_a)
	{}
	explicit Color(unsigned char s, unsigned char _a = 255):
		r(s/255.0f),
		g(s/255.0f),
		b(s/255.0f),
		a(_a/255.0f)
	{}
	Color(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a = 255):
		r(_r/255.0f),
		g(_g/255.0f),
		b(_b/255.0f),
		a(_a/255.0f)
	{}

	Color& operator+=(float f) {
		r+=f;
		b+=f;
		g+=f;
		saturate();
		return *this;
	}

	Color& operator-=(float f) {
		r-=f;
		b-=f;
		g-=f;
		saturate();
		return *this;
	}

	Color operator*=(float f) {
		r*=f;
		b*=f;
		g*=f;
		a*=f;
		saturate();
		return *this;
	}
	Color operator*=(Color& c) {
		r*=c.r;
		b*=c.b;
		g*=c.g;
		a*=c.a;
		saturate();
		return *this;
	}
	
	void saturate() {
		r = saturate(r);
		b = saturate(b);
		g = saturate(g);
		a = saturate(a);
	}

	void set(float _c) { r=g=b=saturate(_c); }
	void set(float _c, float _a) { r=g=b=saturate(_c); a=saturate(_a); }
	void set(float _r, float _g, float _b) {
		r=_r; b=_b; g=_g; saturate();
	}
	void set(float _r, float _g, float _b, float _a) {
		r=_r; b=_b; g=_g; a=_a; saturate();
	}
	void white() { r=g=b=1.0f; }
	void black() { r=g=b=0.0f; }
	void white(float _a) { r=g=b=1.0f; a=saturate(_a); }
	void black(float _a) { r=g=b=0.0f; a=saturate(_a); }
	unsigned char getRedUB() const { return (unsigned char)round_int(255*r); }
	unsigned char getGreenUB() const { return (unsigned char)round_int(255*g); }
	unsigned char getBlueUB() const { return (unsigned char)round_int(255*b); }
	float* array() const { return (float*)&c[0]; }

	void glColor() const { glColor4fv(array()); }
	void glColor(float _a) const { glColor4f(r, g, b, _a); }
	void apply() const { glColor4fv(array()); }
	void apply(float _a) const { glColor4f(r, g, b, _a); }
	static void apply(Color c) {
		glColor4fv(c.array());
	}
};

//void gfxColor(float r, float g, float b);
//void gfxColor(float r, float g, float b, float a);
//void gfxColor(const Color& c);
//void gfxCurrentColor(const Color& c);

////bool operator== (const Color& c1, const Color& c2)
//{
//	return (floatcmp(c1.r, c2.r) &&
//			floatcmp(c1.b, c2.b) &&
//			floatcmp(c1.g, c2.g) &&
//			floatcmp(c1.a, c2.a));
//}

//class AARect
//{
//    float x,y,w,h;

//    Rect(float _x = 0.0f,
//         float _y = 0.0f,
//         float _w = 0.0f,
//         float _h = 0.0f):
//        x(_x),
//        y(_y),
//        w(_w),
//        h(_h)
//    {}

//    void resize(float _s) {
//        w=h=_s;
//    }

//    void resize(float _w, float _h){
//        w=_w; h=_h;
//    }

//    bool operator ==(const Rect& r) const {
//        return floatcmp(x, r.x) &&
//               floatcmp(y, r.y) &&
//               floatcmp(w, r.w) &&
//               floatcmp(h, r.h);
//    }

//    //bool operator <(const Rect& r) const {
//    //    return r > *this;
//    //}
//    //bool operator <=(const Rect& r) const {
//    //    return r >= *this;
//    //}
	
//    //bool operator >(const Rect& r) const {
//    //    if(!floatcmp(y, r.y)) {
//    //        if(y > r.y)
//    //            return true;
//    //        else
//    //            return false;
//    //    }
//    //    else if(!floatcmp(x, r.x))
//    //        if(x > r.x)
//    //            return true;
//    //        else
//    //            return false;
//    //    }
//    //    return false;
//    //}

//    //bool operator >=(const Rect& r) const {
//    //    if(!floatcmp(y, r.y)) {
//    //        if(y > r.y)
//    //            return true;
//    //        else
//    //            return false;
//    //    }
//    //    else if(!floatcmp(x, r.x))
//    //        if(x > r.x)
//    //            return true;
//    //        else
//    //            return false;
//    //    }
//    //    return true;
//    //}
//};

struct AABB
{
    glm::vec3 min;
    glm::vec3 max;

	AABB() {
		reset();
	}

	bool valid() const {
		return min.x <= max.x && min.y <= max.y && min.z <= max.z;
	}
	
	float area() const {
		glm::vec3 diff = max - min;
		return std::fabs(diff.x * diff.y * diff.z);
	}
	float areaXZ() const {
		glm::vec3 diff = max - min;
		return std::fabs(diff.x * diff.z);
	}
	
	void reset() {
		min = glm::vec3(std::numeric_limits<float>::max());
		max = glm::vec3(std::numeric_limits<float>::min());
	}

	bool inside(const glm::vec3& point) const {
		if ((min.x > point.x)||
			(min.y > point.y)||
			(min.z > point.z)||
			(point.x > max.x - EPSILON)||
			(point.y > max.y - EPSILON)||
			(point.z > max.z - EPSILON))
		{
			return false;
		}
		return true;
	}
	
	bool vLineInside(const glm::vec2& line) const {
		if ((min.x > line.x)||
			//(min.y > line.y)||
			(min.z > line.y)||
			(line.x > max.x - EPSILON)||
			//(line.y > max.y - EPSILON)||
			(line.y > max.z - EPSILON))
		{
			return false;
		}
		return true;
	}
};

#endif

