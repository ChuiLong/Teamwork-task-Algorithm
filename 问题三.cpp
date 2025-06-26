#include<bits/stdc++.h>
using namespace std;
typedef long long ll;
const int N=1000000,M=1000000;
int n,m,edgeCount=1,source,sink,head[N],item[N],breakpoints[N];
ll minCost[N],taskDemand[N],totalDemand,totalAnger;
bool inQueue[N];
struct str
{
    int m,q;
    ll r,w;
}a[M];
void road(int x,int y,ll r,ll w){
    a[++edgeCount].m=y;
    a[edgeCount].q=head[x];
    head[x]=edgeCount;
    a[edgeCount].r=r;
    a[edgeCount].w=w;}
bool SPFA()
{
    queue<int> Q;
    Q.push(source);
    for(int i=1;i<=sink;++i)
    {
        minCost[i]=1e18;
        inQueue[i]=false;
    }
    minCost[source]=0;
    inQueue[source]=true;
    while(!Q.empty())
    {
        int k=Q.front();
        Q.pop();
        if(inQueue[k]==false) continue;
        inQueue[k]=false;
        for(int i=head[k];i!=0;i=a[i].q)
        {
            if(a[i].r>0&&minCost[k]+a[i].w<minCost[a[i].m])
            {
                minCost[a[i].m]=minCost[k]+a[i].w;
                Q.push(a[i].m);
                inQueue[a[i].m]=true;
            }
        }
    }
    if(minCost[sink]!=1e18) return true;
    return false;
}
ll dfs(int x,ll r)
{
    if(x==sink) return r;
    ll s=0;
    for(int i=item[x];i!=0;i=a[i].q)
    {
        item[x]=i;
        if(inQueue[a[i].m]==false&&a[i].r>0&&minCost[a[i].m]==minCost[x]+a[i].w)
        {
            inQueue[a[i].m]=true;
            ll z=dfs(a[i].m,min(r,a[i].r));
            inQueue[a[i].m]=false;
            if(z!=0)
            {
                a[i].r-=z;
                a[i^1].r+=z;
                r-=z;
                s+=z;
                totalAnger+=z*a[i].w;
            }
            else minCost[a[i].m]=0;
            if(r==0) return s; }
    }
    return s;
}
int main() {
    printf("=== 任务分配问题求解器 ===\n\n");
    
    scanf("%d%d",&m,&n);
    printf("问题规模：%d 名组员，%d 种任务\n", m, n);
    
    for(int i=1;i<=n;++i)
    {
        scanf("%lld",&taskDemand[i]);
        totalDemand+=taskDemand[i];
    }
    
    printf("任务需求：\n");
    for(int i=1;i<=n;++i)
    {
        printf("任务 %d：需要 %lld 个工作单位\n", i, taskDemand[i]);
    }
    printf("总工作量：%lld 个工作单位\n\n", totalDemand);
    
    // 读取并输出能力矩阵
    printf("组员能力矩阵：\n");
    printf("     ");
    for(int j=1;j<=n;++j) printf("任务%2d ", j);
    printf("\n");
    
    int **capability = new int*[m+1];
    for(int i=1;i<=m;++i) capability[i] = new int[n+1];
    
    for(int i=1;i<=m;++i)
    {
        printf("组员%2d: ", i);
        for(int j=1;j<=n;++j)
        {
            scanf("%d",&capability[i][j]);
            printf("%4d ", capability[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    
    source=n+m+1;
    sink=n+m+2;
    
    // 建立源点到任务的边
    for(int i=1;i<=n;++i)
    {
        road(source,i,taskDemand[i],0);
        road(i,source,0,0);
    }
    
    // 建立任务到组员的边
    for(int i=1;i<=m;++i)
    {
        for(int j=1;j<=n;++j)
        {
            if(capability[i][j]==0) continue;
            road(j,n+i,1e18,0);
            road(n+i,j,0,0);
        }
    }
    
    // 读取并输出愤怒值函数
    printf("组员愤怒值函数：\n");
    for(int i=1;i<=m;++i)
    {
        int s;
        scanf("%d",&s);
        printf("组员 %d 的愤怒值函数（%d 个分段点）：\n", i, s);
        
        breakpoints[0]=0;
        breakpoints[s+1]=totalDemand;
        for(int j=1;j<=s;++j)
        {
            scanf("%d",&breakpoints[j]);
        }
        
        printf("  分段点：");
        for(int j=0;j<=s+1;++j)
        {
            if(j==s+1) printf("∞");
            else printf("%d ", breakpoints[j]);
        }
        printf("\n  各段单位愤怒值：");
        
        for(int j=1;j<=s+1;++j)
        {
            ll w;
            scanf("%lld",&w);
            printf("%lld ", w);
            road(n+i,sink,breakpoints[j]-breakpoints[j-1],w);
            road(sink,n+i,0,-w);
            
            if(j<=s) {
                printf("(工作量%d-%d) ", breakpoints[j-1]+1, breakpoints[j]);
            } else {
                printf("(工作量%d+) ", breakpoints[j-1]+1);
            }
        }
        printf("\n\n");
    }
    
    printf("开始求解最小费用最大流...\n");
    int iterations = 0;
    while(SPFA())
    {
        iterations++;
        printf("第 %d 次增广路径搜索...\n", iterations);
        for(int i=1;i<=sink;++i) 
        {
            item[i]=head[i];
            inQueue[i]=false;
        }
        ll flow = dfs(source,1e18);
        printf("本次增广流量：%lld\n", flow);
    }
    printf("求解完成，共进行 %d 次增广。\n\n", iterations);
    
    printf("=== 求解结果 ===\n");
    printf("组员的最小愤怒值总和：%lld\n\n", totalAnger);
    
    // 统计每个组员的工作分配
    ll *workerLoad = new ll[m+1];
    ll *workerAnger = new ll[m+1];
    for(int i=1;i<=m;++i) {
        workerLoad[i] = 0;
        workerAnger[i] = 0;
    }
    
    printf("详细任务分配：\n");
    bool hasAssignment = false;
    for(int j = 1; j <= n; j++){
        bool taskAssigned = false;
        for(int ei = head[j]; ei; ei = a[ei].q){
            int to = a[ei].m;
            if(to >= n+1 && to <= n+m){
                long long used = a[ei^1].r; 
                if(used > 0){
                    int worker = to - n;     
                    printf("任务 %d → 组员 %d : %lld 单位\n",
                           j, worker, used);
                    workerLoad[worker] += used;
                    hasAssignment = true;
                    taskAssigned = true;
                }
            }
        }
        if(!taskAssigned) {
            printf("任务 %d：未分配\n", j);
        }
    }
    
    if(!hasAssignment) {
        printf("没有找到可行的任务分配方案！\n");
    }
    
    printf("\n各组员工作量统计：\n");
    ll totalAssigned = 0;
    for(int i=1;i<=m;++i) {
        printf("组员 %d：分配工作量 %lld 单位", i, workerLoad[i]);
        totalAssigned += workerLoad[i];
        printf("\n");
    }
    
    printf("\n工作量验证：\n");
    printf("总需求工作量：%lld\n", totalDemand);
    printf("实际分配工作量：%lld\n", totalAssigned);
    if(totalAssigned == totalDemand) {
        printf("✓ 所有任务都已完成分配\n");
    } else {
        printf("✗ 分配不完整，缺少 %lld 个工作单位\n", totalDemand - totalAssigned);
    }
    
    // 清理内存
    for(int i=1;i<=m;++i) delete[] capability[i];
    delete[] capability;
    delete[] workerLoad;
    delete[] workerAnger;
    
    return 0;
}