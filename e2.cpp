#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <utility>
#include <vector>

#define random(x) rand() % x

class Vector
{
public:
    Vector(const Vector &vec);
    Vector(int n = 0, int num = 0);
    Vector &operator=(const Vector &vec);
    const Vector &operator+=(const Vector &right);
    const Vector &operator-=(const Vector &right);
    friend Vector operator+(const Vector &left, const Vector &right);
    friend Vector operator-(const Vector &left, const Vector &right);
    bool operator<=(const Vector &right) const;
    bool operator<=(const int &right) const;
    //     bool operator<(const Vector &right) const;
    // bool operator<(const int &right) const;
    bool operator>=(const Vector &right) const;
    bool operator>=(const int &right) const;
    bool operator==(const Vector &right) const;
    bool operator==(const int &right) const;
    int operator[](int i) const;
    int &operator[](int i);

    std::vector<int> resource;

    void push_back(const int v);
};

Vector::Vector(int n, int num)
{
    for (int i = 0; i < n; ++i)
        resource.push_back(num);
}

Vector::Vector(const Vector &vec)
{
    //this->resource = vec.resource;
    this->resource.clear();
    for (unsigned int i = 0; i < vec.resource.size(); ++i)
        this->resource.push_back(vec[i]);
}

Vector &Vector::operator=(const Vector &vec)
{
    this->resource = vec.resource;
    return (*this);
}

const Vector &Vector::operator+=(const Vector &right)
{
    //std::vector<int> temp(this->resource.size(), 0);
    //Vector temp;
    for (unsigned int i = 0; i < this->resource.size(); ++i)
        //(*this)[i] = (*this)[i] + right[i];
        this->resource[i] += right.resource[i];
    return *this;
}

const Vector &Vector::operator-=(const Vector &right)
{
    //Vector temp;
    for (unsigned int i = 0; i < this->resource.size(); ++i)
        this->resource[i] -= right.resource[i];
    return *this;
}

Vector operator+(const Vector &left, const Vector &right)
{
    Vector temp;
    for (unsigned int i = 0; i < left.resource.size(); ++i)
    {
        temp.resource.push_back(left.resource[i] + right.resource[i]);
    }
    return temp;
}
Vector operator-(const Vector &left, const Vector &right)
{
    Vector temp;
    for (unsigned int i = 0; i < left.resource.size(); ++i)
    {
        temp.resource.push_back(left.resource[i] - right.resource[i]);
    }
    return temp;
}

bool Vector::operator<=(const Vector &right) const
{
    for (unsigned int i = 0; i < this->resource.size(); ++i)
    {
        if ((*this)[i] > right[i])
            return false;
    }
    return true;
}

bool Vector::operator<=(const int &right) const
{
    for (unsigned int i = 0; i < this->resource.size(); ++i)
        if ((*this)[i] > right)
            return false;
    return true;
}

bool Vector::operator>=(const Vector &right) const
{
    //return right <= (*this);
    for (unsigned int i = 0; i < this->resource.size(); ++i)
        if ((*this)[i] < right[i])
            return false;
    return true;
}

bool Vector::operator>=(const int &right) const
{
    //return right <= (*this);
    for (unsigned int i = 0; i < this->resource.size(); ++i)
        if ((*this)[i] < right)
            return false;
    return true;
}

bool Vector::operator==(const Vector &right) const
{
    for (unsigned int i = 0; i < this->resource.size(); ++i)
    {
        if ((*this)[i] != right[i])
            return false;
    }
    return true;
}

bool Vector::operator==(const int &right) const
{
    for (unsigned int i = 0; i < this->resource.size(); ++i)
    {
        if ((*this)[i] != right)
            return false;
    }
    return true;
}

int Vector::operator[](int i) const
{
    int v = this->resource[i];
    return v;
}

int &Vector::operator[](int i)
{
    return this->resource[i];
}

void Vector::push_back(const int v)
{
    this->resource.push_back(v);
}

enum class LogType : int
{
    request_gt_need = 1,
    request_not_safe = 2,
    request_gt_available = 3,
    all_request_bad = 9,
    finish = 4,
    request_success = 5,
    wait = 6,
    request_info = 7,
    system_info = 8
};

class Log
{
public:
    // static void WriteError(LogType info, int process_id)
    // {
    //     switch (info)
    //     {
    //     case LogType::request_gt_need:
    //     {
    //         printf("[Error] Process %d ???????????????????????????????????????\n", process_id);
    //         break;
    //     }
    //     case LogType::request_not_safe:
    //     {
    //         printf("[Error] Process %d ?????????????????????????????????????????????\n", process_id);
    //         break;
    //     }
    //     case LogType::request_gt_available:
    //     {
    //         printf("[Error] Process %d ????????????????????????????????????\n", process_id);
    //     }
    //     }
    // }
    static void WriteError(LogType info)
    {
        switch (info)
        {
        case LogType::all_request_bad:
        {
            printf("[Error] ????????????????????????????????????\n");
            break;
        }
        }
    }
    static void WriteInfo(LogType info, int process_id)
    {
        switch (info)
        {
        case LogType::request_success:
        {
            printf("[Info] Process %d ?????????????????????:\n", process_id);
            break;
        }
        case LogType::finish:
        {
            printf("[Info] Process %d ????????????!\n", process_id);
            break;
        }
        case LogType::wait:
        {
            printf("[Info] process %d ??????\n", process_id);
            break;
        }
        case LogType::request_gt_need:
        {
            printf("[Error] Process %d ???????????????????????????????????????\n", process_id);
            break;
        }
        case LogType::request_not_safe:
        {
            printf("[Error] Process %d ?????????????????????????????????????????????\n", process_id);
            break;
        }
        case LogType::request_gt_available:
        {
            printf("[Error] Process %d ????????????????????????????????????\n", process_id);
        }
        }
    }
    static void WriteInfo(LogType info, int process_id, Vector &req)
    {
        switch (info)
        {
        case LogType::request_info:
        {
            printf("[Info] Process %d ????????????:\t", process_id);
            break;
        }
        case LogType::request_success:
        {
            printf("[Info] Process %d ??????????????????:\t", process_id);
            break;
        }
        }
        for (unsigned int i = 0; i < req.resource.size(); ++i)
            printf("%d\t", req[i]);
        printf("\n");
    }
    static void WriteInfo(LogType info, Vector &avai)
    {
        switch (info)
        {
        case LogType::system_info:
        {
            printf("[Info] ????????????????????????:\n");
            for (unsigned int i = 0; i < avai.resource.size(); ++i)
                printf("%d\t", avai[i]);
            printf("\n");
            break;
        }
        }
    }
};

typedef std::pair<int, Vector> Req;

enum Algorithm
{
    BANKER,
    LINEAR_ORDERING
};

class Process
{
public:
    /*
     * @param n ???????????????
     * @param num ??????????????????
     */
    Process(int n, int num);
    Process(const Process &p);

    Process &operator=(const Process &p);
    /*
     * @return ???????????????0???????????????
     */
    Vector Request();
    Vector GetNeed();
    /*
     * @brief ?????????????????????request
     */
    void PreRun(Vector &alloc);
    /*
     *@brief PreRun()?????????????????????PreRun???????????????
     */
    void Reverse(Vector &alloc);

    /*
     * @brief need???allocation?????????????????????????????????OSOperator???RoolBack??????
     */
    void RoolBack();

    Vector FreeResource();

    /*
     * @brief allocation_???need??????
     * @param alloc ??????????????????
     */
    void Run(Vector &alloc);
    bool IsFinish();
    void Print();

private:
    int resource_kn_;
    Vector max_;
    Vector need_;
    Vector allocation_;
    Vector request_;
};

Process::Process(int n, int num) : resource_kn_(n)
{

    for (int i = 0; i < n; ++i)
    {
        max_.push_back(random(num));
        allocation_.push_back(0);
        int k = max_[i] + 1;
        request_.push_back(random(k));
    }
    //printf("%d", request_.resource.size());
    need_ = max_;
}

void Process::Print()
{
    printf("MAX: ");
    for (int i = 0; i < resource_kn_; ++i)
    {
        printf("%d\t", max_[i]);
    }
    printf("ALLOCATION: ");
    for (int i = 0; i < resource_kn_; ++i)
    {
        printf("%d\t", allocation_[i]);
    }
    printf("NEED: ");
    for (int i = 0; i < resource_kn_; ++i)
    {
        printf("%d\t", need_[i]);
    }
    printf("\n");
}

Process::Process(const Process &p)
{
    this->resource_kn_ = p.resource_kn_;
    this->max_ = p.max_;
    this->need_ = p.need_;
    this->allocation_ = p.allocation_;
    this->request_ = p.request_;
}

Process &Process::operator=(const Process &p)
{
    this->resource_kn_ = p.resource_kn_;
    this->max_ = p.max_;
    this->need_ = p.need_;
    this->allocation_ = p.allocation_;
    this->request_ = p.request_;
    return *this;
}

Vector Process::Request()
{
    return request_;
}

Vector Process::GetNeed()
{
    return need_;
}

void Process::PreRun(Vector &alloc)
{
    allocation_ += alloc;
    need_ -= alloc;
}

void Process::Reverse(Vector &alloc)
{
    allocation_ -= alloc;
    need_ += alloc;
}

void Process::RoolBack()
{
    allocation_ = Vector(need_.resource.size(), 0);
    need_ = max_;
}

Vector Process::FreeResource()
{
    return max_;
}

void Process::Run(Vector &alloc)
{
    allocation_ += alloc;
    need_ -= alloc;
    do
    {
        request_.resource.clear();
        for (int i = 0; i < resource_kn_; ++i)
        {
            //request_[i] = random(need_[i] + 1);
            int kk = need_[i] + 1;
            int k = random(kk);
            request_.resource.push_back(k);
        }
    } while (!(need_ == 0) && request_ == 0);
}

bool Process::IsFinish()
{
    //return allocation_ == max_;
    return need_ == 0;
}

class OSOperator
{
public:
    OSOperator(Algorithm alg = BANKER);
    /*
     * @param option ??????1???????????????
     */
    void Run(int option);
    void SetAlgorithm(Algorithm alg);
    bool Safe(int pid);
    void RollBack();
    void End();
    void PrintCurentState();

private:
    std::queue<Req> request_queue_;
    std::vector<Process> processes_;
    Algorithm alg_;

    std::queue<int> finisted_queue_;
    std::queue<int> running_queue_;
    Vector available_;
    Vector available_duplicate_;
    bool end_;
};

OSOperator::OSOperator(Algorithm alg) : end_(false), alg_(alg)
{
    available_ = Vector(3, 8);
    available_duplicate_ = available_;
    for (int i = 0; i < 5; ++i)
    {
        Process temp(3, 5);
        running_queue_.push(i);
        processes_.push_back(temp);
    }
}

void OSOperator::PrintCurentState()
{
    printf("??????????????????:\n");
    printf("AVAILABLE: ");
    for (unsigned int i = 0; i < available_.resource.size(); ++i)
        printf("%d\t", available_[i]);
    printf("\n");
    for (unsigned int i = 0; i < processes_.size(); ++i)
    {
        printf("Process %d: ", i);
        processes_[i].Print();
    }
}

inline void OSOperator::End()
{
    end_ = true;
}

void OSOperator::SetAlgorithm(Algorithm alg)
{
    alg_ = alg;
}

void OSOperator::Run(int option)
{
    printf("<====================================????????????????????????===================================>\n");
    //PrintCurentState();
    Vector request;
    //???????????????????????????????????????
    int active_num = running_queue_.size();
    while (!end_)
    {
        printf("<======================================??????????????????=====================================>\n");
        if (running_queue_.empty())
        {
            End();
            printf("????????????????????????!\n");
            break;
        }
        //?????????????????????
        int pid = running_queue_.front();
        running_queue_.pop();
        //??????????????????????????????
        request = processes_[pid].Request();
        //???????????????
        request_queue_.push(Req(pid, request));
        Log::WriteInfo(LogType::request_info, pid, request);
        available_ -= request;
        //?????????????????????
        if (available_ >= 0)
        {
            //?????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
            if (option)
            {
                //PrintCurentState();
                //??????????????????process???need???allocation
                processes_[pid].PreRun(request);
                if (Safe(pid))
                {
                    //??????????????????????????????????????????run??????????????????????????????request
                    processes_[pid].Reverse(request);
                    processes_[pid].Run(request);
                    Log::WriteInfo(LogType::request_success, pid, request);
                    if (processes_[pid].IsFinish())
                    {
                        finisted_queue_.push(pid);
                        Log::WriteInfo(LogType::finish, pid);
                        available_ += processes_[pid].FreeResource();
                        //?????????????????????available??????????????????active_num
                        active_num = running_queue_.size();
                        PrintCurentState();
                        continue;
                    }
                    //????????????????????????????????????
                    running_queue_.push(pid);
                    PrintCurentState();
                }
                //?????????????????????
                else
                {
                    //???????????????????????????
                    processes_[pid].Reverse(request);
                    Log::WriteInfo(LogType::request_not_safe, pid);
                    Log::WriteInfo(LogType::wait, pid);
                    //??????available
                    available_ += request;
                    //???available??????????????????????????????????????????
                    active_num--;
                    running_queue_.push(pid);
                    if (!active_num)
                    {
                        Log::WriteError(LogType::all_request_bad);
                        End();
                    }
                }
            }
            //????????????????????????
            else
            {
                //????????????
                processes_[pid].Run(request);
                Log::WriteInfo(LogType::request_success, pid, request);
                if (processes_[pid].IsFinish())
                {
                    finisted_queue_.push(pid);
                    Log::WriteInfo(LogType::finish, pid);
                    available_ += processes_[pid].FreeResource();
                    active_num = running_queue_.size();
                    PrintCurentState();
                    continue;
                }
                running_queue_.push(pid);
                PrintCurentState();
            }
        }
        else
        {
            Log::WriteInfo(LogType::wait, pid);
            available_ += request;
            active_num--;
            running_queue_.push(pid);
            if (!active_num)
            {
                Log::WriteError(LogType::all_request_bad);
                End();
            }
        }
    }
}

bool OSOperator::Safe(int pid)
{
    //???????????????????????????????????????????????????????????????????????????
    std::queue<int> temp_running_queue = running_queue_;
    //???????????????????????????????????????????????????????????????
    if (!processes_[pid].IsFinish())
    {
        temp_running_queue.push(pid);
    }
    Vector temp_availabe = available_;
    std::vector<Process> temp_processes = processes_;
    Vector need;
    int active_num = temp_running_queue.size();
    while (!temp_running_queue.empty())
    {
        //printf("5555\n");
        int pid = temp_running_queue.front();
        temp_running_queue.pop();

        need = temp_processes[pid].GetNeed();

        temp_availabe -= need;

        if (temp_availabe >= 0)
        {
            temp_availabe += temp_processes[pid].FreeResource();
            active_num = temp_running_queue.size();
        }
        else
        {
            temp_availabe += need;
            temp_running_queue.push(pid);
            active_num--;
            if (!active_num)
                return false;
        }
    }
    return true;
}

void OSOperator::RollBack()
{
    //??????end_???
    end_ = false;
    //????????????
    available_ = available_duplicate_;
    running_queue_ = std::queue<int>();
    finisted_queue_ = std::queue<int>();
    //????????????process
    for (unsigned int i = 0; i < processes_.size(); ++i)
    {
        processes_[i].RoolBack();
    }
    printf("<===================================???????????????????????????===================================>\n");
    PrintCurentState();
    //???????????????????????????
    int pid = request_queue_.front().first;
    Vector request = request_queue_.front().second;
    int active_request_num = request_queue_.size();
    while ((!end_))
    {
        printf("<======================================??????????????????=====================================>\n");
        if (request_queue_.empty())
        {
            End();
            printf("?????????????????????????????????????????????\n");
            break;
        }
        int pid = request_queue_.front().first;
        Vector request = request_queue_.front().second;
        request_queue_.pop();
        Log::WriteInfo(LogType::request_info, pid, request);
        available_ -= request;
        //?????????????????????
        if (available_ >= 0)
        {
            //?????????????????????
            processes_[pid].PreRun(request);
            if (Safe(pid))
            {
                processes_[pid].Reverse(request);
                processes_[pid].Run(request);
                Log::WriteInfo(LogType::request_success, pid, request);
                if (processes_[pid].IsFinish())
                {
                    finisted_queue_.push(pid);
                    //Log::WriteInfo(LogType::finish, pid);
                    available_ += processes_[pid].FreeResource();
                }
                active_request_num = request_queue_.size();
                PrintCurentState();
                continue;
            }
            else
            {
                processes_[pid].Reverse(request);
                Log::WriteInfo(LogType::request_not_safe, pid);
                Log::WriteInfo(LogType::wait, pid);
                available_ += request;
                active_request_num--;
                request_queue_.push(Req(pid, request));
                if (!active_request_num)
                {
                    Log::WriteError(LogType::all_request_bad);
                    End();
                }
            }
        }
        else
        {
            available_ += request;
            Log::WriteInfo(LogType::wait, pid);
            active_request_num--;
            request_queue_.push(Req(pid, request));
            if (!active_request_num)
            {
                Log::WriteError(LogType::all_request_bad);
                End();
            }
        }
    }
}

int main()
{
    srand(time(0));
    //Vector vec;
    int option;
    printf(">>>>>>>>?????????????????????????????????1???????????????0??????????????????");
    scanf("%d", &option);
    printf("\n\n");
    while (1)
    {
        
        OSOperator os(BANKER);
        os.PrintCurentState();
        os.Run(option);
        if (!option)
        {
            printf(">>>>>>>>???????????????1???????????????0??????????????????");
            int if_rollback;
            scanf("%d", &if_rollback);
            printf("\n");
            if (if_rollback)
                os.RollBack();
        }
        printf("\n\n\n");
        printf(">>>>>>>>??????????????????????????????????????????1????????????????????????0???????????????");
        int if_continue;
        scanf("%d", &if_continue);
        printf("\n");
        if (!if_continue)
            break;
        printf("\n\n\n");
    }
    system("pause");
    return 0;
}

/*
OS????????????????????????run???process???????????????OS??????????????????
?????????????????????
???????????????
??????????????????

?????????????????????????????????

*/
