#include "model.h"

// Lots of temporary code in that file.

#if ENABLE_FBX

#include "fbx/fbx_output_stream.h"

FbxManager* Model::s_fbxManager = nullptr;

#endif

Model::~Model()
{
#if ENABLE_FBX

    if (s_fbxManager)
    {
        s_fbxManager->Destroy();
        s_fbxManager = nullptr;
    }

#endif
}

Chunk* Model::create()
{
#if ENABLE_FBX

    if (!s_fbxManager)
    {
        s_fbxManager = FbxManager::Create();
        FbxIOSettings* fbxIos = FbxIOSettings::Create(s_fbxManager, IOSROOT);
        s_fbxManager->SetIOSettings(fbxIos);
    }

#endif

    return new Model();
}

uint8_t Model::getNumberOfFiles() const
{
    return 1;
}

std::string Model::getFilename(uint8_t file) const
{
    return std::string(m_name);
}

bool Model::extract(uint8_t file, std::ostream& output) const
{
    Vertex* vertices = (Vertex*)(m_buffer);
    uint32_t* indices = (uint32_t*)((char*)vertices + sizeof(Vertex) * m_header->m_vertexCount);

#if ENABLE_FBX

    FbxScene* scene = FbxScene::Create(s_fbxManager, "Root");
    FbxMesh* mesh = FbxMesh::Create(s_fbxManager, m_name);

    // Vertices
    mesh->InitControlPoints(m_header->m_vertexCount);

    // Normals
    FbxGeometryElementNormal* elementNormal = mesh->CreateElementNormal();
    elementNormal->SetMappingMode(FbxGeometryElement::eByControlPoint);
    elementNormal->SetReferenceMode(FbxGeometryElement::eDirect);
    elementNormal->GetDirectArray().Resize(m_header->m_vertexCount);

    // UVs
    FbxGeometryElementUV* elementUV = mesh->CreateElementUV("uv");
    elementUV->SetMappingMode(FbxGeometryElement::eByControlPoint);
    elementUV->SetReferenceMode(FbxGeometryElement::eDirect);
    elementUV->GetDirectArray().Resize(m_header->m_vertexCount);

    FbxVector4* controlPoints = mesh->GetControlPoints();
    for (uint32_t vertex = 0; vertex < m_header->m_vertexCount; ++vertex)
    {
        controlPoints[vertex] = FbxVector4{ vertices[vertex].m_pos[0], vertices[vertex].m_pos[1], vertices[vertex].m_pos[2] };
        elementNormal->GetDirectArray().Add(FbxVector4{ vertices[vertex].m_normal[0], vertices[vertex].m_normal[1], vertices[vertex].m_normal[2] });
        elementUV->GetDirectArray().Add(FbxVector2{ vertices[vertex].m_uv[0], vertices[vertex].m_uv[1] });
    }

    FbxNode* node = FbxNode::Create(s_fbxManager, m_name);
    node->SetNodeAttribute(mesh);
    node->SetShadingMode(FbxNode::eFlatShading);

    // Texture
    FbxSurfacePhong* material = nullptr;
    FbxNode* meshNode = mesh->GetNode();
    if (meshNode)
    {
        material = meshNode->GetSrcObject<FbxSurfacePhong>(0);
        if (!material)
        {
            material = FbxSurfacePhong::Create(s_fbxManager, "mat_phong");
            material->Emissive.Set(FbxDouble3{ 0.0, 0.0, 0.0 });
            material->Ambient.Set(FbxDouble3{ 1.0, 1.0, 1.0 });
            material->Diffuse.Set(FbxDouble3{ 1.0, 1.0, 1.0 });
            material->TransparencyFactor.Set(0.0);
            material->ShadingModel.Set("Phong");

            meshNode->AddMaterial(material);
        }
    }

    for (uint32_t triangle = 0; triangle < m_header->m_indexCount / 3; ++triangle)
    {
        mesh->BeginPolygon(0);
        mesh->AddPolygon(indices[triangle * 3]);
        mesh->AddPolygon(indices[triangle * 3 + 1]);
        mesh->AddPolygon(indices[triangle * 3 + 2]);
        mesh->EndPolygon();
    }

    mesh->BuildMeshEdgeArray();

    mesh->GetElementMaterial(0)->SetMappingMode(FbxLayerElement::eAllSame);

    scene->GetRootNode()->AddChild(node);

    {
        bool lStatus = true;
        int pFileFormat = -1;
        bool pEmbedMedia = false;

        // Create an exporter.
        FbxExporter* lExporter = FbxExporter::Create(s_fbxManager, "");

        if (pFileFormat < 0 || pFileFormat >= s_fbxManager->GetIOPluginRegistry()->GetWriterFormatCount())
        {
            // Write in fall back format if pEmbedMedia is true
            pFileFormat = s_fbxManager->GetIOPluginRegistry()->GetNativeWriterFormat();

            if (!pEmbedMedia)
            {
                //Try to export in ASCII if possible
                int lFormatIndex, lFormatCount = s_fbxManager->GetIOPluginRegistry()->GetWriterFormatCount();

                for (lFormatIndex = 0; lFormatIndex < lFormatCount; lFormatIndex++)
                {
                    if (s_fbxManager->GetIOPluginRegistry()->WriterIsFBX(lFormatIndex))
                    {
                        FbxString lDesc = s_fbxManager->GetIOPluginRegistry()->GetWriterFormatDescription(lFormatIndex);
                        const char *lASCII = "ascii";
                        if (lDesc.Find(lASCII) >= 0)
                        {
                            pFileFormat = lFormatIndex;
                            break;
                        }
                    }
                }
            }
        }

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(s_fbxManager->GetIOSettings()))
#endif

        // Set the export states. By default, the export states are always set to 
        // true except for the option eEXPORT_TEXTURE_AS_EMBEDDED. The code below 
        // shows how to change these states.
        IOS_REF.SetBoolProp(EXP_FBX_MATERIAL, true);
        IOS_REF.SetBoolProp(EXP_FBX_TEXTURE, true);
        IOS_REF.SetBoolProp(EXP_FBX_EMBEDDED, pEmbedMedia);
        IOS_REF.SetBoolProp(EXP_FBX_ANIMATION, true);
        IOS_REF.SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);

        FbxOutputStream stream;

        // Initialize the exporter by providing a filename.
        if (lExporter->Initialize(&stream, &output, pFileFormat, s_fbxManager->GetIOSettings()) == false)
        {
            FBXSDK_printf("Call to FbxExporter::Initialize() failed.\n");
            FBXSDK_printf("Error returned: %s\n\n", lExporter->GetStatus().GetErrorString());
            return false;
        }

        // Export the scene.
        lStatus = lExporter->Export(scene);

        // Destroy the exporter.
        lExporter->Destroy();
    }

    scene->Destroy(true);

#else

    output << "# " << getName() << ".obj" << std::endl << std::endl;

    output << "# Vertex Attributes" << std::endl;
    for (uint32_t v = 0; v < m_header->m_vertexCount; ++v)
    {
        output << "v " << vertices[v].m_pos[0] << " " << vertices[v].m_pos[1] << " " << vertices[v].m_pos[2] << std::endl;
        output << "vt " << vertices[v].m_uv[0] << " " << vertices[v].m_uv[1] << std::endl;
        output << "vn " << vertices[v].m_normal[0] << " " << vertices[v].m_normal[1] << " " << vertices[v].m_normal[2] << std::endl;

    }

    output << std::endl << "# Triangles" << std::endl;
    for (uint32_t triangle = 0; triangle < m_header->m_indexCount / 3; ++triangle)
    {
        output << "f " << indices[triangle * 3] + 1 << " " << indices[triangle * 3 + 1] + 1 << " " << indices[triangle * 3 + 2] + 1 << std::endl;
    }

#endif

    return true;
}