using HumanFactors.Geometry;
using System;
using System.Numerics;
using System.Runtime.Remoting.Messaging;

namespace HumanFactors.RayTracing
{
    /*!
        \brief A Bounding Volume Hierarchy for the \link EmbreeRayTracer \endlink.
            
        \details
        Upon calling the constructor for this class, a Bounding Volume Hierarchy is generated 
        for the \link EmbreeRaytracer \endlink. Without this structure, the raytracer cannot be used.

        \remarks
        A more detailed description of Embree's BVH is available in intel's paper:
        Embree: A Kernel Framework for Efficient CPU Ray Tracing
        http://cseweb.ucsd.edu/~ravir/274/15/papers/a143-wald.pdf.

        \internal
            \note
            In C++ this is is an EmbreeRayTracer.

            \todo
            Functions to add meshes to an existing BVH
        \endinternal

        \see EmbreeRaytracer for functions to use this BVH for ray intersections. 
    */
    public class EmbreeBVH : HumanFactors.NativeUtils.NativeObject
    {
        /*!
            \brief Generate a BVH for an instance of MeshInfo. 
            
            \param MI Mesh to generate the BVH from. 

            \exception Exception Embree failed to create a BVH for the given mesh.

            \internal
                \todo Add missing dependency exception
            \endinternal
       */
        public EmbreeBVH(MeshInfo MI) : base(NativeMethods.C_ConstructRaytracer(MI.DangerousGetHandle()), MI.pressure) { }

        protected override bool ReleaseHandle()
        {
            NativeMethods.C_DestroyRayTracer(this.handle);
            return true;
        }
    }
}