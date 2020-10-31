/// @brief render loop of the software
void Renderer::RenderSoftware()
{
    SDL_LockSurface(m_pBackBuffer);
    RGBColor targetColor;
    software::VS_OUTPUT vertexOUT;
    std::vector<software::VS_OUTPUT> transformedVertices;
    std::vector<uint32_t> indices;
    size_t sizeIndexBuffer = 0;

    ClearBackBuffer();

    //go over all the triangles
    for(Primitive* primitive : SceneManager::GetInstance()->GetActiveScene()->GetPrimitives())
    {
        transformedVertices.clear();

        //get Indices
        indices = primitive->GetMesh()->pMeshData->indexBufferSR;

        //get topology
        const PrimitiveTopology currentTopology = primitive->GetTopology();

        //store resulting verts from transformations (world,camera,ndc,raster)
        VertexTransformationFunction(primitive->GetMesh()->pMeshData->vertexBufferSR
            , transformedVertices , primitive->GetWorldMatrix());

//set index buffer size based on topology
        const sizeIndexBuffer = (currentTopology == PrimitiveTopology::TriangleList) ? indices.size() : indices.size() - 2;

        //set looping based on topology
        const int increaseIndexValue = (currentTopology == PrimitiveTopology::TriangleList) ? 3 : 1;

        //for every triangle
        for(uint64_t i = 0; i < sizeIndexBuffer; i += increaseIndexValue)
        {
            const uint64_t evenIndex = (currentTopology == PrimitiveTopology::TriangleStrip) ? (i % 2) : 0;

            //set index based on topology
            const uint64_t index0 = indices[i] , index1 = indices[i + static_cast<uint64_t>(1)
                + evenIndex] , index2 = indices[i + static_cast<uint64_t>(2) - evenIndex];

                            //get 3 triangle verts
            FPoint4 vertex0{transformedVertices[index0].position} ,
                vertex1{transformedVertices[index1].position} , vertex2{transformedVertices[index2].position};

                            //check if triangle is outside frustum
            if(GameManager::GetInstance()->GetFrustumCullingMode() == FrustumCullingMode::OneVertexMode)
            {
                if(!GetIsPointInsideFrustum(vertex0) && !GetIsPointInsideFrustum(vertex1)
                    && !GetIsPointInsideFrustum(vertex2)) continue;
            }
            else
            {
                if(!GetIsPointInsideFrustum(vertex0) || !GetIsPointInsideFrustum(vertex1)
                    || !GetIsPointInsideFrustum(vertex2)) continue;
            }

            //NDC to raster
            NDCToRaster(vertex0);
            NDCToRaster(vertex1);
            NDCToRaster(vertex2);

            const float areaParallelogram = Cross(FVector2(vertex2 - vertex0) , FVector2(vertex1 - vertex0));

            //if any of the triangles, the 2 vectors are on top of each other 
                                                //-> triangle doesn't exist -> go to next triangle
            if(abs(areaParallelogram) < FLT_EPSILON) continue;

            //culling, if area is under 0 -> backface culling 
                                                // if area above 0, front face culling

            switch(primitive->GetModelCullMode())
            {

                case ModelCullingMode::backface:
                    if(areaParallelogram < 0.0f) continue;
                    break;
                case ModelCullingMode::frontface:
                    if(areaParallelogram > 0.0f) continue;
                    break;
                case ModelCullingMode::noCulling:
                      //do nothing
                    break;
                default:
                      //do nothing
                    break;
            }


            BoundingBoxTriangle boundingBox = GetBoundingBox(vertex0 , vertex1 , vertex2);

            //inverse depth vertex
            const float vertex0InvDepth = 1.0f / vertex0.w;
            const float vertex1InvDepth = 1.0f / vertex1.w;
            const float vertex2InvDepth = 1.0f / vertex2.w;

                              //loop over boundingbox
            for(uint64_t r = static_cast<uint64_t>(boundingBox.topLeft.y); (int) r < boundingBox.bottomRight.y && r < m_Height; ++r)
            {
                for(uint64_t c = static_cast<uint64_t>(boundingBox.topLeft.x); (int) c < boundingBox.bottomRight.x && c < m_Width; ++c)
                {
                    const FPoint2 pixel = FPoint2((float) c , (float) r);

                    //get sign of each side to check wither pixel is on triangle
                    float weight0 = GetWeight(vertex1 , vertex2 , pixel);
                    float weight1 = GetWeight(vertex2 , vertex0 , pixel);
                    float weight2 = GetWeight(vertex0 , vertex1 , pixel);

                    //check if inside triangle
                    if((weight0 * areaParallelogram < 0.0f || weight1 * areaParallelogram < 0.0f || weight2 * areaParallelogram < 0.0f)) continue;

                    //get barycentric coordinates
                    weight0 /= areaParallelogram;
                    weight1 /= areaParallelogram;
                    weight2 /= areaParallelogram;

                                                  //get inverse interpolated zBuffer
                    const float zBuffer =
                        1 / (
                            (1.0f / vertex0.z * weight0) +
                            (1.0f / vertex1.z * weight1) +
                            (1.0f / vertex2.z * weight2));

                    if(zBuffer > 1.0f || zBuffer < 0.0f) continue;

                    //check if new depth value is closer to camera
                    if((zBuffer >= m_DepthBuffer[c + static_cast<uint64_t>(r * static_cast<uint64_t>(m_Width))])) continue;

                    if(primitive->GetShouldWriteDepthBuffer())
                    {
                          //write to depth buffer
                        m_DepthBuffer[static_cast<uint64_t>(c + static_cast<uint64_t>(r * static_cast<uint64_t>(m_Width)))] = zBuffer;
                    }

                    const float vertex0InvDepthMULWeight0{vertex0InvDepth * weight0};
                    const float vertex1InvDepthMULWeight1{vertex1InvDepth * weight1};
                    const float vertex2InvDepthMULWeight2{vertex2InvDepth * weight2};

                    //interpolate w value
                    const float wInterpolated = 1.0f / (vertex0InvDepthMULWeight0 + vertex1InvDepthMULWeight1 + vertex2InvDepthMULWeight2);

                    //interpolate uv, normal, tangent and view direction (templated function)
                    Interpolate
                    (
                        vertexOUT
                        , vertex0InvDepthMULWeight0
                        , vertex1InvDepthMULWeight1
                        , vertex2InvDepthMULWeight2
                        , wInterpolated
                        , transformedVertices[index0]
                        , transformedVertices[index1]
                        , transformedVertices[index2]
                    );
                    //output vertex
                    vertexOUT.position = FPoint4(pixel , zBuffer , wInterpolated);

                    //Get color from backbuffer to blend
                    if(primitive->GetShouldBlend())
                    {
                        SDL_Color colorRGB;
                        uint32_t pixelBlend = *(static_cast<uint32_t*>(m_pBackBuffer->pixels)
                            + static_cast<uint64_t>(r * static_cast<uint64_t>(m_pBackBuffer->w) + c));
                        SDL_GetRGB(pixelBlend , m_pBackBuffer->format , &colorRGB.r , &colorRGB.g , &colorRGB.b);
                        targetColor = RGBColor(colorRGB.r / 255.0f , colorRGB.g / 255.0f , colorRGB.b / 255.0f);
                    }
                    CalculatePixelColor(vertexOUT , vertexOUT.color , primitive , targetColor
                        , m_DepthBuffer[c + (r * static_cast<uint64_t>(m_Width))]);
                    SetBackBufferPixels((uint32_t) vertexOUT.position.x ,
                        (uint32_t) vertexOUT.position.y , vertexOUT.color);
                }
            }
        }

    }

    //reset depth buffer
    m_DepthBuffer.clear();
    m_DepthBuffer = std::vector<float>(size_t(m_Width) * m_Height , FLT_MAX);

    SDL_UnlockSurface(m_pBackBuffer);
    SDL_BlitSurface(m_pBackBuffer , 0 , m_pFrontBuffer , 0);
    SDL_UpdateWindowSurface(m_pWindow);
}