/// @see <Material_Lambert>::<Shade>
const RGBColor Material_PhongBRDF::Shade(const HitRecord& hitrecord , const FVector3& lightDirection , const FVector3& viewDirection) const
{
      //base reflectivity of the surface (F0) is already determined through the constructor and initializer list
    const FVector3 halfVector{GetNormalized(viewDirection + lightDirection)};
    const RGBColor F{BRDF::Schlick(halfVector , viewDirection , m_Albedo)};
    const float D{BRDF::TrowbridgeReitzGGX(hitrecord.normal,halfVector, m_Roughness)};
    const float k{Square(m_Roughness * m_Roughness + 1.f) / 8.f}; // how the formula is --> not really magic numbers
    const float G{BRDF::SmithMethod(hitrecord.normal , viewDirection , lightDirection , k)};
    const RGBColor kd = (RGBColor{1.0f, 1.0f, 1.0f} - F) * (!m_IsMetalness);

    const RGBColor specular = (F * D * G) /
        (
            4.0f //formula
            * std::max(Dot(viewDirection , hitrecord.normal) , std::nextafter(0.f , 1.f))
            * std::max(Dot(lightDirection , hitrecord.normal) , std::nextafter(0.f , 1.f))
            );

    const RGBColor diffuse = BRDF::Lambert(m_DiffuseColor , kd);
    return specular + diffuse;
}

namespace BRDF
{
      /// @brief Lambert BRDF function , ideal for basic matte objects
      /// @note theory: in other words, all incident radiance is scattered equally in all directions over a hemisphere
      /// @param diffuseColor the color of a material
      /// @param diffuseReflectance how reflective the (matte) material is. In the range of [0, 1]
      /// @return a constant perfect diffuse reflection
    inline static const RGBColor Lambert(const RGBColor& diffuseColor , const float diffuseReflectance)
    {
        return (diffuseColor * diffuseReflectance) / static_cast<float>(E_PI);
    }

    /// @brief Lambert BRDF function , ideal for basic matte objects
    /// @note theory: in other words, all incident radiance is scattered equally in all directions over a hemisphere
    /// @param diffuseColor the color of a material
    /// @param diffuseReflectance how reflective the (matte) material is. In the range of [0, 1]
    /// @return a constant perfect diffuse reflection
    inline static const RGBColor Lambert(const RGBColor& diffuseColor , const RGBColor& diffuseReflectance)
    {
        return (diffuseColor * diffuseReflectance) / static_cast<float>(E_PI);
    }

    /// @brief BRDF - Specular: Phong model, calculate the phong specular reflection
    /// @param specularReflectanceFactor how specular you want something to be
    /// @param phongExponent changing this value changes how much specular you see (bigger,smaller)
    /// @param lightDir the light direction
    /// @param viewDir  the view direction
    /// @param surfaceNormal the normal of the surface
    /// @return the color of the specular reflection
    inline static const RGBColor Phong(float specularReflectanceFactor , int phongExponent , const FVector3& lightDir , const FVector3& viewDir , const FVector3& surfaceNormal)
    {
        const FVector3 reflect = -lightDir + (2.f * Dot(lightDir , surfaceNormal) * surfaceNormal);
        float cosAngle = Dot(GetNormalized(reflect) , viewDir);

        //if the reflect and viewdir are not perpendicular or orthogonal (not equal to zero) and positive angle
        if(cosAngle > 0.0f)
        {
            const float phongSpecularReflection = specularReflectanceFactor * static_cast<float>(pow(cosAngle , phongExponent));
            return RGBColor{phongSpecularReflection,phongSpecularReflection,phongSpecularReflection};
        }
        return RGBColor{};
    }

    /// @brief normal distribution function using the Trowbridge-Reitz GGX way
    /// @see other methods of doing normal distribution: http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
    /// @param normalSurface normal of the surface / hitpoint
    /// @param halfVector the half vector between the light and view direction
    /// @param rougness rougness value of the surface
    /// @return normal distribution -> how many microfacets point in the right direction
    inline static const float TrowbridgeReitzGGX(const FVector3& normalSurface , const FVector3& halfVector , float roughness)
    {
        const float roughnessSqSq{Square(Square(roughness))};
        const float nDotH{std::max(Dot(normalSurface , halfVector) , std::nextafter(0.f , 1.f))};
        return roughnessSqSq / (static_cast<float>(E_PI) * Square((Square(nDotH) * (roughnessSqSq - 1.f) + 1.f)));
    }

    /// @brief Fresnel function using the Schlick way
    /// @see other methods of doing Fresnel: http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
    /// @param halfVector the half vector between the light and view direction
    /// @param viewDir the view direction from the eye/camera/view
    /// @param baseReflectivitySurface the base reflectivity of the surface (0.04,0.04, 0.04) for dielectrics or the albedo value for metals. for example, silver(0.95, 0.93, 0.88) --> the color you want
    /// @return the specular reflectivity color
    inline static const RGBColor Schlick(const FVector3& halfVector , const FVector3& viewDir , const RGBColor& baseReflectivitySurface)
    {
        const float hDotV{std::max(Dot(halfVector , viewDir) , std::nextafter(0.f , 1.f))};
        return baseReflectivitySurface + ((RGBColor{1.0f,1.0f ,1.0f} - baseReflectivitySurface) * powf(1.f - hDotV , 5));
    }

    /// @brief a Fresnel function from Schlich that describes the reflectivity of the microfacets
    /// @param halfVector the half vector between the view direction and the light direction
    /// @param viewDir the view direction
    /// @param k the base reflectivity of the surface -> (0.04,0.04,0.04) for dielectrics or the albedo value for the metals.
    /// @return a float describing the reflectivity of the microfacets
    inline static const float SchlickGGX(const FVector3& halfVector , const FVector3& viewDir , float k)
    {
        const float nDotV = std::max(Dot(GetNormalized(halfVector) , GetNormalized(viewDir)) , std::nextafter(0.f , 1.f));
        return nDotV / (nDotV * (1.0f - k) + k);
    }

    /// @brief Smith method: using the Schlick method for both the shadowing using the light direction and the masking using the view direction and multiplying them
    /// @param normalSurface the normal of the surface/hitpoint
    /// @param viewDir the view direction
    /// @param lightDir the light direction
    /// @param k the roughness reampped based on whether you use the function with direct or indirect lighting. (using direct lighting ATM)
    /// @return it returns a float that describes the overshadowing of microfacets
    inline static const float SmithMethod(const FVector3& normalSurface , const FVector3& viewDir , const FVector3& lightDir , float k)
    {
        return SchlickGGX(normalSurface , viewDir , k) * SchlickGGX(normalSurface , lightDir , k);
    }
}