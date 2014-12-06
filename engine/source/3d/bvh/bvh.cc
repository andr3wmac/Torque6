// Copyright(C) David W. Jeske, 2014, and released to the public domain. 
//
// Dynamic BVH (Bounding Volume Hierarchy) using incremental refit and tree-rotations
//
// initial BVH build based on: Bounding Volume Hierarchies (BVH) – A brief tutorial on what they are and how to implement them
//              http://www.3dmuve.com/3dmblog/?p=182
//
// Dynamic Updates based on: "Fast, Effective BVH Updates for Animated Scenes" (Kopta, Ize, Spjut, Brunvand, David, Kensler)
//              http://www.cs.utah.edu/~thiago/papers/rotations.pdf
//
// see also:  Space Partitioning: Octree vs. BVH
//            http://thomasdiewald.com/blog/?p=1488
//
//

namespace bvh
{
    enum Axis { X,Y,Z, };

    template <class T> class SSBVHNodeAdaptor<T> 
    {
        //ssBVH<T> BVH { get; }
        void setBVH(ssBVH<T> bvh);
        Vector3 objectpos(T obj);
        float radius(T obj);
        void mapObjectToBVHLeaf(T obj, ssBVHNode<T> leaf);
        void unmapObject(T obj);
        void checkMap(T obj);
        ssBVHNode<T> getLeaf(T obj);
    };

    template <class T> class ssBVH<T>
    {
        ssBVHNode<T> rootBVH;
        SSBVHNodeAdaptor<T> nAda;
        U32 LEAF_OBJ_MAX;
        U32 nodeCount = 0;

        HashSet<ssBVHNode<T>> refitNodes = new HashSet<ssBVHNode<T>>();

        bool NodeTest(SSAABB box);

        // internal functional traversal...
        void _traverse(ssBVHNode<T> curNode, NodeTest hitTest, List<ssBVHNode<T>> hitlist) {
            if (curNode == null) { return; }
            if (hitTest(curNode.box)) {
                hitlist.Add(curNode);
                _traverse(curNode.left,hitTest,hitlist);
                _traverse(curNode.right,hitTest,hitlist);
            }
        };

        // public interface to traversal..
        List<ssBVHNode<T>> traverse(NodeTest hitTest) {
            var hits = new List<ssBVHNode<T>>();
            this._traverse(rootBVH,hitTest,hits);
            return hits;
        };
        
        // left in for compatibility..
        List<ssBVHNode<T>> traverseRay(SSRay ray) {
            float tnear = 0f, tfar = 0f;

            return traverse( box => OpenTKHelper.intersectRayAABox1(ray,box,ref tnear, ref tfar) );
        };

        List<ssBVHNode<T>> traverse(SSRay ray) {
            float tnear = 0f, tfar = 0f;

            return traverse( box => OpenTKHelper.intersectRayAABox1(ray,box,ref tnear, ref tfar) );
        };
        List<ssBVHNode<T>> traverse(SSAABB volume) {
            return traverse( box => box.intersectsAABB(volume) );            
        };

        /// <summary>
        /// Call this to batch-optimize any object-changes notified through 
        /// ssBVHNode.refit_ObjectChanged(..). For example, in a game-loop, 
        /// call this once per frame.
        /// </summary>

        void optimize() {  
            if (LEAF_OBJ_MAX != 1) {
                throw new Exception("In order to use optimize, you must set LEAF_OBJ_MAX=1");
            }
                  
            while (refitNodes.Count > 0) {                
                U32 maxdepth = refitNodes.Max( n => n.depth );
            
                var sweepNodes = refitNodes.Where( n => n.depth == maxdepth ).ToList();
                sweepNodes.ForEach( n => refitNodes.Remove(n) );

                sweepNodes.ForEach( n => n.tryRotate(this) );                
            }            
        };

        void addObject(T newOb) {
            SSAABB box = SSAABB.fromSphere(nAda.objectpos(newOb),nAda.radius(newOb));
            float boxSAH = rootBVH.SAH(ref box);
            rootBVH.addObject(nAda,newOb, ref box, boxSAH);
        };

        void removeObject(T newObj) {
            var leaf = nAda.getLeaf(newObj);
            leaf.removeObject(nAda,newObj);
        };

        U32 countBVHNodes() {
            return rootBVH.countBVHNodes();
        };

        /// <summary>
        /// initializes a BVH with a given nodeAdaptor, and object list.
        /// </summary>
        /// <param name="nodeAdaptor"></param>
        /// <param name="objects"></param>
        /// <param name="LEAF_OBJ_MAX">WARNING! currently this must be 1 to use dynamic BVH updates</param>
        ssBVH(SSBVHNodeAdaptor<T> nodeAdaptor, List<T> objects, U32 LEAF_OBJ_MAX = 1) {
            LEAF_OBJ_MAX = LEAF_OBJ_MAX;
            nodeAdaptor.setBVH(this);
            nAda = nodeAdaptor;
            
            if (objects.Count > 0) {
                rootBVH = new ssBVHNode<T>(this,objects);            
            } else {                
                rootBVH = new ssBVHNode<T>(this);
                rootBVH.gobjects = new List<T>(); // it's a leaf, so give it an empty object list
            }
        };   
    }; 
}