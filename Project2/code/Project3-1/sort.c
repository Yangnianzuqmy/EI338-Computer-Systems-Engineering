#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>

/* sort the list by using multi-thread */

/* the size of the list */
#define LEN 80
#define ARG_LEN 10

/* for multiple arguments to be passed 
 * we have to define a struct */
typedef struct _param{
    int start;
    int end;
} funcParam;

/* sort thread */
void *sortThread(void *param);
/* merge thread */
void *mergeThread(void *param);
/* input array */
int *in_arr;
/* output array */
int *out_arr;

int main(int argc, char *argv[]){
    int i, mid = LEN >> 1;
    pthread_t tid[3];
    pthread_attr_t attr[3];
    /* arguments */
    funcParam params[2];
    /* allocate the space and initilize the in_array */
    in_arr = malloc(sizeof(int) * LEN);
    out_arr = malloc(sizeof(int) * LEN);
    printf("Before sorting, the input array is:\n");
    int i1;
    for(i1=0;i1<LEN;i1++){
        in_arr[i1] = LEN - i1;
        printf("%d ", in_arr[i1]);
    }
    printf("\n\n");

    /* sort the first sub list */
    /* make the args */
    params[0].start = 0;    params[0].end = mid; 
    pthread_attr_init(&attr[0]);
    pthread_create(&tid[0], &attr[0], sortThread, &params[0]);

    /* sort the second sub list */
    /* mark the args */
    params[1].start = mid;    params[1].end = LEN;
    pthread_attr_init(&attr[1]);
    pthread_create(&tid[1], &attr[1], sortThread, &params[1]);

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    /* merge the sort results */
    pthread_attr_init(&attr[2]);
    pthread_create(&tid[2], &attr[2], mergeThread, &mid);

    pthread_join(tid[2], NULL);
    
    /* print the output after the sorting */
    printf("After the sorting, the output array is:\n");
    for(i=0;i<LEN;i++)  printf("%d ", out_arr[i]);
    printf("\n");
    return 0;
}

void *sortThread(void *param){
    funcParam arg = *(funcParam *)param;
    int start = arg.start, end = arg.end;
    int min, tmp, idx;
    int i1;
    for(i1=start;i1<end;i1++){
        min = in_arr[i1];
        idx = i1;
        int j1;
        for(j1=i1;j1<end;j1++){
            if(in_arr[j1] < min){
                min = in_arr[j1];
                idx = j1;
            }
            tmp = in_arr[i1];
            in_arr[i1] = in_arr[idx];
            in_arr[idx] = tmp;
        }
    }
}

void *mergeThread(void *param){
    /* merge the two threads */
    int mid = *(int *)param;
    int i = 0, j = mid, k=0;
    while(i < mid && j < LEN){
        if(in_arr[i] < in_arr[j])   out_arr[k++] = in_arr[i++];
        else    out_arr[k++] = in_arr[j++];
        
    }
    if(i == mid)
        while(j < LEN)  out_arr[k++] = in_arr[j++];
    else
        while(i < mid)  out_arr[k++] = in_arr[i++];
    
}
