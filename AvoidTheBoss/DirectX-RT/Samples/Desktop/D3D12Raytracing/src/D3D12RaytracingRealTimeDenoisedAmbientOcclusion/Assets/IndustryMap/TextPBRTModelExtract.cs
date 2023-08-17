using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Windows;
using System.IO;

public class TextPBRTModelExtract : MonoBehaviour
{
    public AnimationClip[] m_raAnimationClips;

    private SkinnedMeshRenderer[] m_pSkinnedMeshRenderers = null;

    private List<string> m_rTextureNamesListForCounting = new List<string>();
    private List<string> m_rTextureNamesListForWriting = new List<string>();

    private StreamWriter m_rTextWriter = null;
    private int m_nFrames = 0;

    const float EPSILON = 1.0e-6f;
    int m_MaterialIndex = 0;
    bool IsZero(float fValue) { return ((Mathf.Abs(fValue) < EPSILON)); }
    bool IsEqual(float fA, float fB) { return (IsZero(fA - fB)); }

    bool FindTextureByName(List<string> textureNamesList, Texture texture)
    {
        if (texture)
        {
            string strTextureName = string.Copy(texture.name).Replace(" ", "_");
            for (int i = 0; i < textureNamesList.Count; i++)
            {
                if (textureNamesList.Contains(strTextureName)) return(true);
            }
            textureNamesList.Add(strTextureName);
            return(false);
        }
        else
        {
            return(true);
        }
    }

    void WriteTabs(int nLevel)
    {
        for (int i = 0; i < nLevel; i++) m_rTextWriter.Write("\t");
    }

     void WriteObjectName(int nLevel, string strHeader, Object obj)
    {
        WriteTabs(nLevel);
        m_rTextWriter.Write(strHeader + " ");
        m_rTextWriter.WriteLine((obj) ? string.Copy(obj.name).Replace(" ", "_") : "null");
    }

   void WriteString(string strToWrite)
    {
        m_rTextWriter.Write(strToWrite);
    }

    void WriteString(int nLevel, string strToWrite)
    {
        WriteTabs(nLevel);
        m_rTextWriter.Write(strToWrite);
    }

    void WriteLineString(string strToWrite)
    {
        m_rTextWriter.WriteLine(strToWrite);
    }

    void WriteLineString(int nLevel, string strToWrite)
    {
        WriteTabs(nLevel);
        m_rTextWriter.WriteLine(strToWrite);
    }

    void WriteTextureName(int nLevel, string strHeader, Texture texture)
    {
        //WriteTabs(nLevel);
        m_rTextWriter.Write(strHeader);
        if (texture)
        {
            if (FindTextureByName(m_rTextureNamesListForWriting, texture))
            {
                m_rTextWriter.Write(string.Copy(texture.name));
            }
            else
            {
                m_rTextWriter.Write(string.Copy(texture.name));
            }
        }
        else
        {
            m_rTextWriter.Write("null");
        }
    }

    void WriteVector(Vector2 v)
    {
        m_rTextWriter.Write(v.x + " " + v.y + " ");
    }

    void WriteVector(string strHeader, Vector2 v)
    {
        m_rTextWriter.Write(strHeader);
        WriteVector(v);
    }

    void WriteVector(Vector3 v)
    {
        m_rTextWriter.Write(v.x + " " + v.y + " " + v.z + " ");
    }

    void WriteVector(string strHeader, Vector3 v)
    {
        m_rTextWriter.Write(strHeader);
        WriteVector(v);
    }

    void WriteVector(Vector4 v)
    {
        m_rTextWriter.Write(v.x + " " + v.y + " " + v.z + " " + v.w + " ");
    }

    void WriteVector(string strHeader, Vector4 v)
    {
        m_rTextWriter.Write(strHeader);
        WriteVector(v);
    }

    void WriteVector(Quaternion q)
    {
        m_rTextWriter.Write(q.x + " " + q.y + " " + q.z + " " + q.w + " ");
    }

    void WriteVector(string strHeader, Quaternion q)
    {
        m_rTextWriter.Write(strHeader);
        WriteVector(q);
    }

    void WriteVectors(int nLevel, string strHeader, Vector2[] vectors)
    {
        WriteString(nLevel, strHeader + " " );
        if (vectors.Length > 0)
        {
            foreach (Vector2 v in vectors) m_rTextWriter.Write(v.x + " " + v.y + " ");
        }
       // m_rTextWriter.Write(" ");
    }

    void WriteVectors(int nLevel, string strHeader, Vector3[] vectors)
    {
        WriteString(nLevel, strHeader + " " );
        if (vectors.Length > 0)
        {
            foreach (Vector3 v in vectors) m_rTextWriter.Write(v.x + " " + v.y + " " + v.z + " ");
        }
      //  m_rTextWriter.Write(" ");
    }

    void WriteVectors(int nLevel, string strHeader, Vector4[] vectors)
    {
        WriteString(nLevel, strHeader + " ");
        if (vectors.Length > 0)
        {
            foreach (Vector4 v in vectors) m_rTextWriter.Write(v.x + " " + v.y + " " + v.z + " " + v.w + " ");
        }
       // m_rTextWriter.Write(" ");
    }

    void WriteColors(int nLevel, string strHeader, Color[] colors)
    {
        WriteString(nLevel, strHeader + " " + colors.Length + " ");
        if (colors.Length > 0)
        {
            foreach (Color c in colors) m_rTextWriter.Write(c.r + " " + c.g + " " + c.b + " " + c.a + " ");
        }
       // m_rTextWriter.WriteLine(" ");
    }

    void WriteTextureCoords(int nLevel, string strHeader, Vector2[] uvs)
    {
        WriteString(nLevel, strHeader + " " );
        if (uvs.Length > 0)
        {
            foreach (Vector2 uv in uvs) m_rTextWriter.Write(uv.x + " " + (1.0f - uv.y) + " ");
        }
       // m_rTextWriter.Write(" ");
    }

    void WriteIntegers(int nLevel, string strHeader, int[] integers)
    {
        WriteString(nLevel, strHeader + " ");
        if (integers.Length > 0)
        {
            foreach (int i in integers) m_rTextWriter.Write(i + " ");
        }
       // m_rTextWriter.Write(" ");
    }

    void WriteMatrix(Matrix4x4 matrix)
    {
        m_rTextWriter.Write(matrix.m00 + " " + matrix.m10 + " " + matrix.m20 + " " + matrix.m30 + " ");
        m_rTextWriter.Write(matrix.m01 + " " + matrix.m11 + " " + matrix.m21 + " " + matrix.m31 + " ");
        m_rTextWriter.Write(matrix.m02 + " " + matrix.m12 + " " + matrix.m22 + " " + matrix.m32 + " ");
        m_rTextWriter.Write(matrix.m03 + " " + matrix.m13 + " " + matrix.m23 + " " + matrix.m33 + " ");
    }

    void WriteMatrix(Vector3 position, Quaternion rotation, Vector3 scale)
    {
        Matrix4x4 matrix = Matrix4x4.identity;
        matrix.SetTRS(position, rotation, scale);
        WriteMatrix(matrix);
    }

    void WriteTransform(int nLevel, string strHeader, Transform current)
    {
        WriteString(nLevel, strHeader + " ");
        WriteVector(current.localPosition);
        WriteVector(current.localEulerAngles);
        WriteVector(current.localScale);
        WriteVector(current.localRotation);
        m_rTextWriter.WriteLine(" ");
    }

    void WriteLocalMatrix(int nLevel, string strHeader, Transform current)
    {
        WriteString(nLevel, strHeader);
        Matrix4x4 matrix = Matrix4x4.identity;
        matrix.SetTRS(current.localPosition, current.localRotation, current.localScale);
        WriteMatrix(matrix);
        //m_rTextWriter.Write(" ");
    }

    void WriteWorldMatrix(int nLevel, string strHeader, Transform current)
    {
        WriteString(nLevel, strHeader + " ");
        Matrix4x4 matrix = Matrix4x4.identity;
        matrix.SetTRS(current.position, current.rotation, current.lossyScale);
        WriteMatrix(matrix);
        m_rTextWriter.Write(" ");
    }

    void WriteBoneTransforms(int nLevel, string strHeader, Transform[] bones)
    {
        WriteString(nLevel, strHeader + " " + bones.Length + " ");
        if (bones.Length > 0)
        {
            foreach (Transform bone in bones)
            {
                WriteMatrix(bone.localPosition, bone.localRotation, bone.localScale);
            }
        }
        m_rTextWriter.WriteLine(" ");
    }

    void WriteBoneNames(int nLevel, string strHeader, Transform[] bones)
    {
        WriteString(nLevel, strHeader + " " + bones.Length + " ");
        if (bones.Length > 0)
        {
            foreach (Transform transform in bones)
            {
                m_rTextWriter.Write(string.Copy(transform.gameObject.name).Replace(" ", "_") + " ");
            }
        }
        m_rTextWriter.WriteLine(" ");
    }

    void WriteMatrixes(int nLevel, string strHeader, Matrix4x4[] matrixes)
    {
        WriteString(nLevel, strHeader + " " + matrixes.Length + " ");
        if (matrixes.Length > 0)
        {
            foreach (Matrix4x4 matrix in matrixes)
            {
                WriteMatrix(matrix);
            }
        }
        m_rTextWriter.Write(" ");
    }

    void WriteBoneIndices(int nLevel, string strHeader, BoneWeight[] boneWeights)
    {
        WriteString(nLevel, strHeader + " " + boneWeights.Length + " ");
        if (boneWeights.Length > 0)
        {
            foreach (BoneWeight boneWeight in boneWeights)
            {
                m_rTextWriter.Write(boneWeight.boneIndex0 + " " + boneWeight.boneIndex1 + " " + boneWeight.boneIndex2 + " " + boneWeight.boneIndex3 + " ");
            }
        }
        m_rTextWriter.WriteLine(" ");
    }

    void WriteBoneWeights(int nLevel, string strHeader, BoneWeight[] boneWeights)
    {
        WriteString(nLevel, strHeader + " " + boneWeights.Length + " ");
        if (boneWeights.Length > 0)
        {
            foreach (BoneWeight boneWeight in boneWeights)
            {
                m_rTextWriter.Write(boneWeight.weight0 + " " + boneWeight.weight1 + " " + boneWeight.weight2 + " " + boneWeight.weight3 + " ");
            }
        }
        m_rTextWriter.WriteLine(" ");
    }

    int GetTexturesCount(Material[] materials)
    {
        int nTextures = 0;
        for (int i = 0; i < materials.Length; i++)
        {
            if (materials[i].HasProperty("_MainTex"))
            {
                if (!FindTextureByName(m_rTextureNamesListForCounting, materials[i].GetTexture("_MainTex"))) nTextures++;
            }
            if (materials[i].HasProperty("_SpecGlossMap"))
            {
                if (!FindTextureByName(m_rTextureNamesListForCounting, materials[i].GetTexture("_SpecGlossMap"))) nTextures++;
            }
            if (materials[i].HasProperty("_MetallicGlossMap"))
            {
                if (!FindTextureByName(m_rTextureNamesListForCounting, materials[i].GetTexture("_MetallicGlossMap"))) nTextures++;
            }
            if (materials[i].HasProperty("_BumpMap"))
            {
                if (!FindTextureByName(m_rTextureNamesListForCounting, materials[i].GetTexture("_BumpMap"))) nTextures++;
            }
            if (materials[i].HasProperty("_EmissionMap"))
            {
                if (!FindTextureByName(m_rTextureNamesListForCounting, materials[i].GetTexture("_EmissionMap"))) nTextures++;
            }
            if (materials[i].HasProperty("_DetailAlbedoMap"))
            {
                if (!FindTextureByName(m_rTextureNamesListForCounting, materials[i].GetTexture("_DetailAlbedoMap"))) nTextures++;
            }
            if (materials[i].HasProperty("_DetailNormalMap"))
            {
                if (!FindTextureByName(m_rTextureNamesListForCounting, materials[i].GetTexture("_DetailNormalMap"))) nTextures++;
            }
        }
        return(nTextures);
    }

    int GetTexturesCount(Transform current)
    {
        int nTextures = 0;
        MeshRenderer meshRenderer = current.gameObject.GetComponent<MeshRenderer>();
        if (meshRenderer) nTextures = GetTexturesCount(meshRenderer.materials);

        for (int k = 0; k < current.childCount; k++) nTextures += GetTexturesCount(current.GetChild(k));

        return(nTextures);
    }

    void WriteMeshInfo(int nLevel, Mesh mesh)
    {
        //Shape "trianglemesh" "integer indices" [] "point P" [] "normal N" [] "float uv" []
        //WriteString(");
        if (mesh.subMeshCount > 0)
        {
            for (int i = 0; i < mesh.subMeshCount; i++)
            {
                int[] subindicies = mesh.GetTriangles(i);
                WriteLineString(nLevel, "");
                WriteIntegers(nLevel, "\tShape \"trianglemesh\" \"integer indices\" [", subindicies);
            }
        }
        WriteString("]");

        //9."point P" [ ] //Positions
        WriteLineString(nLevel, "");
        WriteVectors(nLevel, "\t\"point P\" [", mesh.vertices);
        WriteString("]");

        //10."normal N" [] //Normals
        WriteLineString(nLevel, "");
        WriteVectors(nLevel, "\t\"normal N\" [", mesh.normals);
        WriteString("]");
        //11."float uv" [] //TextureCoords0
        WriteLineString(nLevel, "");
        WriteTextureCoords(nLevel, "\t\"float uv\" [", mesh.uv);
        WriteString("]");

    }

    void WriteMaterials(int nLevel, Material[] materials)
    {
        //WriteLineString(nLevel, "<Materials>: " + materials.Length);
        if (materials.Length > 0)
        {
            for (int i = 0; i < materials.Length; i++)
            {
                //WriteLineString(nLevel+1, "<Material>: " + i);
                Texture mainAlbedoMap = materials[i].GetTexture("_MainTex");
                if (materials[i].HasProperty("_MainTex"))
                {
                    // 3.Texture "텍스쳐이름" "spectrum" "imagemap" "string filename" [
                    // 텍스쳐 이름 : "Industry_Field_Tile.png"
                    // ] "bool trilinear" [ "true" ]
                    WriteLineString(nLevel,"");
                    WriteTextureName(nLevel, "\tTexture \"", mainAlbedoMap);
                    WriteTextureName(nLevel, "\" \"spectrum\" \"imagemap\" \"string filename\" [ \"", mainAlbedoMap);
                    WriteString(".png\" ] \"bool trilinear\" [ \"true\" ]");
                }
                if (materials[i].HasProperty("_SpecGlossMap"))
                {
                    Texture specularcMap = materials[i].GetTexture("_SpecGlossMap");
                    //WriteTextureName(nLevel+2, "<SpecularMap>:", specularcMap);
                }
                if (materials[i].HasProperty("_MetallicGlossMap"))
                {
                    Texture metallicMap = materials[i].GetTexture("_MetallicGlossMap");
                    //WriteTextureName(nLevel+2, "<MetallicMap>:", metallicMap);
                }
                Texture bumpMap = null;
                if (materials[i].HasProperty("_BumpMap"))
                {
                    bumpMap = materials[i].GetTexture("_BumpMap");
                    // 4.if(TextureNormal)
                    // Texture "텍스쳐이름" "spectrum" "imagemap" "string filename"[
                    // "Industry_Field_Tile_Normal.png"
                    // ] "bool trilinear"["true"]
                    if (bumpMap)
                    {
                        WriteLineString(nLevel, "");
                        WriteTextureName(nLevel, "\tTexture \"", bumpMap);
                        WriteTextureName(nLevel, "\" \"spectrum\" \"imagemap\" \"string filename\" [ \"", bumpMap);
                        WriteString(".png\" ] \"bool trilinear\" [ \"true\" ]");
                    }
                }
                if (materials[i].HasProperty("_EmissionMap"))
                {
                    Texture emissionMap = materials[i].GetTexture("_EmissionMap");
                    //WriteTextureName(nLevel+2, "<EmissionMap>:", emissionMap);
                }
                if (materials[i].HasProperty("_DetailAlbedoMap"))
                {
                    Texture detailAlbedoMap = materials[i].GetTexture("_DetailAlbedoMap");
                    //WriteTextureName(nLevel+2, "<DetailAlbedoMap>:", detailAlbedoMap);
                }
                if (materials[i].HasProperty("_DetailNormalMap"))
                {
                    Texture detailNormalMap = materials[i].GetTexture("_DetailNormalMap");
                    //WriteTextureName(nLevel+2, "<DetailNormalMap>:", detailNormalMap);
                }

                //5.MakeNamedMaterial "재질이름" "string type" [ "matte" ] "texture Kd" [ "텍스쳐이름1" ] "texture normal" [ "텍스쳐이름2" ]
                WriteLineString(nLevel, "");
                WriteString(nLevel, "\tMakeNamedMaterial \"" + "material" + m_MaterialIndex);
                WriteTextureName(nLevel, "\" \"string type\" [ \"matte\" ] \"texture Kd\" [ \"", mainAlbedoMap);
                if (bumpMap)
                    WriteTextureName(nLevel, "\" ] \"texture Normal\" [ \"", bumpMap);
                WriteString("\" ]");
                //6.NamedMaterial "재질이름"
                WriteLineString(nLevel, "");
                WriteString(nLevel, "\tNamedMaterial \"" + "material" + m_MaterialIndex);
                WriteString("\"");
            }
        }
        m_MaterialIndex++;
        //WriteLineString(nLevel, "");
    }

    void WriteSkinnedMeshInfo(int nLevel, SkinnedMeshRenderer skinMeshRenderer)
    {
        WriteObjectName(nLevel, "<SkinningInfo>:", skinMeshRenderer);
        int nBonesPerVertex = (int)skinMeshRenderer.quality; //SkinQuality.Auto:0, SkinQuality.Bone1:1, SkinQuality.Bone2:2, SkinQuality.Bone4:4
        if (nBonesPerVertex == 0) nBonesPerVertex = 4;
        WriteLineString(nLevel+1, "<BonesPerVertex>: " + nBonesPerVertex);
        WriteLineString(nLevel+1, "<Bounds>: " + skinMeshRenderer.localBounds.center.x + " " + skinMeshRenderer.localBounds.center.y + " " + skinMeshRenderer.localBounds.center.z + " " + skinMeshRenderer.localBounds.extents.x + " " + skinMeshRenderer.localBounds.extents.y + " " + skinMeshRenderer.localBounds.extents.z);
        WriteBoneNames(nLevel+1, "<BoneNames>:", skinMeshRenderer.bones);
        WriteMatrixes(nLevel+1, "<BoneOffsets>:", skinMeshRenderer.sharedMesh.bindposes);
        WriteBoneIndices(nLevel+1, "<BoneIndices>:", skinMeshRenderer.sharedMesh.boneWeights);
        WriteBoneWeights(nLevel+1, "<BoneWeights>:", skinMeshRenderer.sharedMesh.boneWeights);
        WriteLineString(nLevel, "</SkinningInfo>");

        Mesh mesh = skinMeshRenderer.sharedMesh;
        print("Blend Shape Count: " + mesh.blendShapeCount);
        WriteMeshInfo(nLevel, mesh);
    }

    void WriteFrameInfo(int nLevel, Transform current)
    {
        //int nTextures = GetTexturesCount(current);
        //WriteObjectName(nLevel, "<Frame>: " + m_nFrames++ + " " + nTextures, current.gameObject);

        // Integrator "path" "integer maxdepth" [ 17 ] 
        // Transform [ 0.678116 -0.167853 -0.715531 -0 -0 0.973571 -0.228385 -0 -0.734955 -0.154871 -0.660194 -0 -0.554465 -6.29907 82.2988 1]
        // Sampler "sobol" "integer pixelsamples" [ 64 ]
        // PixelFilter "triangle" "float xwidth" [ 1.000000 ] "float ywidth" [ 1.000000 ]
        // Film "image" "integer xresolution" [ 1280 ] "integer yresolution" [ 720 ] "string filename" [ "dragon.png" ]
        // Camera "perspective" "float fov" [ 20.114292 ]
        // WorldBegin"

        // 2.Transform[] //TransformMatrix
        WriteLocalMatrix(0, "\tTransform [ ", current);
        WriteString("]");

        MeshFilter meshFilter = current.gameObject.GetComponent<MeshFilter>();
        MeshRenderer meshRenderer = current.gameObject.GetComponent<MeshRenderer>();

        if (meshFilter && meshRenderer)
        {
            Material[] materials = meshRenderer.materials;
            if (materials.Length > 0) WriteMaterials(0, materials);

            WriteMeshInfo(0, meshFilter.sharedMesh);
        }

    }

    void WriteFrameHierarchyInfo(int nLevel, Transform current)
    {
        WriteFrameInfo(nLevel, current);

        if (current.childCount > 0)
        {
            for (int k = 0; k < current.childCount; k++) WriteFrameHierarchyInfo(nLevel+2, current.GetChild(k));
        }
    }

    void WriteFrameAnimationHierarchy(Transform current)
    {
        WriteMatrix(current.localPosition, current.localRotation, current.localScale);

        if (current.childCount > 0)
        {
            for (int k = 0; k < current.childCount; k++) WriteFrameAnimationHierarchy(current.GetChild(k));
        }
    }

    void WriteAnimationTransforms(int nLevel, string strHeader)
    {
        WriteString(nLevel, strHeader);
        WriteFrameAnimationHierarchy(transform);
        m_rTextWriter.WriteLine(" ");
    }

    void WriteSkinnedMeshAnimationTransforms(int nLevel, string strHeader, SkinnedMeshRenderer skinnedMeshRenderer)
    {
        WriteString(nLevel, strHeader);
        for (int i = 0; i < skinnedMeshRenderer.bones.Length; i++)
        {
            WriteMatrix(skinnedMeshRenderer.bones[i].localPosition, skinnedMeshRenderer.bones[i].localRotation, skinnedMeshRenderer.bones[i].localScale);
        }
        m_rTextWriter.WriteLine(" ");
    }

    void WriteSkinnedMeshAnimationTransforms(int nLevel, string strHeader)
    {
        WriteLineString(nLevel, strHeader);
        for (int i = 0; i < m_pSkinnedMeshRenderers.Length; i++)
        {
            WriteSkinnedMeshAnimationTransforms(nLevel+1, i + " ", m_pSkinnedMeshRenderers[i]);
        }
    }

    void WriteSkinnedMeshFrameNames(int nLevel, string strHeader, SkinnedMeshRenderer skinnedMeshRenderer)
    {
        WriteString(nLevel, strHeader + skinnedMeshRenderer.bones.Length + " ");
        for (int i = 0; i < skinnedMeshRenderer.bones.Length; i++)
        {
            m_rTextWriter.Write(string.Copy(skinnedMeshRenderer.bones[i].gameObject.name).Replace(" ", "_") + " ");
        }
        m_rTextWriter.WriteLine(" ");
    }

    void WriteSkinnedMeshFrameNames(int nLevel, string strHeader)
    {
        WriteLineString(nLevel, strHeader);
        for (int i = 0; i < m_pSkinnedMeshRenderers.Length; i++)
        {
            WriteSkinnedMeshFrameNames(nLevel+1, i + " " + string.Copy(m_pSkinnedMeshRenderers[i].name).Replace(" ", "_") + " ", m_pSkinnedMeshRenderers[i]);
        }
    }

    void WriteFrameNameHierarchy(Transform current)
    {
        m_rTextWriter.Write(string.Copy(current.gameObject.name).Replace(" ", "_") + " ");

        if (current.childCount > 0)
        {
            for (int k = 0; k < current.childCount; k++) WriteFrameNameHierarchy(current.GetChild(k));
        }
    }

    void WriteFrameNames(int nLevel, string strHeader)
    {
        WriteString(nLevel, strHeader);
        WriteFrameNameHierarchy(transform);
        m_rTextWriter.WriteLine(" ");
    }

    void WriteAnimationClipsInfo(int nLevel)
    {
        WriteLineString(nLevel, "<AnimationSets>: " + m_raAnimationClips.Length);
#if _WITH_SKINNED_BONES_ANIMATION
        WriteSkinnedMeshFrameNames(nLevel+1, "<FrameNames>: " + m_pSkinnedMeshRenderers.Length);
#else
        WriteFrameNames(nLevel+1, "<FrameNames>: " + m_nFrames + " ");
#endif
        for (int j = 0; j < m_raAnimationClips.Length; j++)
        {
            int nFramesPerSec = (int)m_raAnimationClips[j].frameRate;
            int nKeyFrames = Mathf.CeilToInt(m_raAnimationClips[j].length * nFramesPerSec);
            WriteLineString(nLevel+1, "<AnimationSet>: " + j + " " + string.Copy(m_raAnimationClips[j].name).Replace(" ", "_") + " " + m_raAnimationClips[j].length + " " + nFramesPerSec + " " + nKeyFrames);

            float fFrameRate = (1.0f / nFramesPerSec), fKeyFrameTime = 0.0f;
            for (int k = 0; k < nKeyFrames; k++)
            {
                m_raAnimationClips[j].SampleAnimation(gameObject, fKeyFrameTime);

#if _WITH_SKINNED_BONES_ANIMATION
                WriteSkinnedMeshAnimationTransforms(nLevel+2, "<Transforms>: " + k + " " + fKeyFrameTime);
#else
                WriteAnimationTransforms(nLevel+2, "<Transforms>: " + k + " " + fKeyFrameTime + " ");
#endif
                fKeyFrameTime += fFrameRate;
            }
        }

        WriteLineString(nLevel, "</AnimationSets>");
    }

    void Start()
    {
        m_pSkinnedMeshRenderers = GetComponentsInChildren<SkinnedMeshRenderer>();
        for (int i = 0; i < m_pSkinnedMeshRenderers.Length; i++)
        {
            m_pSkinnedMeshRenderers[i].forceMatrixRecalculationPerRender = true;
        }

        //m_rTextWriter = new StreamWriter(string.Copy(gameObject.name).Replace(" ", "_") + ".PBRT");
        m_rTextWriter = new StreamWriter("scene.PBRT");

        WriteLineString("Integrator \"path\" \"integer maxdepth\" [ 17 ]");
        WriteLineString("Transform [ 0.678116 -0.167853 -0.715531 -0 -0 0.973571 -0.228385 -0 -0.734955 -0.154871 -0.660194 -0 -0.554465 -6.29907 82.2988 1 ]");
        WriteLineString("Sampler \"sobol\" \"integer pixelsamples\" [ 64 ]");
        WriteLineString("PixelFilter \"triangle\" \"float xwidth\" [ 1.000000 ] \"float ywidth\" [ 1.000000 ]");
        WriteLineString("Film \"image\" \"integer xresolution\" [ 1280 ] \"integer yresolution\" [ 720 ] \"string filename\" [ \"temp.png\" ]");
        WriteLineString("Camera \"perspective\" \"float fov\" [ 20.114292 ]");
        WriteLineString("WorldBegin");

        WriteFrameHierarchyInfo(1, transform);

        //12.WorldEnd
        WriteLineString(0, "");
        WriteLineString("WorldEnd");

        m_rTextWriter.Flush();
        m_rTextWriter.Close();

        print("Model Text Write Completed");
    }
}

