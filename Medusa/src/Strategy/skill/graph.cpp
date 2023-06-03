#include "graph.h"
#include "string.h"
#include <stdio.h>
#include "graph.h"
#include <iostream>
#include<queue>
#include <stdbool.h>

/*邻接表（Adjacency Lists）*/
#define MaxVertexNum 100
typedef int Vertex;
typedef int WeightType;
typedef int DataType;

/*边的定义*/
typedef struct ENode *PtrToENode;
struct ENode{
    Vertex V1, V2;      //有向边V1，V2
    WeightType Weight;  //权重
};
typedef PtrToENode Edge;

/*邻接点的定义*/
typedef struct AdjVNode *PtrToAdjVNode;
struct AdjVNode{
    Vertex AdjV;        //邻接点下标
    WeightType Weight;  //边权重
    PtrToAdjVNode Next; //指向下一个邻接点的指针
};

/*顶点表头结点的定义*/
typedef struct Vnode{
    PtrToAdjVNode FirstEdge;//边表头指针
    DataType Data;          //存顶点的数据
    /*注意：很多情况下，顶点无数据，此时Data可以不用出现*/

}AdjList[MaxVertexNum];     //AdjList是邻接表类型

/*图结点的定义*/
typedef  struct GNode *PtrToGNode;
struct GNode{
    int Nv;                 //顶点数
    int Ne;                 //边数
    AdjList G;              //邻接表
};
typedef PtrToGNode LGraph;  //以邻接表方法存储的图类型
///////////////////////////
/*以上是邻接表的结构和类型声明*/
///////////////////////////

LGraph CreateGraph(int VertexNum)
{/*初始化一个有VertexNum个顶点但没有边的图*/
Vertex V;
LGraph Graph;

Graph = (LGraph)malloc(sizeof(struct GNode)); /*建立图*/
Graph->Nv = VertexNum;
Graph->Ne=0;
/*初始化邻接表头指针*/
/*注意:这里默认顶点编号从0开始，到( Graph->Nv- 1)*/
for(V=0;  V<Graph->Nv; V++)
    Graph->G[V]. FirstEdge =NULL;


return Graph;
}

void InsertEdge(LGraph Graph,Edge E)
{
    PtrToAdjVNode NewNode;
    /*插入边<V1， V2>*/
    /*为V2建立新的邻接点*/
    NewNode = (PtrToAdjVNode)malloc(sizeof(struct AdjVNode));
    NewNode->AdjV = E->V2;
    NewNode->Weight = E->Weight;
    /*将V2插入V1的表头*/
    NewNode->Next = Graph->G[E->V1].FirstEdge;
    Graph->G[E->V1].FirstEdge = NewNode;


}

void Visit(Vertex V)
{
    printf("正在访问顶点%d\n",V);
}

int Player = 12;
bool Visited [12];
//for(int i = 0; i < 12; i++)
//{
//    Visited[i] = false;
//}
/*Visited[]为全局变量，已经初始化为false*/
void DFS(Graph G ,Vertex V ,void(*Visit)(Vertex))
{/*从第V个顶点出发递归地深度优先遍历图G*/
    PtrToAdjVNode w;

    Visit(V);//访问第V个顶点
    Visited[V] = true;//标记V已访问

//    for(W = Graph->G[V].FirstEdge;w;w = w->Next)//对V的每个邻接点W->AdjV
//    {
//        if(!Visited[W->AdjV])//若W->AdjV未被访问
//            DFS(Graph, W->AdjV, Visit);
//    }
}


Graph::Graph()
{

}
