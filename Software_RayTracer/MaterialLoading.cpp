bool MaterialManager::AddLambertPhongMaterial(const std::string& color , int reflectivenessEnvironment)
{
    const std::string materialID = "LambertPhong_" + color + "_RE" + std::to_string(reflectivenessEnvironment);
    auto material = m_pMaterials.find(materialID); //auto iterator
    if(material == m_pMaterials.end()) //does not exist 
    {
        RGBColor diffuseColor = RGBColor{m_Lua["diffuseRGB"][color][1], m_Lua["diffuseRGB"][color][2], m_Lua["diffuseRGB"][color][3]};
        diffuseColor /= 255.f;
        const float diffuseReflectance = m_Lua["materialProperties"]["DiffuseReflectance"];
        const float specularReflectance = m_Lua["materialProperties"]["SpecularReflectance"];
        const float reflectivityEnvValue = m_Lua["materialProperties"]["reflectivityEnvironment"][reflectivenessEnvironment];
        const int phongExponent = m_Lua["materialProperties"]["PhongExponent"];
        m_pMaterials.emplace(materialID , new Material_LambertPhong(diffuseColor , diffuseReflectance , specularReflectance , phongExponent , reflectivityEnvValue));
        return true;
    }
    return false;
}

// =============================================================================
//                               Example
// =============================================================================

namespace World
{
            //adding materials
    static void CreateMaterials()
    {
          // ---- Lambert ----
        MaterialManager::GetInstance()->AddLambertMaterial("Grey" , 4);
        MaterialManager::GetInstance()->AddLambertMaterial("White");

        // ---- Lambert-Phong ----
        MaterialManager::GetInstance()->AddLambertPhongMaterial("Blue");
        MaterialManager::GetInstance()->AddLambertPhongMaterial("Green" , 8);

        // ---- Cook-Torrance Specular BRDF ----
        // -- METALS --
        MaterialManager::GetInstance()->AddPhongBRDF("Silver" , true , "Smooth");
        MaterialManager::GetInstance()->AddPhongBRDF("Gold" , true , "HalfRough");
        MaterialManager::GetInstance()->AddPhongBRDF("Zinc" , true , "Smooth" , 3);
        MaterialManager::GetInstance()->AddPhongBRDF("Copper" , true , "Smooth");

        // -- NON-METALS / DIELECTRICS --
        MaterialManager::GetInstance()->AddPhongBRDF("SkyBlue" , false , "Rough");
        MaterialManager::GetInstance()->AddPhongBRDF("PaleGreen" , false , "HalfRough" , 1);
        MaterialManager::GetInstance()->AddPhongBRDF("Orange" , false , "Smooth" , 1);
    }

    #define MATERIAL(X) MaterialManager::GetInstance()->GetMaterial( X ) //macro usage to make my life easier when adding dozens of materials
    static void CreateScene1
    {
      // Code...
      pScene->AddObject(new SphereObject(FPoint3(-2.5f , 3.5f , 0.0f) , 1.0f , MATERIAL("PhongBRDF_SkyBlue_Dielectric_Rough_RE0")));
      // Code...
    }

// Continued world creation...
} //end namespace World