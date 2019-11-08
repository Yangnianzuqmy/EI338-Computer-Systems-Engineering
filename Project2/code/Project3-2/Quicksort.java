import java.util.concurrent.*;

public class Quicksort extends RecursiveAction
{
    static final int SIZE = 50;
    static final int THRESHOLD = 10;

    private int begin;
    private int end;
    private int[] array;

    public Quicksort(int begin, int end, int[] array){
        this.begin = begin;
        this.end = end;
        this.array = array;
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
            int low = begin, high = end;
            int val = array[low];
            do{
                while(low < high && array[high] >= val) high--;
                if(low < high)  { array[low] = array[high]; low++;}
                while(low < high && array[low] <= val)  low++;
                if(low < high) { array[high] = array[low];high--; }
            }while(low!=high);

            array[low] = val;

            /* from the place we put val */
            int mid = low + 1;
            Quicksort left = new Quicksort(begin, mid - 1, array);
            Quicksort right = new Quicksort(mid + 1, end, array);

            left.fork();
            right.fork();

            left.join();
            right.join();
            
        }
    } 

    public static void main(String [] args){
        ForkJoinPool pool = new ForkJoinPool();
        int [] array = new int[SIZE];
        
        /* initialize the array */
        java.util.Random rand = new java.util.Random();

        /* print the input */
        System.out.println("Quicksort:");
        System.out.println("Before sorting, input array is:");
        for(int i=0;i<SIZE;i++){
            array[i] = rand.nextInt(SIZE);
        }
        for(int i=0;i<50;i++){
            System.out.print(array[i] + " ");
        }
        System.out.println("");

        Quicksort task = new Quicksort(0, SIZE-1, array);

        pool.invoke(task);

        /* print the output */
        System.out.println("After sorting, the array turns to:");
        for(int i=0;i<SIZE;i++){
            System.out.print(array[i] + " ");
        }
        System.out.println("");
        System.out.println("Finish sorting!");
    }
        
}

