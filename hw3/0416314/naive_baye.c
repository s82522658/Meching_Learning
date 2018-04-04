//p=a*e^-ax a=1/mean
/* fdata 
離 X 1~9
離 Y 2~9
離 month 
離 day
連 attribute[8]
*/
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<string.h>

#define smoothk 3
#define fclass 6
#define iclass 3
#define inum 150
#define fnum 517 // num of data of forestfires

int node_num;

enum Month {jan,feb,mar,apr,may,jun,jul,aug,sep,oct,nov,dec};
enum Day {mon,tue,wed,thu,fri,sat,sun};

struct forest{
	enum Month month;
	enum Day day;
	int x;
	int y;
	float attribute[8];
	int class;
	int index;
};
struct flower{
	double sepal_l;
	double sepal_w;
	double petal_l;
	double petal_w;
	int class;
};
struct fanaly{
	int x[9];
	int y[8];
	int month[12];
	int day[7];
	int num;
};


double PDF(float x,float mean){
	if(mean == 0) return 1;
	double p=(1/mean)*exp(-(x/mean));
	//printf("p:%f\n",p);
	return p;
}
int randseq(int map[],int num_data){
	int seq;
	int result;
	seq=rand()%num_data;
	while(1){
		if(map[seq]!= 1){
			result=seq;
			map[seq]++;
			return result;
		}
		else if(seq == (num_data-1)) seq=0;
		else seq++;
	}
}

int main(int argc,char* argv[]){
	srand(time(NULL));
	FILE* pfile;
 	long lSize;
	char * buffer;
	size_t result;
/*open train.csv*/
 	pfile=fopen(argv[1],"r");
 	if(pfile == NULL){
 		printf("forest file open failure!\n");
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

/*store fdata into forest strcuct*/
 	struct fanaly* analysis=calloc(fclass,sizeof(struct fanaly));
 	double mean[fclass][8]={0};
 	int comma=0;
 	int cnt=0;
 	int map[fnum]={0};
 	int seq=randseq(map,fnum);
 	int i=0;
 	char buf[16]={};
 	int counter=0;
 	//declare our training data structure
 	struct forest* fdata=malloc(sizeof(struct forest)*fnum);
 	//discard first row
 	for(i=0;buffer[i]!='\n';i++);
 	//skip the first '\n'	
 	i++;
 	//store data
 	for(;i<lSize;i++){
 		if(buffer[i] == ','){
 			comma++;
 			switch(comma){
 				case 1:fdata[seq].x=atoi(buf);
 				break;
 				case 2:fdata[seq].y=atoi(buf);
 				break;
 				case 3:
 					if(!strcmp(buf,"jan")) fdata[seq].month=0;
 					else if(!strcmp(buf,"feb")) fdata[seq].month=1;
 					else if(!strcmp(buf,"mar")) fdata[seq].month=2;
 					else if(!strcmp(buf,"apr")) fdata[seq].month=3;
 					else if(!strcmp(buf,"may")) fdata[seq].month=4;
 					else if(!strcmp(buf,"jun")) fdata[seq].month=5;
 					else if(!strcmp(buf,"jul")) fdata[seq].month=6;
 					else if(!strcmp(buf,"aug")) fdata[seq].month=7;
 					else if(!strcmp(buf,"sep")) fdata[seq].month=8;
 					else if(!strcmp(buf,"oct")) fdata[seq].month=9;
 					else if(!strcmp(buf,"nov")) fdata[seq].month=10;
 					else if(!strcmp(buf,"dec")) fdata[seq].month=11;
 					else printf("month error!\n");
 					//fdata[seq].month=buf;
 					break;
 				case 4:
 					if(!strcmp(buf,"mon")) fdata[seq].day=0;
 					else if(!strcmp(buf,"tue")) fdata[seq].day=1;
 					else if(!strcmp(buf,"wed")) fdata[seq].day=2;
 					else if(!strcmp(buf,"thu")) fdata[seq].day=3;
 					else if(!strcmp(buf,"fri")) fdata[seq].day=4;
 					else if(!strcmp(buf,"sat")) fdata[seq].day=5;
 					else if(!strcmp(buf,"sun")) fdata[seq].day=6;
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
 				case 12:fdata[seq].attribute[comma-5]=atof(buf);
 				break;		 		
 			}
 			for(int j=0;j<15;j++) buf[j]=0;	
 			cnt=0;
 		}
 		else if(buffer[i] == '\n'|| buffer[i] == '\0'){
 			counter++;
 			//printf("%d th \n",counter );
 			float temp=atof(buf);
 			if(temp == 0) fdata[seq].class=0; 				
 			else if(temp > 0 && temp <= 1) fdata[seq].class=1;
 			else if(temp > 1 && temp <= 10) fdata[seq].class=2;
 			else if(temp > 10 && temp <= 100) fdata[seq].class=3;
 			else if(temp > 100 && temp <= 1000) fdata[seq].class=4;
 			else {
 				fdata[seq].class=5;
 			}
 			if(seq <= (fnum-fnum*3/10)-1){
				analysis[fdata[seq].class].num++;
				analysis[fdata[seq].class].x[fdata[seq].x-1]++;
				analysis[fdata[seq].class].y[fdata[seq].y-2]++;
				analysis[fdata[seq].class].month[fdata[seq].month]++;
				analysis[fdata[seq].class].day[fdata[seq].day]++;
				for(int j=0;j<8;j++) mean[fdata[seq].class][j]+=fdata[seq].attribute[j];
			}
 			fdata[seq].index=seq;
 			for(int j=0;j<15;j++) buf[j]=0;
 			comma=0;
 			cnt=0;
 			if(counter < fnum) seq=randseq(map,fnum);//get random seq
 		}
 		else{
 			buf[cnt]=buffer[i];
 			cnt++;
 		}
 	}
 	for(int i=0;i<fclass;i++){
 		//printf("%d th analysis num :%d\n", i,analysis[i].num);
 		for(int j=0;j<8;j++) {
	 		if(analysis[i].num != 0) mean[i][j]/=analysis[i].num;
	 		else mean[i][j]=0;
	 		//printf("%d th mean:%f\n",j,mean[i][j] );
	 	}
 	}
 /*open iris_data.txt*/
 	pfile=fopen(argv[2],"r");
 	if(pfile == NULL){
 		printf("forest file open failure!\n");
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
 /*store iris_data*/
 	int s=0;
 	int j=0;
 	char* end;
 	double imean[iclass][4]={0};
 	int Inum[iclass]={0};
 	int imap[inum]={0};
 	cnt=0;
 	seq=randseq(imap,inum);
 	struct flower* idata=malloc(sizeof(struct flower)*inum);

	for(int i=0;i<result;i++){
		if(buffer[i]=='\n'){
			//store class into data
			for(int k=0;k<(i-j);k++) buf[k]=buffer[j+k];
			if(!strcmp(buf,"Iris-setosa")) idata[seq].class=0;
			else if(!strcmp(buf,"Iris-versicolor")) idata[seq].class=1; 
			else if(!strcmp(buf,"Iris-virginica")) idata[seq].class=2;  

			if(seq <= (inum-inum*3/10)-1){
				Inum[idata[seq].class]++;
				imean[idata[seq].class][0]+= idata[seq].sepal_l;
				imean[idata[seq].class][1]+= idata[seq].sepal_w;
				imean[idata[seq].class][2]+= idata[seq].petal_l;
				imean[idata[seq].class][3]+= idata[seq].petal_w;
			}
			
			//printf("cnt:%d,seq:%d,class:%d\n",cnt,seq,data[seq].class );
			//clear the buffer
			for(int k=0;k<16;k++) buf[k]=0;
			s=0;
			j=i+1; 
			cnt++;
			//random next data sequence
			if(i<(result-1)) seq=randseq(imap,inum);
		}
		else if(buffer[i]==','){
			//store desscriptive feature into data
			end=buffer+i-1;
			switch(s){
				case 0:idata[seq].sepal_l=strtod(buffer+j,&(end));
				case 1:idata[seq].sepal_w=strtod(buffer+j,&(end));
				case 2:idata[seq].petal_l=strtod(buffer+j,&(end));
				case 3:idata[seq].petal_w=strtod(buffer+j,&(end));
			}
			//printf("seq=%d,speal_l=%f\n",seq,data[seq].sepal_l );
			j=i+1;
			s++;
		}
	}

	for(int i=0;i<iclass;i++){
		for(int j=0;j<4;j++){
			if(Inum[i] != 0) imean[i][j]/=Inum[i];
		}
	}

 	//Test forest testdata
 	double x_p,y_p,month_p,day_p,att_p[8];
 	int correct=0;

 	for(int i=fnum-fnum*3/10;i<fnum;i++){
 		//printf("%dth\n",i );
 		double maxp=0;
 		int max_seq;
 		
 		
 		for(int j=0;j<fclass;j++){
 			double p=1;
 			double temp=1;
 			x_p=(double)(analysis[j].x[fdata[i].x-1])/(analysis[j].num);
 			y_p=(double)analysis[j].y[fdata[i].y-2]/analysis[j].num;
 			month_p=(double)analysis[j].month[fdata[i].month]/analysis[j].num;
 			day_p=(double)analysis[j].day[fdata[i].day]/analysis[j].num;
 			for(int k=0;k<8;k++) {
				att_p[k]=PDF(fdata[i].attribute[k],mean[j][k]);
				temp*=att_p[k];
				//printf("temp %f\n",temp );
			}
			//printf("xmean:%f ymean:%f month_p:%f day_p:%f\n",analysis[j].x[fdata[i].x-1] );
			//printf("x_p:%f y_p:%f month_p:%f day_p:%f temp:%f\n",x_p,y_p,month_p,day_p,temp);
 			p=p*(x_p*y_p*month_p*day_p*temp);
 			//printf("p:%.23f\n",p);
 			if(p > maxp) {
 				maxp = p;
 				max_seq = j;
 			}
 		}
 		//printf("predict seq:%d correct:%d\n",max_seq,fdata[i].class);
 		//pick the largest one
 		if(fdata[i].class == max_seq) correct++;
 	}
 	double accuracy=(double)correct/(fnum*3/10);
 	printf("forestfires accuracy:%f correct:%d\n",accuracy,correct );

 	double p=1;
 	double maxp=0;
 	int max_seq=0;
 	correct=0;
 	for(int i=inum-(inum*3/10);i<inum;i++){
 		maxp=0;
 		max_seq=0;
 		//printf("%dth,", i);
 		for(int j=0;j<iclass;j++){
 			p=1;
 			//printf("%d th\n", j);
 			p*=PDF(idata[i].sepal_l,imean[j][0]);
 			p*=PDF(idata[i].sepal_w,imean[j][1]);
 			p*=PDF(idata[i].petal_l,imean[j][2]);
 			p*=PDF(idata[i].petal_w,imean[j][3]);
 			//printf("sl:%f sw:%f pl:%f pw:%f ",idata[i].sepal_l,idata[i].sepal_w,idata[i].petal_l,idata[i].petal_w );
 			//printf("sl:%f sw:%f pl:%f pw:%f\n",imean[j][0],imean[j][1],imean[j][2],imean[j][3] );
 			//printf("p:%.23f,sepal_l:%f,mean:%f\n",p, idata[i].sepal_l,imean[j][0]);
 			if(p > maxp){
 				maxp=p;
 				max_seq=j;
 			}
 		}
 		//printf("maxp:%f max_seq:%d\n",maxp,max_seq );
 		if(idata[i].class == max_seq) {
 			correct++;
 		}
 	}
 	accuracy=(double)correct/(inum*3/10);
 	printf("iris_data accuracy:%f correct:%d\n",accuracy,correct );
	
	return 0;
}