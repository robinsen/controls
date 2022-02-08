#ifndef TIME_WHEEL_TIMER
#define TIME_WHEEL_TIMER

#include <time.h>
#include <netinet/in.h>
#include <stdio.h>

#define BUFFER_SIZE 64
/*时间轮上槽的数目*/
static const int N = 60;   
/*每1 s时间轮转动一次，即槽问隔为1 s */
static const int TI = 1;

class tw_timer;
//綁定socket和定時器
struct client_data
{
    sockaddr_in address;
    int sockfd;
    int epollfd;
    char buf[ BUFFER_SIZE ];
    tw_timer* timer;
};

class tw_timer
{
public:
    tw_timer( int rot=0, int ts=0 ) 
    : next( NULL ), prev( NULL ), rotation( rot ), time_slot( ts ){}

public:
    int rotation;                       //記錄定時器在時間輪轉多少圈後生效
    int time_slot;                      //記錄定時器屬於時間輪上哪個槽（對應鏈表的位置）
    void (*cb_func)( client_data* );    //定時器回調函數
    client_data* user_data;             //客戶數據
    tw_timer* next;
    tw_timer* prev;

    void init(const int& timeout, const int& cur_slot)
    {
         int ticks = 0;
        /*下面根据特插入定时器的超时值计算它将在时间轮转动多少个滴答后被触发，并将该滴答数存储于变量ticks中。
        如果待插入定时器的超时值小于时间轮的槽间隔SI,则将ticks向上折合为1.否则就将ticks向下折合为timeout/SI */
        if( timeout < TI )
        {
            ticks = 1;
        }
        else
        {
            ticks = timeout / TI;
        }
        /* 计算待插入的定时器在时间轮转动多少圈后被触发*/
        rotation = ticks / N;
        /* 计算待插入的定时器应该被插入哪个槽中 */
        time_slot = ( cur_slot + ( ticks % N ) ) % N;
    }
};

class time_wheel
{
public:
    time_wheel() : cur_slot( 0 )
    {
        for( int i = 0; i < N; ++i )
        {
            slots[i] = NULL;
        }
    }
    ~time_wheel()
    {
        for( int i = 0; i < N; ++i )
        {
            tw_timer* tmp = slots[i];
            while( tmp )
            {
                slots[i] = tmp->next;
                delete tmp;
                tmp = slots[i];
            }
        }
    }
    //根據定時器timeout值創建一改定時器，並插入對應的槽
    tw_timer* add_timer( int timeout)
    {
        if( timeout < 0 )
        {
            return NULL;
        }
        /* 户创建新的定时器。它在时间轮转动rotation圈之后被触发，且位于第ts个槽上 */
        tw_timer* timer = new tw_timer();
        timer->init(timeout, cur_slot);
        int ts = timer->time_slot;
        int rotation = timer->rotation;
        /*如果第ts个槽中尚无任何定时器，则把新建的定时器插入其中，并将该定时器设置为该槽的头结点*/
        if( !slots[ts] )
        {
            slots[ts] = timer;
        }
        /* 否則，將定時器插入第ts個槽中 */
        else
        {
            timer->next = slots[ts];
            slots[ts]->prev = timer;
            slots[ts] = timer;
        }
        printf( "add timer, rotation is %d, ts is %d, cur_slot is %d\n", rotation, ts, cur_slot );
        return timer;
    }
    void del_timer( tw_timer* timer )
    {
        if( !timer )
        {
            return;
        }
        int ts = timer->time_slot;
        printf( "delete timer, rotation is %d, ts is %d, cur_slot is %d\n", timer->rotation, ts, cur_slot );
        /* slots[ts]是目标定时器所在槽的头结点。如果目标定时器就是该头结点，则需要重置第ts个槽的头结点*/
        if( timer == slots[ts] )
        {
            slots[ts] = slots[ts]->next;
            if( slots[ts] )
            {
                slots[ts]->prev = NULL;
            }
            delete timer;
        }
        else
        {
            timer->prev->next = timer->next;
            if( timer->next )
            {
                timer->next->prev = timer->prev;
            }
            delete timer;
        }
    }
    /* SI时间到后，调用该函数，时间轮向前滚动一个槽的间隔*/
    void tick()
    {
        tw_timer* tmp = slots[cur_slot];
        printf( "current slot is %d\n", cur_slot );
        while( tmp )
        {
            printf( "tick the timer once\n" );
            /*如果定时器的rotation值大于0，则它在这一轮不起作用*/
            if( tmp->rotation > 0 )
            {
                tmp->rotation--;
                tmp = tmp->next;
            }
            /*否则，说明定时器已经到期，于是执行定时任务，然后删除该定时器*/
            else
            {
                tmp->cb_func( tmp->user_data );
                if( tmp == slots[cur_slot] )
                {
                    printf( "delete header in cur_slot\n" );
                    slots[cur_slot] = tmp->next;
                    delete tmp;
                    if( slots[cur_slot] )
                    {
                        slots[cur_slot]->prev = NULL;
                    }
                    tmp = slots[cur_slot];
                }
                else
                {
                    tmp->prev->next = tmp->next;
                    if( tmp->next )
                    {
                        tmp->next->prev = tmp->prev;
                    }
                    tw_timer* tmp2 = tmp->next;
                    delete tmp;
                    tmp = tmp2;
                }
            }
        }
        cur_slot = ++cur_slot % N; /*更新时间轮的当前槽，以反映时间轮的转动*/
    }
    //收到數據更新定時時間
    void adjust_timer(int timeout, tw_timer* pTimer)
    {
        assert(pTimer);
        printf( "old timer, rotation is %d, old ts is %d, cur_slot is %d\n", pTimer->rotation, pTimer->time_slot, cur_slot );
        tw_timer* tmp = slots[pTimer->time_slot];
        int oldts = pTimer->time_slot;
        //判斷是不是頭節點
        if (tmp == pTimer)
        {
            slots[oldts] = pTimer->next;
            if( slots[oldts] )
            {
                slots[oldts]->prev = NULL;
            }
        }
        //把自己從鏈表中移除
        else
        {
            //自己的上一個節點的下一個設成自己的下一個
            pTimer->prev->next = pTimer->next;
            //如果自己的下一個不爲空，把自己的下一個節點的上一個設成自己的前一個節點
            if( pTimer->next )
            {
                pTimer->next->prev = pTimer->prev;
            }
        }
        pTimer->prev = nullptr;
        pTimer->next = nullptr;
        //重新生成槽
        pTimer->init(timeout, cur_slot);
        int ts = pTimer->time_slot;
        int rotation = pTimer->rotation;
        tw_timer* newslotTmp = slots[ts];
         /*如果第ts个槽中尚无任何定时器，则把新建的定时器插入其中，并将该定时器设置为该槽的头结点*/
        if( !newslotTmp )
        {
            slots[ts] = pTimer;
        }
        /* 否則，將定時器插入第ts個槽中 第一個位置 */
        else
        {
            pTimer->next = slots[ts];
            slots[ts]->prev = pTimer;
            slots[ts] = pTimer;
        }
        printf( "update timer, rotation is %d, new ts is %d, cur_slot is %d\n", rotation, ts, cur_slot );
    }

private:
    /*时间轮的槽，其中每个元素指向一个定时器链表，链表无序*/
    tw_timer* slots[N];
    /* 时间轮的当前槽*/
    int cur_slot;
};

#endif