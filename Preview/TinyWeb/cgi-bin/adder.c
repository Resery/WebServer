#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXLINE 8192

int main() {
    char *buf, *p;
    char arg1[MAXLINE], arg2[MAXLINE], content[MAXLINE];
    int n1 = 0, n2 = 0;

    // Extrcat Two Arguments
    if ((buf = getenv("QUERY_STRING")) != NULL) {
        // Find THe First '&' In Buf And Return The Rest of String And The Head Is '&'
        p = strchr(buf, '&');
        // Transction The Buf
        *p = '\0';
        // Get First Opt
        strcpy(arg1, buf);
        // Get Second Opt
        strcpy(arg2, p + 1);
        n1 = atoi(arg1);
        n2 = atoi(arg2);
    }

    // Make The Response Body
    sprintf(content, "Welcome to add.com: ");
    sprintf(content, "%sTHe Internet addition portal.\r\n<p>", content);
    sprintf(content, "%sThe answer is: %d + %d = %d\r\n<p>",
            content, n1, n2, n1 + n2);
    sprintf(content, "%sThanks for visiting!\r\n",content);

    // Generate The Http Response
    printf("Content-length: %ld\r\n", strlen(content));
    printf("Content-type: text/html\r\n\r\n");
    printf("%s", content);
    fflush(stdout);
    exit(0);
}
