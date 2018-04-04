#include <iostream>
#include <string> 
#include <fstream>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
using namespace std;

struct DATA{
	float attr[32];
	int Class;
	int index;
};

struct TEST{
	float attr[32];
	int Class;
};

struct Tree{
	Tree *l, *r, *p;
	float information;
	int attr ;
	int Class;
	int ID;
	float dis;
};

struct K_nearest{
	int index;
	float dis;
};

float sort_attr(DATA info[], int attr_no, int num){

	float *info_temp = new float[num];
	for(int i=0;i<num;i++){
		info_temp[i] = info[i].attr[attr_no];
	}
	
	for(int i=0;i<num;i++){
		for(int j=0;j<num-1;j++){
			if(info_temp[j]>info_temp[j+1]){
				float temp;
				temp = info_temp[j+1];
				info_temp[j+1] = info_temp[j];
				info_temp[j] = temp;
			}
			
		}
	}
	float mid = info_temp[num/2];
	
	return mid;
	
}

int sort_id(DATA info[], int attr_no, int num){
	
	int *info_id = new int[num];
	float *info_temp = new float[num];
	for(int i=0;i<num;i++){
		info_temp[i] = info[i].attr[attr_no];
	}
	for(int i=0;i<num;i++){
		info_id[i] = info[i].index;
	}
	
	for(int i=0;i<num;i++){
		for(int j=0;j<num-1;j++){
			if(info_temp[j]>info_temp[j+1]){
				float temp;
				int temp1;
				temp = info_temp[j+1];
				info_temp[j+1] = info_temp[j];
				info_temp[j] = temp;
				temp1 = info_id[j+1];
				info_id[j+1] = info_id[j];
				info_id[j] = temp1;
			}
			
		}
	}

	int id = info_id[num/2];

	return id;
	
}


void Buildtree(Tree *ptr, DATA info[], int num, int attr_no){
	
		if(num == 0){
			ptr->Class = -1;
			ptr->ID = -1;
			ptr->l=NULL;
			ptr->r=NULL;
			return;
		}else if(num == 1){

				ptr->Class = info[0].Class;
				ptr->ID = info[0].index;
				ptr->l=NULL;
				ptr->r=NULL;	
				return;					
			//}

		}
		
		float mid = sort_attr(info, attr_no, num);
		int id = sort_id(info, attr_no, num);
		//cout<<mid<<endl;
		//cout<<id<<endl<<endl;
		DATA *info_l = new DATA[num];
		DATA *info_r = new DATA[num];
		Tree *node_l = new Tree;
		Tree *node_r = new Tree;
		ptr->attr = attr_no;
		ptr->information = mid;
		ptr->ID = id;
		ptr->l = node_l;
		ptr->r = node_r;
		node_l->p = ptr;
		node_r->p = ptr;
		
		//cout<<ptr->ID<<endl;
		
		
		int n_l =0,n_r=0;
		for(int i=0;i<num;i++){
			if(info[i].attr[attr_no] < mid){
				if(info[i].index!=id){
					//cout<<"l "<<info[i].index<<" "<<info[i].attr[attr_no]<<endl;
					info_l[n_l++] = info[i];	
				}
			}else{
				if(info[i].index!=id){
					//cout<<"r "<<info[i].index<<" "<<info[i].attr[attr_no]<<endl;
					info_r[n_r++] = info[i]; 	
				}
			}
		}
		
		//cout<<n_l<<' '<<n_r<<endl;
		//if(attr_no==2) return;
		
		Buildtree(ptr->l, info_l, n_l, (attr_no+1)%31);
		Buildtree(ptr->r, info_r, n_r, (attr_no+1)%31);
		
}

Tree *descendtree(Tree *ptr, TEST test[], int now){

	Tree *x = new Tree;
	
	while(1){
		if(ptr->l == NULL){
			if(ptr->ID==-1){
				if(ptr->p->l->ID == -1){
					x = ptr->p->r;
				}else if(ptr->p->r->ID == -1){
					x = ptr->p->l;
				}
				break;
			}else{
				x = ptr;
				break;	
			}

		}
		if(test[now].attr[ptr->attr] >= ptr->information){
			ptr = ptr->r;
		}else{
			ptr = ptr->l;
		}
	}
	return x;
}

float boundrydist(TEST test[], DATA info[], int now, Tree *x){
	float sum=0;
	for(int i=0;i<31;i++){
		float temp =(test[now].attr[i] - info[x->ID].attr[i]);
		//cout<<temp<<endl;
		sum += temp*temp;
		//cout<<sum<<endl;
	}
	float distance = sqrt(sum);
//	cout<<distance<<endl;
	return distance;
	
}

void sort_K_nearest(vector<K_nearest> *near){
	
	//cout<<near->size()<<endl;
	for(int i=0;i<near->size();i++){
		for(int j=0;j<near->size()-1;j++){
			if((*near)[j].dis > (*near)[j+1].dis){
				K_nearest temp;
				temp = (*near)[j];
				(*near)[j] = (*near)[j+1];
				(*near)[j+1] = temp;
				
			}
		}
	}
	
}

void maintian(vector<K_nearest> *near, float dis, int ID,int k){
	for(int i=0;i<near->size();i++){
		if((*near)[i].index == ID){
			return;
		}
	}
	//cout<<dis<<" "<<ID<<endl<<"size"<<near->size()<<endl;

	K_nearest in;
	in.dis = dis;
	in.index = ID;
	if(near->size()<k){
		near->push_back(in);	
		sort_K_nearest(near);
	}else{
		if(dis < (*near)[k-1].dis){
			(*near)[k-1] = in;	
			sort_K_nearest(near);
		}
		
	}
	
}

void KNN(TEST test[], DATA info[], Tree *x, vector<K_nearest> *near,int now,int K){
	int re[8400];
	for(int i=0;i<8400;i++){
		re[i] =0;
	}
	
	float distance;
	
	distance = boundrydist(test,info,now,x);
	maintian(near,distance,x->ID,K);

	
	while(x->p!=NULL){
		
		float a = test[now].attr[x->p->attr] - x->p->information;
		a = (a>=0)?a:-a;
		
		distance = boundrydist(test,info,now,x->p);

		if(distance < a){
			x = x->p;
		}else{
			if(re[x->p->ID] == 0){
				re[x->p->ID]=1;	
				if(x == x->p->r){
					x = x->p->l;
				}else{
					x = x->p->r;
				}
				x = descendtree(x, test, now);
			}else{
				x = x->p;
			}
		}	
		distance = boundrydist(test,info,now,x);	
		maintian(near,distance,x->ID,K);
		
	}
}

int main(int argc, char **argv){
	
	fstream file;
	file.open("1.csv");
	string line;
	
	DATA info[12000];
	int i=0,ran[12000];
	srand(time(NULL));
	for(int i=0;i<12000;i++){
		ran[i]=0;
	}
	
	int j=0;
	int n=0;
	while (getline( file, line,'\n')){
	 	//while(1){
	 		//n=rand()%3000;
	 		//if(ran[n]==0){
	 			i=0;
	            istringstream templine(line); 
	    		string data;
	    		while (getline(templine,data,',')) {
					if(i==31){
						info[n].Class = atof(data.c_str());
	    			}else{
	    				info[n].attr[i] =  atof(data.c_str());
	    			}
				    i++;	    		
	 			}
	 			n++;
	 			//ran[n]=1;
	 			//break;
	 		//}
		//}
 	}
    file.close();
	for(int n=0;n<12000;n++){
		for(int i=0;i<31;i++){
			cout<<info[n].attr[i]<<' ';
		}
		cout<<info[n].Class<<endl;
	}
    
    for(int i=0;i<12000;i++){
		info[i].index = i;
	}
	
	Tree *root = new Tree;
	Tree *ptr = new Tree;
	ptr = root;
	Buildtree(ptr,info,8400,0);
    //cout<<test[0].Class;

    TEST test[3600];
    for(int i=0;i<3600;i++){
    	for(int j=0;j<31;j++){
    		test[i].attr[j] = info[i+8400].attr[j];
    	}
    	test[i].Class = info[i+8400].Class;
    	//cout<<test[i].Class<<endl;
    }


    root->p = NULL;
	int K=300;
    
    //while(1){
    	
    	cout<<"KNN accuracy: ";
	
		Tree *x = new Tree;
		int T=0;
		for(int i=0;i<3600;i++){
			float cl[12];
			for(int j=0;j<12;j++){
				cl[j]=0;
			}
		    Tree *x = new Tree;
			
			x = descendtree(root, test, i);
			//cout<<"A"<<endl;
			vector<K_nearest> *near = new vector<K_nearest>;
		
			KNN(test, info, x, near, i, K);
			
			for(int i=0;i<K;i++){
				for(int j=0;j<12;j++){
					if(info[(*near)[i].index].Class == j){
						cl[j] += (float)(K-i)/(float)K;
					}	
				}
			}

			int max=0,it;
			for(int j=0;j<12;j++){
				if(cl[j]>max){
					max = cl[j];
					it = j;
				}
			}
			//cout<<it<<test[i].Class<<endl;
			//cout<<it<<test[i].Class<<endl;
			if(it == test[i].Class){
				T++;
			}
			//cout<<T<<endl;
		}
		
		float accu = T/3600.0;
		cout<<accu<<endl;
			
	   	for(int test_no=0;test_no<1;test_no++){
		    Tree *x = new Tree;
			
			x = descendtree(root, test, test_no);
			//cout<<x->ID<<endl;
			
			vector<K_nearest> *near = new vector<K_nearest>;
		
			KNN(test, info, x, near, test_no, K);
		
			for(int i=0;i<K;i++){
				cout<<(*near)[i].index<<' ';
				//cout<<(*near)[i].index<<' ';
			}	
			cout<<endl;
		}	
		

		cout<<endl;
	//}
	cout<<endl;


		return 0;
	
}
