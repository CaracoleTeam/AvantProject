#include <stdio.h>

int main(){
    float x[100],y[100],a,s=1,t=1,k=0;
    int n,i,j,d = 1;
    printf("\n\n Digite o número de termos da tabela: \n");
    scanf("%d",&n);
    printf("\n\n Digite os valores de x e y respectivamente: \n");
    for(i = 0;i<n;i++){
        scanf("%f",&x[i]);
        scanf("%f",&y[i]);
    }
    printf("tabela: \n\n");
    for(i = 0;i<n;i++){
        printf("%0.3f\t%0.3f",x[i],y[i]);
        printf("\n");
    }
    while(d==1){
        printf("\n\n\n Digite o valor de x para achar o valor de y: \n\n\n");
        scanf("%f",&a);
        for(i = 0;i<n;i++){
            s=1;
            t=1;
            for(j = 0;j<n;j++){
                if(j!=i){
                    s = s*(a-x[j]);
                    t=t*(x[i]-x[j]);
                }
            }
            k = k + ((s/t)*y[i]);
        }
        printf("\n\n o valor da variavel y é: %f\n",k);
        printf("Deseja continuar? 1 para continuar, qualquer outra tecla para sair");
        scanf("%d",&d);
    }
}
