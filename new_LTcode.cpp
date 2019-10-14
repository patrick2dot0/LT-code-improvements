#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<vector>
#include<math.h>
#include<conio.h>
#include<windows.h>
#include<algorithm>
#include <graphics.h>
//改 被channel刪掉的次數仍要進encoder，且最後x軸是有接收到的次數 
int p_size = 1000;
int num, errors = 0, all_thresh[4] = /*{1950,1200,3600,1200}*/{0,0,0,0};
double erase_prob = 0.01;
using namespace std;

double accurate = 1e-3;
int tests = (int)(100/accurate), final_tests,MAX_ARRAY = 5001;
int *solved_array, *send_times_array;

template<typename T> 
inline void vec_clr(vector<T> & v){
    vector<T> t;
    v.swap(t);
}

int getDegree()
{
	int p;
	while((p=rand())>30000);
	if(p<3)
		return 0;
	else if(p<927)
		return 1;
	else if(p<16947)
		return 2;
	else if(p<24051)
		return 4;
	else if(p<27882)
		return 9;
	else if(p<29409)
		return 30;
	else if(p<29574)
		return 52;
	else
		return 58;
}
int real_rand(int target){
	int repeat = 32768 - 32768%target;
	int tmp;
	while((tmp=rand())>=repeat);
	return tmp%target;
}
bool in(double tar){
	while(true){
		tar = tar * 2;
		int bi = floor(tar);
		int rnd = real_rand(2);
		tar = tar - bi;
		if(rnd>bi)return false;
		else if(rnd<bi)return true;
	}	
}
int encoder(vector<int> &source,vector< vector<int> > &record, bool erase)
{
	int d;
	int ret = 0;
	while((d=getDegree())==0);
	
	vector<int> lookup(p_size,0);
	vector<int> tmp;
	for(int j=0;j<d;j++){
		int n;
		do{
			n = real_rand(p_size);	
		}while(lookup[n]);
		ret ^= source[n];
		lookup[n] = 1;
		tmp.push_back(n);
	}
	if(!erase)
		record.push_back(tmp);
	return ret;
}
int find_rand_least_used(vector<int> &lookup, vector<int> l_used)
{	
	int find;
	do{	
		find = rand()%l_used.size();
	}while(lookup[l_used[find]]!=0);
	return l_used[find];
}
int edge_encoder(vector<int> &source, vector< vector<int> >&record, vector<int> &l_used,bool erase)
{
	int deg,n;  //n=被選到的bit 
	int ret = 0;

	while((deg=getDegree())==0);
	
	vector<int> tmp;
	vector<int> lookup(p_size,0);
	for(int j=0;j<deg;j++)
	{	
		if(l_used.size()==0)
			for(int i=0;i<p_size;i++)
				l_used.push_back(i);
	
		do{
			n = find_rand_least_used(lookup,l_used);
		}while(lookup[n]); //避免同一bit重複放入packet 
		ret ^= source[n];
		lookup[n] = 1;
		tmp.push_back(n);
		vector<int>::iterator pos = find(l_used.begin(),l_used.end(),n);
		l_used.erase(pos);
	}
	if(!erase)
		record.push_back(tmp);
	return ret;
}

int my_encoder(vector<int> &source,vector< vector<int> > &record,vector<int> &l_used,int &send_bits,vector< vector<int> > &chain,int times,bool erase)
{
	int deg,n;
	int ret = 0;
	while((deg=getDegree())==0);
	//printf("deg = %d\   ",deg);
	vector<int> tmp;
	vector<int> lookup(p_size,0);
	if(send_bits<p_size)
	{
		for(int j=0;j<deg;j++)
		{	
			if(l_used.size()==0)
				for(int i=0;i<p_size;i++)
					l_used.push_back(i);
			
			do{
				n = find_rand_least_used(lookup,l_used);
			}while(lookup[n]); //避免同一bit重複放入packet 
			
			chain[deg].push_back(n);
			
			ret ^= source[n];
			lookup[n] = 1;
			tmp.push_back(n);
			
			vector<int>::iterator pos = find(l_used.begin(),l_used.end(),n);
			l_used.erase(pos);
		}
	}
	else if(times<p_size)
	{
		int tag = 0;
		for(int j=0;j<deg;j++)
		{
			//printf("chain_size = %d\n",chain[deg].size());
			if(j<deg/2)
			{
				if(!chain[deg].empty()&&tag ==0)
				{
					n = chain[deg][0];
					chain[deg].erase(chain[deg].begin(),chain[deg].begin()+1);
				}
				else
				{
					do{
						n = real_rand(p_size);	
					}while(lookup[n]); 
					tag = 1;
				}
			}
			else{
				do{
					n = real_rand(p_size);	
				}while(lookup[n]);
			}
			chain[deg].push_back(n);
			ret ^= source[n];
			lookup[n] = 1;
			tmp.push_back(n);
		}
	}
	else{
		for(int j=0;j<deg;j++)
		{	
			if(l_used.size()==0)
				for(int i=0;i<p_size;i++)
					l_used.push_back(i);
		
			do{
				n = find_rand_least_used(lookup,l_used);
			}while(lookup[n]); //避免同一bit重複放入packet 
			ret ^= source[n];
			lookup[n] = 1;
			tmp.push_back(n);
			vector<int>::iterator pos = find(l_used.begin(),l_used.end(),n);
			l_used.erase(pos);
		}	
	}
	if(!erase)
		record.push_back(tmp);
	send_bits+=deg;
	return ret;
}

int patrick_encoder(vector<int> &source,vector< vector<int> > &record,vector<double>&probability,vector<bool>&G1,vector<int> &l_used,int times,bool erase)
{
	int deg,n;  //n=被選到的bit 
	int ret = 0;
	int m;
	while((deg=getDegree())==0);
	vector<int> tmp;
	vector<int> lookup(p_size,0);
	double p=1;
	int cnt=0;
	for(int ii=0;ii<G1.size();ii++){
		cnt+=G1[ii];
	}
	if(cnt!=G1.size() /*&& (times<1000)*/){
		if(cnt<deg-1){
			for(int j=0;j<deg;j++){
				do{
					m = real_rand(p_size);	
				}while(lookup[m]);
				ret ^= source[m];
				lookup[m] = 1;
				tmp.push_back(m);
				
			}
			
		}
		else{
			for(int j=0;j<deg-1;j++){
				do{
					m = real_rand(cnt);
					int temp=0;
					for(int ii=0;ii<p_size;ii++){
						if(G1[ii]==1){
							if(temp==m){
								m=ii;
								break;
							}
							temp++;
						}
					}
				}while(/*G1[m]==0||*/lookup[m]);
				ret ^= source[m];
				lookup[m] = 1;
				p *= probability[m];
				tmp.push_back(m);
			}
			if(p>0.5){
				int add;
				do{
					add=real_rand(p_size);
				}while(G1[add]);
				ret ^= source[add];
				tmp.push_back(add);
				G1[add]=1;
				probability[add]=p*(1-erase_prob);
			}
			else{
				int add;
				do{
					add=real_rand(p_size);
				}while(1-G1[add]);
				ret ^= source[add];
				tmp.push_back(add);
				double temp_prob[p_size];
				for(int ii=0;ii<tmp.size();ii++){
					temp_prob[tmp[ii]]=1;
					for(int jj=0;jj<tmp.size();jj++){
						if(ii!=jj){
							temp_prob[tmp[ii]]*=probability[tmp[jj]];
						}
					}
					temp_prob[tmp[ii]]*=(1-probability[tmp[ii]]);
					temp_prob[tmp[ii]]*=(1-erase_prob);
				}
				for(int ii=0;ii<tmp.size();ii++){
					probability[tmp[ii]]+=temp_prob[tmp[ii]];
				}
			}
		}
	}
	else{
		for(int j=0;j<deg;j++)
		{	
			if(l_used.size()==0)
				for(int i=0;i<p_size;i++)
					l_used.push_back(i);
		
			do{
				n = find_rand_least_used(lookup,l_used);
			}while(lookup[n]); //避免同一bit重複放入packet 
			ret ^= source[n];
			lookup[n] = 1;
			tmp.push_back(n);
			vector<int>::iterator pos = find(l_used.begin(),l_used.end(),n);
			l_used.erase(pos);
		}	
	}
	if(!erase)
		record.push_back(tmp);
	return ret;
}

bool decoder(vector<int> &packet,vector< vector<int> > &record,vector<int> &back_data,vector<int> &flag)
{
	
	int n = packet.size()-1;
	for(int i=0;i<record[n].size();i++)
		if(flag[record[n][i]]==1){ 
			packet[n] = packet[n] ^ back_data[record[n][i]];
			record[n].erase(record[n].begin()+i);
			i--;
		}
	if(record[n].size()==0){
		record.pop_back();
		packet.pop_back();
		return true;
	}
	if(record[n].size()>1)return true;
//	printf("%d\n",record[0].size());
	int know = record[n][0];
	back_data[know] = packet[n];
	flag[know] = 1;
	packet.pop_back();
	record.pop_back();
	
	vector<int> dolist;
	dolist.push_back(know);
	
	for(int i=0;i<dolist.size();i++){
		
		for(int j=0;j<record.size();j++){
			for(int k=0;k<record[j].size();k++){
				if(record[j][k] == dolist[i]){
					record[j].erase(record[j].begin()+k);
					packet[j] = packet[j] ^ back_data[dolist[i]];
					break;
				}	
			}
		}
		
		for(int j=0;j<record.size();j++){
			if(record[j].size()==1){
				int know = record[j][0];
				back_data[know] = packet[j];
				flag[know] = 1;
				dolist.push_back(know);
				record.erase(record.begin()+j);
				packet.erase(packet.begin()+j);
				j--;
			}
		}
		
	}
//	printf("c ");
//	printf("b\n");
	for(int i=0;i<flag.size();i++)
		if(flag[i]==0)
			return true;
	
	return false;
}
bool compare(vector<int> &a,vector<int> &b){
	for(int i=0;i<a.size();i++)
		if(a[i]!=b[i])
			return false;
	return true;
} 

void draw(int color)
{
	for(int i=1;i<MAX_ARRAY;i++)  //因為解完的話我記錄是p_size不是0，對於提早解完的情況要加上那p_size以免反而解的比晚解完的少 
		if(send_times_array[i]<final_tests)
			solved_array[i]+=(final_tests-send_times_array[i])*p_size;
			
	printf("errors = %d ",errors);
	setcolor(color);
	int i=2,k = p_size;
	double solved_dec,ber_log,solved_dec2,ber_log2;
	while(send_times_array[i+1]!=0&&i<4999)   //如果發現是最後一步，代表已經傳完，此次solver_dec會是p_size，使log10(0)出現因此不進去 
	{
		solved_dec = (double)solved_array[i]/final_tests;
		solved_dec2 = (double)solved_array[i-1]/final_tests;
		ber_log = log10((p_size-solved_dec)/(p_size));
		ber_log2 = log10((p_size-solved_dec2)/(p_size));
		//putpixel(i/2,ber_log*(1000.0/(log10(1.0/p_size))),color);
		line((i-1)/2,(int)(ber_log*(1000.0/(log10(accurate)))),i/2,(int)(ber_log2*(1000.0/(log10(accurate)))));
		//line((i-1)/2,solved_dec2,i/2,solved_dec);
		//putpixel(i/2,solved_dec,color);
		//putpixel(i/2,solved_dec2,color);
		i++;		
	}
	line((i-1)/2,(int)(ber_log*(1000.0/(log10(accurate)))),i/2,1000);
	printf("final_test = %d\n",final_tests);
	printf("solved_array = %d\n",solved_array[i-1]);
	printf("%lf\n",solved_dec);
	printf("%d\n",(int)(ber_log*(1000.0/(log10(accurate)))));
}

void Save(char *fname)
{
	double *errorrate = new double[MAX_ARRAY];
	for(int i=0;i<MAX_ARRAY;i++)  //因為解完的話我記錄是p_size不是0，對於提早解完的情況要加上那p_size以免反而解的比晚解完的少 
	{
		if(send_times_array[i]<final_tests)
			solved_array[i]+=(final_tests-send_times_array[i])*p_size;
		errorrate[i] = (p_size-((double)solved_array[i]/final_tests))/p_size;
	}
	
	//output to file
	FILE *fp;
	strcat(fname,".txt");
	if((fp=fopen(fname,"w"))==NULL){
		printf("Cannot opne file!\n");
		exit(1);
	}
	if(MAX_ARRAY>=1){
		fprintf(fp,"%.5lf",errorrate[1]);
	}
	for(int i=2;i<MAX_ARRAY;i++)
		fprintf(fp," %.5lf",errorrate[i]);
		
	fclose(fp); 	
}

void Demo(char *type,int thresh)
{	
	vector<int>data;
	vector<int>back_data(p_size,0);  //已經被解開的bit，list會變成1 
	vector<int>packet;
	vector<int>flag(p_size,0);
	vector<int>temp; //for myencoder
	vector<double>probability(p_size,0);  //for patrick_encoder
	vector<bool>G1(p_size,0);//for patrick_encoder
	int times=0;
	int times_for_p=0;  //times for patrick's encoder
	int send_bits = 0;  //for myencoder
	vector< vector<int> > chain; //for myencoder
	vector< vector<int> > record;
	vector<int> l_used;
	for(int i=0;i<p_size;i++)
		data.push_back(1);
	
	for(int i=0;i<59;i++)
		chain.push_back(temp);   

	do{
		if(!in(erase_prob)){
			if(strcmp(type,"encoder")==0)
			{
				packet.push_back(encoder(data,record,false));
			}
			else if(strcmp(type,"edge_encoder")==0)
			{ 
				packet.push_back(edge_encoder(data,record,l_used,false));
			}
			else if(strcmp(type,"my_encoder")==0)
				packet.push_back(my_encoder(data,record,l_used,send_bits,chain,times,false));
			else if(strcmp(type,"patrick_encoder")==0)
				packet.push_back(patrick_encoder(data,record,probability,G1,l_used,times_for_p,false));		
			decoder(packet,record,back_data,flag);
		
			times++;
			times_for_p++;
			int solved=0;
			for(int i=0;i<p_size;i++)
				if(flag[i]==1)
				{
					solved++;
				}
			if(times<MAX_ARRAY){
				solved_array[times]+=solved;
				send_times_array[times]++;
			}
			else
				break;
			if(solved==p_size)
			{
				if(times>thresh)
					errors++; 
				break;
			} 
		}
		else
		{
			if(strcmp(type,"encoder")==0)
			{
				encoder(data,record,true);
			}
			else if(strcmp(type,"edge_encoder")==0)
			{ 
				edge_encoder(data,record,l_used,true);
			}
			else if(strcmp(type,"patrick_encoder")==0)
				patrick_encoder(data,record,probability,G1,l_used,times_for_p,true);
		}		
	}while(1);
	vec_clr(l_used);
	vec_clr(temp);
	vec_clr(chain);
}

int main(int argc,char** argv)
{
	initwindow(p_size*2,p_size);

	srand(time(NULL));

	solved_array = new int[MAX_ARRAY];     //紀錄在1000次test內每次隨send packet數被解bit數量的和 
	send_times_array = new int[MAX_ARRAY]; //紀錄在1000次test內send packet=某數發生幾次 

	char a[20];
	int now_times;
	
	int line_log = log10(500/(p_size));
	
	
	setcolor(WHITE);
	line(500,0,500,1000);

	line(0,log10(500.0/(p_size))*(1000.0/(log10(accurate))),p_size*2,log10(500.0/(p_size))*(1000.0/(log10(accurate))));
	line(0,log10(100.0/(p_size))*(1000.0/(log10(accurate))),p_size*2,log10(100.0/(p_size))*(1000.0/(log10(accurate))));
	line(0,log10(10.0/(p_size))*(1000.0/(log10(accurate))),p_size*2,log10(10.0/(p_size))*(1000.0/(log10(accurate))));
	
	
	strcpy(a, "encoder");
	for(int i=0;i<MAX_ARRAY;i++)
	{ 
		solved_array[i] = 0;
		send_times_array[i] = 0;
	}	 
	for(now_times=0;now_times<tests||errors<(int)(1/erase_prob);now_times++)
		Demo(a,all_thresh[0]);
	now_times>tests? final_tests=now_times : final_tests=tests;	
	printf("%d \n",final_tests);
	errors = 0;
	//draw(BLUE);	
	Save(a);

		
	strcpy(a, "edge_encoder");	
	for(int i=0;i<MAX_ARRAY;i++)
	{ 
		solved_array[i] = 0;
		send_times_array[i] = 0;
	}	
	for(now_times=0;now_times<tests/*||errors<(int)(1/erase_prob)*/;now_times++)
		Demo(a,all_thresh[1]);
	now_times>tests? final_tests=now_times : final_tests=tests;	
	errors = 0;
	printf("%d \n",final_tests);
	//draw(CYAN);
	Save(a); 
/*	
	erase_prob = 0.2;
	strcpy(a, "edge_encoder");	
	for(int i=0;i<MAX_ARRAY;i++)
	{ 
		solved_array[i] = 0;
		send_times_array[i] = 0;
	}	
	for(now_times=0;now_times<tests||errors<(int)(1/erase_prob);now_times++)
		Demo(a,all_thresh[1]);
	now_times>tests? final_tests=now_times : final_tests=tests;	
	errors = 0;
	printf("%d \n",final_tests);
	draw(CYAN);
*/	
/*	erase_prob=0.01;
	strcpy(a, "my_encoder");	
	for(int i=0;i<MAX_ARRAY;i++)
	{ 
		solved_array[i] = 0;
		send_times_array[i] = 0;
	}	
	for(now_times=0;now_times<tests||errors<(int)(1/erase_prob);now_times++)
		Demo(a,all_thresh[2]);
	now_times>tests? final_tests=now_times : final_tests=tests;	
	errors = 0;
	printf("%d \n",final_tests);
	draw(YELLOW);
*/
/*	
	erase_prob=0.2;
	strcpy(a, "my_encoder");	
	for(int i=0;i<MAX_ARRAY;i++)
	{ 
		solved_array[i] = 0;
		send_times_array[i] = 0;
	}	
	for(now_times=0;now_times<tests||errors<(int)(1/erase_prob);now_times++)
		Demo(a,all_thresh[2]);
	now_times>tests? final_tests=now_times : final_tests=tests;	
	errors = 0;
	printf("%d \n",final_tests);
	draw(GREEN);
*/	
	
	
	strcpy(a, "patrick_encoder");	
	for(int i=0;i<MAX_ARRAY;i++)
	{ 
		solved_array[i] = 0;
		send_times_array[i] = 0;
	}	
	for(now_times=0;now_times<tests/*||errors<(int)(1/erase_prob)*/;now_times++)
		Demo(a,all_thresh[3]);
	now_times>tests? final_tests=now_times : final_tests=tests;	
	errors = 0;
	printf("%d \n",final_tests);
	//draw(GREEN);
	Save(a);
	
	getch();
	closegraph();
/*	
	for(int i=0;i<num;i++){
		vector<int> packet;
		vector<int> data(p_size,0);
		int times = 0;
		vector<int> back_data(p_size,0);
		vector<int> flag(p_size,0);
		unsigned char trew[p_size/8];
//		fread(trew,1,p_size/8,fp);
		
		int size = i==num-1?sz%p_size:p_size;
				
		for(int j=0;j<p_size;j+=8){
			for(int k=0;k<8;k++)
				data[j+k] = ((1<<(7-k))&trew[j/8])>>(7-k);
		}
		do{
			if(!in(erase_prob)){
				packet.push_back(edge_encoder(data,record,count,size));
				//if(i==num-1)
				//	printf("%d\n",times);
				if(!decoder(packet,record,back_data,flag))
					break;
			}
			
			times++;
		}while(true);
		
		//printf("%d times to sucess , rate = %lf , ",times,double(times)/p_size);
		//if(compare(data,back_data))
		//	printf("compare correct\n");
		//else
		//	printf("compare wrong!!!!!!!!!!!!\n");
		
		unsigned char asdf[p_size/8];
		for(int j=0;j<back_data.size();j+=8){
			int tmm = 0;
			for(int k=0;k<8;k++){
//				printf("%d\n",back_data[j+k]);
				tmm |= back_data[j+k]<<(7-k);
			}

			unsigned char xxx = (unsigned char)tmm;
			asdf[j/8] = xxx;
				
		}
		
		int write_n = ((sz - i*p_size)>=p_size)?p_size/8:(sz - i*p_size)/8;
		if(i==0)
			fwrite(trew,1,p_size/8,fp2);
		else
			fwrite(asdf,1,write_n,fp2);
		printf("%d\n",check++);
		
	}
	fclose(fp);
	fclose(fp2);*/
}
