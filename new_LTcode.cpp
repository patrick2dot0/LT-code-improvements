#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<vector>
#include<math.h>
#include<conio.h>
#include<windows.h>
#include<algorithm>
#include <graphics.h>
//�� �Qchannel�R�������Ƥ��n�iencoder�A�B�̫�x�b�O�������쪺���� 
int p_size = 1000;
int num, errors = 0, all_thresh[4] = /*{1950,1200,3600,1200}*/{0,0,0,0};
double erase_prob = 0.1;
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
	/*int deg,n;  //n=�Q��쪺bit 
	int ret = 0;

	while((deg=getDegree())==0);
	
	vector<int> tmp;
	vector<int> lookup(p_size,0);
	for 
		if(l_used.size()==0)
			for(int i=0;i<p_size;i++)
				l_used.push_back(i);
	
		do{
			n = find_rand_least_used(lookup,l_used);
		}while(lookup[n]); //�קK�P�@bit���Ʃ�Jpacket 
		ret ^= source[n];
		lookup[n] = 1;
		tmp.push_back(n);
		vector<int>::iterator pos = find(l_used.begin(),l_used.end(),n);
		l_used.erase(pos);
	}
	if(!erase)
		record.push_back(tmp);
	return ret;*/ 
}

//source, record is used for decoder
//l_used contains input symbols which are linked for less times
//all_chain contains 1000 output symbols(no order) and the data are the used output symbols  //���Aall_chain�ΨӳB�zchain overlap����deg-1������ 
//times shows how many output symbols have been successfully sent
//erase shows this packet is sent successfully or not
//�`�Nchains_sizes���j�p�A0�Y�S��chain�A�ëDindex�q0�}�l 
//chains_deg4s�����o��chain�w�g���X�ӯS��deg4 

int my_encoder(vector<int> &high_deg_stack, vector<int> &cut_bit, vector<int> &source,vector< vector<int> > &record,vector< vector<int> > &chains,vector<int> &chains_deg4s,vector<bool> &chains_complete,vector<int> &chains_sizes,int &chains_ind,vector<int>&chain_deg,vector<int>&l_used,vector<int> &all_chain,vector<int> &deg1chain,vector<int> &deg4chain,int &ind1,int times,bool erase)
{
	bool no_overlap = false,linkdeg4;
	int deg,n,tag,special;
	int ret = 0;
	int high_deg_use = -1, high_deg_ind1 = 0, high_deg_ind2 = 0; //for high_deg under 1000 symbols
	vector<int> non_chosen_bit1;  //for high_deg under 1000 symbols
	while((deg=getDegree())==0);
	while(deg>9 && high_deg_stack.size()+times<1000)  //high deg���d�ۤ���A�U�h 
	{
		high_deg_stack.push_back(deg);
		while((deg=getDegree())==0);
	}
	if((!high_deg_stack.empty())&&high_deg_stack.size()+times==1000)
	{
		deg = high_deg_stack.back();
		high_deg_stack.pop_back();
	}
	vector<int> tmp;
	vector<int> lookup(p_size,0);
	
//	printf("deg = %d\n",deg);
	if(times<1500&&all_chain[1000]!=-1)
	{
		int total_chain_size = 0;   //�ثe�Ҧ��pchain�[�_�Ӫ�����(�`�N�Y���۳s�B�|���ƺ��) 
		for (int cc=0;cc<chains.size();cc++)
			total_chain_size += chains[cc].size();
		
//		p+rintf("total_chain_size : %d\n",total_chain_size);
		
		int chains_complete_num = 0;   //�ثe�Ҧ��w�g�s�_�t�@��chain�Y���pchain�ƶq 
		for(int i=0;i<chains_complete.size();i++)
			if(chains_complete[i] ==true )
				chains_complete_num++;
//		printf("complete # : %d\n",chains_complete_num);
		// �W��-�U�� �Y�u���`chain������  
		bool all_bit_sent = (total_chain_size-chains_complete_num==1000);
		
		tag = true;
		int special = rand()%3;
		if(times>900) special = 1;   //������b�s�W�S��deg4 
		
		linkdeg4 = false;   //�קK�b�s��overlap��deg-2��deg4���ɿ��s 
		bool fine_deg4 = false;  //�ΨӳB�z��deg4�̫��ӧ����ɡA�]�����sdeg4���A�ӥߨ�S�i�h�s���sdeg4���A�����D 
		if(deg4chain.size()%4==2){   //�B�z�n�s��overlap��deg-2��deg4������ 
			int a = deg4chain.back();
			deg4chain.pop_back();
			int b = deg4chain.back();;
			deg4chain.pop_back();
			if(chains_sizes[chains_ind]>=chains[chains_ind].size())
			{
				chains[chains_ind].push_back(a);
				chains[chains_ind].push_back(b);
			}
			else
			{
				chains[chains_ind].insert(chains[chains_ind].begin()+chains_sizes[chains_ind],a);
				chains[chains_ind].insert(chains[chains_ind].begin()+chains_sizes[chains_ind]-1,b);
			}
			chains_sizes[chains_ind]+=2;
			chains_deg4s[chains_ind]++;
			fine_deg4 = true;
			cut_bit.push_back(a);
		}
		if(deg4chain.size()%4!=0) linkdeg4 = true;
		
		for(int j=0;j<deg;j++)
		{			
			if(all_bit_sent) tag = true;   //all_bit_sent�A�h��j-1 bit�����A�H�K�� 
			
			if(l_used.size()==0)     //�ΨӬ�input symbol��������symbol�٨S�Q�Ǧ�chain 
				for(int i=0;i<p_size;i++)
					l_used.push_back(i);
			if(deg <= 9)  //Deg<=9 ����low degree, �]���X�{�v�`�M�F9X% 
			{	
				if(deg==1){    //deg1�n�񪺦�m�A �Y�w�g��31��chain�A���A�}�s��chain 
					bool temp = true;
					for(int i=0;i<chains.size();i++){
						if(all_chain[chains[i][0]]==0)
						{
							n = chains[i][0];
							temp = false;
							break;
						}
					}
					if(temp && chains.size()<31)
					{
						n = l_used[real_rand(p_size)%l_used.size()];
						vector<int>::iterator pos = find(l_used.begin(),l_used.end(),n);
						l_used.erase(pos);
						tag = false;	
						
						//all_chain[deg1chain[ind1]] = n;
						vector<int> tempo;
						chains.push_back(tempo);
						chains_complete.push_back(false);
						chains[chains.size()-1].push_back(n);
						chains_sizes[chains.size()-1] = 1;
					}
					else if(temp)
					{
						if(cut_bit.empty())
							n = rand()%p_size;
						else{
							n = cut_bit[rand()%cut_bit.size()];
							vector<int>::iterator pos = find(cut_bit.begin(),cut_bit.end(),n);
							cut_bit.erase(pos);
						}
					}
					all_chain[n] = n;					
//					_getch();
				}
				else if(deg==4 && special==0 && linkdeg4==false){  //1/3���|deg4��overlap�Odeg-2�A�o�����o��deg4�w�m�n 
					n = l_used[real_rand(p_size)%l_used.size()];
					vector<int>::iterator pos = find(l_used.begin(),l_used.end(),n);
					l_used.erase(pos);

					deg4chain.push_back(n);
				}
				else if((linkdeg4==true&&j==deg-1) ||(fine_deg4==false&&((deg4chain.size()%4!=0&&j==deg-1)||(j==deg-1&&deg4chain.size()!=0 && chains_sizes[chains_ind]>=11 && chains_sizes[chains_ind]<23))&&(j==deg-1&&chains_deg4s[chains_ind]<3 && chains_complete[chains_ind]==false))){  //�s�Woverlap��deg-2��deg4(�`�Nchains_sizes�|�W�[�@�w�O��deg�����̫�@��bit�F) 
//					printf("spe:chains.size() = %d  chains_ind = %d  this size = %d",chains.size(),chains_ind,chains_sizes[chains_ind]);                   //����S��deg4�X�{�b�U�pchain�����߳��� - 34   
					
	//				printf("\ndeg4chain: ");
		//			for(int ss=0;ss<deg4chain.size();ss++)
			//			printf("%d ",deg4chain[ss]);
				//	printf("\n");
				//	_getch();
		//			if(deg4chain.size()%4==2)
		//				deg4chain.pop_back();
					n = deg4chain[deg4chain.size()-1];
					deg4chain.pop_back();
					
					if(chains_sizes[chains_ind]>=chains[chains_ind].size())
						chains[chains_ind].push_back(n);
					else
						chains[chains_ind].insert(chains[chains_ind].begin()+chains_sizes[chains_ind],n);
				}
				else{   
					printf("chains.size() = %d  chains_ind = %d  this size = %d",chains.size(),chains_ind,chains_sizes[chains_ind]);
					if(tag==false||chains.size()==0){ // �ǨS�ǹL�� 
						n = l_used[real_rand(p_size)%l_used.size()];
						vector<int>::iterator pos = find(l_used.begin(),l_used.end(),n);
						l_used.erase(pos);
						tag = false;
						if(chains.size()==0)     //�Ĥ@��deg����1�A�����}�@�ӷs��chain 
						{
							vector<int> tempo;
							chains.push_back(tempo);
							chains_complete.push_back(false);
						}
						chains[chains_ind].push_back(n);
						if(no_overlap == true && j==deg-1)
							chain_deg[n] = deg;	//�B�zdeg4��9���������boverlap������
						
						if(chains[chains_ind].size()==1&&deg==2)//�B�zdeg2�}�Y�ɭ��ƶǤG���@��packet�����D 
							chains_sizes[chains_ind]++;	
					}
					else if(deg-1>chains_sizes[chains_ind])  //ex:chain��overlap���פ����H�����A�����᩵��chain 
					{
						n = chains[chains_ind][chains[chains_ind].size()-j-1];
						if(j==deg-2||chains[chains_ind].size()-j-1==0) tag = false;
						no_overlap = true;
					}
					else{
						n = chains[chains_ind][chains_sizes[chains_ind]-j-1];
						if(j==deg-2)  //���\overlap deg-1��
						{
							if(chains[chains_ind].size()>chains_sizes[chains_ind]||chains_sizes[chains_ind]>32)
								tag = true;
							else
								tag = false;
							
							chains_sizes[chains_ind]++;
						}
						else if(j==deg-1&&((chains_sizes[chains_ind]>32&&linkdeg4==false)||all_bit_sent))  //��chain�����A�s��U�@��chain�Y//�|�Ө�o��chains�@�w�O�٨Scomplete 
						{		
							int temp_ind = chains_ind;
							bool full_chains = false;
							if((chains_ind<chains.size()-1)) //�s��U�@��chain���Y  
								n = chains[chains_ind+1][0];
							else if(chains_ind==30) //�w�O��31��chain,�h�s�^�Ĥ@��chain
							{
								n = chains[0][0];
								//full_chains = true;
								
							}
							else if(chains_ind==chains.size()-1)//�}�@�ӷs��chain
							{
								n = l_used[real_rand(p_size)%l_used.size()];
								vector<int>::iterator pos = find(l_used.begin(),l_used.end(),n);
								l_used.erase(pos);
								
								vector<int> tempo;
								chains.push_back(tempo);
								chains_complete.push_back(false);
								chains_ind++;
								chains[chains_ind].push_back(n);
							} 	
							//if(!full_chains){
								chains[temp_ind].push_back(n);
								chains_sizes[temp_ind]++;
							//}
							chains_complete[temp_ind] = true;
						/*	if(all_bit_sent){
								printf("temp_ind = %d\n",temp_ind);
								printf("n = %d\n",n);
								_getch();
							}*/ 
						}
						else if(j==deg-1){
							while(chains_sizes[chains_ind]-1<chains[chains_ind].size() && chain_deg[chains[chains_ind][chains_sizes[chains_ind]-1]]!=0)   //�H�e�S��overlap deg-1��deg4 deg9�p���w�g�i�H�Ѷ}�A���ݭn�Aoverlap�̦� 
							{
								chains_sizes[chains_ind]++;
								if(chains_sizes[chains_ind]>=11&&deg4chain.size()!=0&&chains_deg4s[chains_ind]<3)
								{
									linkdeg4 = true;
									break;
								}
							}
							if(linkdeg4==true){
//								printf("\n deg4chain_size : %d\nchains_deg4s[chains_ind] : %d\n",deg4chain.size(),chains_deg4s[chains_ind]);
//								_getch();
								j--;
								continue;
							}
							if(chains_sizes[chains_ind]>chains[chains_ind].size())
							{
								n = l_used[real_rand(p_size)%l_used.size()];
								vector<int>::iterator pos = find(l_used.begin(),l_used.end(),n);
								l_used.erase(pos);
								chains[chains_ind].push_back(n);
							} 
							else
								n = chains[chains_ind][chains_sizes[chains_ind]-1];
						}
					}
				}
				printf(" n = %d\n",n);
			}
			else // this is for high deg before chain complete 
			{
				int x = rand()%2;
				printf("times = %d deg = %d  j = %d hello!\n",times,deg,j);
				do{
					if(j == deg-1 && cut_bit.empty()==false) 
					{
						n = cut_bit.back();
						cut_bit.pop_back();
					}
					else if(cut_bit.empty()==false)
					{
						//��ӳshigh deg���W�h ---- ���q�Ҧ��pchain����1 bit��@�ӡA�ӫᦳ50%���v�~���Pchain��2 bit�A
						//�t�~50%���v��s��chain�A�H������
						//�M�w�s�s��chain��1 bit || ��high deg���Ĥ@�ӳs || �pchain���פ����H�ѵ� 
						if(x==0||high_deg_use == -1||high_deg_ind1>=chains[high_deg_use].size()-1) // ���A�Y�P�@��high_deg��쭫�Ъ��pchain�|�]lookup�ӭ��� 
						{
							if(non_chosen_bit1.empty()){
								high_deg_ind2++;
								for(int i=0;i<chains.size();i++)   //�j�p�������pchain���b�[�i�h��high deg�� 
									if(chains[i].size()>high_deg_ind2)
										non_chosen_bit1.push_back(i);
							}
							high_deg_use = non_chosen_bit1[rand()%non_chosen_bit1.size()];
							vector<int>::iterator pos = find(non_chosen_bit1.begin(),non_chosen_bit1.end(),high_deg_use);
							non_chosen_bit1.erase(pos);
							
			//				for(int i=0;i<non_chosen_bit1.size();i++)
			//					printf("%d ",non_chosen_bit1[i]);
			//				printf("\n");
							high_deg_ind1 = high_deg_ind2;;
							n = chains[high_deg_use][high_deg_ind1];;
						} 
						else 
						{
							n = chains[high_deg_use][++high_deg_ind1];
						}
											
					}
					else{
			//			printf("no_cut_bit\n");
						n = real_rand(p_size);	
					}
				}while(lookup[n]);
			}
			ret ^= source[n];
			lookup[n] = 1;
			tmp.push_back(n);
		}
		
		printf("times :%d\n",times);
		_getch();
		//�Y�ݩ�Ĳoverlap��deg-2���ؼЮɶ�(�Ĥ@�P�_������0)�A������chain 
		linkdeg4 = (deg4chain.size()%4!=0);
/*		if(linkdeg4==false && chains.size()>1&&(chains_sizes[chains_ind]>=chains_sizes[(chains_ind+1)%chains.size()]||chains_sizes[chains_ind]>32))
		{ 
			chains_ind = (chains_ind+1)%chains.size();
		}
*/		int temp_ind = chains_ind;
		while((chains_complete.size()!=0 && chains_complete[chains_ind]==true) || (chains.size()>1&&(chains_sizes[chains_ind]>=chains_sizes[(chains_ind+1)%chains.size()])))  //�o��chain�w�����A�ΤU�@��chain����u�h�����ܤU�@��chain 
		{
			chains_ind = (chains_ind+1)%chains.size();
			if(chains_ind==temp_ind)  //�Ҧ�chain���w�����A�����|�o�͡A�b��chains�ܦ�complete����Y�J�즹���p�|�����[�}�@��chain 
			{
//				printf("times = %d  chains_size = %d\n",times,chains.size());
				if(chains_complete.size()!=0 && chains_complete[chains_ind]==true)
				{
				 
					all_chain[1000] = -1;
					//printf("all_chain finished : times = %d\n",times);
					//_getch();
				}
				break;
			}
		}
//----------------
/*		if(deg == 1)
		{
			int final_test[1000] = {0};
			printf("\ndeg = %d\n",deg);
			//printf ("times = %d\n",times);
			printf("n = %d\n", n);
			for(int xx=0;xx<chains.size();xx++){
				printf("chain:%d   ", xx);
				//std::sort(chains[xx].begin(),chains[xx].end());
				for(int cc=0;cc<chains[xx].size();cc++)
				{
					printf("%d ",chains[xx][cc]);
		//			final_test[chains[xx][cc]] += 1;;
				}
				printf("\n");
//				printf("chdeg:     ");
				for(int cc=0;cc<chains[xx].size();cc++)
				{
					printf("%d ",chain_deg[chains[xx][cc]]);
				}
				printf("\n");
			}
			for(int i=0;i<chains.size();i++)
				printf("%d ",chains[i].size());
			printf("\n");
			printf("deg4chain : ");
			for(int i=0;i<deg4chain.size();i++)  //���կS��deg4���S���ݯd�٨S�Ϊ� 
				printf("%d ",deg4chain[i]);
	//		printf("\nfinal_test : \n");
	//		for(int i=0;i<1000;i++)           //�����٦������٨S�Q�ǹL(�bchain��) 
	//			if(final_test[i]==0)
	//				printf("%d ", i);
			printf("\n");
			
			printf("cut_bit : ");
			for(int qq=0;qq<cut_bit.size();qq++)
				printf("%d  ",cut_bit[qq]);
			printf("\n");
			
			_getch();
		} */
	}
	else{
		for(int j=0;j<deg;j++)
		{	
			/*if(l_used.size()==0)
				for(int i=0;i<p_size;i++)
					l_used.push_back(i);
		
			do{
				n = find_rand_least_used(lookup,l_used);
			}while(lookup[n]); //�קK�P�@bit���Ʃ�Jpacket 
			ret ^= source[n];
			lookup[n] = 1;
			tmp.push_back(n);
			vector<int>::iterator pos = find(l_used.begin(),l_used.end(),n);
			l_used.erase(pos);*/
			do{
				n = real_rand(p_size);	
			}while(lookup[n]);
			ret ^= source[n];
			lookup[n] = 1;
			tmp.push_back(n);
		}	
	}
	if(!erase)
		record.push_back(tmp);
	return ret;
}

int patrick_encoder_forced1000_least(vector<int> &source,vector< vector<int> > &record,vector<double>&probability,vector<bool>&G1,vector<int>&l_used,int times,bool erase)
{
	int deg,n;  //n=�Q��쪺bit 
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
	if(cnt!=G1.size() && (times<1000)){
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
				}while(G1[m]==0||lookup[m]);
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
			}while(lookup[n]); //�קK�P�@bit���Ʃ�Jpacket 
			ret ^= source[n];
			lookup[n] = 1;
			tmp.push_back(n);
			vector<int>::iterator pos = find(l_used.begin(),l_used.end(),n);
			l_used.erase(pos);
			
			/*int n;
			do{
				n = real_rand(p_size);	
			}while(lookup[n]);
			ret ^= source[n];
			lookup[n] = 1;
			tmp.push_back(n);*/
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
	for(int i=1;i<MAX_ARRAY;i++)  //�]���ѧ����ܧڰO���Op_size���O0�A��󴣦��ѧ������p�n�[�W��p_size�H�K�ϦӸѪ���߸ѧ����� 
		if(send_times_array[i]<final_tests)
			solved_array[i]+=(final_tests-send_times_array[i])*p_size;
			
	printf("errors = %d ",errors);
	setcolor(color);
	int i=2,k = p_size;
	double solved_dec,ber_log,solved_dec2,ber_log2;
	while(send_times_array[i+1]!=0&&i<4999)   //�p�G�o�{�O�̫�@�B�A�N��w�g�ǧ��A����solver_dec�|�Op_size�A��log10(0)�X�{�]�����i�h 
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
	for(int i=0;i<MAX_ARRAY;i++)  //�]���ѧ����ܧڰO���Op_size���O0�A��󴣦��ѧ������p�n�[�W��p_size�H�K�ϦӸѪ���߸ѧ����� 
	{
		if(send_times_array[i]<final_tests)
			solved_array[i]+=(final_tests-send_times_array[i])*p_size;
		errorrate[i] = (p_size-((double)solved_array[i]/final_tests))/p_size;
	}
	
	//output to file
	FILE *fp;
	strcat(fname,"_era0.1.txt");
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
	vector<int>back_data(p_size,0);  //�w�g�Q�Ѷ}��bit�Alist�|�ܦ�1 
	vector<int>packet;
	vector<int>flag(p_size,0);
	vector<int>temp, temp2; //for myencoder
	vector<double>probability(p_size,0);  //for patrick_encoder
	vector<bool>G1(p_size,0);//for patrick_encoder
	int times=0;
	int times_for_p=0;  //times for patrick's encoder
	int send_bits = 0;  //for myencoder
	vector< vector<int> > chains; //for myencoder
	vector<int> cut_bit, high_deg_stack;//for myencoder
	vector< vector<int> > record;
	vector<int> all_chain(p_size+1,0), chains_sizes(1500,1),chain_deg(1000,0),chains_deg4s(1000,0);   //for myencoder
	vector<int> l_used, deg1chain,deg4chain;
	vector<bool> chains_complete;   //for myencoder,�ΨӰO���o��chain�O�_�w�g�s��U�@�� 
	int ind1 = 0, chains_ind=0;
	for(int i=0;i<p_size;i++)
		data.push_back(1);
	
	deg1chain.push_back(500); //for myencoder
	/*for(int i=0;i<59;i++)
	{
		chain.push_back(temp); 
	}*/
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
				packet.push_back(my_encoder(high_deg_stack,cut_bit,data,record,chains,chains_deg4s,chains_complete,chains_sizes,chains_ind,chain_deg,l_used,all_chain,deg1chain,deg4chain,ind1,times,false));
			else if(strcmp(type,"patrick_encoder_forced1000_least")==0)
				packet.push_back(patrick_encoder_forced1000_least(data,record,probability,G1,l_used,times_for_p,false));		
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
			else if(strcmp(type,"patrick_encoder_forced1000_least")==0)
				patrick_encoder_forced1000_least(data,record,probability,G1,l_used,times_for_p,true);
			else if(strcmp(type,"my_encoder")==0)
				my_encoder(high_deg_stack,cut_bit,data,record,chains,chains_deg4s,chains_complete,chains_sizes,chains_ind,chain_deg,l_used,all_chain,deg1chain,deg4chain,ind1,times,true);
		}
	}while(1);
	
/*	printf("times = %d\n",times);
	_getch();
*/	
	vec_clr(l_used);
	vec_clr(temp);
}

int main(int argc,char** argv)
{
//	initwindow(p_size*2,p_size);

	srand(time(NULL));

	solved_array = new int[MAX_ARRAY];     //�����b1000��test���C���Hsend packet�ƳQ��bit�ƶq���M 
	send_times_array = new int[MAX_ARRAY]; //�����b1000��test��send packet=�Y�Ƶo�ʹX�� 

	char a[50];
	int now_times;
	
	int line_log = log10(500/(p_size));
	
	
//	setcolor(WHITE);
//	line(500,0,500,1000);

//	line(0,log10(500.0/(p_size))*(1000.0/(log10(accurate))),p_size*2,log10(500.0/(p_size))*(1000.0/(log10(accurate))));
//	line(0,log10(100.0/(p_size))*(1000.0/(log10(accurate))),p_size*2,log10(100.0/(p_size))*(1000.0/(log10(accurate))));
//	line(0,log10(10.0/(p_size))*(1000.0/(log10(accurate))),p_size*2,log10(10.0/(p_size))*(1000.0/(log10(accurate))));
	
/*strcpy(a, "encoder");	
	for(int i=0;i<MAX_ARRAY;i++)
	{ 
		solved_array[i] = 0;
		send_times_array[i] = 0;
	}	
	for(now_times=0;now_times<tests||errors<(int)(1/erase_prob);now_times++)
	{ 
		Demo(a,all_thresh[2]);
		if(now_times%(tests/100)==0)
				printf("%d percent fine\n",now_times/(tests/100));
	} 
	now_times>tests? final_tests=now_times : final_tests=tests;	
	errors = 0;
	printf("fine1");
	//draw(YELLOW);
	Save(a);
*/	
/*strcpy(a, "edge_encoder");	
	for(int i=0;i<MAX_ARRAY;i++)
	{ 
		solved_array[i] = 0;
		send_times_array[i] = 0;
	}	
	for(now_times=0;now_times<tests||errors<(int)(1/erase_prob);now_times++)
	{ 
		Demo(a,all_thresh[2]);
		if(now_times%(tests/100)==0)
				printf("%d percent fine\n",now_times/(tests/100));
	} 
	now_times>tests? final_tests=now_times : final_tests=tests;	
	errors = 0;
	printf("fine2");
	//draw(YELLOW);
	Save(a);
*/ 
/*		
	strcpy(a, "edge_encoder");	
	for(int i=0;i<MAX_ARRAY;i++)
	{ 
		solved_array[i] = 0;
		send_times_array[i] = 0;
	}	
	int rate,nrate=0;
	for(now_times=0;now_times<tests /*errors<(int)(1/erase_prob)*//*;now_times++)
	{
		Demo(a,all_thresh[1]);
			
		if(now_times%(tests/100)==0)
			printf("%d percent fine\n",now_times/(tests/100));
	}
	now_times>tests? final_tests=now_times : final_tests=tests;	
	errors = 0;
	//draw(CYAN);
	Save(a); 
*/	

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
/*	strcpy(a, "patrick_encoder_forced1000_least");	
	for(int i=0;i<MAX_ARRAY;i++)
	{ 
		solved_array[i] = 0;
		send_times_array[i] = 0;
	}	
	for(now_times=0;now_times<tests;now_times++)
	{ 
		Demo(a,all_thresh[2]);
		if(now_times%(tests/100)==0)
				printf("%d percent fine\n",now_times/(tests/100));
	} 
	now_times>tests? final_tests=now_times : final_tests=tests;	
	errors = 0;
	printf("fine");
	//draw(YELLOW);
	Save(a);
*/	
	
	erase_prob=0.1;
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
	
	
/*	
	strcpy(a, "patrick_encoder_forced100_rand");	
	for(int i=0;i<MAX_ARRAY;i++)
	{ 
		solved_array[i] = 0;
		send_times_array[i] = 0;
	}	
	for(now_times=0;now_times<tests/*||errors<(int)(1/erase_prob)*//*;now_times++)
	{
		Demo(a,all_thresh[3]);
		if(now_times%(tests/100)==0)
			printf("%d percent fine\n",now_times/(tests/100));
	}
	now_times>tests? final_tests=now_times : final_tests=tests;	
	errors = 0;
	//printf("%d \n",final_tests);
	//draw(GREEN);
	Save(a);
	printf("fine");	
*/ 
	_getch();
//	closegraph();
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
