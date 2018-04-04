import java.util.ArrayList;
import java.util.List;
 
public class KDTree {
 
    public static void main(String[] args) {
        List<Node> nodeList=new ArrayList<Node>();
        nodeList.add(new Node(new double[]{2,3}));
        nodeList.add(new Node(new double[]{5,4}));
        nodeList.add(new Node(new double[]{9,6}));
        nodeList.add(new Node(new double[]{4,7}));
        nodeList.add(new Node(new double[]{8,1}));
        nodeList.add(new Node(new double[]{7,2}));
         
        KDTree kdTree=new KDTree();
        Node root=kdTree.buildKDTree(nodeList,0);
        System.out.println(root);
        System.out.println(kdTree.searchKNN(root,new Node(new double[]{2.1,3.1}),2));
        System.out.println(kdTree.searchKNN(root,new Node(new double[]{2,4.5}),1));
        System.out.println(kdTree.searchKNN(root,new Node(new double[]{2,4.5}),3));
    }
     
    static class Node implements Comparable<Node>{
        public double[] data;//树上节点的数据  是一个多维的向量
        public double distance;//与当前查询点的距离  初始化的时候是没有的
        public Node left,right,parent;//左右子节点  以及父节点
        public int dim=-1;//维度  建立树的时候判断的维度
         
        public Node(double[] data)
        {
            this.data=data;
        }
         
        /**
         * 返回指定索引上的数值
         * @param index
         * @return
         */
        public double getData(int index)
        {
            if(data==null || data.length<=index)
                return Integer.MIN_VALUE;
            return data[index];
        }
 
        @Override
        public int compareTo(Node o) {
            if(this.distance>o.distance)
                return 1;
            else if(this.distance==o.distance)
                return 0;
            else return -1;
        }
         
        /**
         * 计算距离 这里返回欧式距离
         * @param that
         * @return
         */
        public double computeDistance(Node that)
        {
            if(this.data==null || that.data==null || this.data.length!=that.data.length)
                return Double.MAX_VALUE;//出问题了  距离最远
            double d=0;
            for(int i=0;i<this.data.length;i++)
            {
                d+=Math.pow(this.data[i]-that.data[i], 2);
            }
             
            return Math.sqrt(d);
        }
         
        public String toString()
        {
            if(data==null || data.length==0)
                return null;
            StringBuilder sb=new StringBuilder();
            for(int i=0;i<data.length;i++)
                sb.append(data[i]+" ");
            sb.append(" d:"+this.distance);
            return sb.toString();
        }
    }
     
     
    /**
     * 构建kd树  返回根节点
     * @param nodeList
     * @param index
     * @return
     */
    public Node buildKDTree(List<Node> nodeList,int index)
    {
        if(nodeList==null || nodeList.size()==0)
            return null;
        quickSortForMedian(nodeList,index,0,nodeList.size()-1);//中位数排序
        Node root=nodeList.get(nodeList.size()/2);//中位数 当做根节点
        root.dim=index;
        List<Node> leftNodeList=new ArrayList<Node>();//放入左侧区域的节点  包括包含与中位数等值的节点-_-
        List<Node> rightNodeList=new ArrayList<Node>();
         
        for(Node node:nodeList)
        {
            if(root!=node)
            {
                if(node.getData(index)<=root.getData(index))
                    leftNodeList.add(node);//左子区域 包含与中位数等值的节点
                else
                    rightNodeList.add(node);
            }
        }
         
        int newIndex=index+1;//进入下一个维度
        if(newIndex>=root.data.length)
            newIndex=0;//从0维度开始再算
        root.left=buildKDTree(leftNodeList,newIndex);//添加左右子区域
        root.right=buildKDTree(rightNodeList,newIndex);
         
        if(root.left!=null)
            root.left.parent=root;//添加父指针  
        if(root.right!=null)
            root.right.parent=root;//添加父指针  
        return root;
    }
     
     
    /**
     * 查询最近邻
     * @param root kd树
     * @param q 查询点
     * @param k
     * @return
     */
    public List<Node> searchKNN(Node root,Node q,int k)
    {
        List<Node> knnList=new ArrayList<Node>();
        Node almostNNode=searchLeaf(root,q);//近似最近点
 
         
        while(almostNNode!=null)
        {
            double curD=q.computeDistance(almostNNode);//最近近似点与查询点的距离 也就是球体的半径
            almostNNode.distance=curD;
            maintainMaxHeap(knnList,almostNNode,k);
            if(almostNNode.parent!=null &&
                    curD>Math.abs(q.getData(almostNNode.parent.dim)-almostNNode.parent.getData(almostNNode.parent.dim)))
            {
                //这样可能在另一个子区域中存在更加近似的点
                Node brother=getBrother(almostNNode);
                brother.distance=q.computeDistance(brother);
                maintainMaxHeap(knnList,brother,k);
            }
             
            almostNNode=almostNNode.parent;//返回上一级
        }
         
        return knnList;
    }
     
    /**
     * 获取兄弟节点
     * @param node
     * @return
     */
    public Node getBrother(Node node)
    {
        if(node==node.parent.left)
            return node.parent.right;
        else
            return node.parent.left;
    }
     
    /**
     * 查询到叶子节点
     * @param root
     * @param q
     * @return
     */
    public Node searchLeaf(Node root,Node q)
    {
        Node leaf=root,next=null;
        int index=0;
        while(leaf.left!=null || leaf.right!=null)
        {
            if(q.getData(index)<leaf.getData(index))
            {
                next=leaf.left;//进入左侧
            }else if(q.getData(index)>leaf.getData(index))
            {
                next=leaf.right;
            }else{
                //当取到中位数时  判断左右子区域哪个更加近
                if(q.computeDistance(leaf.left)<q.computeDistance(leaf.right))
                    next=leaf.left;
                else
                    next=leaf.right;
            }
            if(next==null)
                break;//下一个节点是空时  结束了
            else{
                leaf=next;
                if(++index>=root.data.length)
                    index=0;
            }
        }
         
        return leaf;
    }
     
    /**
     * 维护一个k的最大堆
     * @param listNode
     * @param newNode
     * @param k
     */
    public void maintainMaxHeap(List<Node> listNode,Node newNode,int k)
    {
        if(listNode.size()<k)
        {
            maxHeapFixUp(listNode,newNode);//不足k个堆   直接向上修复
        }else if(newNode.distance<listNode.get(0).distance){
            //比堆顶的要小   还需要向下修复 覆盖堆顶
            maxHeapFixDown(listNode,newNode);
        }
    }
     
    /**
     * 从上往下修复  将会覆盖第一个节点
     * @param listNode
     * @param newNode
     */
    private void maxHeapFixDown(List<Node> listNode,Node newNode)
    {
        listNode.set(0, newNode);
        int i=0;
        int j=i*2+1;//j=1
        while(j<listNode.size())
        {
            if(j+1<listNode.size() && listNode.get(j).distance<listNode.get(j+1).distance)
                j++;
             
            if(listNode.get(i).distance>=listNode.get(j).distance)
                break;
             
            Node t=listNode.get(i);
            listNode.set(i, listNode.get(j));
            listNode.set(j, t);
             
            i=j;
            j=i*2+1;
        }
    }
     
    private void maxHeapFixUp(List<Node> listNode,Node newNode)
    {
        listNode.add(newNode);
        int j=listNode.size()-1;
        int i=(j+1)/2-1;//i是parent节点
        while(i>=0)
        {
             
            if(listNode.get(i).distance>=listNode.get(j).distance)
                break;
            //swap node i and node j in nodelist 
            Node t=listNode.get(i);
            listNode.set(i, listNode.get(j));
            listNode.set(j, t);
             
            j=i;
            i=(j+1)/2-1;
        }
    }
     
     
     
    /**
     * 使用快排进进行一个中位数的查找  完了之后返回的数组size/2即中位数
     * @param nodeList
     * @param index
     * @param left
     * @param right
     */
    private void quickSortForMedian(List<Node> nodeList,int index,int left,int right)
    {
        if(left>=right || nodeList.size()<=0)
            return ;
         
        Node kn=nodeList.get(left);
        double k=kn.getData(index);//取得向量指定索引的值
         
        int i=left,j=right;
        while(i<j)
        {
            while(nodeList.get(j).getData(index)>=k && i<j)
                j--;
            nodeList.set(i, nodeList.get(j));
            while(nodeList.get(i).getData(index)<=k && i<j)
                i++;
            nodeList.set(j, nodeList.get(i));
        }
        nodeList.set(i, kn);
        if(i==nodeList.size()/2)
            return ;//完成中位数的排序了
        else if(i<nodeList.size()/2)
        {
            quickSortForMedian(nodeList,index,i+1,right);//只需要排序右边就可以了
        }else{
            quickSortForMedian(nodeList,index,left,i-1);//只需要排序左边就可以了
        }
    }
     
     
     
 
}