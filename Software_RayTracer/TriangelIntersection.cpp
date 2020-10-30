/// @brief check if the given ray intersects the given triangle, and if it does, return true and modify the hitrecord
/// @param ray the ray that is being cast and checked
/// @param hitRecord information (tvalue, hitpoint, normal, material) to fill in if hit is true
/// @return true if ray intersects with triangle
bool TriangleObject::Hit(const Ray& ray , HitRecord& hitRecord , bool isShadow) const
{
    const float vDotN = Dot(ray.direction , m_Normal);

    //opposite culling mode when calculating shadows
    if(isShadow) vDotN *= -1;

    //check if same or opposite direction with normal and view dir, and depending on the mode, make it "invisible" or not
    switch(m_CullMode)
    {
        case CullMode::backFace:
              //If positive we are looking at the back
            if(vDotN < 0.f) break;
            else return false;
        case CullMode::frontFace:
              //If negative then we are looking to the front
            if(vDotN > 0.f) break;
            else return false;
        case CullMode::none: //do nothing
            break;
        default: //do nothing
            break;
    }

    //RESET (see above)
    if(isShadow) vDotN *= -1;

    //if ray and normal of triangle are parallel, then there is no triangle hit 
    if(AreEqual(vDotN , 0.0f)) return false; //--> are equal manages floats to be close to the value

    FPoint3 center{0,0,0};
    for(auto& vertex : m_Vertices)
    {
        center += vertex + FVector3(m_Position);
    }

    center = static_cast<FPoint3>(static_cast<FVector3>(center) / 3.0f);

    const float t = Dot((center - ray.origin) , m_Normal) / vDotN;

    //check if the previous hit was closer or if it's too close
    if(t < ray.tMin || t > ray.tMax) return false;
    //check if the previous hit was closer
    if(t > hitRecord.tValue && !isShadow) return false;

    //get hitpoint
    const FPoint3 hitpoint = ray.origin + t * ray.direction;

    //we check for the 3 triangle edges if the point is on the "correct" side of the triangle, if it is the same for all triangles, it's in the triangle

    // - first edge -
    FVector3 edge = (m_Position + m_Vertices[1]) - (m_Position + m_Vertices[0]);

    FVector3 sideToPoint = (m_Position + m_Vertices[0]) - hitpoint;

    if(Dot(m_Normal , Cross(edge , sideToPoint)) < 0.0f) return false;

    // - second edge -
    edge = (m_Position + m_Vertices[2]) - (m_Position + m_Vertices[1]);

    sideToPoint = (m_Position + m_Vertices[1]) - hitpoint;

    if(Dot(m_Normal , Cross(edge , sideToPoint)) < 0.0f) return false;

    // - third edge -
    edge = (m_Position + m_Vertices[0]) - (m_Position + m_Vertices[2]);

    sideToPoint = (m_Position + m_Vertices[2]) - hitpoint;

    if(Dot(m_Normal , Cross(edge , sideToPoint)) < 0.0f) return false;

    //fill in hitrecord
    hitRecord.tValue = t;
    hitRecord.hitpoint = hitpoint;
    hitRecord.normal = m_Normal;
    hitRecord.material = m_pMaterial;

    return true;
}