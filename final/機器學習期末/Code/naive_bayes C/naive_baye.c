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
#define attrnum 29
#define zclass 12
#define znum 11914 // num of data 

int node_num;

struct zodiac{
	float attribute[attrnum];
	int type;//self(0) or others(1)
	int class;
	int index;
};

struct analy{
	int self_num;
	int others_num;
};


double PDF(float x,float mean){
	if(mean == 0) return 1;
	double p=(1/mean)*exp(-(x/mean));
	//printf("p:%f\n",p);
	return p;
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
 	struct analy* analysis=calloc(zclass,sizeof(struct analy));
 	double self_mean[zclass][attrnum]={0};
 	double others_mean[zclass][attrnum]={0};
 	
 	int comma=0;
 	int cnt=0;
 	int seq=0;
 	int i=0;
 	char buf[50]={};
 	int self_num=0;
 	int others_num=0;
 	int temp=0;
 	//declare our training data structure
 	struct zodiac* data=malloc(sizeof(struct zodiac)*znum);
 	//discard first row
 	for(i=0;buffer[i]!='\n';i++);
 	//skip the first '\n'	
 	i++;
 	//store data
 	for(;i<lSize;i++){
 		if(buffer[i] == ','){
 			comma++;
 			switch(comma){
 				case 1: break;
 				case 2:
 					if(!strcmp(buf,"我本人")) {
 						data[seq].type=0;
 						self_num++;
 					}
 					else if(!strcmp(buf,"他人")) {
 						data[seq].type=1;
 						others_num++;
 					}
 				break;
 				case 3:
 					if(!strcmp(buf,"摩羯座")) data[seq].class=0;
 					else if(!strcmp(buf,"水瓶座")) data[seq].class=1;
 					else if(!strcmp(buf,"雙魚座")) data[seq].class=2;
 					else if(!strcmp(buf,"牡羊座")) data[seq].class=3;
 					else if(!strcmp(buf,"金牛座")) data[seq].class=4;
 					else if(!strcmp(buf,"雙子座")) data[seq].class=5;
 					else if(!strcmp(buf,"巨蟹座")) data[seq].class=6;
 					else if(!strcmp(buf,"獅子座")) data[seq].class=7;
 					else if(!strcmp(buf,"處女座")) data[seq].class=8;
 					else if(!strcmp(buf,"天秤座")) data[seq].class=9;
 					else if(!strcmp(buf,"天蠍座")) data[seq].class=10;
 					else if(!strcmp(buf,"射手座")) data[seq].class=11;
 					else printf("zodiac error!\n");
 					//fdata[seq].month=buf;
 					break;
 				case 4:
 				case 5:
 				case 6:
 				case 7:
 				case 8:
 				case 9:
 				case 10:
 				case 11:
 				case 12:
 				case 13:
 				case 14:
 				case 15:
 				case 16:
 				case 17:
 				case 18:
 				case 19:
 				case 20:
 				case 21:
 					temp=atoi(buf);
 					if(1<=temp && temp < 3) temp=1;
 					else if(3==temp) temp=2;
 					else if(4<=temp && temp < 6) temp=3;
 					data[seq].attribute[comma-4]=temp; 
 				break;
 				case 22://潔癖
 					temp=atoi(buf);
 					if(1<=temp && temp < 3) temp=1;
 					else if(3==temp) temp=2;
 					else if(4<=temp && temp < 6) temp=3;

 					if(seq < 66) data[seq].attribute[comma-4]=6-temp; 
 					else data[seq].attribute[comma-4]=temp; 
 				break;	
 				//case 23://專情
 				//case 24://重視友情
 				case 25:
 				case 26:
 				case 27:
 				case 28:
 				case 29:
 				case 30:
 				case 31:
 				case 32:
 				case 33:
 				case 34:
 					temp=atoi(buf);
 					if(1<=temp && temp < 3) temp=1;
 					else if(3==temp) temp=2;
 					else if(4<=temp && temp < 6) temp=3;
 					data[seq].attribute[comma-6]=temp;
 				break;	 		
 			}
 			for(int j=0;j<50;j++) buf[j]=0;	
 			cnt=0;
 		}
 		else if(buffer[i] == '\n'|| buffer[i] == '\0'){
 			//printf("%d th \n",counter );
 			if(seq <= (znum-znum*3/10)-1){
 				if(data[seq].type == 0) {
 					analysis[data[seq].class].self_num++;
 					for(int j=0;j<attrnum;j++) self_mean[data[seq].class][j]+=data[seq].attribute[j];
 				}
 				else if(data[seq].type == 1){
 					analysis[data[seq].class].others_num++;
					for(int j=0;j<attrnum;j++) others_mean[data[seq].class][j]+=data[seq].attribute[j];
				}
			}
 			data[seq].index=seq;
 			for(int j=0;j<50;j++) buf[j]=0;
 			comma=0;
 			cnt=0;
 			seq++;//get random seq
 		}
 		else if (buffer[i] != '"'){
 			buf[cnt]=buffer[i];
 			cnt++;
 		}
 	}
 	for(int i=0;i<zclass;i++){
 		//printf("%d th analysis num :%d\n", i,analysis[i].num);
 		for(int j=0;j<attrnum;j++) {
	 		if(analysis[i].self_num != 0) self_mean[i][j]/=analysis[i].self_num;
	 		else self_mean[i][j]=0;
	 		if(analysis[i].others_num != 0) others_mean[i][j]/=analysis[i].others_num;
	 		else others_mean[i][j]=0;
	 		//printf("%d th mean:%f\n",j,mean[i][j] );
	 	}
 	}


 	//Test forest testdata
 	double satt_p[attrnum];
 	double oatt_p[attrnum];
 	int scorrect=0,ocorrect=0;
 	int snum=0,onum=0;

 	for(int i=znum-znum*3/10;i<znum;i++){
 		//printf("%dth\n",i );
 		double smaxp=0,omaxp=0;
 		int smax_seq,omax_seq;
 		
 		for(int j=0;j<zclass;j++){
 			double sp=1,op=1;
 			for(int k=0;k<attrnum;k++) {
				if(data[i].type == 0) {
					satt_p[k]=PDF(data[i].attribute[k],self_mean[j][k]);
					sp*=satt_p[k];
				}
				else {
					oatt_p[k]=PDF(data[i].attribute[k],others_mean[j][k]);
					op*=oatt_p[k];
				}
				//printf("temp %f\n",temp );
			}
			//printf("xmean:%f ymean:%f month_p:%f day_p:%f\n",analysis[j].x[fdata[i].x-1] );
			//printf("x_p:%f y_p:%f month_p:%f day_p:%f temp:%f\n",x_p,y_p,month_p,day_p,temp);
 			//printf("p:%.23f\n",p);
 			if(data[i].type == 0){
 				
 				if(sp > smaxp) {
	 				smaxp = sp;
	 				smax_seq = j;
	 			}
 			}
 			else{
 				
 				if(op > omaxp) {
	 				omaxp = op;
	 				omax_seq = j;
	 			}
 			}
 			

 		}
 		//printf("predict seq:%d correct:%d\n",max_seq,fdata[i].class);
 		//pick the largest one
 		if(data[i].type == 0){
 			snum++;
 			if(data[i].class == smax_seq) scorrect++;
 		}
 		else {
 			onum++;
 			if(data[i].class == omax_seq) ocorrect++;
 		}
 	}
 	double saccuracy=(double)scorrect/(snum);
 	double oaccuracy=(double)ocorrect/(onum);
	printf("total selfnum :%d total othersnum:%d\n",self_num,others_num);
 	printf("self testnum:%d others testnum:%d\n",snum,onum );
 	printf("(self version)zodiac accuracy:%f correct:%d\n",saccuracy,scorrect );
 	printf("(others version)zodiac accuracy:%f correct:%d\n",oaccuracy,ocorrect );

	
	return 0;
}