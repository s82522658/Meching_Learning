/*8 class type:cp, im, pp, imU, om, omL, inL, imS*/
/*9 attribute type*/
//目的：用測資ecoli建一棵9-dtree，尋找k個neighbor(1,5,10,100)，判斷預測結果
//方法：
//1.讀入train.csv、test.csv：採用參數讀檔法
//2.儲存training data 至 "data"
//3.儲存test data 至 "test_data"
//4.建9-dtree
//5.tracing test data
//6.
//7.
//8.
//9.export output to output.txt

/* 2017/11/19 3:54pm */
/* 建好 9-d tree，還沒有trace*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#define knn 5
#define num_data 105
#define num_test_data 45
int node_num;


struct ecoli{
	double attribute[4];
	int class;
	int index;
	double distance;
};
struct Node{
	struct Node* left;
	struct Node* right;
	struct Node* parent;
	struct ecoli data;
	double value;
	int index;
	int class;
};


struct Node* KDtree(struct ecoli* dataList);
struct Node* BuildKDtree(struct ecoli* dataList,int index,int size);
void quicksort(struct ecoli* data,int index, int left, int right);
void swap(struct ecoli *a, struct ecoli *b);
struct ecoli* SearchKNN(struct Node* root,int k,struct ecoli test_node);
void BroSearchKNN(struct Node* root,int k,struct ecoli test_node,struct ecoli* knnlist,int* knn_size);
struct Node* Getbrother(struct Node* curnode);
struct Node* reSearching(struct Node* node,struct ecoli test_node);
double ComputeDistance(struct ecoli curnode,struct ecoli testnode);
void MaintainMaxHeap(struct ecoli* knnlist,struct ecoli curnode,int* knn_size,int k);
void FixUp(struct ecoli* knnlist,struct ecoli curnode,int* knn_size);
void FixDown(struct ecoli* knnlist,struct ecoli curnode,int knn_size);
int PredictClass(struct ecoli* knnlist,int real_class,int k);
void Bubblesort(struct ecoli* knnlist,int k);

struct Node* KDtree(struct ecoli* dataList){
	struct Node* root=BuildKDtree(dataList,0,num_data);
	return root;
}
struct Node* BuildKDtree(struct ecoli* dataList,int index,int size){
	//print the number of node
	node_num++;
	//printf("%d-th node\n",node_num );
	if(size == 0) return NULL;
	//create new node
	struct Node* newnode=malloc(sizeof(struct Node));
	newnode->left=NULL;
	newnode->right=NULL;
	newnode->parent=NULL;
	newnode->class=3;
	//if leafnode,return (leafnode's data_size ==1)
	if(size == 1) {
		newnode->data=dataList[0];
		newnode->index=index;
		newnode->value=dataList[0].attribute[index];
		newnode->class=dataList[0].class;
		//printf("leafnode-index:%d,value:%f,class:%d\n",index,newnode->value,newnode->class );
		//printf("broke here3\n");
		return newnode;
	}
	
	//quicksort for attribute[index]
	quicksort(dataList,index,0,size-1);
	//store median to node 
	int median=size/2;
	newnode->index=index;
	newnode->value=dataList[median-1].attribute[index];
	newnode->data=dataList[median-1];
	//printf("median-index:%d,value:%f\n",index,newnode->value);

	//compute size of left,right child
	int left_num=0;
	int right_num=0;
	for(int i=0;i<size;i++){
		if((dataList[i].attribute[index] <= newnode->value)&& i != median-1){
			left_num++;
		}
		else if(dataList[i].attribute[index] > newnode->value) right_num++;
	}
	//build left,right child datalist
	//not include median 
	struct ecoli* leftList=malloc(sizeof(struct ecoli)*left_num);
	struct ecoli* rightList=malloc(sizeof(struct ecoli)*right_num);
	int cnt=0,cnt2=0;
	//printf("left_num:%d,right_num:%d\n",left_num,right_num);
	for(int i=0;i<size;i++){
		if((dataList[i].attribute[index] <= newnode->value)&& i != median-1){
			
			leftList[cnt]=dataList[i];
			//printf("LeftList[%d],attribute[%d]-%f,class-%d\n",i,index,leftList[cnt].attribute[index],leftList[cnt].class);
			cnt++;
		} 
		else if(dataList[i].attribute[index] > newnode->value) {
			rightList[cnt2]=dataList[i];
			//printf("RightList[%d],attribute[%d]-%f,class-%d\n",i,index,rightList[cnt2].attribute[index],rightList[cnt2].class);
			cnt2++;
		}
	}
	//choose new index
	index=(index+1)%4;
	//addChild
	//printf("add left child\n");
	newnode->left=BuildKDtree(leftList,index,left_num);
	//printf("add right child\n");
	newnode->right=BuildKDtree(rightList,index,right_num);

	if(newnode->left != NULL) {
		newnode->left->parent=newnode;
		//printf("node-%dth,leftchild-%dth\n",newnode->data.index,newnode->left->data.index );
	}
	if(newnode->right != NULL) {
		newnode->right->parent=newnode;
		//printf("node-%dth,rightchild-%dth\n",newnode->data.index,newnode->right->data.index );
	}

	//if(newnode->left != NULL && newnode->left->parent!=newnode) printf("leftchild incorrect\n");
	//if(newnode->right != NULL && newnode->right->parent!=newnode) printf("rightchild incorrect\n");
	
	return newnode;
}
void quicksort(struct ecoli* data,int index, int left, int right){
	double pivot;
    int i, j;

    if (left >= right) return;

    pivot = data[left].attribute[index];

    i = left + 1;
    j = right;

    while (1)
    {
        while (i <= right){
            if (data[i].attribute[index] > pivot) break;
            i++;
        }

        while (j > left){
            if (data[j].attribute[index] < pivot) break;
            j--;
        }

        if (i > j) break;

        swap(&data[i], &data[j]);
    }

    swap(&data[left], &data[j]);

    quicksort(data,index, left, j - 1);
    quicksort(data,index, j + 1, right);
}

void swap(struct ecoli *a, struct ecoli *b){
    struct ecoli temp = *a;
    *a = *b;
    *b = temp;
}
/****** New part ******/
/**SearchKNN haven't finish yet**/
struct ecoli* SearchKNN(struct Node* root,int k,struct ecoli test_node){
	//printf("enter SearchKNN\n");
	struct Node* Curnode=reSearching(root,test_node);
	struct ecoli Curecoli=Curnode->data;

	struct ecoli* knnlist=malloc(sizeof(struct ecoli)*k);
	int knn_size=0;
	double curDistance;

	while(Curnode != NULL){
		//printf("enter While\n");
		curDistance=ComputeDistance(Curecoli,test_node);
		Curecoli.distance=curDistance;
		//we use a binary tree to maintain knnlist
		//and we will maintain knnlist by distance form far to near
		//printf("parent node-distance:%f,class:%d\n",Curecoli.distance,Curecoli.class);
		MaintainMaxHeap(knnlist,Curecoli,&knn_size,k);	
		if(Curnode->parent == NULL) break;//not sure if this operation is right
		if(curDistance > fabs(test_node.attribute[Curnode->parent->index]-Curecoli.attribute[Curnode->parent->index]))
		{
			//printf("brother node\n");
			//get brother node, distance
			struct Node* bro=Getbrother(Curnode);
			double broDistance;
			if(bro != NULL) BroSearchKNN(bro,k,test_node,knnlist,&knn_size);	
		}
		Curnode=Curnode->parent;
		Curecoli=Curnode->data;
	}
	//printf("leave searchKNN\n");
	return knnlist;
}
void BroSearchKNN(struct Node* root,int k,struct ecoli test_node,struct ecoli* knnlist,int* knn_size){
	/*Set entry point*/
	struct Node* Splitnode=root->parent;
	//printf("die here2\n");
	/*Find start point*/
	struct Node* Curnode=reSearching(root,test_node);
	//printf("die here \n");
	struct ecoli Curecoli=Curnode->data;
	//printf("die here1\n");
	

	double curDistance;

	while(Curnode != Splitnode && Curnode != NULL){//bro meet it's parent
		//printf("enter While\n");
		curDistance=ComputeDistance(Curecoli,test_node);
		Curecoli.distance=curDistance;
	
		//printf("parent node-distance:%f,class:%d\n",Curecoli.distance,Curecoli.class);
		MaintainMaxHeap(knnlist,Curecoli,knn_size,k);
		if(Curnode->parent == NULL) break;//which means this is root
		if((Splitnode != Curnode->parent) && curDistance > fabs(test_node.attribute[Curnode->parent->index]-Curecoli.attribute[Curnode->parent->index]))
		{
			struct Node* bro=Getbrother(Curnode);
			double broDistance;
			if(bro != NULL){
				//printf("brother node-distance:%f,class:%d\n",Curecoli.distance,Curecoli.class);
				BroSearchKNN(bro,k,test_node,knnlist,knn_size);		
			}
		}
		Curnode=Curnode->parent;
		Curecoli=Curnode->data;
	}
	return ;
}
struct Node* Getbrother(struct Node* curnode){
	if(curnode->parent->left == curnode) return curnode->parent->right;
	else if(curnode->parent->right == curnode) return curnode->parent->left;
	else {
		//printf("node-%dth\n",curnode->data.index );
		//printf("brother error!\n");
		exit(1);
	}
}
struct Node* reSearching(struct Node* node,struct ecoli test_node){
	if(node->left == NULL && node->right == NULL){
		//printf("died here 8\n");
		return node;
	}
	else{
		int index=node->index;
		if(node->left == NULL)	{
			//printf("died here4\n");
			node=node->right;
		}
		else if(node->right == NULL) {
			//printf("died here5\n");
			node=node->left;
		}
		else if(test_node.attribute[index] < node->value) {
			//printf("died here1\n");
			node=node->left;
		}	
		else if(test_node.attribute[index] > node->value) {
			//printf("died here2\n");
			//printf("node->index:%d\n",node->index);
			//printf("node->value:%f\n",node->value);
			node=node->right;
		}	
		else {
			//printf("died here3\n");
			double D_l,D_r;
			D_l=ComputeDistance((node->left->data),test_node);
			D_r=ComputeDistance((node->right->data),test_node);
			if(D_l >= D_r) node=node->right;
			else node=node->left;
		}	
	}

	return reSearching(node,test_node);
}
double ComputeDistance(struct ecoli curnode,struct ecoli testnode){
	double sum=0;
	double radius=0;
	for(int i=0;i<4;i++){
		sum+=pow((curnode.attribute[i]-testnode.attribute[i]),2);
	}
	double x=1/4.0;
	radius=pow(sum,x);
	return radius;
}
//this function is used to sorts knnlist
void MaintainMaxHeap(struct ecoli* knnlist,struct ecoli curnode,int* knn_size,int k){
	//printf("knn_size=%d\n",(*knn_size) );
	//int k=knn;
	if((*knn_size)<k){//knnlist is not full
		FixUp(knnlist,curnode,knn_size);
		//printf("in fixup\n");
	}
	else if(curnode.distance < knnlist[0].distance){//knnlist is full
		FixDown(knnlist,curnode,*knn_size);
		//printf("in fixdown\n");
	}
	/*for(int i=0;i<k;i++){
		printf("%d-th distance:%f,class:%d\n",i,knnlist[i].distance,knnlist[i].class);
	}*/
}
void FixUp(struct ecoli* knnlist,struct ecoli curnode,int* knn_size){
	//insert curnode to the end of knnlist
	knnlist[(*knn_size)]=curnode;
	(*knn_size)++;
	int j=(*knn_size)-1;//child=curnode
	int i=(j+1)/2-1;//parent
	while(i >= 0){//parent 至少要是第0個 
		if(knnlist[i].distance >= knnlist[j].distance) break;
		swap(&knnlist[i],&knnlist[j]);
		j=i;
		i=(j+1)/2-1;
	}

}
void FixDown(struct ecoli* knnlist,struct ecoli curnode,int knn_size){
	int i=0; //parent node
    int j=i*2+1;//child node
    knnlist[0]=curnode;
	
	//search until the last element
    while(j < (knn_size)){
    	//find the "farest" child among two child
    	if((j+1) < (knn_size) && (knnlist[j].distance < knnlist[j+1].distance)) j++;
    	//find the "farest" node among farest child and parent
    	//if parent node is farther, then don't have to compare
    	if(knnlist[i].distance >= knnlist[j].distance) break;
    	//else if child is farther,swap child and parent
    	swap(&knnlist[j],&knnlist[i]);
    	i=j;
    	j=i*2+1;
    }

}

int PredictClass(struct ecoli* knnlist,int real_class,int k){
	int array[3]={};
	int max=0,max_index=0;
	for(int i=0;i<k;i++){
		//printf("class:%d\n",knnlist[i].class );
		array[knnlist[i].class]++;
	}
	for(int i=0;i<3;i++){
		if(array[i] > max){
			max=array[i];
			max_index=i;
		}
	}
	if(max_index == real_class) return 1;
	else return 0;
}

void Bubblesort(struct ecoli* knnlist,int k){
	for(int i=0;i<k-1;i++){
		for(int j=0;j<k-i-1;j++){
			if(knnlist[j].distance>knnlist[j+1].distance) swap(&knnlist[j],&knnlist[j+1]);
		}
	}

}
int main(int argc,char* argv[]){
 	FILE* pfile;
 	long lSize;
	char * buffer;
	size_t result;
/*open train.csv*/
 	pfile=fopen(argv[1],"r");
 	if(pfile == NULL){
 		printf("Train open failure!\n");
 		exit(1);
 	}
 	else{
		fseek (pfile , 0 , SEEK_END);
		lSize = ftell (pfile);
		rewind (pfile);
		// allocate memory to contain the whole file:
		buffer = (char*) malloc (sizeof(char)*lSize);
		if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}
		// copy the file into the buffer:
		result = fread (buffer,1,lSize,pfile);
		//printf("size:%zu\n",result );
		if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
 	
	 	fclose(pfile);
 	}

/*store training data into ecoli strcuct*/
 	int comma=0;
 	int cnt=0;
 	int seq=0;
 	int i=0;
 	char buf[15]={};
 	//declare our training data structure
 	struct ecoli* data=malloc(sizeof(struct ecoli)*num_data);
 	struct ecoli* testdata=malloc(sizeof(struct ecoli)*num_test_data);
 	//discard first row
 	for(i=0;buffer[i]!='\n';i++);
 	//skip the first '\n'	
 	i++;
 	//store data
 	for(;i<lSize;i++){
 		if(buffer[i] == ','){
 			comma++;
 			if(seq < num_test_data){
 				if(comma <= 4)data[seq].attribute[comma-1]=atof(buf);
 			}
 			else{
 				if(comma <= 4)testdata[seq-num_test_data].attribute[comma-1]=atof(buf);
 			}
 			for(int j=0;j<15;j++) buf[j]=0;	
 			cnt=0;
 		}
 		else if(buffer[i] == '\n'|| buffer[i] == '\0'){
 			if(seq < num_test_data){
	 			float temp=atof(buf);
	 			if(!strcmp(buf,"Iris-setosa")) data[seq].class=0;
				else if(!strcmp(buf,"Iris-versicolor")) data[seq].class=1; 
				else if(!strcmp(buf,"Iris-virginica")) data[seq].class=2;  
	 			
	 			data[seq].index=seq;
 			}
 			else{
 				float temp=atof(buf);
 				if(!strcmp(buf,"Iris-setosa")) testdata[seq-num_test_data].class=0;
				else if(!strcmp(buf,"Iris-versicolor")) testdata[seq-num_test_data].class=1; 
				else if(!strcmp(buf,"Iris-virginica")) testdata[seq-num_test_data].class=2;  
	 			testdata[seq-num_test_data].index=seq;
 			}
 			for(int j=0;j<15;j++) buf[j]=0;
 			comma=0;
 			cnt=0;
 			seq++;
 		}
 		else{
 			buf[cnt]=buffer[i];
 			cnt++;
 		}
 	}
 	/*Build 9-D tree*/
 	node_num=0;
 	struct Node* root=KDtree(data);
 	/*Trace test data with KNN method*/
 	int k=1;	
	for(int j=0;j<4;j++){
		if(j==0) k=1;
		else if(j==1) k=5;
		else if(j==2) k=10;
		else if(j==3) k=100;
		int* seq=malloc(sizeof(int)*k*3);
		int ans;
		int count=0;
		double accuracy=0;
		for(int i=0;i<num_test_data;i++){
			/*Trace test data*/
	 		struct ecoli* knnlist=SearchKNN(root,k,testdata[i]);
	 		/*Calculate accuracy*/
	 		ans=PredictClass(knnlist,testdata[i].class,k);
	 		if(ans) count++;
	 		if(i < 3){
	 			Bubblesort(knnlist,k);
	 			for(int l=0;l<k;l++) seq[i*k+l]=knnlist[l].index;
	 		}	
	 	}
	 	accuracy=(double)count/num_test_data;
	 	/* Output results to output.txt */
	 	printf("KNN accuracy:%f\n",accuracy );
	 	for(int l=0;l<3;l++){
	 		for(int i=0;i<k;i++){
				printf( "%d ",seq[l*k+i]);
		 	}
		 	printf("\n" );
	 	}
	 	printf("\n" );	
	}

 	
 	
 	exit(0);

 }