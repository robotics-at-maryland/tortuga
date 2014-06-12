#ifndef __transfer_ownership_to_be_exported_hpp__
#define __transfer_ownership_to_be_exported_hpp__

struct event_t
{
    virtual ~event_t(){}
    void invoke(){
        notify();
    }
protected:
    virtual void notify() = 0;
};

struct do_nothing_t : event_t{
protected:
    virtual void notify(){};
};

struct simulator_t{
    
    void schedule(event_t *event) {
        m_event = event;
    };

    const event_t* get_event(){
        return m_event;
    }
    
    void run() {
        m_event->invoke();
        delete m_event;
        m_event = 0;
    };

private:
    event_t* m_event;
};


#endif//__transfer_ownership_to_be_exported_hpp__
