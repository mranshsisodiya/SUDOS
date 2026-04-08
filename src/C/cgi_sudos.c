#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Content-Type: text/html; charset=UTF-8\n\n");
    printf("<html><body>");
    printf("<h2>SUDOS - Smart Urban Delivery Optimization</h2>");
    printf("<pre>");

    FILE *fp = popen("C:/xampp/cgi-bin/sudos.exe", "r");

    if (fp == NULL) {
        printf("Error: Could not run sudos.exe");
        return 1;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }

    pclose(fp);

    printf("</pre></body></html>");
    return 0;
}