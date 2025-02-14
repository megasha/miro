#ifndef CSE168_MATERIAL_H_INCLUDED
#define CSE168_MATERIAL_H_INCLUDED

#include "Miro.h"
#include "Vector3.h"

class Material
{
public:
    Material();
    virtual ~Material();

    virtual void preCalc() {}
	virtual Vector3 getKd() { return Vector3(0.0f); }
	virtual float getRefrac() { return 0.0f; }
	virtual Vector3 getGlass() { return Vector3(0.0f); }
    
    virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          Scene& scene) const;
};

#endif // CSE168_MATERIAL_H_INCLUDED
