#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<string.h>

#define fclass 6
#define fold 5
#define num 103//每個fold取樣的數量
#define quantity 517//總data數量
int node_num;
//目的：用測資花朵的四種屬性(descriptive feature)預測花朵種類(class)
//方法：
//v 1.打亂 data.txt 中的資料（因已照種類排好了）
//v 2.分割成5組(1 test data、4 training data)
//v 3.計算現有花瓣種類的熵(entropy of all data)
//v 4.因4種decriptive feature為連續性資料，故先計算其threshold
//v 5.分別計算4種屬性的熵(entropy)
//補充:四種屬性的每個threshold皆可視為不同屬性。四種屬性共有120種threshold，故視為120多種屬性。
//6.熵值最小者(或information gain最大者)設為leaf node
//7.在此leaf node下用其他沒用過的屬性(因是連續性資料，故四種皆可再用，不過threshold要換一個)，重複步驟6.
//8.不斷重複步驟7.，直到每個leaf node 裡只有一種花瓣種類
//9.將test data丟入DTree
//10.計算準確率
//main->threshold. main->DTree->recursive(insert_node->calentropy->parentropy)

int node_num;
struct boundary {
	double bound_min;
	double bound_max;
};
struct Node{
	struct Node* left;
	struct Node* right;
	struct Node* parent;
	double value;
	int type;
	int class;
};
struct thre{
	int seq;
	double value;
	int flag;
};
struct forest{
	int month;
	int day;
	float x;
	float y;
	float attribute[8];
	int class;
	int index;
};
struct min_entropy{
	double entropy;
	double value;
	int seq;//seq in flower
	int row;//seq in thre 0~3
	int col;//seq in thre 0~119
	int l_num;
	int r_num;
};
struct revariable{
	struct min_entropy min_e;
	struct forest* mini_data;
};

int traversal(struct forest test_data,struct Node* node);
struct Node* DTree(struct forest* mini_data,struct thre thre_arr[][(quantity*(fold-1))/fold]);
struct Node* insert_node(struct forest* mini_data,int data_n,struct boundary table[],struct thre thre_arr[][(quantity*(fold-1))/fold]);
struct revariable calentropy(int data_n,struct boundary table[],struct thre thre_arr[][(quantity*(fold-1))/fold],struct forest* mini_data);
double parentropy(struct forest* temp,int data_n,int* cnt);
void creat_newdata(struct forest* data,int data_n,int feature,int* cnt,double thre_val);
double totalentropy(struct forest* data,int data_n);
void threshold(struct forest temp[],struct thre array[][(quantity*(fold-1))/fold]);
int randseq(int map[]);

int traversal(struct forest test_data,struct Node* node){
	if(node->class<fclass) return node->class;
	else{
		switch(node->type){
			case 0:
				if(test_data.month>node->value) node=node->left;
				else node=node->right;
				break;
			case 1:
				if(test_data.day>node->value) node=node->left;
				else node=node->right;
				break;
			case 2:
				if(test_data.x>node->value) node=node->left;
				else node=node->right;
				break;
			case 3:
				if(test_data.y>node->value) node=node->left;
				else node=node->right;
				break;
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
				if(test_data.attribute[node->type-4]>node->value) node=node->left;
				else node=node->right;
				break;
			default:printf("traversal error!\n");		
		}
	}
	return traversal(test_data,node);
}

struct Node* DTree(struct forest* mini_data,struct thre thre_arr[][(quantity*(fold-1))/fold]){
	struct boundary table[12]={};
	struct Node* root=NULL;
	int total_n=quantity*(fold-1)/fold;
	root=insert_node(mini_data,total_n,table,thre_arr);
	return root;
}
//e.g.
//start=d3,end=d7(set of d3~d7)
//table
struct Node* insert_node(struct forest* mini_data,int data_n,struct boundary table[],struct thre thre_arr[][(quantity*(fold-1))/fold]){
	printf("num of node:%d\n",node_num);
	node_num++;
	if(data_n == 0) return NULL;
	struct Node* newnode;
	struct forest* new_data=malloc(sizeof(struct forest)*((quantity*(fold-1))/fold));
	struct forest* new_data_l=malloc(sizeof(struct forest)*((quantity*(fold-1))/fold));
	//allocate memory to newnode
	newnode =(struct Node*)malloc(sizeof(struct Node));
	newnode->left=NULL;
	newnode->right=NULL;
	newnode->class=fclass;//3不是class的範圍，表示此node不是leafnode（只有leafnode有class）

	//calculate this node's entropy
	double entropy=totalentropy(mini_data,data_n);
	printf("node's entropy:%f\n",entropy );
	if(entropy == 0){
		newnode->class=mini_data[0].class;	
		//printf("all the same class,class:%d\n",newnode->class);
		return newnode;
	}
	//copy data 
	for(int i=0;i<(quantity*(fold-1))/fold;i++) new_data[i]=mini_data[i];

	struct revariable revar;
	struct min_entropy new_bound={};
	struct boundary local_tab[12]={};
	for(int i=0;i<12;i++) {
		local_tab[i]=table[i];
		//printf("table %d th,min:%fmax:%f\n",i,table[i].bound_min,table[i].bound_max );
	}
	int k=0;
	revar=calentropy(data_n,table,thre_arr,new_data);

	new_bound=revar.min_e;

	for(int i=0;i<new_bound.r_num;i++){
		new_data[i]=revar.mini_data[i];
		//printf("%d th right index:%d\n",i,new_data[i].index );
		//沒有把new_data複製過去的部分歸零，不知道會不會錯？
	}
	for(int i=new_bound.r_num;i<new_bound.r_num+new_bound.l_num;i++){
		new_data_l[k]=revar.mini_data[i];	
		//printf("%d th left index:%d\n",i-new_bound.r_num,new_data_l[k].index );
		k++;

		//沒有把new_data複製過去的部分歸零，不知道會不會錯？
	}
	
		double temp=0.0;
		int new_r=new_bound.row;
		
		//store node condition
		newnode->value=new_bound.value;
		newnode->type=new_bound.row;

		temp=local_tab[new_r].bound_min;
		local_tab[new_r].bound_min=new_bound.value;

		newnode->left=insert_node(new_data_l,new_bound.l_num,local_tab,thre_arr);
		
		local_tab[new_r].bound_min=temp;
		local_tab[new_r].bound_max=new_bound.value;
		newnode->right=insert_node(new_data,new_bound.r_num,local_tab,thre_arr);	
	
	return newnode;
}
struct revariable calentropy(int data_n,struct boundary table[],struct thre thre_arr[][(quantity*(fold-1))/fold],struct forest* mini_data){
	int total_num=(quantity*(fold-1))/fold;
	struct revariable revar;
	revar.mini_data=(struct forest*)malloc(sizeof(struct forest)*((quantity*(fold-1))/fold));
	struct min_entropy min_e;
	min_e.value=0;
	min_e.entropy=10.0;
	double entropy=0.0;

	int *cnt=(int*)malloc(sizeof(int));

	//creat_newdata(temp, data_n,feature,cnt);
	
//repeat about 120 times = calculate total 120 features' entropy
	for(int i=0;i<12;i++){
		for(int j=0;j<total_num;j++){
			//printf("loop i:%d,j:%d\n",i,j);
			//printf("table min:%fmax:%f\n",table[i].bound_min,table[i].bound_max );
			if(thre_arr[i][j].value==0) {
				break;
			}
			if(!table[i].bound_min&&!table[i].bound_max){
				//printf("thre value is:%f\n",thre_arr[i][j].value );
				creat_newdata(mini_data,data_n,i,cnt,thre_arr[i][j].value);
				entropy=parentropy(mini_data,data_n,cnt);
				//printf("cnt:%d,data_n:%d,entropy:%f\n",(*cnt),data_n,entropy );
				if(!((((*cnt)==data_n) || ((*cnt)==0)) && entropy!=0)){
					if(entropy <= min_e.entropy) {
						for(int k=0;k<data_n;k++) revar.mini_data[k]=mini_data[k];
						min_e.entropy=entropy;
						min_e.value=thre_arr[i][j].value;
						min_e.seq=thre_arr[i][j].seq;
						min_e.row=i;
						min_e.col=j;
						min_e.r_num=(*cnt);
						min_e.l_num=data_n-(*cnt);
					}
				}
				
			}
			else if(!table[i].bound_min&&table[i].bound_max){
				if(thre_arr[i][j].value < table[i].bound_max){	
					//printf("thre value is:%f\n",thre_arr[i][j].value );
					creat_newdata(mini_data,data_n,i,cnt,thre_arr[i][j].value);		
					
					entropy=parentropy(mini_data,data_n,cnt);
					//printf("cnt:%d,data_n:%d,entropy:%f\n",(*cnt),data_n,entropy );
					if(!((((*cnt)==data_n) || ((*cnt)==0)) && entropy!=0)){	
						if(entropy <= min_e.entropy) {
							for(int k=0;k<data_n;k++) revar.mini_data[k]=mini_data[k];
							min_e.entropy=entropy;
							min_e.value=thre_arr[i][j].value;
							min_e.seq=thre_arr[i][j].seq;
							min_e.row=i;
							min_e.col=j;
							min_e.r_num=(*cnt);
							min_e.l_num=data_n-(*cnt);
						}
					}
				}
			}
			else if(table[i].bound_min&&!table[i].bound_max){
				if(thre_arr[i][j].value > table[i].bound_min){
					//printf("thre value is:%f\n",thre_arr[i][j].value );
					creat_newdata(mini_data,data_n,i,cnt,thre_arr[i][j].value);		
					entropy=parentropy(mini_data,data_n,cnt);
					//printf("cnt:%d,data_n:%d,entropy:%f\n",(*cnt),data_n,entropy );
					if(!((((*cnt)==data_n) || ((*cnt)==0)) && entropy!=0)){
						if(entropy <= min_e.entropy) {
							for(int k=0;k<data_n;k++) revar.mini_data[k]=mini_data[k];
							min_e.entropy=entropy;
							min_e.value=thre_arr[i][j].value;
							min_e.seq=thre_arr[i][j].seq;
							min_e.row=i;
							min_e.col=j;
							min_e.r_num=(*cnt);
							min_e.l_num=data_n-(*cnt);
						}
					}
				}
			}
			else {
				if((thre_arr[i][j].value < table[i].bound_max)&&(thre_arr[i][j].value > table[i].bound_min)){	
					//printf("thre value is:%f\n",thre_arr[i][j].value );
					creat_newdata(mini_data,data_n,i,cnt,thre_arr[i][j].value);		
					
					entropy=parentropy(mini_data,data_n,cnt);
					//printf("cnt:%d,data_n:%d,entropy:%f\n",(*cnt),data_n,entropy );
					if(!((((*cnt)==data_n) || ((*cnt)==0)) && entropy!=0)){
						if(entropy <= min_e.entropy) {
							for(int k=0;k<data_n;k++) revar.mini_data[k]=mini_data[k];
							min_e.entropy=entropy;
							min_e.value=thre_arr[i][j].value;
							min_e.seq=thre_arr[i][j].seq;
							min_e.row=i;
							min_e.col=j;
							min_e.r_num=(*cnt);
							min_e.l_num=data_n-(*cnt);
						}
					}
				}
				
			}	
					
		}
	}
	
	revar.min_e=min_e;
	
	return revar;

}
void creat_newdata(struct forest* data,int data_n,int feature,int* cnt,double thre_val){
	struct forest* mini_data;
	mini_data=(struct forest*)malloc(sizeof(struct forest)*((quantity*(fold-1))/fold));
	
	(*cnt)=0;

	//temp 為新的map 裡面放著每個節點的元素。例如:90,100,115被分在這個節點，則在這個map中其標記變為0,1,2
	switch(feature){
		case 0:for(int i = 0;i<data_n;i++) {
			if(data[i].month<=thre_val) (*cnt)++;
		}
		break;
		case 1:for(int i = 0;i<data_n;i++) {
				if(data[i].day<=thre_val) (*cnt)++;		
		}
		break;
		case 2:for(int i = 0;i<data_n;i++) {
				if(data[i].x<=thre_val) (*cnt)++;
		}
		break;
		case 3:for(int i = 0;i<data_n;i++) {
				if(data[i].y<=thre_val) (*cnt)++;
		}
		break;
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		for(int i = 0;i<data_n;i++) if(data[i].attribute[feature-4]<=thre_val) (*cnt)++;
		break;
	}
	int j=0,k=0;
	switch(feature){
		case 0:for(int i=0;i<data_n;i++){
				if(data[i].month<=thre_val){
					mini_data[j]=data[i];
					//printf("right %dth:sepal_l=%f\n",j,mini_data[j].sepal_l);
					j++;
				}
				else{
					mini_data[(*cnt)+k]=data[i];
					//printf("left %dth:sepal_l=%f\n",k,mini_data[(*cnt)+k].sepal_l);
					k++;
				}
			}
		break;
		case 1:for(int i=0;i<data_n;i++){
				if(data[i].day<=thre_val){
					mini_data[j]=data[i];
					//printf("right %dth:sepal_w=%f\n",j,mini_data[j].sepal_w);
					j++;
				}
				else{
					mini_data[(*cnt)+k]=data[i];
					//printf("left %dth:sepal_w=%f\n",k,mini_data[(*cnt)+k].sepal_w);
					k++;
				}
			}
		break;
		case 2:for(int i=0;i<data_n;i++){
				if(data[i].x<=thre_val){
					mini_data[j]=data[i];
					//printf("right %dth:petal_l=%f\n",j,mini_data[j].petal_l);
					j++;
				}
				else{
					mini_data[(*cnt)+k]=data[i];
					//printf("left %dth:petal_l=%f\n",k,mini_data[(*cnt)+k].petal_l);
					k++;
				}
				//printf("i:%d,new data:%f\n",i,mini_data[i].petal_l);
			}
		break;
		case 3:for(int i=0;i<data_n;i++){
				if(data[i].y<=thre_val){
					mini_data[j]=data[i];
					j++;
				}
				else{
					mini_data[(*cnt)+k]=data[i];
					k++;
				}
			}
		break;
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		for(int i=0;i<data_n;i++){
			if(data[i].attribute[feature-4]<=thre_val){
				mini_data[j]=data[i];
				j++;
			}
			else{
				mini_data[(*cnt)+k]=data[i];
				k++;
			}
		}
		break;
	}

	
	for(int i=0;i<data_n;i++) data[i]=mini_data[i];
	
	return ;
	
}
double parentropy(struct forest* temp,int data_n,int* cnt){
	double class_a[fclass]={};
	double class_b[fclass]={};
	double entropy_a=0.0;
	double entropy_b=0.0;
	double finalentropy;
	double ratio_a=(double)(*cnt)/data_n;;//ratio below of the threshold
	double ratio_b=1-ratio_a;//ratio higher of the threshold
	//entropy before threshold
	//printf("data_n:%d,thre_seq:%d\n",data_n,thre_seq );

	for(int i=0;i<(*cnt);i++) class_a[temp[i].class]++;
	for(int i=0;i<fclass;i++){
		if((*cnt)!= 0)class_a[i]/=(*cnt);
		if(class_a[i] != 0) entropy_a+=class_a[i]*(log(class_a[i])/log(2.0));
	}
	//entropy after threshold
	for(int i=(*cnt);i<data_n;i++) class_b[temp[i].class]++;
	for(int i=0;i<fclass;i++){
		if((data_n-(*cnt)) != 0) class_b[i]/=(data_n-(*cnt));
		if(class_b[i] != 0) entropy_b+=class_b[i]*(log(class_b[i])/log(2.0));
	}

	return -(ratio_a*entropy_a+ratio_b*entropy_b);
}
/*calcuate total entropy*/
double totalentropy(struct forest* data,int data_n){
	//int data_n=(quantity*(fold-1))/fold;
	printf("data_n:%d\n",data_n );
	double class[fclass]={0};
	double entropy=0.0;
	
	for(int i=0;i<data_n;i++){
		switch(data[i].class){
			case 0:class[0]++; break;
			case 1:class[1]++; break;
			case 2:class[2]++; break;
			case 3:class[3]++; break;
			case 4:class[4]++; break;
			case 5:class[5]++; break;
			default:printf("ftotalentropy error!\n");
		}
	}

	for(int i=0;i<fclass;i++){
		class[i]/=data_n;
		if(class[i] != 0) entropy+=class[i]*(log(class[i])/log(2.0));
	} 
	entropy=-entropy;
	return entropy;

}
void threshold(struct forest temp[],struct thre array[][(quantity*(fold-1))/fold]){
//create a structure for storing results from bubblesort
	int data_n=(quantity*(fold-1))/fold;
	struct forest ttemp;
	int cnt=0;
	
	/**************** bubble sort ******************/
	/*calculate only training cases,don't include test case!*/
	
	/*0-th feature:month*/
	cnt=0;
	for(int i=0;i<data_n-1;i++){
		for(int j=0;j<data_n-i-1;j++){
			if(temp[j].month>temp[j+1].month){
				ttemp=temp[j+1];
				temp[j+1]=temp[j];
				temp[j]=ttemp;
			}
		}
	}
	for(int i=0;i<data_n-1;i++){
		if(temp[i].class!=temp[i+1].class) {
			array[0][cnt].value=(temp[i].month+temp[i+1].month)/2;
			array[0][cnt].seq=i+1;
			cnt++;
		}
	}
		
	/*1-th feature:day*/
	cnt=0;
	for(int i=0;i<data_n-1;i++){
		for(int j=0;j<data_n-i-1;j++){
			if(temp[j].day>temp[j+1].day){
				ttemp=temp[j+1];
				temp[j+1]=temp[j];
				temp[j]=ttemp;
			}
		}
	}
	for(int i=0;i<data_n-1;i++){
		if(temp[i].class!=temp[i+1].class) {
			array[1][cnt].value=(temp[i].day+temp[i+1].day)/2;
			array[1][cnt].seq=i+1;
			cnt++;
		}
		//printf("%d th,sepal_w:%f\n",i,temp[i].sepal_w);
	}
	/*2-th feature:x*/
	
	cnt=0;
	for(int i=0;i<data_n-1;i++){
		for(int j=0;j<data_n-i-1;j++){
			if(temp[j].x>temp[j+1].x){
				ttemp=temp[j+1];
				temp[j+1]=temp[j];
				temp[j]=ttemp;
			}
		}
	}
	for(int i=0;i<data_n-1;i++){
		if(temp[i].class!=temp[i+1].class) {
			array[2][cnt].value=(temp[i].x+temp[i+1].x)/2;
			array[2][cnt].seq=i+1;
			cnt++;
		}
		//printf("%d th,class:%d\n",i,temp[i].class);
	}
	/*3-th feature:y*/
	
	cnt=0;
	for(int i=0;i<data_n-1;i++){
		for(int j=0;j<data_n-i-1;j++){
			if(temp[j].y>temp[j+1].y){
				ttemp=temp[j+1];
				temp[j+1]=temp[j];
				temp[j]=ttemp;
			}
		}
	}
	for(int i=0;i<data_n-1;i++){
		if(temp[i].class!=temp[i+1].class) {
			array[3][cnt].value=(temp[i].y+temp[i+1].y)/2;
			array[3][cnt].seq=i+1;
			cnt++;
		}
	}

	/*4-12th feature:attribute[8]*/
	
	for(int k=0;k<8;k++){
		cnt=0;
		for(int i=0;i<data_n-1;i++){
			for(int j=0;j<data_n-i-1;j++){
				if(temp[j].attribute[k]>temp[j+1].attribute[k]){
					ttemp=temp[j+1];
					temp[j+1]=temp[j];
					temp[j]=ttemp;
				}
			}
		}
		for(int i=0;i<data_n-1;i++){
			if(temp[i].class!=temp[i+1].class) {
				array[k][cnt].value=(temp[i].attribute[k]+temp[i+1].attribute[k])/2;
				array[k][cnt].seq=i+1;
				cnt++;
			}
		}
	}
	
	/***************** end of bubble sort *****************/	
	
	
}
int randseq(int map[]){
	int seq;
	int result;
	seq=rand()%quantity;
	while(1){
		if(map[seq]!= 1){
			result=seq;
			map[seq]++;
			return result;
		}
		else if(seq == (quantity-1)) seq=0;
		else seq++;
	}
}

int main(){
	srand(time(NULL));
	//open file
	FILE *pFile;
	long lSize;
  	char * buffer;
  	size_t result;

	pFile=fopen("forestfires.csv","r");

	if(pFile==NULL){
		printf("open failure");
		exit(1);
	}
	else{
		// obtain file size:
		fseek (pFile , 0 , SEEK_END);
		lSize = ftell (pFile);
		rewind (pFile);
		// allocate memory to contain the whole file:
		buffer = (char*) malloc (sizeof(char)*lSize);
		if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

		// copy the file into the buffer:
		result = fread (buffer,1,lSize,pFile);
		//printf("size:%zu\n",result );
		if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
	}
	fclose(pFile);

	/*store fdata into forest strcuct*/

 	int comma=0;
 	int cnt=0;
 	int map[quantity]={0};
 	int seq=randseq(map);
 	int i=0;
 	char buf[16]={};
 	int counter=0;
 	//declare our training data structure
 	struct forest* data=malloc(sizeof(struct forest)*quantity);
 	//discard first row
 	for(i=0;buffer[i]!='\n';i++);
 	//skip the first '\n'	
 	i++;
 	//store data
 	for(;i<lSize;i++){
 		if(buffer[i] == ','){
 			comma++;
 			switch(comma){
 				case 1:data[seq].x=atoi(buf);
 				break;
 				case 2:data[seq].y=atoi(buf);
 				break;
 				case 3:
 					if(!strcmp(buf,"jan")) data[seq].month=0;
 					else if(!strcmp(buf,"feb")) data[seq].month=1;
 					else if(!strcmp(buf,"mar")) data[seq].month=2;
 					else if(!strcmp(buf,"apr")) data[seq].month=3;
 					else if(!strcmp(buf,"may")) data[seq].month=4;
 					else if(!strcmp(buf,"jun")) data[seq].month=5;
 					else if(!strcmp(buf,"jul")) data[seq].month=6;
 					else if(!strcmp(buf,"aug")) data[seq].month=7;
 					else if(!strcmp(buf,"sep")) data[seq].month=8;
 					else if(!strcmp(buf,"oct")) data[seq].month=9;
 					else if(!strcmp(buf,"nov")) data[seq].month=10;
 					else if(!strcmp(buf,"dec")) data[seq].month=11;
 					else printf("month error!\n");
 					//fdata[seq].month=buf;
 					break;
 				case 4:
 					if(!strcmp(buf,"mon")) data[seq].day=0;
 					else if(!strcmp(buf,"tue")) data[seq].day=1;
 					else if(!strcmp(buf,"wed")) data[seq].day=2;
 					else if(!strcmp(buf,"thu")) data[seq].day=3;
 					else if(!strcmp(buf,"fri")) data[seq].day=4;
 					else if(!strcmp(buf,"sat")) data[seq].day=5;
 					else if(!strcmp(buf,"sun")) data[seq].day=6;
 					else printf("day error!\n");
 					//fdata[seq].day=buf;
 					break;
 				case 5:
 				case 6:
 				case 7:
 				case 8:
 				case 9:
 				case 10:
 				case 11:
 				case 12:data[seq].attribute[comma-5]=atof(buf);
 				break;		 		
 			}
 			for(int j=0;j<15;j++) buf[j]=0;	
 			cnt=0;
 		}
 		else if(buffer[i] == '\n'|| buffer[i] == '\0'){
 			counter++;
 			//printf("%d th \n",counter );
 			float temp=atof(buf);
 			if(temp == 0) data[seq].class=0; 				
 			else if(temp > 0 && temp <= 1) data[seq].class=1;
 			else if(temp > 1 && temp <= 10) data[seq].class=2;
 			else if(temp > 10 && temp <= 100) data[seq].class=3;
 			else if(temp > 100 && temp <= 1000) data[seq].class=4;
 			else {
 				data[seq].class=5;
 			}
 			
 			data[seq].index=seq;
 			for(int j=0;j<15;j++) buf[j]=0;
 			comma=0;
 			cnt=0;
 			if(counter < quantity) seq=randseq(map);//get random seq
 		}
 		else{
 			buf[cnt]=buffer[i];
 			cnt++;
 		}
 	}
 	
	free (buffer);
	
	double a;
	double t_accuracy=0;
	double accuracy=0;
	double precision[fclass]={};
	double recall[fclass]={};
/*doing k-fold cross validation(k=5)*/
	for(int i=0;i<fold;i++){
		node_num=0;
		int data_n=(quantity*(fold-1))/fold;
		int cnt=0;
		
		/*transfer the whole data into training data(150->120)*/
		//struct flower mini_data[(quantity*(fold-1))/fold]={};
		struct forest* mini_data=malloc(sizeof(struct forest)*((quantity*(fold-1))/fold));

		for(int k=0;k<fold;k++){
			for(int j=0;j<num;j++){
				if(k == i) break;
				else{
					mini_data[cnt]=data[k*num+j];
					cnt++;
				}
			}
		}
/*calculate total entropy*/
		double total_entropy=0.0;
		total_entropy=totalentropy(mini_data,data_n);
		//printf("total_entropy is:%f\n",total_entropy );
/*calculate threshold*/
		struct thre thre_arr[12][(quantity*(fold-1))/fold]={};
		threshold(mini_data,thre_arr);
		/*
		for(int j=0;j<4;j++){
			for(int k=0;k<(quantity*(fold-1))/fold;k++){
				printf("threshold[%d][%d]:%f\n",j,k,thre_arr[j][k].value );
			}
		}*/
		
		struct Node* root;
		root=DTree(mini_data,thre_arr);
		
		int class[fclass]={};
		int ans[fclass]={};
		int ans_class[fclass]={};
		int predict=0;

		
			
		//test test_data
		for(int j=0;j<num;j++){
			predict=traversal(data[i*num+j],root);
			//predict setosa
			class[predict]++; 
			//true setosa
			class[data[i*num+j].class]++;

			if(predict == data[i*num+j].class) ans_class[predict]++;	
		}
		//printf("Actual answers: type0=%d, type1=%d, type2=%d\n",ans[0],ans[1],ans[2]);
		//printf("DT's prediction is: type0=%d, type1=%d, type2=%d\n",class[0],class[1],class[2]);
		t_accuracy=0;
		for(int j=0;j<fclass;j++){
			//precision:(true&&predict) setosa/predict setosa
			precision[j]+=(double)ans_class[j]/class[j];
			//recall:(true&&predict) setosa/true setosa
			recall[j]+=(double)ans_class[j]/ans[j];
			//total accuracy:succesful predic/total data
			t_accuracy+=ans_class[j];
		}
		t_accuracy/=num;	
		
		accuracy+=t_accuracy;
	
	}	
	accuracy/=fold;
	printf("%.3f\n",accuracy );
	for(int i=0;i<fclass;i++){
		precision[i]/=fold;
		recall[i]/=fold;
		printf("%.3f %.3f\n",precision[i],recall[i] );
	}

	exit(0);

}