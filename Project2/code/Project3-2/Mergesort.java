import java.util.concurrent.*;

public class Mergesort extends RecursiveAction
{
    static final int SIZE = 50;
    static final int THRESHOLD = 10;

    private int begin;
    private int end;
    private int[] array;

    public Mergesort(int begin, int end, int[] array){
        this.begin = begin;
        this.end = end;
        this.array = array;
    }

    protected void merge(int begin, int end, int [] array){
        int mid = (begin+end)/2;
        int i=begin, j=mid+1, k=0;
        int [] tmp = new int[end-begin+1];
        while(i<=mid && j<=end){
            if(array[i]<array[j])   tmp[k++] = array[i++];
            else    tmp[k++] = array[j++];
        }
        while(j<=end)    tmp[k++] = array[j++];
        while(i<=mid)    tmp[k++] = array[i++];

        for(i=begin;i<=end;i++){
            array[i] = tmp[i-begin];
        }
    }

    @Override
    protected void compute(){
        if (end - begin < THRESHOLD){
            /* conquer stage */
            /* use comparison sort to sort the array */
            int min, idx;
            for(int i=begin;i<end;i++){
                min = array[i];
                idx = i;
                for(int j=i;j<=end;j++){
                    if(array[j] < min){
                        min = array[j];
                        idx = j;
                    }
                }
                array[idx] = array[i];
                array[i] = min;
            }

        }else {
            /* divide stage */
            int mid = (begin + end) / 2;
            Mergesort left = new Mergesort(begin, mid, array);
            Mergesort right = new Mergesort(mid + 1, end, array);
            left.fork();
            right.fork();
            left.join();
            right.join();
            
            /* call the merge function */
            merge(begin, end, array);
        }
    } 

    public static void main(String [] args){
        ForkJoinPool pool = new ForkJoinPool();
        int [] array = new int[SIZE];
        
        /* initialize the array */
        java.util.Random rand = new java.util.Random();

        /* print the input */
        System.out.println("Mergesort:");

        System.out.println("Before sorting, input array is:");
        for(int i=0;i<SIZE;i++){
            array[i] = rand.nextInt(SIZE);
        }
        for(int i=0;i<50;i++){
            System.out.print(array[i] + " ");
        }
        System.out.println("");

        Mergesort task = new Mergesort(0, SIZE-1, array);

        pool.invoke(task);

        /* print the output */
        System.out.println("After sorting, the array turns to:");
        for(int i=0;i<50;i++){
            System.out.print(array[i] + " ");
        }
        System.out.println("");
        System.out.println("Finish sorting!");
    }
        
}

