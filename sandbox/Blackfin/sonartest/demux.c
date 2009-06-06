#include <stdio.h>


void main()
{
    FILE * f1 = fopen("/tmp/dataA.txt", "w");
    FILE * f2 = fopen("/tmp/dataB.txt", "w");
    FILE * f3 = fopen("/tmp/dataC.txt", "w");
    FILE * f4 = fopen("/tmp/dataD.txt", "w");


    while(!feof(stdin))
    {
        unsigned short t = 0;

//         t = (((signed short) getchar()) & 0xFF) | (((signed short) getchar) << 8);

//         printf("* %04x ", getchar() | (getchar() << 8));
//         printf("%d ", (signed short) (getchar() | (getchar() << 8)));
//         printf("%04x ", getchar() | (getchar() << 8));
//         printf("%04x\n", getchar() | (getchar() << 8));

        fprintf(f1, "%d\n", (signed short) (getchar() | (getchar() << 8)));
        fprintf(f2, "%d\n", (signed short) (getchar() | (getchar() << 8)));
        fprintf(f3, "%d\n", (signed short) (getchar() | (getchar() << 8)));
        fprintf(f4, "%d\n", (signed short) (getchar() | (getchar() << 8)));
    }

    fclose(f1);
    fclose(f2);
    fclose(f3);
    fclose(f4);
}