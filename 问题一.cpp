#include<bits/stdc++.h>
using namespace std;

#define MAXN 1000
#define MAXM 100000
#define MEMINF 0x3f
pair<int,int> task_assignment[65];
#define INF 0x3f3f3f3f
#define P(x,y)    (~-(x)*N+(y))

struct Edge {
    int to,nex,c,w;
    Edge(){}
    Edge(int _to, int _nex, int _c, int _w):to(_to),nex(_nex),c(_c),w(_w){}
};

Edge e[MAXM+5];
int first[MAXN+5], q[MAXN+5], dis[MAXN+5], pre[MAXN+5], processing_time, tot, num_tasks, num_members, N, M, S, T, V, total_cost, total_flow;
bool book[MAXN+5];

inline void Add(int a, int b, int c, int w) {
    e[tot] = Edge(b,first[a],c,w);
    first[a] = tot++;
    e[tot] = Edge(a,first[b],0,-w);
    first[b] = tot++;
    return;
}

inline bool SPFA() {
    memset(dis,MEMINF,sizeof(dis));
    q[0] = S, dis[S] = 0, book[S] = true;
    for(register int head = 0, tail = 1, t, u; head != tail; book[t] = false)
        for(t = q[head++], head %= MAXN, u = first[t]; u+1; u = e[u].nex)
            if(e[u].c > 0 && dis[e[u].to] > dis[t] + e[u].w)
            {
                dis[e[u].to] = dis[t] + e[u].w, pre[e[u].to] = u;
                if(!book[e[u].to])
                    book[e[u].to] = true, q[tail++] = e[u].to, tail %= MAXN;
            }
    return (dis[T] != INF);
}

inline void CALC() {
    int del = INF;
    for(register int p = T, u = pre[p]; p != S; p = e[u^1].to, u = pre[p])
        del = min(del,e[u].c);
    for(register int p = T, u = pre[p]; p != S; p = e[u^1].to, u = pre[p])
        e[u].c -= del, e[u^1].c += del, total_cost += del*e[u].w;
    total_flow += del;
    return;
}

inline void MCMF() {
    for(; SPFA(); CALC());
    return;
}

int main() {
    memset(first,-1,sizeof(first));
    printf("请输入子任务数量和组员数量：");
    scanf("%d%d",&num_members,&num_tasks);
    M = num_members; N = num_tasks;
    S = 0, T = (V = M*N)+N+1;    //slots:M*N    tasks:N
    
    printf("\n请输入每个子任务在各组员处的处理时间：\n");
    for(register int task_id = 1, member_id, position; task_id <= N; task_id++)        //tasks
    {
        printf("子任务%d在各组员处的处理时间：", task_id);
        for(member_id = 1; member_id <= M; member_id++)                        //members
        {
            scanf("%d",&processing_time);
            for(position = 1; position <= N; position++)
                Add(V+task_id,P(member_id,position),1,position*processing_time);
        }
    }
    
    for(register int task_id = 1; task_id <= N; task_id++)            //tasks
        Add(S,V+task_id,1,0);
    for(register int slot = 1; slot <= V; slot++)            //slots
        Add(slot,T,1,0);
    
    printf("\n正在计算最优分配方案...\n");
    MCMF();
    
    // 解析分配结果
    for(int task_id = 1; task_id <= N; ++task_id){
        int u = V + task_id;  // 子任务 task_id 对应的网络节点
        // 遍历它所有出边
        for(int eid = first[u]; eid != -1; eid = e[eid].nex){
            // 这条边连到 P(member_id,position) 槽位节点
            if(e[eid].to >= 1 && e[eid].to <= M*N){
                // 如果反向边的容量 c==1，就说明这条 task_id→slot 被用了
                if(e[eid^1].c == 1){
                    int slot = e[eid].to;      // slot = (member_id-1)*N + position
                    int member_id = (slot-1) / N + 1;   // 组员编号
                    int position = (slot-1) % N + 1;   // 该组员队列中的第 position 个任务
                    task_assignment[task_id] = {member_id,position};
                    break;
                }
            }
        }
    }
    
    vector<pair<int,int>> member_tasks[10]; // member_tasks[member_id]: 存 {position, task_id}
    for(int task_id = 1; task_id <= N; ++task_id){
        auto [member_id,position] = task_assignment[task_id];
        member_tasks[member_id].push_back({position,task_id});
    }
    
    // 输出结果
    printf("\n=================== 最优分配结果 ===================\n");
    printf("最小平均完成时间：%.2f\n", double(total_cost) / N);
    printf("总完成时间：%d\n", total_cost);
    printf("子任务总数：%d\n", N);
    
    printf("\n各组员的任务分配详情：\n");
    for(int member_id = 1; member_id <= M; ++member_id){
        sort(member_tasks[member_id].begin(), member_tasks[member_id].end());
        printf("组员%d的任务执行顺序：", member_id);
        if(member_tasks[member_id].empty()){
            printf("无任务分配");
        } else {
            for(auto &pos_task : member_tasks[member_id]){
                printf("子任务%d(第%d个) ", pos_task.second, pos_task.first);
            }
        }
        printf("\n");
    }
    
    printf("\n各子任务的分配情况：\n");
    for(int task_id = 1; task_id <= N; ++task_id){
        auto [member_id, position] = task_assignment[task_id];
        printf("子任务%d -> 分配给组员%d (执行顺序第%d个)\n", task_id, member_id, position);
    }
    
    printf("==================================================\n");
    
    return 0;
}