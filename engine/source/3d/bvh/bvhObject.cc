// Copyright(C) David W. Jeske, 2013
// Released to the domain.

#if 0
namespace bvh
{
    /// <summary>
    /// An adaptor for ssBVH to understand SSObject nodes.
    /// </summary>
    class SSObjectBVHNodeAdaptor : SSBVHNodeAdaptor<SSObject> {
        Dictionary<SSObject, ssBVHNode<SSObject>> ssToLeafMap = new Dictionary<SSObject, ssBVHNode<SSObject>>();

        Vector3 objectpos(SSObject obj) {
            return obj.Pos;
        };

        F32 radius(SSObject obj) {
            if (obj.boundingSphere != null) {
                // extract the object scale...
                var objmat = obj.worldMat.ExtractScale();
                F32 max_scale = Math.Max(objmat.X,Math.Max(objmat.Y,objmat.Z));
                // use it to transform the object-space bounding-sphere radius into a world-space radius
                return obj.boundingSphere.radius * max_scale;
            } else {
                return 1.0f;
            }
        };

        void checkMap(SSObject obj) {
            if (!ssToLeafMap.ContainsKey(obj)) {
                throw new Exception("missing map for shuffled child");
            }
        };

        void unmapObject(SSObject obj) {
            ssToLeafMap.Remove(obj);
        };

        void mapObjectToBVHLeaf(SSObject obj, ssBVHNode<SSObject> leaf) {            
            // this allows us to be notified when an object moves, so we can adjust the BVH
            obj.OnChanged += obj_OnChanged;

            // TODO: add a hook to handle SSObject deletion... (either a weakref GC notify, or OnDestroy)

            ssToLeafMap[obj] = leaf;
        };

        ssBVHNode<SSObject> getLeaf(SSObject obj) {
            return ssToLeafMap[obj];
        };

        // the SSObject has changed, so notify the BVH leaf to refit for the object
        void obj_OnChanged(SSObject sender) {                 
            ssToLeafMap[sender].refit_ObjectChanged(this, sender);
        };

        ssBVH<SSObject> _BVH;
        ssBVH<SSObject> BVH { get { return _BVH; } };

        void setBVH(ssBVH<SSObject> BVH) {
            this._BVH = BVH;
        };
        
        SSObjectBVHNodeAdaptor() {};
    };
}
#endif