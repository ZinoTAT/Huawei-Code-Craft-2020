#include<iostream>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<vector>
#include<unordered_map>
#include<set>
#include<map>
#include<unordered_set>
#include<algorithm>
#include<queue>
#include <thread>
#include<mutex>
#include <chrono>
using namespace std;
//#define BUF_SIZE 536870912
#define BUF_SIZE 100000000
//#define BUF_SIZE 2147483648
#define NODE_SIZE 4000000
//#define NODE_SIZE_MAR 4000768‬
#define MARGIN 768
#define INPUT_SIZE 2000000
//#define DFS_SIZE 4
#define id_t unsigned int
#define ull unsigned long long
#define ANS_RESERVE 120000
#define ANS_RESERVE_7 140000000
#define ANS_SIZE 20000000
#define ANS_BLOCK 200
#define MASK 4294967295
#define PTHREAD_NUM 4
//#define DEBUG
#define FENZI 1
#define FENMU 4 //FENZI / FENMU
#define TASK_NUM 256
/*
#define PTHREAD_NUM 4
//#define DEBUG
#define FENZI 1
#define FENMU 4 //FENZI / FENMU
#define TASK_NUM 32
*/


#define HASH_BIT_NUM 21
#define HASH_BIT_REMAIN 2

#define CMP(a, b) (a > b)
#define PANDUAN(X,Y) (X <= 5 * Y && Y <= 3 * X)
/*inline bool panduan(long long X, long long Y) {
	return (X <= 5 * Y && Y <= 3 * X);
}*/

//大数据部分


struct node
{
	ull id;
	ull money;
};


//hash部分

id_t hashmap[(1 << (HASH_BIT_NUM + HASH_BIT_REMAIN)) + NODE_SIZE][2];
bool hash_flag[(1 << (HASH_BIT_NUM + HASH_BIT_REMAIN)) + NODE_SIZE + MARGIN];


//预处理部分
ull __input[INPUT_SIZE * 3];


int __G_size[NODE_SIZE] = {};
int __in_G_size[NODE_SIZE] = {};
//用于剪枝
int __G_size_1[NODE_SIZE] = {};
int __in_G_size_1[NODE_SIZE] = {};


//公用部分
id_t __newid2id[NODE_SIZE] = {};
int __newid2edges[NODE_SIZE] = {};
//unsigned long long __G[INPUT_SIZE] = {};
node __G_new[INPUT_SIZE] = {};
//int __in_G[INPUT_SIZE * 2] = {};
node __in_G_new[INPUT_SIZE * 2] = {};

int __G_point[NODE_SIZE + 32] = {};
int __in_G_point[NODE_SIZE + 32] = {};

char __newid2char[NODE_SIZE][16];

bool __ignored[NODE_SIZE + MARGIN] = {};


//读取部分
char __buf[BUF_SIZE];
//个人过程修改数据部分
//char __newid2step[NODE_SIZE] = {};




//小数据


//预处理部分
int __input_size = 0;
int __node_size;
//公用部分


//互斥部分
int __task_id = 0;
std::mutex __mutex;


struct pthread_info
{
	bool __visited[NODE_SIZE + MARGIN] = {};


	ull __list_of_last2step[INPUT_SIZE];
	ull __list_of_last3step[INPUT_SIZE];

	bool __in_last2step[NODE_SIZE + MARGIN] = {};
	bool __in_last3step[NODE_SIZE + MARGIN] = {};


	ull __last_2step_node[INPUT_SIZE * 4];
	ull __last_2step_id2node[NODE_SIZE];

}__p_info[PTHREAD_NUM];



//个人答案记录部分
class new_answer {
public:
	//记录当前ans所读应的起始和截至id
	int begin_id;
	int end_id;
	int total_num;
	vector<vector<pair<int, char*>>>ans_block_point;//<当前存储量，指针>
	bool need_malloc[8];//记录存入前是否要开空间
	int ans_capacity[8];//记录每个长度答案的每一块内存的最大容量
	int ans_left_bargin[8];//记录每个长度答案在超过某个量时就不再往里存，重新开个新的，以减少内存越界判断次数并简化代码
	//int ans_len_total[5];//记录每个长度答案的总长度
	int ans_begin[6];//记录各长度答案在最终输出中的起始位置

	void answer_init(int task_id) {
		ans_block_point = vector<vector<pair<int, char*>>>(6);
		total_num = 0;
		int ans_block = ANS_BLOCK;
		for (int i = 0; i < 6; ++i) {
			//ans_block_size[i] = 0;
			ans_capacity[i] = ans_block * (i + 3) * 11;
			ans_left_bargin[i] = (ans_block - 1) * (i + 3) * 11;
			ans_block *= 7;
			//ans_len_total[i] = 0;
			need_malloc[i] = 1;
		}
	}
	void clear() {
		for (auto& vec : ans_block_point) {
			for (auto& x : vec) {
				free(x.second);
			}
			vec.clear();
		}
		total_num = 0;
		//ans_block_point.clear();
		for (int i = 0; i < 6; ++i) {
			need_malloc[i] = 1;
		}
	}
	void set_range(int begin, int end) {
		begin_id = begin;
		end_id = end;
	}
	void get_range(int& begin, int& end) {
		begin = begin_id;
		end = end_id;
	}
	void add_ans(int* road, int len) {
		total_num += 1;
		int index = len - 3;
		if (need_malloc[index]) {
			char* new_p = (char*)malloc(sizeof(char) * ans_capacity[index]);
			ans_block_point[index].push_back(make_pair(0, new_p));
			need_malloc[index] = 0;
		}
		//填入字符串
		char* buf = ans_block_point[index].back().second;
		int  point = ans_block_point[index].back().first;
		for (int i = 0; i < len; ++i) {
			memcpy(buf + point, __newid2char[road[i]] + 1, __newid2char[road[i]][0]);
			point += __newid2char[road[i]][0];
			buf[point++] = ',';
		}
		buf[point - 1] = '\n';
		//判断是否进入margin
		ans_block_point[index].back().first = point;
		if (point > ans_left_bargin[index]) {
			need_malloc[index] = 1;
		}
	}
} __ans[TASK_NUM];

class last2step {
public:
	ull* last_2step_node;
	int size;
	ull* last_2step_id2node;
	int index;
	void init(int pthread) {
		last_2step_node = __p_info[pthread].__last_2step_node;
		last_2step_id2node = __p_info[pthread].__last_2step_id2node;
		last_2step_node[0] = NODE_SIZE + 10;//所有边的最后一个节点，哨兵
		size = 1;
	}
	void clear() {
		size = 1;
	}
	void add_init(int id) {
		last_2step_id2node[id] = 0;
	}
	void add(ull id, ull u, ull m_u, ull m_v) {
		int new_node_id = (4 * size++);
		ull* pre = last_2step_id2node + id;
		ull* next = last_2step_node + *pre;
		last_2step_node[new_node_id] = u;
		last_2step_node[new_node_id + 1] = m_u;
		last_2step_node[new_node_id + 2] = m_v;
		while (*next < u)
		{
			pre = next + 3;
			next = last_2step_node + *(next + 3);
		}
		*(pre) = new_node_id;
		last_2step_node[new_node_id + 3] = next - last_2step_node;
	}
	void visit(int id) {
		index = last_2step_id2node[id];
	}
	bool get(ull& u, ull& m_u, ull& m_v) {
		bool flag = index > 0;
		u = last_2step_node[index];
		m_u = last_2step_node[index + 1];
		m_v = last_2step_node[index + 2];
		index = last_2step_node[index + 3];
		return flag;
	}
};

//vector<vector<int>> _last2step;

//int _edges_size;

/*
void readfile(char* str);
void changeid();
void mysort(int* begin, int* end);
void build_G();

void build_last2step(int* list_of_last2step, int& list_of_last2step_end, int* list_of_last3step, int& list_of_last3step_end, int i, int pthread);
void dfs_last2step(int* road, long long m_pre, long long m_pre_3, long long m_first, long long m_first_5, int deep, int s, int v, int pthread);
void solve_problem_last2step(int pthread);

void build_last2step_List(last2step& l2s, int* list_of_last2step, int& list_of_last2step_end, int* list_of_last3step, int& list_of_last3step_end, int i, int pthread);
void dfs_last2step_List(last2step& l2s, int* road, long long m_pre, long long m_pre_3, long long m_first, long long m_first_5, int deep, int s, int v, int pthread);
void solve_problem_last2step_List(int pthread);



int i2s(char* buf, answer& sub_ans, int i, FILE* file);
void output(char* str);
*/

void readfile(char* str) {
	FILE* file = fopen(str, "r");
	int size = 0;
	int flag = 0;
	int point = 0, begin, end;
	int ret;
	ull ids[3], u, v, m, x;
	int input_size = 0;
	ret = fread(__buf, sizeof(char), BUF_SIZE, file);
	if (ret < BUF_SIZE) flag = 0;
	else flag = 1;
	while (point < ret) {
		for (int i = 0; i < 2; ++i) {
			x = 0;
			while (point < ret && __buf[point] < '0' || __buf[point] > '9')
			{
				++point;
			}
			if (point >= ret) {
				//_edges_size = size;
				__input_size = input_size;
				return;
			}
			begin = end = point;
			while (__buf[end] >= '0' && __buf[end] <= '9')
			{
				x *= 10;
				x += __buf[end] - '0';
				++end;
			}
			point = end;
			ids[i] = x;
			//_input.push_back(x);

		}
		x = 0;
		
		while (point < ret && __buf[point] < '0' || __buf[point] > '9')
		{
			++point;
		}
		begin = end = point;
		while (__buf[end] >= '0' && __buf[end] <= '9')
		{
			x *= 10;
			x += __buf[end] - '0';
			++end;
		}
		if (__buf[end] == '.') {
			++end;
			x *= 10;
			x += __buf[end] - '0';
			++end;
			x *= 10;
			if(__buf[end] >= '0' && __buf[end] <= '9')
			{
				
				x += __buf[end] - '0';
				++end;
			}

		}
		else {
			x *= 100;
		}
		point = end;
		ids[2] = x;

		if (ids[1] != ids[0]) {

			__input[input_size++] = ids[0];
			__input[input_size++] = ids[1];
			__input[input_size++] = ids[2];
		}
	}
	__input_size = input_size;
	fclose(file);
}






class myHash {
public:
	id_t mask;
	myHash() {//将mask低HASH_BIT_NUM位置1
		mask = 0;
		int i = 0;
		for (; i < HASH_BIT_NUM; ++i) {
			mask <<= 1;
			mask += 1;
		}

	}
	bool count(id_t id) {
		id_t index = id & mask;
		//id_t index = id >> (32 - HASH_BIT_NUM);
		while (hash_flag[index] && hashmap[index][0] != id)
		{
			++index;
		}
		return hash_flag[index];
	}
	void add(id_t id) {
		id_t index = id & mask;
		//id_t index = id >> (32 - HASH_BIT_NUM);
		while (hash_flag[index])
		{
			++index;
		}
		hash_flag[index] = 1;
		hashmap[index][0] = id;
	}
	void set(id_t id, id_t val) {
		id_t index = id & mask;
		//id_t index = id >> (32 - HASH_BIT_NUM);
		while (hashmap[index][0] != id)
		{
			++index;
		}
		hashmap[index][1] = val;
	}
	int get(id_t id) {
		id_t index = id & mask;
		//id_t index = id >> (32 - HASH_BIT_NUM);
		while (hashmap[index][0] != id)
		{
			++index;
		}
		return hashmap[index][1];
	}
};

void changeid_new() {//可以合并至readfile
	myHash id2newid;

	int maxid = 0;
	int size = __input_size;
	int x, node_size = 0;
	int newid = 0, u, v;
	//生成新旧id映射
	for (int i = 0; i < size; i += 3) {
		u = __input[i];
		v = __input[i + 1];

		if (!id2newid.count(u)) {
			id2newid.add(u);
			__newid2id[node_size++] = u;
		}
		if (!id2newid.count(v)) {
			id2newid.add(v);
			__newid2id[node_size++] = v;
		}
		/*
		if (!_id2newid.count(u)) {
			_id2newid[u] = -1;
			__newid2id[node_size++] = u;
		}
		if (!_id2newid.count(v)) {
			_id2newid[u] = -1;
			__newid2id[node_size++] = v;
		}
		*/
	}
	sort(__newid2id, __newid2id + node_size);
	for (int i = 0; i < node_size; ++i) {
		id2newid.set(__newid2id[i], i);
	}
	//创建新id到旧id字符串映射
	int j = 0, k;
	char str_re[10];
	for (int i = 0; i < node_size; ++i) {
		id_t x = __newid2id[i];


		if (x == 0) {
			__newid2char[i][0] = 1;
			__newid2char[i][1] = '0';
		}
		else {
			j = 0;
			while (x)
			{
				str_re[j++] = x % 10 + '0';
				x /= 10;
			}
			__newid2char[i][0] = j;
			k = 1;
			while (j)
			{
				__newid2char[i][k++] = str_re[--j];
			}
		}

	}
	//替换input为新id并统计出入边
	for (int i = 0; i < size; i += 3) {
		__input[i] = u = id2newid.get(__input[i]);
		__input[i + 1] = v = id2newid.get(__input[i + 1]);
		++__G_size[u];
		++__in_G_size[v];
	}
	__node_size = node_size;
	//_last2step = vector<vector<int>>(node_size);
}


void changeid() {//可以合并至readfile
	unordered_set<id_t>id_hashset;
	unordered_map<id_t, int>_id2newid;
	int maxid = 0;
	//int size = _input.size();
	int size = __input_size;
	int x, node_size = 0;
	int newid = 0, u, v;
	//生成新旧id映射
	for (int i = 0; i < size; i += 3) {
		u = __input[i];
		v = __input[i + 1];
		//id_set.insert(u);
		//id_set.insert(v);

		if (!id_hashset.count(u)) {
			id_hashset.insert(u);
			__newid2id[node_size++] = u;
		}
		if (!id_hashset.count(v)) {
			id_hashset.insert(v);
			__newid2id[node_size++] = v;
		}
		/*
		if (!_id2newid.count(u)) {
			_id2newid[u] = -1;
			__newid2id[node_size++] = u;
		}
		if (!_id2newid.count(v)) {
			_id2newid[u] = -1;
			__newid2id[node_size++] = v;
		}
		*/
	}
	sort(__newid2id, __newid2id + node_size);
	for (int i = 0; i < node_size; ++i) {
		_id2newid[__newid2id[i]] = i;
	}
	//创建新id到旧id字符串映射
	int j = 0, k;
	char str_re[10];
	for (int i = 0; i < node_size; ++i) {
		id_t x = __newid2id[i];


		if (x == 0) {
			__newid2char[i][0] = 1;
			__newid2char[i][1] = '0';
		}
		else {
			j = 0;
			while (x)
			{
				str_re[j++] = x % 10 + '0';
				x /= 10;
			}
			__newid2char[i][0] = j;
			k = 1;
			while (j)
			{
				__newid2char[i][k++] = str_re[--j];
			}
		}

	}
	//替换input为新id并统计出入边
	for (int i = 0; i < size; i += 3) {
		__input[i] = u = _id2newid[__input[i]];
		__input[i + 1] = v = _id2newid[__input[i + 1]];
		++__G_size[u];
		++__in_G_size[v];
	}
	__node_size = node_size;
	//_last2step = vector<vector<int>>(node_size);
}
void mysort(int* begin, int* end) {
	int m, u;
	for (; begin != end; begin += 2) {
		for (int* point = end - 2; point != begin; point -= 2) {
			if (CMP(*(point - 2), *point)) {
				u = *point;
				*point = *(point - 2);
				*(point - 2) = u;
				m = *(point + 1);
				*(point + 1) = *(point - 1);
				*(point - 1) = m;
			}
		}
	}
}
/*
void build_G_prun() {
	//int i = 0;
	//int size = _input.size();
	int size = __input_size;
	int node_size = __node_size;
	int sub_size;
	queue<int>q;
	ull u, v, m;
	//构建图的指针
	for (int i = 0; i < node_size; ++i) {
		__G_point[i + 1] = __G_point[i] + __G_size[i];
		__in_G_point[i + 1] = __in_G_point[i] + __in_G_size[i] * 2;
		if (__G_size[i] == 0 || __in_G_size[i] == 0) {
			q.push(i);
		}
	}
	memcpy(__G_size_1, __G_size, node_size * sizeof(int));
	memcpy(__in_G_size_1, __in_G_size, node_size * sizeof(int));
	//构件图
	for (int i = 0; i < size; i += 3) {
		u = __input[i];
		v = __input[i + 1];
		m = __input[i + 2];
		--__G_size[u];
		__G[__G_point[u] + __G_size[u]] = (v << 32) + m;

		--__in_G_size[v];
		__in_G[__in_G_point[v] + __in_G_size[v] * 2] = u;
		__in_G[__in_G_point[v] + __in_G_size[v] * 2 + 1] = m;
	}
	//进行剪枝

	while (!q.empty())
	{
		v = q.front();
		q.pop();
		if (__ignored[v]) continue;
		__ignored[v] = 1;
		int i = __G_point[v], end = __G_point[v + 1];
		for (; i < end; i += 1) {
			u = __G[i] >> 32;
			if (!__ignored[u]) {
				//--__G_size_1[u];
				--__in_G_size_1[u];
				if (__in_G_size_1[u] == 0) {
					q.push(u);
				}

			}
		}
		i = __in_G_point[v];
		end = __in_G_point[v + 1];
		for (; i < end; i += 2) {
			u = __in_G[i];
			if (!__ignored[u]) {
				--__G_size_1[u];
				//--__in_G_size_1[u];
				if (__G_size_1[u] == 0) {
					q.push(u);
				}
			}
		}
	}
	//int ret = test(node_size);cout << "ret: " << ret << "\tedges_size: " << __G_point[node_size] << endl;

	int point = 0, new_end, begin, end;
	for (int i = 0; i < node_size; ++i) {
		begin = __G_point[i];
		__G_point[i] = point;
		end = __G_point[i + 1];
		for (; begin != end; begin += 1) {
			if (!__ignored[__G[begin] >> 32]) {
				__G[point++] = __G[begin];
				//__G[point++] = __G[begin + 1];
			}
		}
	}
	__G_point[node_size] = point;
	point = 0;
	for (int i = 0; i < node_size; ++i) {
		begin = __in_G_point[i];
		__in_G_point[i] = point;
		end = __in_G_point[i + 1];
		for (; begin != end; begin += 2) {
			if (!__ignored[__in_G[begin]]) {
				__in_G[point++] = __in_G[begin];
				__in_G[point++] = __in_G[begin + 1];
			}
		}
	}
	__in_G_point[node_size] = point;
	//完成剪枝

	//对出边图进行排序
	for (int i = 0; i < node_size; ++i) {
		//mysort(__G + __G_point[i], __G + __G_point[i + 1]);
		sort(__G + __G_point[i], __G + __G_point[i + 1]);
	}
}
*/
bool cmp(node a, node b) {
	return a.id < b.id;
}
void build_G() {
	//int i = 0;
	//int size = _input.size();
	int size = __input_size;
	int node_size = __node_size;
	int sub_size;
	ull u, v, m;
	//构建图的指针
	for (int i = 0; i < node_size; ++i) {
		__G_point[i + 1] = __G_point[i] + __G_size[i];
		__in_G_point[i + 1] = __in_G_point[i] + __in_G_size[i];
	}
	//构件图
	for (int i = 0; i < size; i += 3) {
		u = __input[i];
		v = __input[i + 1];
		m = __input[i + 2];
		--__G_size[u];
		__G_new[__G_point[u] + __G_size[u]].id = v;
		__G_new[__G_point[u] + __G_size[u]].money = m;
		//__G[__G_point[u] + __G_size[u]] = (v << 32) + m;

		--__in_G_size[v];
		__in_G_new[__in_G_point[v] + __in_G_size[v]].id = u;
		__in_G_new[__in_G_point[v] + __in_G_size[v]].money = m;
		//__in_G[__in_G_point[v] + __in_G_size[v] + 1] = m;
	}

	//ret = test(node_size); cout << "ret: " << ret << "\tedges_size: " << __G_point[node_size] << endl;

	//对出边图进行排序
	for (int i = 0; i < node_size; ++i) {
		//mysort(__G + __G_point[i], __G + __G_point[i + 1]);
		sort(__G_new + __G_point[i], __G_new + __G_point[i + 1], cmp);
	}
}
void build_last2step_List(last2step& l2s, ull* list_of_last2step, int& list_of_last2step_end, ull* list_of_last3step, int& list_of_last3step_end, bool* _in_last2step, bool* _in_last3step, int s, int pthread) {
	int i = __in_G_point[s], end = __in_G_point[s + 1];
	int list2end = 0, list3end = 0;
	int u, v, w;
	long long m_v, m_u, m_w;
	for (; i < end; i += 1) {
		v = __in_G_new[i].id;
		m_v = __in_G_new[i].money;
		if (v > s) {
			int begin2 = __in_G_point[v], j = __in_G_point[v + 1] - 1;
			for (; j >= begin2; j -= 1) {
				u = __in_G_new[j].id;
				m_u = __in_G_new[j].money;
				if (u > s && PANDUAN(m_u, m_v) && u != v) {


					//标记反向第三层
					int k = __in_G_point[u], end3 = __in_G_point[u + 1];
					for (; k < end3; k += 1) {
						w = __in_G_new[k].id;
						m_w = __in_G_new[k].money;
						if (w > s && PANDUAN(m_w, m_u) && w != u && !_in_last3step[w]) {
							_in_last3step[w] = 1;
							list_of_last3step[list3end++] = w;
						}
					}

					//存最后两步
					if (!_in_last2step[u]) {
						_in_last2step[u] = 1;
						list_of_last2step[list2end++] = u;
						l2s.add_init(u);
					}
					l2s.add(u, v, m_u, m_v);
				}
			}
		}
	}
	list_of_last2step_end = list2end;
	list_of_last3step_end = list3end;
}
void dfs_last2step_List(last2step& l2s, int* road, ull m_pre, ull m_pre_3, ull m_first, ull m_first_5, int deep, int s, int v, int pthread, int task_id) {
	bool* _visited = __p_info[pthread].__visited;
	_visited[v] = 1;
	ull u;
	ull m, m_last = 0, m_second_last = 0;
	ull last_node = 0, second_last_node;
	int i = __G_point[v], end = __G_point[v + 1];
	for (; i < end; i += 1) {
		//u = __G[i] >> 32;
		//m = __G[i] & MASK;
		//m = __G[i];
		u = __G_new[i].id;
		//m = m & MASK;
		m = __G_new[i].money;
		if (u > s) {
			if (!_visited[u] && deep < 7) {
				if (__p_info[pthread].__in_last2step[u]) {
					//output
					second_last_node = u;
					l2s.visit(second_last_node);
					while (l2s.get(last_node, m_second_last, m_last))
					{
						if (_visited[last_node]) continue;
						if (deep == 1) {
							if (PANDUAN(m_last, m) && PANDUAN(m, m_second_last)) {
								road[deep] = second_last_node;
								road[deep + 1] = last_node;
								__ans[task_id].add_ans(road, deep + 2);
							}
						}
						else {
							if (PANDUAN(m_pre, m) && PANDUAN(m, m_second_last) && PANDUAN(m_last, m_first)) {
								road[deep] = second_last_node;
								road[deep + 1] = last_node;
								__ans[task_id].add_ans(road, deep + 2);
							}
						}
					}
				}
				if (deep < 6) {
					if (deep > 1) {
						if (m_pre <= 5 * m && m <= m_pre_3) {
							if (deep < 5) {
								road[deep] = u;
								dfs_last2step_List(l2s, road, m, m * 3, m_first, m_first_5, deep + 1, s, u, pthread, task_id);
							}
							//else if (__newid2step[u] && deep + __newid2step[u] < 8) {
							else if (__p_info[pthread].__in_last3step[u]) {
								road[deep] = u;
								dfs_last2step_List(l2s, road, m, m * 3, m_first, m_first_5, deep + 1, s, u, pthread, task_id);
							}
						}
					}
					else {
						road[1] = u;
						dfs_last2step_List(l2s, road, m, m * 3, m, m * 5, 2, s, u, pthread, task_id);
					}
				}
			}
		}
	}
	_visited[v] = 0;
}
/*
void dfs_last2step_List_5(last2step& l2s, int* road, long long m_pre, long long m_pre_3, long long m_first, long long m_first_5, int s, int v, int pthread, int task_id) {
	bool* _visited = __p_info[pthread].__visited;
	_visited[v] = 1;
	int u;
	long long m, m_last = 0, m_second_last = 0;
	int last_node = 0;
	int i = __G_point[v], end = __G_point[v + 1];
	for (; i < end; i += 1) {
		m = __G[i];
		u = m >> 32;
		m = m & MASK;
		if (u > s) {
			if (!_visited[u]) {
				if (__p_info[pthread].__in_last2step[u]) {
					//output
					l2s.visit(u);
					while (l2s.get(last_node, m_second_last, m_last))
					{
						if (_visited[last_node]) continue;
						if (PANDUAN(m_pre, m) && PANDUAN(m, m_second_last) && PANDUAN(m_last, m_first)) {
							road[5] = u;
							road[6] = last_node;
							__ans[task_id].add_ans(road, 7);
						}
					}
				}
			}
		}
	}
	_visited[v] = 0;
}
void dfs_last2step_List_4(last2step& l2s, int* road, long long m_pre, long long m_pre_3, long long m_first, long long m_first_5, int s, int v, int pthread, int task_id) {
	bool* _visited = __p_info[pthread].__visited;
	_visited[v] = 1;
	int u;
	id_t mid;
	long long m, m_last = 0, m_second_last = 0;
	int last_node = 0;
	int i = __G_point[v], end = __G_point[v + 1];
	for (; i < end; i += 1) {
		m = __G[i];
		u = m >> 32;
		m = mid = m;;
		if (u > s && u != road[1] && u != road[2]) {
			road[4] = u;
			if (__p_info[pthread].__in_last2step[u]) {
				//output
				l2s.visit(u);
				while (l2s.get(last_node, m_second_last, m_last))
				{
					if (_visited[last_node]) continue;
					if (PANDUAN(m_pre, m) && PANDUAN(m, m_second_last) && PANDUAN(m_last, m_first)) {
						road[5] = last_node;
						__ans[task_id].add_ans(road, 6);
					}
				}
			}
			if (m_pre <= 5 * m && m <= m_pre_3) {
				if (__p_info[pthread].__in_last3step[u]) {
					dfs_last2step_List_5(l2s, road, m, m * 3, m_first, m_first_5, s, u, pthread, task_id);
				}
			}

		}
	}
	_visited[v] = 0;
}
void dfs_last2step_List_3(last2step& l2s, int* road, long long m_pre, long long m_pre_3, long long m_first, long long m_first_5, int s, int v, int pthread, int task_id) {
	bool* _visited = __p_info[pthread].__visited;
	_visited[v] = 1;
	int u;
	id_t mid;
	long long m, m_last = 0, m_second_last = 0;
	int last_node = 0;
	int i = __G_point[v], end = __G_point[v + 1];
	for (; i < end; i += 1) {
		m = __G[i];
		u = m >> 32;
		m = mid = m;
		if (u > s && u != road[1]) {
			road[3] = u;
			if (__p_info[pthread].__in_last2step[u]) {
				//output
				l2s.visit(u);
				while (l2s.get(last_node, m_second_last, m_last))
				{
					if (_visited[last_node]) continue;
					if (PANDUAN(m_pre, m) && PANDUAN(m, m_second_last) && PANDUAN(m_last, m_first)) {
						road[4] = last_node;
						__ans[task_id].add_ans(road, 5);
					}
				}
			}
			if (m_pre <= 5 * m && m <= m_pre_3) {
				dfs_last2step_List_4(l2s, road, m, m * 3, m_first, m_first_5, s, u, pthread, task_id);
			}
		}
	}
	_visited[v] = 0;
}
void dfs_last2step_List_2(last2step& l2s, int* road, long long m_pre, long long m_pre_3, long long m_first, long long m_first_5, int s, int v, int pthread, int task_id) {
	bool* _visited = __p_info[pthread].__visited;
	_visited[v] = 1;
	int u;
	long long m, m_last = 0, m_second_last = 0;
	int last_node = 0;
	int i = __G_point[v], end = __G_point[v + 1];
	for (; i < end; i += 1) {
		m = __G[i];
		u = m >> 32;
		m = m & MASK;
		if (u > s) {
			road[2] = u;
			if (__p_info[pthread].__in_last2step[u]) {
				//output
				l2s.visit(u);
				while (l2s.get(last_node, m_second_last, m_last))
				{
					if (last_node == v) continue;
					if (PANDUAN(m_pre, m) && PANDUAN(m, m_second_last) && PANDUAN(m_last, m_first)) {
						road[3] = last_node;
						__ans[task_id].add_ans(road, 4);
					}
				}
			}
			if (m_pre <= 5 * m && m <= m_pre_3) {
				dfs_last2step_List_3(l2s, road, m, m * 3, m_first, m_first_5, s, u, pthread, task_id);
			}
		}
	}
	_visited[v] = 0;
}

void dfs_last2step_List_1(last2step& l2s, int* road, long long m_pre, long long m_pre_3, long long m_first, long long m_first_5, int s, int v, int pthread, int task_id) {
	bool* _visited = __p_info[pthread].__visited;
	_visited[v] = 1;
	int u;
	long long m, m_last = 0, m_second_last = 0;
	int last_node = 0;
	int i = __G_point[v], end = __G_point[v + 1];
	for (; i < end; i += 1) {
		m = __G[i];
		u = m >> 32;
		m = m & MASK;
		if (u > s) {
			road[1] = u;
			if (__p_info[pthread].__in_last2step[u]) {
				//output
				//road[1] = second_last_node;
				l2s.visit(u);
				while (l2s.get(last_node, m_second_last, m_last))
				{
					if (PANDUAN(m_last, m) && PANDUAN(m, m_second_last)) {
						road[2] = last_node;
						__ans[task_id].add_ans(road, 3);
					}
				}
			}
			dfs_last2step_List_2(l2s, road, m, m * 3, m, m * 5, s, u, pthread, task_id);
		}
	}
	_visited[v] = 0;
}
*/
void solve_problem_last2step_List(int pthread) {
	int road[9], begin_id = 0, end_id = 0, ret, ids_index, task_id;
	long long  money[9], money3[9];//money3[0]的用途与money5[1~7]不同！
	ull* list_of_last2step = __p_info[pthread].__list_of_last2step;
	ull* list_of_last3step = __p_info[pthread].__list_of_last3step;
	bool* _in_last2step = __p_info[pthread].__in_last2step;
	bool* _in_last3step = __p_info[pthread].__in_last3step;
	int list_of_last2step_end, list_of_last3step_end;
	last2step l2s;
	l2s.init(pthread);
	while (true)
	{
		//开始互斥
		std::unique_lock<std::mutex> _lock(__mutex);
		if (__task_id < TASK_NUM) {
			task_id = __task_id++;
		}
		else {
			task_id = -1;
		}
		_lock.unlock();
		//互斥结束
		if (task_id == -1) {
#ifdef DEBUG
			cout << "pthread : " << pthread << " completed!" << endl;
#endif // DEBUG
			break;
		}
		__ans[task_id].get_range(begin_id, end_id);
		for (int i = begin_id; i < end_id; ++i) {
			if (__ignored[i]) continue;
			build_last2step_List(l2s, list_of_last2step, list_of_last2step_end, list_of_last3step, list_of_last3step_end, _in_last2step, _in_last3step, i, pthread);
			road[0] = i;
			dfs_last2step_List(l2s, road, 0, 0, 0, 0, 1, i, i, pthread, task_id);
			//dfs_last2step_List_1(l2s, road, 0, 0, 0, 0, i, i, pthread, task_id);
			for (int x = 0; x < list_of_last2step_end; ++x) {
				_in_last2step[list_of_last2step[x]] = 0;
			}
			for (int x = 0; x < list_of_last3step_end; ++x) {
				_in_last3step[list_of_last3step[x]] = 0;
			}
			l2s.clear();
		}
	}

}
void sub_devide(int begin, int end, int id) {
	int block = (end - begin) / PTHREAD_NUM;
	int sub_end = block + begin;
	for (int i = 0; i < PTHREAD_NUM - 1; ++i) {
		__ans[id * PTHREAD_NUM + i].end_id = __ans[id * PTHREAD_NUM + i + 1].begin_id = sub_end;
		sub_end += block;
	}
	__ans[id * PTHREAD_NUM].begin_id = begin;
	__ans[id * PTHREAD_NUM + PTHREAD_NUM - 1].end_id = end;
}
void devide() {
	int node_size = __node_size;
	int segment_begin, segment_end;
	int res_begin = 0;
	int task_per_pthread = TASK_NUM / PTHREAD_NUM;
	int bgein, end;
	for (int i = 0; i < task_per_pthread - 1; ++i) {
		segment_begin = res_begin;
		segment_end = segment_begin + (node_size - res_begin) / FENMU * FENZI;

		//devide
		sub_devide(segment_begin, segment_end, i);

		res_begin = segment_end;
	}
	segment_begin = res_begin;
	segment_end = node_size;
	//devide
	sub_devide(segment_begin, segment_end, task_per_pthread - 1);
}
void devide_a() {
	int node_size = __node_size;
	int segment_begin, segment_end;
	int res_begin = 0;
	int block = node_size / TASK_NUM;
	int end = block;
	for (int id = 0; id < TASK_NUM - 1; ++id) {
		__ans[id].end_id = __ans[id + 1].begin_id = end;
		end += block;
	}
	__ans[0].begin_id = 0;
	__ans[TASK_NUM - 1].end_id = node_size;
}
int i2s(char* str_num, int total_num) {
	char str_num_re[10];
	int point = 0;
	int ret = 0;
	if (total_num == 0) {
		str_num[0] = '0';
		return 1;
	}
	while (total_num)
	{
		str_num_re[point++] = total_num % 10 + '0';
		total_num /= 10;
	}
	while (point)
	{
		str_num[ret++] = str_num_re[--point];
	}
	str_num[ret++] = '\n';
	return ret;

}
void output(char* str) {
	FILE* file = fopen(str, "w");
	char str_num[11];
	int str_num_size;
	int total_num = 0, ret;
	for (int task_id = 0; task_id < TASK_NUM; ++task_id) {
		total_num += __ans[task_id].total_num;
	}
	str_num_size = i2s(str_num, total_num);
	fwrite(str_num, sizeof(char), str_num_size, file);
	for (int i = 0; i < 5; ++i) {
		for (int task_id = 0; task_id < TASK_NUM; ++task_id) {
			//i2s(__buf, __ans[task_id], i + 3, file);

			for (auto& block_info : __ans[task_id].ans_block_point[i]) {
				fwrite(block_info.second, sizeof(char), block_info.first, file);
			}

		}
	}
	fclose(file);
}
void output_write_only_once(char* str) {
	FILE* file = fopen(str, "w");
	char str_num[11];
	int total_num = 0, ret, str_num_len;
	int point;
	for (int task_id = 0; task_id < TASK_NUM; ++task_id) {
		total_num += __ans[task_id].total_num;
	}
	str_num_len = point = i2s(str_num, total_num);
	//确定每截答案的位置
	for (int i = 0; i < 5; ++i) {
		for (int task_id = 0; task_id < TASK_NUM; ++task_id) {
			__ans[task_id].ans_begin[i] = point;
			int len = 0;
			for (auto& block_info : __ans[task_id].ans_block_point[i]) {
				len += block_info.first;
			}
			point += len;
		}
	}
	int end = point;
	char* buf = (char*)malloc(end * sizeof(char));
	memcpy(buf, str_num, sizeof(char) * str_num_len);
	for (int i = 0; i < 5; ++i) {
		for (int task_id = 0; task_id < TASK_NUM; ++task_id) {
			int begin = __ans[task_id].ans_begin[i];

			for (auto& block_info : __ans[task_id].ans_block_point[i]) {
				memcpy(buf + begin, block_info.second, sizeof(char) * block_info.first);
				begin += block_info.first;
			}

		}
	}
	fwrite(buf, sizeof(char), end, file);
	fclose(file);
}
void output_mmap(char* str) {
	FILE* file = fopen(str, "w");
	char str_num[11];
	int total_num = 0, ret, str_num_len;
	int point;
	for (int task_id = 0; task_id < TASK_NUM; ++task_id) {
		total_num += __ans[task_id].total_num;
	}
#ifdef DEBUG
	cout << "total_num: " << total_num << endl;
#endif // DEBUG
	str_num_len = point = i2s(str_num, total_num);
	//确定每截答案的位置
	for (int i = 0; i < 6; ++i) {
		for (int task_id = 0; task_id < TASK_NUM; ++task_id) {
			__ans[task_id].ans_begin[i] = point;
			int len = 0;
			for (auto& block_info : __ans[task_id].ans_block_point[i]) {
				len += block_info.first;
			}
			point += len;
		}
	}
	int end = point;
	char* buf = (char*)malloc(end * sizeof(char));
	memcpy(buf, str_num, sizeof(char) * str_num_len);
	for (int task_id = 0; task_id < TASK_NUM; ++task_id) {
		for (int i = 0; i < 6; ++i) {
			int begin = __ans[task_id].ans_begin[i];

			for (auto& block_info : __ans[task_id].ans_block_point[i]) {
				memcpy(buf + begin, block_info.second, sizeof(char) * block_info.first);
				begin += block_info.first;
			}

		}
	}
	fwrite(buf, sizeof(char), end, file);
	fclose(file);
}
int main() {
	char input_file[] = "/data/test_data.txt";
	char output_file[] = "/projects/student/result.txt";
	//char input_file[] = "testdata/3512444/test_data.txt";
	//char input_file[] = "testdata/2861665/test_data.txt";
	//char input_file[] = "testdata/19630345/test_data.txt";


	/*


	ull u = 1;
	ull v = 2;
	ull c = (u << 32) +v;
	ull v_1 = c & MASK;
	ull u_1 = c >> 32;

	cout << 1 << endl;

*/




#ifdef DEBUG
	clock_t start, finish, pthread_start, pthread_finish;
	start = clock();
#endif // DEBUG

	for (int x = 0; x < TASK_NUM; x++) {
		__ans[x].answer_init(x);
	}

	readfile(input_file);
	//changeid_new();
	changeid_new();

	build_G();
	//build_G_prun();

	//devide();
	devide_a();

	//S.solve_problem_last2step(0);
#ifdef DEBUG
	pthread_start = clock();
#endif // DEBUG
	
	std::thread th[PTHREAD_NUM];
	/*
	for (int i = 0; i < 5; ++i) {
		for (int x = 0; x < PTHREAD_NUM; x++) {
			th[x] = std::thread(solve_problem_last2step_List, x);
		}
		for (int x = 0; x < PTHREAD_NUM; x++) {
			th[x].join();
		}
		__task_id = 0;
		
		for (int j = 0; j < TASK_NUM; ++j) {
			__ans[j].clear();
		}
	}
	*/
	
	for (int x = 0; x < PTHREAD_NUM; x++) {
		th[x] = std::thread(solve_problem_last2step_List, x);
	}
	for (int x = 0; x < PTHREAD_NUM; x++) {
		th[x].join();
	}





	//solve_problem_last2step_List(0);
#ifdef DEBUG
	pthread_finish = clock();
#endif // DEBUG

	//solve_problem_last2step_List(0);
	output_mmap(output_file);
#ifdef DEBUG
	finish = clock();
	double duration = ((double)finish - (double)start) / CLOCKS_PER_SEC;
	cout << "before pthread:\t" << ((double)pthread_start - (double)start) / CLOCKS_PER_SEC << endl
		<< "pthread:\t" << ((double)pthread_finish - (double)pthread_start) / CLOCKS_PER_SEC << endl
		<< "output:\t\t" << ((double)finish - (double)pthread_finish) / CLOCKS_PER_SEC << endl;
#endif // DEBUG

}
