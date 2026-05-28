#include<stdio.h>

int add(int a,int b){
    return a + b;
}
int sub(int a,int b){
    return a - b;
}
int multi(int a,int b){
    return a * b;
}
int divi(int a,int b){
    if(b == 0) {
        printf("Please Enter Valid Value");
    }
    else return a / b;
}
int rem(int a,int b){
    return a % b;
}
int power(int a,int b){
    int result = 1;
    for(int i = 1; i <= b ; i++)    result *= a;
    return result;
}

int main(){
    int a,b,r,option;
    printf("\nThis is Calculator in c made by Prince Sanchela\n");
    while(1){
        printf("\n1.Addition (+)");
        printf("\n2.Subtraction (-)");
        printf("\n3.Multiplication (x)");
        printf("\n4.Division ( / )");
        printf("\n5.Remainder ()");
        printf("\n6.Power (^)");
        printf("\n7.Exit");
        printf("\nSelect The Option ( give its respected Number ) : ");
        scanf("%d",&option);
        if(option == 7) return 0;
        printf("\nEnter The First Number : ");
        scanf("%d",&a);
        printf("\nEnter The Second Number : ");
        scanf("%d",&b);

        switch(option){
            case 1:
                r = add(a,b);
                break;
            case 2:
                r = sub(a,b);
                break;
            case 3:
                r = multi(a,b);
                break;
            case 4:
                r = divi(a,b);
                break;
            case 5:
                r = rem(a,b);
                break;
            case 6:
                r = power(a,b);
                break;
            default:
                printf("Please Enter Valid Option");
                break;
        }
        printf("\n\nYour Result is %d \n",r);
    }
}