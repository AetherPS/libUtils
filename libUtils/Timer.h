#pragma once

class Timer
{
public:
    static uint64_t GetTime() { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); }
    inline        Timer() { this->Restart(); }
    inline void   Start() { this->StartTime = Timer::GetTime(); }
    inline void   Restart() { this->Start(); }
    inline uint64_t GetElapsed()   const { return Timer::GetTime() - this->StartTime; }
    inline uint64_t GetStartTime() const { return this->StartTime; }

    //
    // Runs a callback if the specified amount of time has passed.
    // 
    // Example:
    // static Timer CoolTimer;
    // CoolTimer.RunIfElapsed( 10000, [] ( ) {
    //         LOG_INFO( "Woooo" );
    //         CoolTimer.Restart( );
    //     }
    // );
    //
    template<typename F, typename...TArgs>
    inline void RunIfElapsed(uint64_t Elapsed, F Function, TArgs&&... Args)
    {
        if (this->GetElapsed() > Elapsed)
            Function(std::forward<TArgs>(Args)...);
    }
private:
    uint64_t StartTime;
};