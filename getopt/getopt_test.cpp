#include <cstdio>
#include <cstdlib>

#include <bits/getopt_core.h>
#include <getopt.h>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[])
{
    int c;
    int option_index = 0;

    // 1. 定义长选项结构体数组
    // 最后一个元素必须是全 0，作为结束标记
    static struct option long_options[] = {
        {"add", no_argument, 0, 'a'}, // -a
        {"file", required_argument, 0, 'f'}, // -f filename
        {"verbose", no_argument, 0, 'v'}, // -v
        {"help", no_argument, 0, 'h'}, // -h
        {0, 0, 0, 0},
    };

    // 2. 循环解析参数
    // "af:vh" 是短选项字符串：avh 无参，f 有参
    while((c = getopt_long(argc, argv, "af:vh", long_options, &option_index)) != -1) {
        switch(c) {
            case 'a':
                printf("Option: Add\n");
                break;
            case 'v':
                printf("Option: Verbose\n");
                break;
            case 'h':
                printf("Option: Help\n");
                break;
            case 'f': 
                // 使用 optarg 获取参数
                printf("Option: file, filename: %s\n", optarg); 
                break;
            case '?':
                // 当用户输入未知选项时，getopt_long 自动返回 '?'
                // 并将无效字符保存到 optopt
                printf("error: unknown option: %c\n", optopt);
                break;
            default:
                abort();
        }
    }

    // 3. 处理剩余的非选项参数
    // optind 现在的索引指向第一个非选项参数
    if(optind < argc) {
        printf("non-optiona argument: ");
        while(optind < argc) {
            printf("%s ", argv[optind ++ ]);
        }
        printf("\n");
    }

    return 0;
}