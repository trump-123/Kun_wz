#include <stdio.h>
#include <iostream>
#include <ctime>
#include "test1.h"
//#include "testacc.cpp"
#define N 2000000
//#include <include/openacc.h>
//#include "include/openacc_predef.h"
//#include "include/_cplus_preinclude.h"
//#include "include/_cplus_macros.h"
//namespace v{
int main(){
        clock_t start,end;
        start = clock();
        int i,a[N],b[N],c[N];
         int s[20000];
        for(i = 0;i<N;++i){
            a[i] = 0;
            b[i] = c[i] = i;
        }
       // #pragma acc parallel num_gangs(1024) num_workers(1024)
        //{
//            #pragma acc loop
//            for(i = 0;i<N;i++){
//                a[i] = b[i] + c[i];
//            }
           // #pragma acc loop //independent
            for(i = 1;i<N;i++){
                b[i]=b[i-1];
                //std::cout<<i<<std::endl;
            }
            //#pragma acc loop independent
            for(i= 0;i<20000;i++){
                //a[i] = b[i] + c[i];
                //std::cout<<s<<std::endl;
//                s[i] = i;
            }
        //}
        end = clock();
        //printf("a[N-1] = %d\n",a[N-1]);
        //printf("b[2] = %d\n",b[3]);
        std::cout<<b[2]<<std::endl;
        std::cout<<"time:"<<(double)(end-start)/1000<<"ms"<<std::endl;
        return 0;
    //}
}
