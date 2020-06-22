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


#include <pthread.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <assert.h>

#include <math.h>
#include <sched.h>




using namespace std;
//#define BUF_SIZE 536870912
#define BUF_SIZE 100000000
//#define BUF_SIZE 2147483648
#define NODE_SIZE 2500000
//#define NODE_SIZE_MAR 4000768‬
#define MARGIN 768
#define INPUT_SIZE 2500000
//#define DFS_SIZE 4
#define id_t unsigned int
#define ull unsigned long long
#define newid_t uint32_t//unsigned int
//#define weight_t Weight
#define weight_t double
#define REMAIN_LEN 1000 //保留小数后几位
#define REMAIN_MAR 10000000000//额外用整型保存的位数 
#define REMAIN_NUM 10
#define MASK 4294967295
#define PTHREAD_NUM 12
//#define DEBUG
#define TASK_NUM 256
#define TOPK 100
#define INF 0xffffffffffffffff
#define LIST_CAP 10
/*
#define PTHREAD_NUM 4
//#define DEBUG
#define FENZI 1
#define FENMU 4 //FENZI / FENMU
#define TASK_NUM 32
*/


#define HASH_BIT_NUM 21
#define HASH_BIT_REMAIN 2

//大数据部分

struct Weight {
	double weight;
	//ull i;
	//double re;
	//Weight() :weight(0), i(0), re(0) {}
};
struct Node
{
	newid_t id;
	ull money;
	Node() :id(0), money(0) {}
	Node(newid_t nid, ull nm) :id(nid), money(nm) {}
	
	bool operator >(const Node& second_node) const {
		return this->money > second_node.money;
	}
	bool operator <(const Node& second_node) const {
		return this->money < second_node.money;
	}
};
/*
struct Edge {
	//newid_t source;
	newid_t target;
	ull money;
	Edge() :target(0), money(0) {}
	Edge(newid_t ntarget, ull nmoney) :target(ntarget), money(nmoney) {}
	//Edge(newid_t s, Node x) : target(x.id), money(x.money) {}
	bool operator >(const Edge& second_node) const {
		//if (this->money == second_node.money) return this->target < second_node.target;
		return this->money > second_node.money;
	}

};*/

id_t hashmap[(1 << (HASH_BIT_NUM + HASH_BIT_REMAIN)) + NODE_SIZE][2];
bool hash_flag[(1 << (HASH_BIT_NUM + HASH_BIT_REMAIN)) + NODE_SIZE + MARGIN];


//预处理部分
ull __input[INPUT_SIZE * 3];


int __G_size[NODE_SIZE] = {};
//int __in_G_size[NODE_SIZE] = {};
//分配任务
//bool __set_G_id[NODE_SIZE] = {};
//vector<vector<newid_t>>__task_set;
//vector < vector<newid_t>>__connected_G;



//公用部分
newid_t __task_num;
id_t __newid2id[NODE_SIZE + 1] = {};
//int __newid2edges[NODE_SIZE] = {};
//unsigned long long __G[INPUT_SIZE] = {};
Node __G[INPUT_SIZE] = {};
//Node __in_G[INPUT_SIZE] = {};
//int __in_G[INPUT_SIZE * 2] = {};
//Node __in_G[INPUT_SIZE * 2] = {};

newid_t __G_point[NODE_SIZE + 32] = {};

//newid_t __in_G_point[NODE_SIZE + 32] = {};
//int __in_G_point[NODE_SIZE + 32] = {};

//char __newid2char[NODE_SIZE][16];

//bool __ignored[NODE_SIZE + MARGIN] = {};
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
/*
struct anwser
{
	weight_t id_weight[NODE_SIZE];
}__ans[PTHREAD_NUM];*/
class VisitEdge {
public:
	Node* pbegin;
	Node* pend;
	//VisitEdge(Node* nbegin, Node* nend) :pbegin(nbegin), pend(nend) {}
	VisitEdge(newid_t index) {
		pbegin = __G + __G_point[index];
		pend = __G + __G_point[index + 1];
	}
	void set(newid_t index) {
		pbegin = __G + __G_point[index];
		pend = __G + __G_point[index + 1];
	}
	Node* begin() {
		return pbegin;
	}
	Node* end() {
		return pend;
	}
};

//#define CMP_G(a,b) ((a.weight.weight -  b.weight.weight)>0.0001 ||abs(a.weight.weight -  b.weight.weight)<=0.0001 && (__newid2id[a.id]<__newid2id[b.id]))
#define CMP_G(a,b) ((a.weight -  b.weight)>0.0001 ||abs(a.weight -  b.weight)<=0.0001 && (__newid2id[a.id]<__newid2id[b.id]))
//#define CMP_L(a,b) ((a.weight/REMAIN_MAR)<(b.weight/REMAIN_MAR))
//#define CMP_Q(a,b) ((a.weight/REMAIN_MAR)==(b.weight/REMAIN_MAR))



#ifdef DEBUG
struct debugnode {
	debugnode(newid_t ns, newid_t nt, ull nm) :s(ns), t(nt), m(nm) {}
	newid_t s, t;
	ull m;
};
vector<debugnode> debug;
#endif // DEBUG




//结果部分
/*
inline void add_weight(weight_t& target, ull count, ull total_count) {
	target.re += (double)count / (double)total_count;

	while (target.re >= 1.0) {
		target.re -= 1.0;
		target.i += 1;
	}
	target.weight = (double)target.i + target.re;
}
inline void add_weight(weight_t& target, weight_t sourse) {
	target.re += sourse.re;

	if (target.re >= 1.0) {
		target.re -= 1.0;
		target.i += 1;
	}
	target.i += sourse.i;
	target.weight = (double)target.i + target.re;
}

*/

//template <class T> //小顶堆，判断只用大于号,使用栈空间
class MyPriorityQueue {
public:
	Node* vec;
	ull size;
	Node x;
	//bool pop_flag = 0;
	MyPriorityQueue() :size(0) {
	}
	MyPriorityQueue(Node* p) :vec(p), size(0) {
	}
	~MyPriorityQueue() {
	}
	void set_vec(Node* p) {
		vec = p;
	}
	//vector<T>vec;
	void ad_back() {
		int i = size - 1;
		if (i == 0) return;
		int f;
		x = vec[i];
		while (i)
		{
			f = (i - 1) / 2;
			if (vec[f] > x) {
				vec[i] = vec[f];
				i = f;
			}
			else {
				break;
			}
		}
		vec[i] = x;
	}
	void ad_begin() {
		int i = 0;
		int f = i * 2 + 1;
		x = vec[i];
		while (f < size)
		{
			if (f + 1 < size && vec[f] > vec[f + 1]) {
				f = f + 1;
			}
			if (x > vec[f]) {
				vec[i] = vec[f];
				i = f;
				f = i * 2 + 1;
			}
			else {
				break;
			}
		}
		vec[i] = x;
	}
	void push(Node node) {
		/*
		if (pop_flag) {
			vec[0] = node;
			ad_begin();
			pop_flag = 0;
		}
		else {
		*/
			//vec.push_back(node);
			vec[size++] = node;
			//调整back
			ad_back();
		//}
	}
	void pop() {
		//vec[0] = vec[size - 1];
		//vec.pop_back();
		/*
		if (pop_flag) {
			vec[0] = vec[size - 1];
			--size;
			//调整头
			ad_begin();
		}
		else {
			pop_flag = 1;
		}*/
		vec[0] = vec[size - 1];
		--size;
		//调整头
		ad_begin();
	
	}
	Node& top() {
		/*
		if (pop_flag) {
			vec[0] = vec[size - 1];
			--size;
			pop_flag = 0;
		}*/
		return vec[0];
	}
	bool empty() {
		//return vec.empty();
		//return (size - pop_flag) == 0;
		return size == 0;
	}
private:
};
class MyPriorityQueue_new{
public:
	Node* vec;
	ull size;
	newid_t* index_p;
	Node x;
	//bool pop_flag = 0;
	MyPriorityQueue_new() :size(0) {
	}
	MyPriorityQueue_new(Node* p, newid_t* nindex_p) :vec(p), index_p(nindex_p), size(0) {
	}
	~MyPriorityQueue_new() {
	}
	/*
	void set_vec(Node* p) {
		vec = p;
	}*/
	//vector<T>vec;
	void ad_back() {
		int i = size - 1;
		if (i == 0) return;
		int f;
		newid_t index;
		x = vec[i];
		while (i)
		{
			f = (i - 1) / 2;
			if (vec[f] > x) {
				vec[i] = vec[f];
				index_p[vec[i].id] = i;
				i = f;
			}
			else {
				break;
			}
		}
		vec[i] = x;
		index_p[vec[i].id] = i;
	}
	void ad_up(newid_t i) {
		//int i = size - 1;
		if (i == 0) return;
		int f;
		x = vec[i];
		while (i)
		{
			f = (i - 1) / 2;
			if (vec[f] > x) {
				vec[i] = vec[f];
				index_p[vec[i].id] = i;
				i = f;
			}
			else {
				break;
			}
		}
		vec[i] = x;
		index_p[vec[i].id] = i;
	}
	void ad_begin() {
		int i = 0;
		int f = i * 2 + 1;
		x = vec[i];
		while (f < size)
		{
			if (f + 1 < size && vec[f] > vec[f + 1]) {
				f = f + 1;
			}
			if (x > vec[f]) {
				vec[i] = vec[f];
				index_p[vec[i].id] = i;
				i = f;
				f = i * 2 + 1;
			}
			else {
				break;
			}
		}
		vec[i] = x;
		index_p[vec[i].id] = i;
	}
	void push(Node node) {
		/*
		if (pop_flag) {
			vec[0] = node;
			ad_begin();
			pop_flag = 0;
		}
		else {
		*/
		//vec.push_back(node);
		if (index_p[node.id] == INPUT_SIZE) {

			index_p[node.id] = size;
			vec[size++] = node;

			//调整back
			ad_back();
		}
		else {

			vec[index_p[node.id]].money = node.money;
#ifdef DEBUG
			if (vec[index_p[node.id]].id != node.id) {
				cout << "!" << endl;
			}
#endif // DEBUG
			ad_up(index_p[node.id]);
		}
		//}
	}
	void pop() {
		index_p[vec[0].id] = INPUT_SIZE;
		--size;
		if (size == 0) return;
		vec[0] = vec[size];

		index_p[vec[0].id] = 0;
		
		//调整头
		ad_begin();

	}
	Node& top() {
		/*
		if (pop_flag) {
			vec[0] = vec[size - 1];
			--size;
			pop_flag = 0;
		}*/
		return vec[0];
	}
	bool empty() {
		//return vec.empty();
		//return (size - pop_flag) == 0;
		return size == 0;
	}
};

struct VisiteInfo//记录要访问的节点的起始、截至位置和当前访问位置
{
	newid_t id;
	newid_t begin;
	newid_t end;
	newid_t point;
	newid_t pid;//当前指针指向位置的id

};
struct TaskInfo
{
	newid_t begin;
	newid_t end;

} __TaskInfo[TASK_NUM];
//TaskInfo* __TaskInfo;
struct GraphNode {
	newid_t val_id;
	newid_t next;
	GraphNode() {}
	GraphNode(newid_t nval_id, newid_t nnext) :val_id(nval_id), next(nnext) {}
};
struct Graph {
	GraphNode list[NODE_SIZE + INPUT_SIZE];//头节点做共同结尾
	newid_t end_point = NODE_SIZE;
	newid_t _point;
	Graph() {
		//fill(list, list + INPUT_SIZE, GraphNode(0, 0));
	}
	void init() {
		end_point = NODE_SIZE;
	}
	void init(newid_t node_size) {
		end_point = NODE_SIZE;
	}
	void add(newid_t id, newid_t pre_id) {

		newid_t point = end_point++;
		list[point].next = list[id].next;
		list[id].next = point;
		list[point].val_id = pre_id;

	}
	void reset(newid_t id, newid_t pre_id) {
		list[id].next = -1;
		list[id].val_id = pre_id;
	}

	void begin(newid_t id) {
		//_next = node2FirstEdge[id];
		_point = id;
	}
	bool end() {
		return _point == -1;
	}
	void next() {
		_point = list[_point].next;
	}
	newid_t visit() {
		return list[_point].val_id;
	}
};
struct VecGraph {
	newid_t* list[NODE_SIZE];//头节点做共同结尾
	newid_t list_capital[NODE_SIZE];
	newid_t list_size[NODE_SIZE];
	//newid_t end_point = NODE_SIZE;
	newid_t* _end;
	newid_t* _p;
	VecGraph() {
		//fill(list, list + INPUT_SIZE, GraphNode(0, 0));
		for (int i = 0; i < NODE_SIZE; ++i) {
			list[i] = (newid_t*)malloc(sizeof(newid_t) * LIST_CAP);
			list_capital[i] = LIST_CAP;
			list_size[i] = 0;
		}
	}
	void init() {
		//end_point = NODE_SIZE;
	}
	void init(newid_t node_size) {
		//end_point = NODE_SIZE;
	}
	void add(newid_t id, newid_t pre_id) {
		if (list_capital[id] == list_size[id]) {
			list_capital[id] *= 2;
			newid_t* linshi = (newid_t*)malloc(sizeof(newid_t) * list_capital[id]);
			memcpy(linshi, list[id], sizeof(newid_t) * list_size[id]);
			free(list[id]);
			list[id] = linshi;
		}
		list[id][list_size[id]++] = pre_id;

	}
	void reset(newid_t id, newid_t pre_id) {
		list[id][0]= pre_id;
		list_size[id] = 1;
	}

	void begin(newid_t id) {
		//_next = node2FirstEdge[id];
		_p = list[id];
		_end = list[id] + list_size[id];
	}
	bool end() {
		return _p == _end;
	}
	void next() {
		++_p;
	}
	newid_t visit() {
		return *_p;
	}
};
struct visit_node
{
	newid_t point;
	newid_t end;
};
struct pthread_info
{
	newid_t E_vec_index[INPUT_SIZE];
	Node E_vec[INPUT_SIZE];//优先队列的数组
	ull id_len[NODE_SIZE] = {};
	newid_t clear_vec[NODE_SIZE];

	newid_t id_pathNum[NODE_SIZE];
	weight_t id_weight[NODE_SIZE];//存储答案
	bool visited[NODE_SIZE + MARGIN] = {};
	
	newid_t path[NODE_SIZE];
	visit_node visit_point[NODE_SIZE];
	ull money[NODE_SIZE];
	//vector<vector<newid_t>>pre_list;
	weight_t single_id_weight[NODE_SIZE];
	VecGraph pre_list;

	newid_t clear_vec_size = 0;
	pthread_info() {
		//pre_list = vector<vector<newid_t>>(NODE_SIZE);
		fill(id_len, id_len + NODE_SIZE, INF);
		fill(E_vec_index, E_vec_index + INPUT_SIZE, INPUT_SIZE);
	}
	void init(newid_t id) {
		//weight_t w;
		//w.i = 0;
		//w.re = 0;
		//w.weight = 0;
		weight_t w = 0;
		for (int i = 0; i < clear_vec_size; ++i) {
			visited[clear_vec[i]] = 0;
			id_len[clear_vec[i]] = INF;
			single_id_weight[clear_vec[i]] = w;
		}
		pre_list.init(__node_size);
		clear_vec_size = 0;
		id_pathNum[id] = 1;
	}

}__p_info[PTHREAD_NUM];

struct ans_node
{
	ans_node() {

	}
	//ans_node(newid_t nid, weight_t nweight) :id(nid), weight(nweight) {}
	newid_t id;
	weight_t weight;
};
class total_answer {
public:
	ull id_weight[NODE_SIZE] = {};
	ans_node topk[TOPK];
	total_answer() {
		for (int i = 0; i < TOPK; ++i) {
			topk[i].id = NODE_SIZE;
			topk[i].weight = weight_t();
			//topk[i].weight.weight = 0;
			//topk[i].weight.remain = 0;
		}
	}
	void add_ans_node(ans_node x) {
		if (CMP_G(x, topk[0])) {
			ans_node linshi;
			int i = 0;
			topk[0] = x;
			int child;
			bool end = 0;
			if (i * 2 + 2 < TOPK && CMP_G(topk[i * 2 + 1], topk[i * 2 + 2])) {
				child = i * 2 + 2;
			}
			else if (i * 2 + 1 < TOPK) {
				child = i * 2 + 1;
			}
			else {
				return;
			}
			while (1)
			{
				if (CMP_G(topk[i], topk[child])) {
					linshi = topk[i];
					topk[i] = topk[child];
					topk[child] = linshi;
					i = child;
				}
				else {
					break;
				}
				if (i * 2 + 2 < TOPK && CMP_G(topk[i * 2 + 1], topk[i * 2 + 2])) {
					child = i * 2 + 2;
				}
				else if (i * 2 + 1 < TOPK) {
					child = i * 2 + 1;
				}
				else {
					break;
				}
			}
		}
	}
	void add(newid_t id) {
		ans_node x;
		x.id = id;
		x.weight = weight_t();
		//x.weight.weight = 0;
		//x.weight.remain = 0;
		//x.weight.remain = 0.0;
		for (int i = 0; i < PTHREAD_NUM; ++i) {
			//x.weight.weight += __p_info[i].id_weight[id].weight;
			x.weight += __p_info[i].id_weight[id];
			//add_weight(x.weight, __p_info[i].id_weight[id]);
		}
		add_ans_node(x);
	}
	void end() {
		sort(topk, topk + TOPK, [](const ans_node a, const ans_node b) {
			//if (CMP_Q(a.weight, b.weight)) return __newid2id[a.id] < __newid2id[b.id];
			return CMP_G(a, b);
			});
	}

}__total_ans;
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
		point = end;
		ids[2] = x;

		if (ids[2] && ids[1] != ids[0]) {

			__input[input_size++] = ids[0];
			__input[input_size++] = ids[1];
			__input[input_size++] = ids[2];
		}
	}
	__input_size = input_size;
	fclose(file);
}

unordered_set<id_t>id_hashset;
unordered_map<id_t, int>_id2newid;
void changeid() {//可以合并至readfile

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
	}
	//sort(__newid2id, __newid2id + node_size);
	for (int i = 0; i < node_size; ++i) {
		_id2newid[__newid2id[i]] = i;
	}
	__newid2id[NODE_SIZE] = 0xfffffffff;
	//替换input为新id并统计出入边
	for (int i = 0; i < size; i += 3) {
		__input[i] = u = _id2newid[__input[i]];
		__input[i + 1] = v = _id2newid[__input[i + 1]];
		++__G_size[u];
		//++__in_G_size[v];
	}
	__node_size = node_size;
	//_last2step = vector<vector<int>>(node_size);
}
void build_G() {
	int size = __input_size;
	int node_size = __node_size;
	int sub_size;
	ull u, v, m;
	//构建图的指针
	for (int i = 0; i < node_size; ++i) {
		__G_point[i + 1] = __G_point[i] + __G_size[i];
		//__in_G_point[i + 1] = __in_G_point[i] + __in_G_size[i];
	}
	//构件图
	for (int i = 0; i < size; i += 3) {
		u = __input[i];
		v = __input[i + 1];
		m = __input[i + 2];
		--__G_size[u];
		//--__in_G_size[v];
		__G[__G_point[u] + __G_size[u]].id = v;
		__G[__G_point[u] + __G_size[u]].money = m;
		//__in_G[__in_G_point[v] + __in_G_size[v]].id = u;
		//__in_G[__in_G_point[v] + __in_G_size[v]].money = m;
	}
	/*
	for (int i = 0; i < node_size; ++i) {
		sort(__G + __G_point[i], __G + __G_point[i + 1], [](const Node& a, const Node& b) {
			a.money < b.money; });
	}*/
}
void devide() {
	__task_num = TASK_NUM;
	//__TaskInfo = new TaskInfo[__task_num];
	int node_size = __node_size;
	int segment_begin, segment_end;
	int res_begin = 0;
	int block = node_size / __task_num;
	int end = block;
	for (int id = 0; id < __task_num - 1; ++id) {
		__TaskInfo[id].end = __TaskInfo[id + 1].begin = end;
		end += block;
	}
	__TaskInfo[0].begin = 0;
	__TaskInfo[__task_num - 1].end = node_size;
}
void devide_sub_G(vector<newid_t>&G) {
	__task_num = TASK_NUM;
	//__TaskInfo = new TaskInfo[__task_num];
	int node_size = G.size();
	int segment_begin, segment_end;
	int res_begin = 0;
	int block = node_size / __task_num;
	int end = block;
	for (int id = 0; id < __task_num - 1; ++id) {
		__TaskInfo[id].end = __TaskInfo[id + 1].begin = end;
		end += block;
	}
	__TaskInfo[0].begin = 0;
	__TaskInfo[__task_num - 1].end = node_size;
}
newid_t __fa[NODE_SIZE], __rank[NODE_SIZE];
bool __set_G[NODE_SIZE];
newid_t __fa2vec[NODE_SIZE];
vector<vector<newid_t>>__connected_G;
int find(int x)
{
	return x == __fa[x] ? x : (__fa[x] = find(__fa[x]));
}
inline void merge(int i, int j)
{
	int x = find(i), y = find(j);
	if (__rank[x] <= __rank[y])
		__fa[x] = y;
	else
		__fa[y] = x;
	if (__rank[x] == __rank[y] && x != y)
		__rank[y]++;
}
void devide_G() {
	newid_t size = __input_size;
	newid_t u, v;
	newid_t node_size = __node_size;
	for (int i = 1; i <= node_size; ++i)
	{
		__fa[i] = i;
		__rank[i] = 1;
	}
	for (int i = 0; i < size; i += 3) {
		u = __input[i];
		v = __input[i + 1];
		merge(u, v);
	}
	newid_t f;
	for (int i = 0; i < node_size; ++i) {
		f = find(i);
		if (__set_G[f]) {
			__connected_G[__fa2vec[f]].push_back(i);
		}
		else {
			__set_G[f] = 1;
			__fa2vec[f] = __connected_G.size();
			__connected_G.push_back(vector<newid_t>(1, i));
		}
	}
	sort(__connected_G.begin(), __connected_G.end(), [](const vector<newid_t>& a, const vector<newid_t>& b) {
		return a.size() > b.size(); });

}

void bfs_Simple(newid_t s, pthread_info* p_info) {
	//Node x;
	newid_t clear_vec_size = 0;
	ull money = 0;
	//MyPriorityQueue q(__p_info[pthread].E_vec);
	//NodePriorityQueue q(p_info->E_vec);
	//MyPriorityQueue q(p_info->E_vec);// , p_info->E_vec_index);
	MyPriorityQueue_new q(p_info->E_vec, p_info->E_vec_index);

	Node n;
	//VisitEdge ve(s);
	newid_t begin, end;
	q.push(Node(s,0));
	p_info->id_pathNum[s] = 1;
	while (!q.empty())
	{
		n = q.top();
		q.pop();
		if (p_info->visited[n.id]) {
			continue;
		}
		p_info->visited[n.id] = 1;
		p_info->clear_vec[clear_vec_size++] = n.id;
		//ve.set(n.id);
		begin = __G_point[n.id];
		end = __G_point[n.id + 1];
		//for (auto x : ve) {
		for(;begin<end;++begin){
			Node x = __G[begin];
			if (p_info->id_len[x.id] > x.money + n.money) {
				//q.push(Edge(e.target, Node(x.id, x.money + e.money)));
				q.push(Node(x.id, x.money + n.money));
				p_info->id_len[x.id] = x.money + n.money;
				p_info->id_pathNum[x.id] = p_info->id_pathNum[n.id];
				p_info->pre_list.reset(x.id, n.id);
			}
			else if (p_info->id_len[x.id] == x.money + n.money) {
				p_info->id_pathNum[x.id] += p_info->id_pathNum[n.id];
				//__p_info[pthread].pre_list[x.id].push_back(n.id);
				p_info->pre_list.add(x.id, n.id);
			}
		}
	}
	p_info->clear_vec_size = clear_vec_size;
	--clear_vec_size;
	newid_t v;
	//__p_info[pthread].pre_list.clear(s);
	//__p_info[pthread].pre_list[s].clear();
	while (clear_vec_size)
	{
		v = p_info->clear_vec[clear_vec_size];
		
		if (p_info->pre_list.list_size[v] == 1) {
			newid_t x = *p_info->pre_list.list[v];

			p_info->single_id_weight[x] += (1 + p_info->single_id_weight[v]);
		}
		else {
			newid_t* p = p_info->pre_list.list[v];
			newid_t* end = p + p_info->pre_list.list_size[v];

			for (; end != p; ++p) {
				newid_t x = *p;
				p_info->single_id_weight[x] += ((double)p_info->id_pathNum[x] / (double)p_info->id_pathNum[v]) * (1 + p_info->single_id_weight[v]);
			}
		}
		/*
		if (p_info->pre_list.list[v].next == -1) {
			newid_t x = p_info->pre_list.list[v].val_id;
			p_info->single_id_weight[x] += (1 + p_info->single_id_weight[v]);
		}
		else {
		
	
			for (p_info->pre_list.begin(v); !p_info->pre_list.end(); p_info->pre_list.next()) {
				newid_t x = p_info->pre_list.visit();
				p_info->single_id_weight[x] += ((double)p_info->id_pathNum[x] / (double)p_info->id_pathNum[v]) * (1 + p_info->single_id_weight[v]);
			}
		}*/
		p_info->id_weight[v] += p_info->single_id_weight[v];
		--clear_vec_size;
	}
}


void *solve_problem(void * arg){
	int pthread =* ((int*)arg);
	pthread_info* p_info = __p_info + pthread;
	
	ull money = 0;
	newid_t begin, end, task_id;
	for (int i = 0; i < __node_size; ++i) {
		__p_info[pthread].id_weight[i] = weight_t();
	}
	while (true)
	{
		//开始互斥
		std::unique_lock<std::mutex> _lock(__mutex);
		if (__task_id < __task_num) {
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
		begin = __TaskInfo[task_id].begin;
		end = __TaskInfo[task_id].end;
		for (int i = begin; i < end; ++i) {
			//p_info->id_count.clear();
			//if(!(i%(__node_size/1000))) cout<<i*1000/__node_size<<"%%"<<endl;
			p_info->init(i);
			//bfs(i, pthread);

			bfs_Simple(i, p_info);

			//dfs(__p_info + pthread, i, 0, 0);
			//dfs_expand(__p_info + pthread, i);
		}
#ifdef DEBUG
		cout << "task " << task_id << " completed" << endl;
#endif // DEBUG

	}

}

void print(ans_node& node, FILE* file) {
	fprintf(file, "%ld,%.3lf\n", __newid2id[node.id], node.weight);
}


void output(char* str) {
	FILE* file = fopen(str, "w");
	
	for (int i = 0; i < __node_size; ++i) {
		__total_ans.add(i);
	}
	
	__total_ans.end();
	int end = min(TOPK, __node_size);
	for (int i = 0; i < end; ++i) {
		print(__total_ans.topk[i], file);
		//fprintf(file, "%d,%d.%d\n", __newid2id[__total_ans.topk[i].id], __total_ans.topk[i].weight / (REMAIN_LEN * REMAIN_MAR), (__total_ans.topk[i].weight % (REMAIN_LEN * REMAIN_MAR)) / REMAIN_MAR);
	}


	//memcpy(buf, str_num, sizeof(char) * str_num_len);
	//fwrite(buf, sizeof(char), end, file);
	fclose(file);
}



int main() {
#ifndef DEBUG
	char input_file[] = "/data/test_data.txt";
	char output_file[] = "/projects/student/result.txt";
#endif // !DEBUG

#ifdef DEBUG



	char input_file[] = "test_data3.txt";
	char output_file[] = "y.txt";

#endif // DEBUG



#ifdef DEBUG
	clock_t start, finish, pthread_start, pthread_finish;
	start = clock();
#endif // DEBUG
	readfile(input_file);
	//changeid_new
	changeid();
	build_G();

	
	devide();
	//devide_for_connected_graph();
#ifdef DEBUG
	pthread_start = clock();
#endif // DEBUG
	
	int i;//, cpu_nums = countCPU();
	int  tmp[PTHREAD_NUM];//cpuset[i]是负责执行某个线程的若干个CPU核
	pthread_t Thread[PTHREAD_NUM];//存放线程ID，ID由函数pthread_create分配
	cpu_set_t mask;
	for (int i = 0; i < 6; ++i) {
		tmp[i] = i;
	}
	for (int i = 6; i < 12; ++i) {
		tmp[i] = i + 2;
	}
	for (i = 0; i < PTHREAD_NUM; i++){
		tmp[i] = i;

		CPU_ZERO(&mask);
		if(i < 6)
			CPU_SET(i, &mask); 
		else
			CPU_SET(i + 2, &mask);
		pthread_create(&Thread[i], NULL, solve_problem, &tmp[i]);//创建线程
	}
	for(i = 0; i < PTHREAD_NUM; ++i)
				pthread_join(Thread[i],NULL);
/*


	std::thread th[PTHREAD_NUM];

	for (int x = 0; x < PTHREAD_NUM; x++) {
		th[x] = std::thread(solve_problem, x);
	}
	for (int x = 0; x < PTHREAD_NUM; x++) {
		th[x].join();
	}
*/
	/*
	devide_G();
	std::thread th[PTHREAD_NUM];
	int G_id = 0;
	for (auto& vec : __connected_G) {
		if (G_id) {
			if ((vec.size() * vec.size()) < __total_ans.topk[0].weight.i) break;
		}
		//if (vec.size() < 5) break;
		devide_sub_G(vec);
		//devide_for_connected_graph();
#ifdef DEBUG
		pthread_start = clock();
#endif // DEBUG

		
		__task_id = 0;
		for (int x = 0; x < PTHREAD_NUM; x++) {
			th[x] = std::thread(solve_problem_sub, G_id, x);
		}
		for (int x = 0; x < PTHREAD_NUM; x++) {
			th[x].join();
		}
		for (int i = 0; i < __node_size; ++i) {
			__total_ans.add(i);
		}
		++G_id;
	}
	*/
#ifdef DEBUG
	pthread_finish = clock();
#endif // DEBUG

	output(output_file);

#ifdef DEBUG
	finish = clock();
	double duration = ((double)finish - (double)start) / CLOCKS_PER_SEC;
	cout << "before pthread:\t" << ((double)pthread_start - (double)start) / CLOCKS_PER_SEC << endl
		<< "pthread:\t" << ((double)pthread_finish - (double)pthread_start) / CLOCKS_PER_SEC << endl
		<< "output:\t\t" << ((double)finish - (double)pthread_finish) / CLOCKS_PER_SEC << endl;
#endif // DEBUG

}
