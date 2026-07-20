#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <windows.h>
#define EPS 1e-6
#define TARGET 24.0
#include <conio.h>// 全局变量：存储所有找到的解法（去重）
char solutions[1000][50];
int sol_count = 0;
// 全局开关：黑暗模式开启标记
int dark_mode = 0;

// 计算两个数的运算结果
double calc(double a, double b, int op) {
    switch (op) {
        case 0: return a + b;
        case 1: return a - b;
        case 2: return a * b;
        case 3:
            if (fabs(b) < EPS) return -999999;
            return a / b;
        default: return 0;
    }
}
// 运算符转字符
char getOp(int op) {
    switch (op) {
        case 0: return '+';
        case 1: return '-';
        case 2: return '*';
        case 3: return '/';
        default: return ' ';
    }
}

// 检查解法是否重复
int isDuplicate(char *s) {
    for (int i = 0; i < sol_count; i++) {
        if (strcmp(solutions[i], s) == 0) return 1;
    }
    return 0;
}

// 添加解法
void addSolution(char *s) {
    if (!isDuplicate(s)) {
        strcpy(solutions[sol_count], s);
        sol_count++;
    }
}

// 穷举所有表达式（5种括号结构）
void findAllSolutions(double nums[], int indices[]) {
    char buf[50];
    double a = nums[0], b = nums[1], c = nums[2], d = nums[3];
    int ia = indices[0], ib = indices[1], ic = indices[2], id = indices[3];
    
    for (int op1 = 0; op1 < 4; op1++) {
        for (int op2 = 0; op2 < 4; op2++) {
            for (int op3 = 0; op3 < 4; op3++) {
                // 结构1: ((a op1 b) op2 c) op3 d
                double r1 = calc(calc(calc(a, b, op1), c, op2), d, op3);
                if (fabs(r1 - TARGET) < EPS) {
                    sprintf(buf, "((%d%c%d)%c%d)%c%d = 24", ia, getOp(op1), ib, getOp(op2), ic, getOp(op3), id);
                    addSolution(buf);
                }
                
                // 结构2: (a op1 (b op2 c)) op3 d
                double r2 = calc(calc(a, calc(b, c, op2), op1), d, op3);
                if (fabs(r2 - TARGET) < EPS) {
                    sprintf(buf, "(%d%c(%d%c%d))%c%d = 24", ia, getOp(op1), ib, getOp(op2), ic, getOp(op3), id);
                    addSolution(buf);
                }
                
                // 结构3: a op1 ((b op2 c) op3 d)
                double r3 = calc(a, calc(calc(b, c, op2), d, op3), op1);
                if (fabs(r3 - TARGET) < EPS) {
                    sprintf(buf, "%d%c((%d%c%d)%c%d) = 24", ia, getOp(op1), ib, getOp(op2), ic, getOp(op3), id);
                    addSolution(buf);
                }
                
                // 结构4: a op1 (b op2 (c op3 d))
                double r4 = calc(a, calc(b, calc(c, d, op3), op2), op1);
                if (fabs(r4 - TARGET) < EPS) {
                    sprintf(buf, "%d%c(%d%c(%d%c%d)) = 24", ia, getOp(op1), ib, getOp(op2), ic, getOp(op3), id);
                    addSolution(buf);
                }
                
                // 结构5: (a op1 b) op2 (c op3 d)
                double r5 = calc(calc(a, b, op1), calc(c, d, op3), op2);
                if (fabs(r5 - TARGET) < EPS) {
                    sprintf(buf, "(%d%c%d)%c(%d%c%d) = 24", ia, getOp(op1), ib, getOp(op2), ic, getOp(op3), id);
                    addSolution(buf);
                }
            }
        }
    }
}
// 生成4个数的全排列
void permute(double arr[], int idx_arr[], int l, int r) {
    if (l == r) {
        findAllSolutions(arr, idx_arr);
        return;
    }
    for (int i = l; i <= r; i++) {
        // 交换
        double t1 = arr[l]; arr[l] = arr[i]; arr[i] = t1;
        int t2 = idx_arr[l]; idx_arr[l] = idx_arr[i]; idx_arr[i] = t2;
        
        permute(arr, idx_arr, l + 1, r);
        
        // 回溯
        t1 = arr[l]; arr[l] = arr[i]; arr[i] = t1;
        t2 = idx_arr[l]; idx_arr[l] = idx_arr[i]; idx_arr[i] = t2;
    }
}
// 求解24点，返回是否有解
int solve24(int nums[4]) {
    sol_count = 0;
    double arr[4];
    int idx_arr[4];
    for (int i = 0; i < 4; i++) {
        arr[i] = nums[i];
        idx_arr[i] = nums[i];
    }
    permute(arr, idx_arr, 0, 3);
    return sol_count > 0;
}

// 打印所有解法
void printSolutions() {
    printf("\n所有正确解法（共%d种）：\n", sol_count);
    for (int i = 0; i < sol_count; i++) {
        printf("  %s\n", solutions[i]);
    }
    printf("\n");
}

// 随机生成保证有解的4个数字
void generateValidNumbers(int nums[4], int min, int max) {
    do {
        for (int i = 0; i < 4; i++) {
            nums[i] = rand() % (max - min + 1) + min;
        }
    } while (!solve24(nums));
}

// 计算表达式的值（双栈算法处理带括号的四则运算）
double calculateExpr(char *expr) {
    double numStack[100];
    char opStack[100];
    int nTop = -1, oTop = -1;
    int i = 0;
    int len = strlen(expr);
    
    while (i < len) {
        if (expr[i] == ' ') { i++; continue; }
        
        if (isdigit(expr[i])) {
            double num = 0;
            while (i < len && isdigit(expr[i])) {
                num = num * 10 + (expr[i] - '0');
                i++;
            }
            numStack[++nTop] = num;
        } else if (expr[i] == '(') {
            opStack[++oTop] = expr[i];
            i++;
        } else if (expr[i] == ')') {
            while (oTop >= 0 && opStack[oTop] != '(') {
                double b = numStack[nTop--];
                double a = numStack[nTop--];
                char op = opStack[oTop--];
                double res;
                if (op == '+') res = a + b;
                else if (op == '-') res = a - b;
                else if (op == '*') res = a * b;
                else if (op == '/') {
                    if (fabs(b) < EPS) return -999999;
                    res = a / b;
                } else return -999999;
                numStack[++nTop] = res;
            }
            oTop--; // 弹出'('
            i++;
        } else { // 运算符
            int priority;
            if (expr[i] == '+' || expr[i] == '-') priority = 1;
            else if (expr[i] == '*' || expr[i] == '/') priority = 2;
            else return -999999;
            
            while (oTop >= 0 && opStack[oTop] != '(') {
                int opPri;
                if (opStack[oTop] == '+' || opStack[oTop] == '-') opPri = 1;
                else opPri = 2;
                if (opPri < priority) break;
                
                double b = numStack[nTop--];
                double a = numStack[nTop--];
                char op = opStack[oTop--];
                double res;
                if (op == '+') res = a + b;
                else if (op == '-') res = a - b;
                else if (op == '*') res = a * b;
                else if (op == '/') {
                    if (fabs(b) < EPS) return -999999;
                    res = a / b;
                }
                numStack[++nTop] = res;
            }
            opStack[++oTop] = expr[i];
            i++;
        }
    }
    
    while (oTop >= 0) {
        double b = numStack[nTop--];
        double a = numStack[nTop--];
        char op = opStack[oTop--];
        double res;
        if (op == '+') res = a + b;
        else if (op == '-') res = a - b;
        else if (op == '*') res = a * b;
        else if (op == '/') {
            if (fabs(b) < EPS) return -999999;
            res = a / b;
        } else return -999999;
        numStack[++nTop] = res;
    }
    
    return numStack[0];
}

// 检查用户输入是否使用了正确的数字（每个恰好一次）
int checkNumbers(char *expr, int nums[4]) {
    int used[4] = {0};
    int i = 0;
    int len = strlen(expr);
    
    while (i < len) {
        if (isdigit(expr[i])) {
            int num = 0;
            while (i < len && isdigit(expr[i])) {
                num = num * 10 + (expr[i] - '0');
                i++;
            }
            int found = 0;
            for (int j = 0; j < 4; j++) {
                if (!used[j] && nums[j] == num) {
                    used[j] = 1;
                    found = 1;
                    break;
                }
            }
            if (!found) return 0;
        } else {
            i++;
        }
    }
    
    for (int j = 0; j < 4; j++) {
        if (!used[j]) return 0;
    }
    return 1;
}

// ===================== 核心：带实时倒计时的输入函数 =====================
// 返回值：1=正常输入完成，0=超时
int timedInput(char *buf, int maxLen, int timeoutSec) {
    int pos = 0;
    time_t startTime = time(NULL);
    int lastRemain = -1;
    
    buf[0] = '\0';
    
    while (1) {
        time_t now = time(NULL);
        int elapsed = (int)difftime(now, startTime);
        int remain = timeoutSec - elapsed;
        
        // 时间到，超时
        if (remain <= 0) {
            printf("\r时间到！本题超时                        \n");
            buf[pos] = '\0';
            return 0;
        }
        
        // 每秒刷新一次倒计时显示
        if (remain != lastRemain) {
            lastRemain = remain;
            // \r 回到行首，覆盖显示
            printf("\r剩余时间：%2d秒  请输入算式：%s", remain, buf);
            fflush(stdout);
        }
        
        // 检查是否有按键按下（非阻塞）
        if (_kbhit()) {
            char ch = _getch();
            
            // 回车键：确认输入
            if (ch == '\r') {
                buf[pos] = '\0';
                printf("\n");
                return 1;
            }
            // 退格键：删除字符
            else if (ch == '\b' || ch == 127) {
                if (pos > 0) {
                    pos--;
                    buf[pos] = '\0';
                    // 退格删除显示
                    printf("\b \b");
                    fflush(stdout);
                }
            }
            // 普通可打印字符
            else if (pos < maxLen - 1 && isprint(ch)) {
                buf[pos++] = ch;
                buf[pos] = '\0';
                printf("%c", ch);
                fflush(stdout);
            }
        }
        
        // 短暂休眠，降低CPU占用
        Sleep(10);
    }
}

// ===================== 【新增黑暗模式函数】数字闪烁3秒后消失 =====================
void darkShowNumber(int nums[4])
{
    printf("\n===== 黑暗模式记忆阶段（3秒记忆时间）=====\n");
    int flash_time = 3000;
    int start = GetTickCount();
    while (GetTickCount() - start < flash_time)
    {
        // 显示数字
        printf("\r数字：%d %d %d %d        ", nums[0], nums[1], nums[2], nums[3]);
        fflush(stdout);
        Sleep(150);
        // 清空闪烁
        printf("\r                                    ");
        fflush(stdout);
        Sleep(150);
    }
    // 彻底清空，无数字可见
    printf("\r数字已隐藏，请凭记忆作答！\n");
}

// 闯关模式
void challengeMode() {
    int difficulty;
    int timeLimit;
    int minNum, maxNum;
    
    printf("\n===== 闯关模式 =====\n");
    printf("请选择难度：\n");
    printf("1. 简单 (数字1-10, 每题60秒)\n");
    printf("2. 中等 (数字1-13, 每题40秒)\n");
    printf("3. 困难 (数字1-13, 每题20秒)\n");
    printf("请输入选择：");
    scanf("%d", &difficulty);
    
    switch (difficulty) {
        case 1: minNum = 1; maxNum = 10; timeLimit = 60; break;
        case 2: minNum = 1; maxNum = 13; timeLimit = 40; break;
        case 3: minNum = 1; maxNum = 13; timeLimit = 20; break;
        default: 
            printf("无效选择，默认中等难度\n");
            minNum = 1; maxNum = 13; timeLimit = 40;
    }
    
    int score = 0;
    char input[100];
    
    for (int round = 1; round <= 10; round++) {
        int nums[4];
        generateValidNumbers(nums, minNum, maxNum);
        
        printf("\n--- 第 %d 题 (当前得分: %d) ---\n", round, score);
        // 黑暗模式分支
        if(dark_mode)
        {
            darkShowNumber(nums);
        }
        else
        {
            printf("四个数字：%d %d %d %d\n", nums[0], nums[1], nums[2], nums[3]);
        }
        // 调用带倒计时的输入
        int inputOk = timedInput(input, sizeof(input), timeLimit);
        
        if (!inputOk) {
            // 超时
            solve24(nums);
            printSolutions();
            continue;
        }
        
        if (strlen(input) == 0) {
            printf("输入为空，本题答错\n");
            solve24(nums);
            printSolutions();
            continue;
        }
        
        if (!checkNumbers(input, nums)) {
            printf("数字使用不正确！必须使用且仅使用给定的4个数字各一次\n");
            solve24(nums);
            printSolutions();
            continue;
        }
        
        double result = calculateExpr(input);
        if (fabs(result - 24.0) < EPS) {
            printf("bingo！+10分\n");
            score += 10;
        } else {
            printf("回答错误！计算结果 = %.2f\n", result);
            solve24(nums);
            printSolutions();
        }
    }
    
    printf("\n===== 闯关结束 =====\n");
    printf("最终得分：%d / 100\n", score);
    if (score == 100) printf("完美通关！你是24点大师！\n");
    else if (score >= 70) printf("表现不错，继续加油！\n");
    else if (score >= 40) printf("还需努力，多多练习！\n");
    else printf("小菜鸡建议多去自由模式练习一下\n");
}

// 自由练习模式
void freeMode() {
    while (1) {
        int choice;
        printf("\n===== 自由练习模式 =====\n");
        printf("1. 随机生成数字\n");
        printf("2. 自己输入数字\n");
        printf("请输入选择：");
        scanf("%d", &choice);
        int nums[4];
        if (choice == 1) {
            int minNum = 1, maxNum = 13;
            generateValidNumbers(nums, minNum, maxNum);
        } else if (choice == 2) {
            printf("\n请输入4个数字（空格分隔）：");
            for (int i = 0; i < 4; i++) {
                scanf("%d", &nums[i]);
            }
        } else {
            printf("无效选择\n");
            return;
        }
        char input[100];
        printf("请输入算式（输入 show 查看答案，输入 quit 退出）：");
        while (getchar() != '\n');
        // 黑暗模式处理
        if(dark_mode)
        {
            darkShowNumber(nums);
        }
        else
        {
            printf("\n生成的数字：%d %d %d %d\n", nums[0], nums[1], nums[2], nums[3]);
        }
        printf("> ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        
        if (strcmp(input, "quit") == 0 || strcmp(input, "exit") == 0) {
            break;
        }
        if (strcmp(input, "show") == 0 || strcmp(input, "答案") == 0) {
            solve24(nums);
            if (sol_count == 0) {
                printf("这组数字无法组成24点\n");
            } else {
                printSolutions();
            }
            continue;
        }
        if (strcmp(input, "换一组") == 0 || strcmp(input, "next") == 0) {
            generateValidNumbers(nums, 1, 13);
            printf("\n新的数字已生成\n");
            continue;
        }
        
        if (strlen(input) == 0) continue;
        
        if (!checkNumbers(input, nums)) {
            printf("数字使用不正确！必须使用且仅使用给定的4个数字各一次\n");
            continue;
        }
        
        double result = calculateExpr(input);
        if (fabs(result - 24.0) < EPS) {
            printf(" 回答正确！\n");
        } else {
            printf("回答错误！计算结果 = %.2f\n", result);
        }
    }
}

// ===================== 【新增好友PK模式函数】双人对战 =====================
void pkMode()
{
    int p1_score = 0, p2_score = 0;
    int round_total, time_limit;
    int min = 1, max = 13;
    int turn; // 1玩家1答题，2玩家2答题
    char input[100];
    printf("\n===== 好友双人PK模式 =====\n");
    printf("请输入总回合数：");
    scanf("%d", &round_total);
    printf("每题作答时限(秒)：");
    scanf("%d", &time_limit);
    printf("计分规则：答对自己+10分，答错/超时对手+10分\n");
    Sleep(1000);

    for(int r = 1; r <= round_total; r++)
    {
        int nums[4];
        generateValidNumbers(nums, min, max);
        printf("\n---------- 第%d回合 ----------\n", r);
        printf("当前比分 玩家1:%d 分 | 玩家2:%d 分\n", p1_score, p2_score);

        // 黑暗模式显示逻辑
        if(dark_mode)
            darkShowNumber(nums);
        else
            printf("本轮数字：%d %d %d %d\n", nums[0], nums[1], nums[2], nums[3]);
        
        printf("请输入当前答题玩家(1/2)：");
        scanf("%d", &turn);
        while(getchar() != '\n'); // 清空缓冲区

        int ok = timedInput(input, sizeof(input), time_limit);
        int right = 0;
        // 判断作答结果
        if(ok && strlen(input) > 0 && checkNumbers(input, nums))
        {
            double res = calculateExpr(input);
            if(fabs(res - 24) < EPS)
                right = 1;
        }
        // 计分
        if(right)
        {
            printf("回答正确！答题玩家+10\n");
            if(turn == 1) p1_score += 10;
            else p2_score += 10;
        }
        else
        {
            printf("答错/超时！对手+10\n");
            if(turn == 1) p2_score += 10;
            else p1_score += 10;
            solve24(nums);
            printSolutions();
        }
        Sleep(1500);
    }
    // PK结束结算
    printf("\n===== PK对战结束 =====\n");
    printf("最终比分：玩家1 %d : %d 玩家2\n", p1_score, p2_score);
    if(p1_score > p2_score)
        printf("玩家1获得胜利！\n");
    else if(p2_score > p1_score)
        printf("玩家2获得胜利！\n");
    else
        printf("双方平局！\n");
}


// 主菜单
void mainMenu() {
    int choice;
    while (1) {
        printf("\n");
        printf("==============================\n");
        printf("   24 点小游戏(究极无敌至臻版)\n");
        printf("==============================\n");
        printf("1. 闯关模式 \n");
        printf("2. 自由练习模式\n");
        printf("3. 好友双人PK模式【新增】\n");
        printf("4. 切换黑暗记忆模式(当前：%s)\n", dark_mode ? "开启" : "关闭");
        printf("5. 退出游戏\n");
        printf("==============================\n");
        printf("请输入选择：");
        
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                challengeMode();
                break;
            case 2:
                freeMode();
                break;
            case 3:
                pkMode();
                break;
            case 4:
                dark_mode = !dark_mode;
                printf("黑暗模式已%s\n", dark_mode ? "开启" : "关闭");
                break;
            case 5:
                printf("感谢游玩，再见！\n");
                return;
            default:
                printf("无效选择，请重新输入\n");
        }
    }
}

int main() {
    srand((unsigned)time(NULL));
    mainMenu();
    return 0;
}