#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <time.h>
#include <windows.h>

#define ISLEGAL(a, b) \
    if (a > b)        \
    printf("随机事件发生时间不合法！\n")

#define random(x) rand() % x

int TIME = 0;

enum State
{
    ready,
    running,
    block,
    finish
};

enum Algorithm
{
    FCFS,
    SJF,
    PSA,
    HRRN,
    RR,
    MFQ
};

class PCB
{
public:
    //static int num;
    PCB(int pid = -1, int all_time = -1, int prioity = -1, int random = -1);
    float GetResponseRatio();
    void ChangeState(State state);
    //阻塞时，返回false，其余返回true
    bool Run(int cpu_slice = 1);
    bool IsFinish();
    int GetPrioity();
    int GetPid();
    //void SetLastTime();
    int enter_time;
    int last_time;
    int total_time;  //共需要占用时间
    int cpu_time;    //剩余占用时间
    int random_time; //随机事件发生的时间
    int finish_time;
    PCB *next_PCB;

private:
    int pid_;
    int prioity_; //优先级
    State state_; //进程状态
    //std::string name_;
};

PCB::PCB(int pid, int all_time, int prioity, int random) : pid_(pid),
                                                           prioity_(prioity_),
                                                           total_time(all_time),
                                                           random_time(random),
                                                           next_PCB(NULL),
                                                           last_time(-1),
                                                           cpu_time(all_time),
                                                           enter_time(TIME),
                                                           state_(ready)
{
}

float PCB::GetResponseRatio()
{
    return 1.0 * (TIME - last_time - 1 + cpu_time) / cpu_time;
}

inline void PCB::ChangeState(State state)
{
    state_ = state;
}

inline bool PCB::Run(int cpu_slice)
{
    printf("运行\n");
    while (cpu_slice--)
    {
        TIME++;
        cpu_time--;
        random_time--;
        if (!cpu_time || !random_time)
            break;
    }
    last_time = TIME;
    printf("运行完毕\n");
    if (!cpu_time)
    {
        finish_time = TIME;
        return true;
    }
    if (!random_time)
        return false;
    return true;
}

inline bool PCB::IsFinish()
{
    return cpu_time == 0;
}

inline int PCB::GetPrioity()
{
    return prioity_;
}

inline int PCB::GetPid()
{
    return pid_;
}

class Queue
{
public:
    Queue(PCB *head = NULL);
    virtual ~Queue();
    virtual void Push(PCB *pcb);
    virtual void Pop();
    virtual PCB *Front();
    virtual bool IsEmpty();

protected:
    PCB *head_;
    PCB *tail_;
    std::mutex queue_mutex_;
};

Queue::Queue(PCB *head) : head_(head)
{
    if (!head)
    {
        head_ = new PCB();
        tail_ = head_;
    }
}

Queue::~Queue()
{
    tail_ = NULL;
    delete head_;
}

void Queue::Push(PCB *pcb)
{
    pcb->next_PCB = NULL;
    std::unique_lock<std::mutex> lock(queue_mutex_);
    // tail_->next_PCB = pcb;
    // tail_ = pcb;
    PCB *pguard = head_;
    while (pguard->next_PCB != NULL)
        pguard = pguard->next_PCB;
    pguard->next_PCB = pcb;
}

void Queue::Pop()
{
    std::unique_lock<std::mutex> lock(queue_mutex_);
    head_->next_PCB = head_->next_PCB->next_PCB;
}

PCB *Queue::Front()
{
    std::unique_lock<std::mutex> lock(queue_mutex_);
    PCB *temp_head = head_->next_PCB;

    lock.unlock();
    Pop();
    temp_head->next_PCB = NULL;
    return temp_head;
}

bool Queue::IsEmpty()
{
    std::unique_lock<std::mutex> lock(queue_mutex_);
    return head_->next_PCB == NULL;
}

//push时确定顺序
class SJFQueue : public Queue
{
public:
    virtual void Push(PCB *pcb);
};

void SJFQueue::Push(PCB *pcb)
{
    pcb->next_PCB = NULL;
    std::unique_lock<std::mutex> lock(queue_mutex_);
    PCB *ptemp = head_;
    while (ptemp->next_PCB != NULL)
    {
        if (ptemp->next_PCB->cpu_time > pcb->cpu_time)
            break;
        ptemp = ptemp->next_PCB;
    }
    pcb->next_PCB = ptemp->next_PCB;
    ptemp->next_PCB = pcb;
}

//push时确定顺序
class PSAQueue : public Queue
{
public:
    virtual void Push(PCB *pcb);
};

void PSAQueue::Push(PCB *pcb)
{
    pcb->next_PCB = NULL;
    std::unique_lock<std::mutex> lock(queue_mutex_);
    PCB *ptemp = head_;
    while (ptemp->next_PCB != NULL)
    {
        if (ptemp->next_PCB->GetPrioity() > pcb->GetPrioity())
            break;
        ptemp = ptemp->next_PCB;
    }
    pcb->next_PCB = ptemp->next_PCB;
    ptemp->next_PCB = pcb;
}

//在提取头部时确定顺序
class HRRNQueue : public Queue
{
public:
    virtual PCB *Front();
};

PCB *HRRNQueue::Front()
{

    printf("读取头\n");
    float max_response_ratio = 0;
    std::unique_lock<std::mutex> lock(queue_mutex_);
    PCB *ptemp = head_->next_PCB, *pp = head_;
    PCB *temp_front, *front_front;
    //找到相应比最高的结点以及其前驱
    while (ptemp != NULL)
    {
        if (ptemp->GetResponseRatio() > max_response_ratio)
        {
            max_response_ratio = ptemp->GetResponseRatio();
            temp_front = ptemp;
            front_front = pp;
        }
        pp = ptemp;
        ptemp = ptemp->next_PCB;
    }
    //删除改结点
    front_front->next_PCB = temp_front->next_PCB;
    temp_front->next_PCB = NULL;

    return temp_front;
}

//基类queue，其它queue继承该queue，以满足不同的插入方法。
//然后osoperator中，只有一堆函数
class OSOperator
{
public:
    // BlockingQueue b_queue;
    // ReadyQueue r_queue;
    OSOperator(Algorithm alg);
    ~OSOperator();
    //void Main();
    //void TimeRun();
    void PrintInfo();
    void InitProcess(int num);
    void HRRNAlgorithm();
    void FCFSAlgorithm();
    void ProcessProducer();
    void MFQAlgorithm();
    void End();

private:
    bool end_;
    std::thread run_;
    std::thread process_producer_;
    Queue *ready_queue_;
    Queue *blocking_queue_;
    Queue *finish_queue_;
    int initi_; //进程
};

OSOperator::OSOperator(Algorithm alg) : end_(false), initi_(1000)
{
    finish_queue_ = new Queue();
    blocking_queue_ = new Queue();
    switch (alg)
    {
    case HRRN:
    {
        ready_queue_ = new HRRNQueue();
        InitProcess(1);
        run_ = std::thread(&OSOperator::HRRNAlgorithm, this);
        //run_.join();
        break;
    }
    case FCFS:
    {
        ready_queue_ = new Queue();
        InitProcess(5);
        run_ = std::thread(&OSOperator::FCFSAlgorithm, this);
        //run_.join();
        break;
    }
    case SJF:
    {
        ready_queue_ = new SJFQueue();
        InitProcess(5);
        run_ = std::thread(&OSOperator::FCFSAlgorithm, this);
        break;
    }
    case RR:
    {
        ready_queue_ = new Queue();
        InitProcess(5);
        run_ = std::thread(&OSOperator::HRRNAlgorithm, this);
        break;
    }
    case PSA:
    {
        ready_queue_ = new PSAQueue();
        InitProcess(5);
        run_ = std::thread(&OSOperator::FCFSAlgorithm, this);
        break;
    }

    case MFQ:
    {
        ready_queue_ = new Queue();
        InitProcess(5);
        run_ = std::thread(&OSOperator::MFQAlgorithm, this);
        break;
    }
    default:
    {
        ready_queue_ = new Queue();
        break;
    }
    }
    process_producer_ = std::thread(&OSOperator::ProcessProducer, this);
}

OSOperator::~OSOperator()
{
    End();
    if (process_producer_.joinable())
        process_producer_.join();
    if (run_.joinable())
        run_.join();
}

void OSOperator::HRRNAlgorithm()
{
    printf("开始算法\n");
    PCB *ready_process = NULL;
    int K = 5;
    while (1)
    {
        if (K <= 0)
        {
            if (!blocking_queue_->IsEmpty())
            {
                PCB *tempf = blocking_queue_->Front();
                ready_queue_->Push(tempf);
                //ready_queue_->Push(blocking_queue_->Front());
            }
            K = 5;
        }
        if (!ready_queue_->IsEmpty())
        {
            ready_process = ready_queue_->Front();
            if (!ready_process->Run(1))
            {
                printf("运行1个\n");
                K -= 1;
                blocking_queue_->Push(ready_process);
            }
            else if (ready_process->IsFinish())
            {
                finish_queue_->Push(ready_process);
            }
            else
            {
                ready_queue_->Push(ready_process);
            }
        }
        else if (ready_queue_->IsEmpty() && blocking_queue_->IsEmpty())
            break;
        else
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            K--;
            TIME++;
        }
    }
}

void OSOperator::FCFSAlgorithm()
{
    PCB *ready_process = NULL;
    int K = 5;
    while (1)
    {
        printf("执行算法\n");
        if (K <= 0)
        {
            if (!blocking_queue_->IsEmpty())
            {
                printf("阻塞队列头部出队\n");
                PCB *tempf = blocking_queue_->Front();
                ready_queue_->Push(tempf);
                //ready_queue_->Push(blocking_queue_->Front());
            }
            K = 5;
        }
        if (!ready_queue_->IsEmpty())
        {
            ready_process = ready_queue_->Front();
            if (!ready_process->Run(ready_process->cpu_time))
            {
                printf("进入阻塞队列\n");
                K -= ready_process->cpu_time;
                blocking_queue_->Push(ready_process);
            }
            else if (ready_process->IsFinish())
            {
                printf("进入完成队列\n");
                finish_queue_->Push(ready_process);
            }
            else
            {
                printf("进入就绪队列\n");
                ready_queue_->Push(ready_process);
            }
        }
        else if (ready_queue_->IsEmpty() && blocking_queue_->IsEmpty())
            break;
        else
        {
            printf("空转\n");
            std::this_thread::sleep_for(std::chrono::seconds(1));
            K--;
            TIME++;
        }
    }
}

void OSOperator::MFQAlgorithm()
{
    Queue *ready_queue_sec = new Queue();
    Queue *ready_queue_th = new Queue();
    printf("开始算法\n");
    PCB *ready_process = NULL;
    int K = 5;
    while (1)
    {
        if (K <= 0)
        {
            if (!blocking_queue_->IsEmpty())
            {
                PCB *tempf = blocking_queue_->Front();
                ready_queue_->Push(tempf);
                //ready_queue_->Push(blocking_queue_->Front());
            }
            K = 5;
        }
        if (!ready_queue_->IsEmpty())
        {
            ready_process = ready_queue_->Front();
            if (!ready_process->Run(1))
            {
                printf("运行1个\n");
                K -= 1;
                blocking_queue_->Push(ready_process);
            }
            else if (ready_process->IsFinish())
            {
                printf("进入完成队列\n");
                finish_queue_->Push(ready_process);
            }
            else
            {
                ready_queue_sec->Push(ready_process);
            }
        }
        //第二队列
        else if (!ready_queue_sec->IsEmpty())
        {
            ready_process = ready_queue_sec->Front();
            int ki = 3;
            while ((!ready_queue_->IsEmpty()) && --ki)
            {
                if (!ready_process->Run(1))
                {
                    K -= 1;
                    blocking_queue_->Push(ready_process);
                }
                else if (ready_process->IsFinish())
                {
                    finish_queue_->Push(ready_process);
                    break;
                }
                // else
                // {
                //     ready_queue_th->Push(ready_process);
                // }
            }
            if (ki == 0)
                ready_queue_th->Push(ready_process);
        }

        //第三
        else if (!ready_queue_th->IsEmpty())
        {
            ready_process = ready_queue_th->Front();
            int ki = 5;
            while ((!ready_queue_->IsEmpty()) && (!ready_queue_sec->IsEmpty()) && ki--)
            {
                if (!ready_process->Run(1))
                {
                    K -= 1;
                    blocking_queue_->Push(ready_process);
                }
                else if (ready_process->IsFinish())
                {
                    finish_queue_->Push(ready_process);
                    break;
                }
                // else
                // {
                //     ready_queue_th->Push(ready_process);
                // }
            }
            if (ki == 0)
                ready_queue_th->Push(ready_process);
        }
        else if (ready_queue_->IsEmpty() && ready_queue_sec->IsEmpty() && ready_queue_th->IsEmpty() && blocking_queue_->IsEmpty())
            break;
        else
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            K--;
            TIME++;
        }
    }
}

void OSOperator::InitProcess(int num)
{
    srand(time(NULL));
    int pid, all_time, priority, random_time;
    PCB *temp_process;
    while (num--)
    {
        printf("生产第%d个\n", num);
        pid = ++initi_;
        all_time = random(10) + 1;
        priority = random(4) + 1;
        // if (pid % 5 == 0)
        // {
        //     //random_time = random(all_time + 1) - 1;
        //     //random_time = (rand() % (all_time + 1)) - 1;
        //     random_time = random(all_time);
        // }
        random_time = random(all_time * 10);
        temp_process = new PCB(pid, all_time, priority, random_time);
        printf("%d %d %d",all_time, priority, random_time);
        ready_queue_->Push(temp_process);
    }
}

void OSOperator::ProcessProducer()
{
    while (!end_)
    {
        //printf("正在生产\n");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        srand(time(NULL));
        int pid, all_time, priority, random_time;
        PCB *temp_process;
        pid = ++initi_;
        all_time = random(10);
        priority = random(4) + 1;
        if (pid % 5 == 0)
        {
            random_time = random(all_time + 1) - 1;
        }
        temp_process = new PCB(pid, all_time, priority, random_time);
        ready_queue_->Push(temp_process);
    }
}

void OSOperator::End()
{
    end_ = true;
    if (process_producer_.joinable())
        process_producer_.join();
    if (run_.joinable())
        run_.join();
    PrintInfo();
}

void OSOperator::PrintInfo()
{
    PCB *pcb;
    printf("进程号\t到达时间\t运转时间\t结束时间\n");
    while (!finish_queue_->IsEmpty())
    {
        pcb = finish_queue_->Front();
        printf("%d\t%d\t%d\t%d\n", pcb->GetPid(), pcb->enter_time, pcb->total_time, pcb->finish_time);
        delete pcb;
    }
}

int main()
{
    //OSOperator OS(FCFS);
    //OSOperator OS(MFQ);   //*
    OSOperator OS(HRRN); //*
    //OSOperator OS(PSA); //*
    //OSOperator OS(RR);
    //OSOperator OS(SJF);
    std::this_thread::sleep_for(std::chrono::seconds(10));
    OS.End();
    // temp tt = temp();
    // std::thread th(&temp::tempMain, &tt);
    // th.join();
    // Sleep(1000);
    // //std::_sleep(2 * 1000);
    // tt.print();
    // ISLEGAL(3, 2);
    system("pause");
    return 0;
}

/*
1. 读取信息、选择算法
2. OS构造时，先生成若干进程，加入就绪队列，新起线程，开始运行
   再建线程，随机生成进程，加入就绪队列
3. 主进程中若结束，调用析构os析构函数，os两个进程，加入主进程

 */