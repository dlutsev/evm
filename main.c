#include <stdio.h>
int main(){
        long int n = 6000000000;
        double pi=0;
        for (long int i = 0; i <= n; i++){
                if(i%2 == 0)
                        pi = pi + 4/(2*1.0*i+1);

                else
                        pi = pi - 4/(2*1.0*i+1);
        }
        printf("%1.9f\n", pi);
        return 0;
}
