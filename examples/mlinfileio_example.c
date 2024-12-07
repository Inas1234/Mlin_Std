#include "../include/mlinstd.h"

int main() {
    // READING FROM FILE
    MlinString *file_content = mlin_file_to_mlinstring("test.txt");
    if (file_content){
        printf("%s\n", file_content->contets);
        mlin_string_free(file_content);
    } 


    char *fileContent = mlin_file_to_char("test.txt");
    if (fileContent) {
        printf("File Content:\n%s\n", fileContent);
        free(fileContent);
    }


    // WRITING TO FILE

    MlinString *str = mlin_string_create_with_src("Hello, File!");
    if (mlin_string_to_file("output.txt", str)) {
        printf("Successfully wrote to file.\n");
    }
    mlin_string_free(str);

    const char *content = "Hello, File!";
    if (mlin_char_to_file("output2.txt", content)) {
        printf("Successfully wrote to file.\n");
    }



    return 0;
}
