using System.Collections;
using System.Collections.Generic;

using UnityEngine;

#if ENABLE_WINMD_SUPPORT
using System.Threading.Tasks;
using HL2IRToolTracking;
#endif

public class DepthStreamerIR : MonoBehaviour
{
#if ENABLE_WINMD_SUPPORT
    HL2IRTracking toolTracking;
#endif

    public GameObject previewQuad;
    Texture2D previewTexture;

    // Start is called before the first frame update
    void Start()
    {
        previewTexture = new Texture2D(512, 512, TextureFormat.Alpha8, false);
        previewQuad.GetComponent<MeshRenderer>().material.mainTexture = previewTexture;

#if ENABLE_WINMD_SUPPORT
        toolTracking = new HL2IRTracking();
        Windows.Perception.Spatial.SpatialCoordinateSystem unityWorldOrigin = Microsoft.MixedReality.OpenXR.PerceptionInterop.GetSceneCoordinateSystem(UnityEngine.Pose.identity) as Windows.Perception.Spatial.SpatialCoordinateSystem;
        toolTracking.SetReferenceCoordinateSystem(unityWorldOrigin);
        toolTracking.StartToolTracking();
#endif
    }

    // Update is called once per frame
    void Update()
    {

#if ENABLE_WINMD_SUPPORT

        // update short-throw AbImage texture --> active brightness image
 
        byte[] frameTexture = toolTracking.GetShortAbImageTextureBuffer();
        previewTexture.LoadRawTextureData(frameTexture);
        previewTexture.Apply();

        int rand = UnityEngine.Random.Range(0,262143);
        Debug.Log("Random pixel Value: " + rand + ": " + frameTexture[rand].ToString());
           
#endif
    }
}
