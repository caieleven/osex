#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
using namespace std;
/*
Process类，存储指令流地址，是否存储页号
每个算法共享内容：
    1. 指令地址流
    2. 页面大小，实存空间（物理块数）
 */

#define random(x) rand() % x

struct Process
{
    /*
     * @param instruct_size_p 指令个数
     * @param v_size_p 虚存大小（程序大小） 2 ^ v_size_p
     */
    Process(int instruct_size_p = 5000, int v_size_p = 2 ^ 15);

    void GenePage(int page_size);
    int instruct_size;
    int v_size;
    int *addr;
    int *pageno;
};

Process::Process(int instruct_size_p, int v_size_p) : instruct_size(instruct_size_p), v_size(v_size_p)
{

    int vir_size = (int)pow(2, v_size);
    srand((unsigned)time(NULL));
    addr = new int[instruct_size_p];

    cout << "v_size=" << v_size << endl;
    cout << "vvv = " << vir_size << endl;
    addr[0] = random(vir_size);
    for (int i = 1; i < instruct_size; ++i)
    {
        addr[i] = addr[i - 1] + 1;
        ++i;
        addr[i] = addr[i - 1] + 1;
        ++i;
        addr[i] = random(addr[i - 1]);
        ++i;
        int temp = random(vir_size) + addr[i - 1];
        addr[i] = temp >= vir_size ? vir_size : temp;
    }
    for (int i = 0; i < 10; ++i)
    {
        cout << addr[i] << " ";
    }
    cout << endl;
}

void Process::GenePage(int page_size)
{
    pageno = new int[instruct_size];
    //这里页号从0开始
    for (int i = 0; i < instruct_size; ++i)
        pageno[i] = addr[i] >> page_size;
    for (int i = 0; i < 50; i++)
        printf("%d ", pageno[i]);
    printf("\n");
}

enum Alg
{
    OPT, //最佳配置
    FIFO,
    LRU, //least recently used
    LFU, //least frequently used
    NRU  //not recently used 简单clock置换
};

/*
 * @brief 对外接口
 */
class OSOperator
{
public:
    OSOperator(int inst_size, int v_size);
    void Main();

private:
    Process *process_;
    int page_size_; //页面大小
    int p_num_;     //物理块数
    float OPTFunc();
    float FIFOFunc();
    float LRUFunc();
    float LFUFunc();
    float NRUFunc();
};

OSOperator::OSOperator(int inst_size, int v_size)
{
    process_ = new Process(inst_size, v_size);
    page_size_ = 2 ^ 10;
    p_num_ = 2;
}

void OSOperator::Main()
{
    float rate_FIFO, rate_LRU, rate_LFU, rate_NRU;
    for (int ps = 10; ps < 15; ++ps)
    {
        page_size_ = ps;
        printf("页面大小为 2^%d:\n", ps);
        process_->GenePage(page_size_);
        printf("物理块数\tFIFO\tLRU\n");
        for (int pn = 4; pn <= 32; pn += 2)
        {
            p_num_ = pn;
            printf("%d\t%f\t%f\n", pn, 1-FIFOFunc(), 1-LRUFunc());
        }
        printf("\n");
    }
}

float OSOperator::LRUFunc()
{
    int *table = new int[p_num_];
    int *page;
    int page_index = 0, count = 0, contain = 0; //索引，缺页中断数， 有数据的物理块数
    while (page_index < process_->instruct_size)
    {
        int k = 0;
        while (k < contain)
        {
            if (process_->pageno[page_index] == table[k])
            {
                //k后面的前移动
                for (int i = k; i < contain - 1; ++i)
                {
                    table[i] = table[i + 1];
                }
                table[contain - 1] = process_->pageno[page_index];
                k = 0;
                page_index++;
            }
            else
            {
                k++;
            }
        }
        //置换
        count++;
        if (contain < p_num_)
        {
            table[contain] = process_->pageno[page_index];
            contain++;
        }
        else
        {
            for (int i = 0; i < p_num_ - 1; ++i)
            {
                table[i] = table[i + 1];
            }
            table[p_num_ - 1] = process_->pageno[page_index];
        }
        page_index++;
    }
    return count * 1.0 / process_->instruct_size;
}

float OSOperator::FIFOFunc()
{
    int *table = new int[p_num_];
    int page_index = 0, count = 0, contain = 0; //索引，缺页中断数， 有数据的物理块数
    while (page_index < process_->instruct_size)
    {
        int k = 0;
        while (k < contain)
        {
            int temppageno = process_->pageno[page_index];
            if (process_->pageno[page_index] == table[k])
            {
                k = 0;
                page_index++;
            }
            else
            {
                k++;
            }
        }
        //置换
        count++;
        if (contain < p_num_)
        {
            table[contain] = process_->pageno[page_index];
            //int temppageno = table[contain];
            contain++;
            // for (int i = 0; i < contain; ++i)
            //     cout << table[i] << "    ";
            // cout << endl;
            //printf("未满，%d进入\n", table[contain - 1]);
        }
        else
        {
            //整体前移
            for (int i = 0; i < p_num_ - 1; ++i)
            {
                table[i] = table[i + 1];
            }
            //int temp = table[p_num_ - 1];
            table[p_num_ - 1] = process_->pageno[page_index];
            // for (int i = 0; i < contain; ++i)
            //     cout << table[i] << "    ";
            // cout << endl;
            //printf("%d出，%d入\n", temp, table[p_num_ - 1]);
        }
        page_index++;
    }
    return count * 1.0 / process_->instruct_size;
}

float OSOperator::NRUFunc()
{
    int *table = new int[p_num_];
    bool *access = new bool[p_num_];
    memset(access, false, sizeof(access));
    int page_index = 0, contain = 0, count = 0, delta = 0;
    while(page_index < process_->instruct_size)
    {
        int k = 0;
        while(contain)
        {
            if(process_->pageno[page_index] == table[k])
            {
                access[k] = 1;
                page_index++;
                delta = 0;
            }
            else
            {
                access[k] = 0;
                k = (k + 1) % contain;
                delta++;
            }
            if(delta == contain - 1)
                break;
        }
        count++;
        if(contain < p_num_)
        {
            table[contain - 1] = process_->pageno[page_index];
            access[contain - 1] = 1;
            contain++;
            delta = 0;
        }
        else
        {
            
            
        }
    }
}

int main()
{
    OSOperator OS(5000, 15);
    OS.Main();
    system("pause");
    return 0;
}
