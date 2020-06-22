//#include<algorithm>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <thread>
#include <mutex>
#include <fcntl.h>

#define DFS_SIZE 3
#define BUF_SIZE 536870912
#define PTHREAD_NUM 4
#define NODE_SIZE 50000
#define ANS3 7500000
#define ANS4 10000000
#define ANS5 25000000
#define ANS6 60000000
#define ANS7 100000000
//#include<unistd.h>


//using namespace std;

int _G[280000][50]; //G[u][v]
int _in_G[280000][50]; //G[v][u]
int _edges[560000];
int _newid2id[280000];
int _edges_size;
int _node_size;

int _id2newid[160000000];
//bool _out_edges[280000] = {};
struct pthread_info
{
	int _step2d[280000];
	bool _visit[280000];
	char ans3[ANS3];
	char ans4[ANS4];
	char ans5[ANS5];
	char ans6[ANS6];
	char ans7[ANS7];

}pthread_info[PTHREAD_NUM];

char buf[BUF_SIZE];
struct ans
{
	int begin;
	int end;
	char* ansp[5];
	int ans_len[5];
	int ans_num[5];
}_ans_arr[PTHREAD_NUM];

void readfile_new(char* str) {
	FILE* file = fopen(str, "r");
	int size = 0;
	int flag = 0;
	int point = 0;
	int ret;
	int u, v;
	do
	{
		ret = fread(buf, sizeof(char), BUF_SIZE, file);
		if (ret < BUF_SIZE) flag = 0;
		else flag = 1;
		while (point < ret) {
			u = 0;
			while (buf[point] != ',') {
				//u *= 10;
				u <<= 4;
				u += buf[point] - '/';
				++point;
			}
			++point;
			v = 0;
			while (buf[point] != ',') {
				//u *= 10;
				v <<= 4;
				v += buf[point] - '/';
				++point;
			}
			while (buf[point] != '\n')
			{
				++point;
			}
			++point;
			_edges[size++] = u;
			_edges[size++] = v;
		}
	} while (flag);
	_edges_size = size / 2;
}


void changid_new() {
	int maxid = 0;
	int size = _edges_size * 2;
	int x;
	int newid = 0;
	for (int i = 0; i < size; ++i) {
		x = _edges[i];
		if (x > maxid) maxid = x;
		_id2newid[x] = 1;
	}
	for (int i = 0; i <= maxid; ++i) {
		if (_id2newid[i]) {
			_id2newid[i] = newid;
			_newid2id[newid] = i;
			++newid;
		}
	}
	_node_size = newid;
}


int cmpfunc(const void* a, const void* b) {
	return *((int*)a) - *((int*)b);
}
void mysort(int* arr, int num) {
	int x;
	for (int i = 0; i < num; ++i) {
		for (int j = num - 1; j > i; --j) {
			if (arr[j] < arr[j - 1]) {
				x = arr[j];
				arr[j] = arr[j - 1];
				arr[j - 1] = x;
			}
		}
	}
}
void build_G_new() {
	int i = 0, size = _edges_size * 2;
	int sub_size;
	int u, v;
	for (; i < size; i += 2) {
		u = _id2newid[_edges[i]];
		v = _id2newid[_edges[i + 1]];
		//sub_size = ;
		_G[u][++_G[u][0]] = v;
		//sub_size = ++_in_G[v][0];
		_in_G[v][++_in_G[v][0]] = u;
	}
	size /= 2;
	int node_size = _node_size;
	int edges_num = 0;
	int pthread_id = 0;
	int node_wide = size / PTHREAD_NUM;
	for (i = 0; i < node_size; ++i) {
		
		edges_num += _G[i][0];
		if (edges_num > node_wide) {
			_ans_arr[pthread_id++].end = i;
			edges_num = 0;
		}
		//qsort(_G[i] + 1, _G[i][0], sizeof(int), cmpfunc);
		mysort(_G[i] + 1, _G[i][0]);
		
	}
}
void divide() {
	int node_size;
	if (_node_size < NODE_SIZE) {
		node_size = _node_size;
	}
	else {
		node_size = NODE_SIZE;
	}
	int begin_end = node_size / 3;
	
	_ans_arr[PTHREAD_NUM - 1].end = node_size;
	_ans_arr[PTHREAD_NUM - 1].begin = begin_end;
	
	int end = begin_end / 3;
	for (int i = PTHREAD_NUM - 2; i > 0; --i) {
		_ans_arr[i].end = _ans_arr[i + 1].begin;
		_ans_arr[i].begin = end;
		end /= 3;
	}
	//_ans_arr[PTHREAD_NUM - 2].end = begin_end;
	_ans_arr[0].begin = 0;
	_ans_arr[0].end = _ans_arr[1].begin;
}
int int2char(int len,char* buf,int* road) {
	int x;
	int ret = 0, wide;
	char str_num[10];
	for (int i = 0; i < len; ++i) {
		x = _newid2id[road[i]];
		wide = 0;
		while (x)
		{
			str_num[wide++] = (x & 15) + '/';
			x >>= 4;
		}

		while (wide)
		{
			buf[ret++] = str_num[--wide];
		}
		buf[ret++] = ',';
	}
	buf[ret - 1] = '\n';
	return ret;
}
void dfs_p(int* road, int deep, int s, int v, int pthread_id) {
	road[deep - 1] = v;
	int* point = _G[v];
	int size = _G[v][0];
	int* anspoint;
	bool* _visit = pthread_info[pthread_id]._visit;
	int* _step2d = pthread_info[pthread_id]._step2d;
	struct ans* _ans = _ans_arr + pthread_id;

	_visit[v] = 1;
	int u;
	for (int i = 1; i <= size; ++i) {
		u = _G[v][i];
		if (u == s && deep > 2) {
			//output
			_ans->ans_len[deep - 3] += int2char(deep, _ans->ansp[deep - 3] + _ans->ans_len[deep - 3], road);
			//anspoint = _ans->ansp[deep - 3];
			++_ans->ans_num[deep - 3];
			//memcpy(anspoint + deep * _ans->ans_len[deep - 3], road, deep * sizeof(int));
			//++_ans->ans_len[deep - 3];
		}
		if (u > s && !_visit[u]) {
			if (deep < 7 - DFS_SIZE) {
				dfs_p(road, deep + 1, s, u, pthread_id);
			}
			else if (deep < 6 && _step2d[u]) {
				if (deep + _step2d[u] < 8) {
					dfs_p(road, deep + 1, s, u, pthread_id);
				}
			}
			else if (deep == 6 && _step2d[u] == 1) {
				road[6] = u;
				_ans->ans_len[4] += int2char(7, _ans->ansp[4] + _ans->ans_len[4], road);
				//memcpy(_ans->ansp[4] + 7 * _ans->ans_len[4], road, 7 * sizeof(int));
				++_ans->ans_num[4];
			}

		}
	}
	_visit[v] = 0;
}
int bfs_re_p(int* ids, int start, int end, int s, int deep, int pthread_id) {
	int* _step2d = pthread_info[pthread_id]._step2d;
	int ret = end;
	int u, v;
	int* point;
	for (; start < end; ++start) {
		u = ids[start];
		point = _in_G[u];
		for (int i = point[0]; i > 0; --i) {
			v = point[i];
			if (v > s && !pthread_info[pthread_id]._step2d[v]) {
				_step2d[v] = deep;
				ids[ret++] = v;
			}
		}
	}
	return ret;
}
void solve_problem_p(int pthread_id) {
	int node_size = _node_size;
	int road[10];
	int ids[90000];
	int ids_index = 0, start, ret;
	int* _step2d = pthread_info[pthread_id]._step2d;
	int i = _ans_arr[pthread_id].begin;
	int end = _ans_arr[pthread_id].end;
	for (; i < end; ++i) {
		//road[0] = i;
		ids_index = 1;
		ids[0] = i;
		start = 0;
		for (int j = 1; j <= DFS_SIZE; ++j) {
			ret = bfs_re_p(ids, start, ids_index, i, j, pthread_id);
			start = ids_index;
			ids_index = ret;
		}

		dfs_p(road, 1, i, i, pthread_id);

		for (int j = ids_index - 1; j > 0; --j) {
			_step2d[ids[j]] = 0;
		}

	}
}

int itoa_(char* buf, int x) {
	int ret = 0;
	int num_re[10];
	int i = 0;
	if (!x) {
		buf[0] = 0;
		buf[1] = '\n';
		return 2;
	}
	while (x)
	{
		num_re[i++] = x % 10 + '0';
		x /= 10;
	}
	buf[i] = '\n';
	ret = i + 1;
	int j = 0;
	while (i)
	{
		buf[--i] = num_re[j++];
	}
	return ret;
}
void output_new(char* str) {
	//struct ans* _ans = _ans_arr + 0;
	int ans_num = 0;
	int buf_p = BUF_SIZE - 1;
	int ret;
	FILE* file = fopen(str, "w");
	for (int i = 0; i < PTHREAD_NUM; ++i) {
		for (int j = 0; j < 5; ++j) {
			ans_num += _ans_arr[i].ans_num[j];
		}
	}
	char num_str[10];
	int len = itoa_(num_str, ans_num);
	fwrite(num_str, sizeof(char), len, file);
	//fprintf(file, "%d\n", ans_num);
	for (int i = 0; i < 5; ++i) {
		//ret = i2s_new(buf , buf_size, 3 + i);
		//buf_p = i2s_new(buf, buf_p, 3 + i);
		//buf_size -= ret;
		for (int j = 0; j < PTHREAD_NUM; ++j) {
			fwrite(_ans_arr[j].ansp[i], sizeof(char), _ans_arr[j].ans_len[i], file);
		}
	}
	//itoa_(buf, buf_p, ans_num);
	//fwrite(buf + buf_p + 1, sizeof(char), BUF_SIZE - buf_p - 1, file);

	//fprintf(file, "\n");
}
int main() {
	char str_test_data[] = "/data/test_data.txt";
	char str_result[] = "/projects/student/result.txt";
	for (int i = 0; i < PTHREAD_NUM; ++i) {
		_ans_arr[i].ansp[0] = pthread_info[i].ans3;
		_ans_arr[i].ansp[1] = pthread_info[i].ans4;
		_ans_arr[i].ansp[2] = pthread_info[i].ans5;
		_ans_arr[i].ansp[3] = pthread_info[i].ans6;
		_ans_arr[i].ansp[4] = pthread_info[i].ans7;
		for (int j = 0; j < 5; ++j) {
			_ans_arr[i].ans_len[j] = 0;
			_ans_arr[i].ans_num[j] = 0;
		}
	}
	

	
	readfile_new(str_test_data);
	
	changid_new();
	build_G_new();
	divide();
	std::thread th[PTHREAD_NUM];
	for (int x = 0; x < PTHREAD_NUM; x++) {
		th[x] = std::thread(solve_problem_p, x);
	}
	for (int x = 0; x < PTHREAD_NUM; x++) {
		th[x].join();
	}


	output_new(str_result);
	//cout << _edges_size << endl;
	return 0;
}
